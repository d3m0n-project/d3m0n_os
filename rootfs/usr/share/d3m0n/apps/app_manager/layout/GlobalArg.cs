using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Globalization;

namespace d3m0n
{
	public partial class layout
	{
		private async void setGlobalArgs(Control into, Dictionary<string, string> args, Graphics graphics)
		{
			// await Task.Delay(500);
			
			await Task.Delay(200);
			if(args.ContainsKey("name")) { into.Name = args["name"]; } else { into.Name = utils.RandomString(10); }
				
				
			// size
			if(args.ContainsKey("width")) { 
				utils.logn("[*] "+into.Name+"  => '"+args["width"]+"'", ConsoleColor.Magenta);
				if(args["width"].EndsWith("%"))
				{
					float width=(float)(4.8f * float.Parse(args["width"].Replace("%", ""), CultureInfo.InvariantCulture.NumberFormat));
					into.Width = (int)width;
					// into.Size = new Size(Int32.Parse(width.ToString()), into.Height);
				}
				else
				{
					// into.Size = new Size(Int32.Parse(args["width"]), into.Height);
					into.Width = Int32.Parse(args["width"]);
				}
			}
			if(args.ContainsKey("height")) { 
				if(args["height"].EndsWith("%"))
				{
					float height=(float)(6.4f * float.Parse(args["height"].Replace("%", ""), CultureInfo.InvariantCulture.NumberFormat));
					// into.Size = new Size(into.Width, (int)height);
					into.Height = (int)height;
				}
				else
				{
					// into.Size = new Size(into.Width, Int32.Parse(args["height"]));
					into.Height = Int32.Parse(args["height"]);
				}
				
			}
				
			// color
			if(args.ContainsKey("bg_color")) {into.BackColor = getLayoutColor(args["bg_color"]);}
			if(args.ContainsKey("color")) {into.ForeColor = getLayoutColor(args["color"]);}
				
			// position
			if(args.ContainsKey("position")) { 
				into.Location = getPosition(into.Width, into.Height, args["position"], graphics); 
				// MessageBox.Show(into.Name+".position => "+args["position"]);
			}
			// margin
			if(args.ContainsKey("margin_top")) { 
					into.Top += Int32.Parse(args["margin_top"]);
			}
			if(args.ContainsKey("margin_left")) { 
					into.Left += Int32.Parse(args["margin_left"]);
			}
			if(args.ContainsKey("margin_bottom")) { 
					// MessageBox.Show("Top: "+into.Top.ToString()+" Left: "+into.Left.ToString());
					into.Top -= Int32.Parse(args["margin_bottom"]);
					// MessageBox.Show("Top: "+into.Top.ToString()+" Left: "+into.Left.ToString());
			}
			if(args.ContainsKey("margin_right")) { 
					into.Left -= Int32.Parse(args["margin_right"]);
			}

			if(args.ContainsKey("parent")) {
				try
				{
					into.Parent = graphics.into.Controls.Find(args["parent"],true)[0];
				}
				catch(Exception)
				{
					utils.logn("[x] "+into.Name+" can't have "+args["parent"]+" as parent", ConsoleColor.Red);
				}
				
			}
			else
			{
				try
				{
					into.Parent = graphics.into;
				}
				catch(Exception)
				{}
			}
			
			
			// visibility
			if(args.ContainsKey("visible")) { 
				if(args["visible"] == "false")
				{
					into.Visible = false; 
				}
			}

			// Events
			if(!args.ContainsKey("disabled"))
			{
				args["disabled"] = "false";
			}
			if(args["disabled"] == "false")
			{
				into.Click          += (sender, EventArgs) => { Interpreter.loadEvent(into.Name+".OnClick", Graphics.src_file, Graphics.app_path); };
				into.Enter          += (sender, EventArgs) => { Interpreter.loadEvent(into.Name+".OnEnter", Graphics.src_file, Graphics.app_path); };
				into.Leave          += (sender, EventArgs) => { Interpreter.loadEvent(into.Name+".OnLeave", Graphics.src_file, Graphics.app_path); };
			}
			


			utils.logn("[~] ("+into.Name+") width: "+into.Width.ToString()+" height: "+into.Height.ToString(), ConsoleColor.Yellow);
			utils.logn("    BackColor: "+into.BackColor.ToString()+" ForeColor: "+into.ForeColor.ToString(), ConsoleColor.Yellow);
			utils.logn("    Location(position): "+into.Location.ToString()+" Dock: "+into.Dock.ToString(), ConsoleColor.Yellow);
			utils.logn("    Top: "+into.Top.ToString()+" Left: "+into.Left.ToString(), ConsoleColor.Yellow);
		}
	}
}