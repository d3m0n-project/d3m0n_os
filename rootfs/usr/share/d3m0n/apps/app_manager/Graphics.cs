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
	public class Graphics
	{
		public static Dictionary<string, Form> available_windows = new Dictionary<string, Form>();
		public Form into;
		public bool already_init = false;
		public static bool running=false;
		public static string app_path;
		public static string src_file;
		public static string current_appname;
		public static string current_package;

		public async void init(string path, string appname, string package)
		{
			// Application.EnableVisualStyles();
			// Dispose();

			app_path=path;
			current_appname=appname;
			current_package=package;

			// define app vars for winform class
			into = new app_display();

			already_init=true;
        	// into.Show();

			

			Application.Run(into);
			
		}

		public static void destroy(string package)
		{
			// close app by package
			MessageBox.Show("closing '"+package+"'");
			try
			{
				if(available_windows.ContainsKey(package))
				{
					available_windows[package].Close();
					available_windows.Remove(package);
				}
				else
				{
					MessageBox.Show("App '"+package+"' seems to be not oppened");
				}
			}
			catch(Exception e)
			{
				MessageBox.Show("an error occured while closing an app");
			}
			
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

		public static void display(string appInstance, Control control)
		{
			try {
				// displays element
				if(!displayAlreadyDelayed) {
					System.Threading.Thread.Sleep(500);
					displayAlreadyDelayed=true;
				}
				System.Threading.Thread.Sleep(250);
				// MessageBox.Show(control.ToString());
				available_windows[appInstance].Controls.Add(control);
			}
			catch(Exception)
			{
				utils.logn("[x] error => Graphics.cs can't add '"+control.Name+"' app '"+appInstance+"'", ConsoleColor.Red);
			}
		}
		
		public Control layout_to_form(string control, Dictionary<string, string> args, Control ctrl=null)
		{
			if(control=="Window") {
				layout.Window(into, args, this);
				return null;
			}
			else if(control=="Text") {
				return layout.Text(args, this, ctrl as Label);
			}
			else if(control=="RawHtml") {
				return layout.RawHtml(args);
			}
			else if(control=="TextBox") {
				return layout.TextBox(args, this, ctrl as TextBox);
			}
			else if(control=="ListView") {
				return layout.ListView(args, this, ctrl as ListView);
			}
			else if(control=="ProgressBar") {
				return layout.ProgressBar(args, this, ctrl as ProgressBar);
			}
			else if(control=="CheckBox") {
				return layout.CheckBox(args, this, ctrl as CheckBox);
			}
			else if(control=="RadioButton") {
				return layout.RadioButton(args, this, ctrl as RadioButton);
			}
			// error idk what
			// else if(control=="WebView") {
			// 	return layout.WebView(args, ctrl as WebView2);
			// }
			else if(control=="Rect") {
				return layout.Rect(args, this, ctrl as Panel);
			}
			else if(control=="Switch") {
				return layout.Switch(args, this, ctrl as CheckBox);
			}
			else if(control=="Image") {
				return layout.Image(args, this, ctrl as PictureBox);
			}
			else if(control=="Button") {
				return layout.Button(args, this, ctrl as Button);
			}
			else if(control=="Vscroll") {
				return layout.Vscroll(args, this, ctrl as Panel);
			}
			else if(control=="Hscroll") {
				return layout.Hscroll(args, this, ctrl as Panel);
			}
			else if(control=="RoundButton") {
				return layout.RoundButton(args, this, ctrl as RoundedButton);
			}
			else {
				utils.logn("[x] graphics.cs => conrol '"+control+"' does not exists", ConsoleColor.Red);
				return null;
			}
		}

	    public void Dispose()
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
		public string current_package = Graphics.current_package;
		public string current_appname = Graphics.current_appname;
		public string src_file = Graphics.src_file;
		public string app_path = Graphics.app_path;
		
		
		
		public static Panel topbar;
		public static Panel CustomView { get; set; }

	    public app_display()
	    {
			// MessageBox.Show("added '"+current_package+"'");
			Graphics.available_windows.Add(current_package, this);

			// destroy application when it close
			this.Closing += (sender, EventArgs) => { Graphics.destroy(current_appname); };
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
	    	this.Size = new Size(480, 640);
            this.StartPosition = FormStartPosition.CenterScreen;

            
            this.Load   += (sender, EventArgs) => { Interpreter.loadEvent("Window.OnCreate", src_file, app_path); };
            this.Closed += (sender, EventArgs) => { Interpreter.loadEvent("Window.OnDestroy", src_file, app_path); };
            
            // Program prgm = new Program();

            topbar = Program.getTopbar().Clone();
			topbar.Name = "topbar";
			Task.Run(() => TopBar_Update());
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

		public async void TopBar_Update()
		{
	    	while(true)
	    	{
	    		// update topbar
				topbar = Program.getTopbar().Clone();
	    		await Task.Delay(2000);
	    	}
		}
	}
}