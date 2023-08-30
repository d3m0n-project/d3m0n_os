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
		private async void setGlobalArgs(Control into, Dictionary<string, string> args)
		{
			for(int g=0; g<2; g++)
			{
				await Task.Delay(200);
				if(args.ContainsKey("name")) { into.Name = args["name"]; } else { into.Name = utils.RandomString(10); }
				
				
				// size
				if(args.ContainsKey("width")) { 
					if(args["width"].EndsWith("%"))
					{
						float width=(float)(4.8f * float.Parse(args["width"].Replace("%", ""), CultureInfo.InvariantCulture.NumberFormat));
						into.Width = (int)width;
						// into.Size = new Size(Int32.Parse(width.ToString()), into.Height);
					}
					else
					{
						// into.Size = new Size(Int32.Parse(args["width"]), into.Height);
						into.Height = Int32.Parse(args["width"]);
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
					try
					{
						if(Int32.Parse(args["position"].Split(",")[0].Replace(" ", "")) == 0)
						{}
						into.Location = getPosition(args["position"]); 
					}
					catch(Exception)
					{
						into.Dock = getDockPosition(args["position"]);
					}
					
				}
				
				
				// visibility
				if(args.ContainsKey("visible")) { 
					if(args["visible"] == "false")
					{
						into.Visible = false; 
					}
				}
			
			
			}
			// Events
			into.Click += (sender, EventArgs) => { Interpreter.loadEvent(into.Name+".OnClick", Graphics.src_file, Graphics.app_path); };

			utils.logn("[~] ("+into.Name+") width: "+into.Width.ToString()+" height: "+into.Height.ToString(), ConsoleColor.Yellow);
			utils.logn("    BackColor: "+into.BackColor.ToString()+" ForeColor: "+into.ForeColor.ToString(), ConsoleColor.Yellow);
			utils.logn("    Location(position): "+into.Location.ToString()+" height: "+into.Height.ToString(), ConsoleColor.Yellow);
		}
	}
}