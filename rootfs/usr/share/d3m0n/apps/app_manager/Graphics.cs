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
using Microsoft.Web.WebView2.WinForms;

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

		private static bool displayAlreadyDelayed = false;
		public static void display(Control control)
		{
			// displays element
			if(!displayAlreadyDelayed) {
				System.Threading.Thread.Sleep(500);
				displayAlreadyDelayed=true;
			}
			System.Threading.Thread.Sleep(250);
			// MessageBox.Show(control.ToString());
			into.Controls.Add(control);
		}
		public static Control layout_to_form(string control, Dictionary<string, string> args, Control ctrl=null)
		{
			while(!already_init)
			{
				utils.logn("[x] app not loaded", ConsoleColor.Red);
				return null;
			}

			if(control=="Window") {
				layout.Window(into, args);
				return null;
			}
			else if(control=="Text") {
				return layout.Text(args, ctrl as Label);
			}
			else if(control=="RawHtml") {
				return layout.RawHtml(args);
			}
			else if(control=="TextBox") {
				return layout.TextBox(args, ctrl as TextBox);
			}
			else if(control=="ListView") {
				return layout.ListView(args, ctrl as ListView);
			}
			else if(control=="ProgressBar") {
				return layout.ProgressBar(args, ctrl as ProgressBar);
			}
			else if(control=="CheckBox") {
				return layout.CheckBox(args, ctrl as CheckBox);
			}
			else if(control=="RadioButton") {
				return layout.RadioButton(args, ctrl as RadioButton);
			}
			// error idk what
			// else if(control=="WebView") {
			// 	return layout.WebView(args, ctrl as WebView2);
			// }
			else if(control=="Rect") {
				return layout.Rect(args, ctrl as Panel);
			}
			else if(control=="Switch") {
				return layout.Switch(args, ctrl as CheckBox);
			}
			else if(control=="Image") {
				return layout.Image(args, ctrl as PictureBox);
			}
			else if(control=="Button") {
				return layout.Button(args, ctrl as Button);
			}
			else {
				utils.logn("[x] graphics.cs => conrol '"+control+"' does not exists", ConsoleColor.Red);
				return null;
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
		public void setControlProperty(string controlName, string property, string value)
		{
			try
			{
				Control control = this.Controls.Find(controlName,true)[0];
				control = properties.parseProperties(control, property, value);
			}
			catch(Exception)
			{
				utils.logn("[x] control '"+controlName+"' does not exists in current app", ConsoleColor.Red);
			}
		}

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