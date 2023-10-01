using System;
using System.IO;
using System.Windows.Forms;
using System.Diagnostics;
using System.Collections.Generic;
using System.Drawing;
using System.Threading.Tasks;
using System.Windows;
using System.Reflection;
using System.Linq;

namespace d3m0n
{
	class Graphics
	{
		public static Dictionary<string, Form> available_windows = new Dictionary<string, Form>();
		public static Form into;
		public static bool already_init = false;
		public static bool running=false;
		public static string app_path;
		public static string src_file;
		public static string current_appname;

		public static async void init(string path, string appname)
		{
			// Application.EnableVisualStyles();
			// Dispose();
			// closeLayout();
			app_path=path;
			into = new app_display();
			already_init=true;
        	// into.Show();
			Application.Run(into);
			current_appname=appname;
			available_windows.Add(appname, into);
		}

		public static void destroy(string name)
		{
			// close app by name
			try
			{
				if(available_windows.ContainsKey(name) && into != null)
				{
					available_windows[name].Close();
					available_windows.Remove(name);
				}
			}
			catch(Exception)
			{}
			
		}

		public static void hideCustomView()
		{
			app_display.setCustomView(new Panel());
			app_display.getCustomView().Visible = false;
		}

		public static void displayCustomView(Panel a)
		{
			app_display.setCustomView(a);
			app_display.getCustomView().Visible = true;
		}

		private static void display(Control control)
		{
			into.Controls.Add(control);
		}
		public static void layout_to_form(string control, Dictionary<string, string> args)
		{
			while(!already_init)
			{
				utils.logn("[x] app not loaded", ConsoleColor.Red);
			}
			switch(control)
			{
				default:
					Console.WriteLine("[x] graphics.cs => conrol '"+control+"' does not exists");
					break;
				case "RawHtml":
					display(layout.RawHtml(args));
					break;
				case "TextBox":
					display(layout.TextBox(args));
					break;
				case "ListView":
					display(layout.Rect(args));
					break;
				case "ProgressBar":
					display(layout.ProgressBar(args));
					break;
				case "CheckBox":
					display(layout.CheckBox(args));
					break;
				case "RadioButton":
					display(layout.RadioButton(args));
					break;
				case "WebView":
					display(layout.WebView(args));
					break;
				case "Rect":
					display(layout.Rect(args));
					break;
				case "Switch":
					display(layout.Switch(args));
					break;
				case "Text":
					display(layout.Text(args));
					break;
				case "Image":
					display(layout.Image(args));
					break;
				case "Button":
					display(layout.Button(args));
					break;
				case "Window":
					layout.Window(into, args);
					break;
			}
		}
	    public static void Dispose()
		{
			if(into != null)
			{
				GC.Collect();
		    	GC.SuppressFinalize(into);
		    }
		}
	}
	public class app_display : Form
	{	 
		public Control getTopbar(Control ctrl)
		{
			return topbar;
		}

		public static void setCustomView(Panel a)
		{
			CustomView = a;
		}
		public static Control getCustomView()
		{
			return CustomView;
		}

		public static Panel topbar;
		public static Panel CustomView { get; set; }

	    public app_display()
	    {
	    	Graphics.into = this;

			// destroy application when it close
			Graphics.into.Closing += (sender, EventArgs) => { Graphics.destroy(Graphics.current_appname); };
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
	    	this.Size = new Size(480, 640);
            this.StartPosition = FormStartPosition.CenterScreen;

            
            Graphics.into.Load   += (sender, EventArgs) => { Interpreter.loadEvent("Window.OnCreate", Graphics.src_file, Graphics.app_path); };
            Graphics.into.Closed += (sender, EventArgs) => { Interpreter.loadEvent("Window.OnDestroy", Graphics.src_file, Graphics.app_path); };
            
            // Program prgm = new Program();

            topbar = Program.getTopbar().Clone();
			topbar.Name = "topbar";

			// custom view
			CustomView = new Panel();
			CustomView.Size = new Size(480, 640);
			CustomView.Location = new Point(0, 0);
			CustomView.BackColor = Color.Lime;
			CustomView.Visible = false;

			
            Controls.Add(topbar);
			Controls.Add(CustomView);
			

	    }

		public static void OnLoadComplete()
		{
			
		}
	}
}