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
		public static Form into;
		public static bool already_init = false;
		public static bool running=false;
		public static string app_path;
		public static string src_file;

		public static async void init(string path)
		{
			Application.EnableVisualStyles();
			Dispose();
			closeLayout();
			app_path=path;
			into = new app_display();
			already_init=true;
        	// into.Show();
			Application.Run(into);
		}


		public static void closeLayout()
		{
			if(into != null)
			{
				into.Close();
			}
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
	public static class ControlExtensions
	{
	    public static T Clone<T>(this T controlToClone) 
	        where T : Control
	    {
	        PropertyInfo[] controlProperties = typeof(T).GetProperties(BindingFlags.Public | BindingFlags.Instance);

	        T instance = Activator.CreateInstance<T>();

	        foreach (PropertyInfo propInfo in controlProperties)
	        {
	            if (propInfo.CanWrite)
	            {
	                if(propInfo.Name != "WindowTarget")
	                    propInfo.SetValue(instance, propInfo.GetValue(controlToClone, null), null);
	            }
	        }

	        var controls = controlToClone.Controls;
	        for(int i=0; i < controls.Count; i++) {
	        	Control control = processChildren(controls[i]);
	        	var children_controls = controls[i].Controls;
		       	for(int j=0; j < children_controls.Count; j++) {
		       		Control child = processChildren(children_controls[j]);
		       		control.Controls.Add(child);
		       	}
	        	instance.Controls.Add(control);
	        	
	        }
	        return instance;
	    }
	    public static Control processChildren(Control controls)
	    {
	    	if(controls.ToString().StartsWith("System.Windows.Forms.Label"))
	        {
	        	Label control = controls as Label;
	        	Label newcontrol = new Label();
	        	newcontrol.ForeColor = control.ForeColor;
	        	newcontrol.BackColor = control.BackColor;
	        	newcontrol.Location = control.Location;
	        	newcontrol.Font = control.Font;
	        	newcontrol.TextAlign = control.TextAlign;
	        	newcontrol.Dock = control.Dock; 
	        	newcontrol.Size = control.Size;
		       	newcontrol.Text = control.Text;
		       	newcontrol.Name = control.Name;

		        return newcontrol;
		       	
	        }
	        if(controls.ToString().StartsWith("System.Windows.Forms.PictureBox"))
	        {
	        	PictureBox control = controls as PictureBox;
	        	PictureBox newcontrol = new PictureBox();
	        	newcontrol.Size = control.Size;
				newcontrol.SizeMode = control.SizeMode;
				newcontrol.Image = control.Image;
		       	newcontrol.Dock = control.Dock;
		       	newcontrol.Location = control.Location;
		       	newcontrol.Name = control.Name;
	       		newcontrol.ForeColor = control.ForeColor;
	       		newcontrol.BackColor = control.BackColor;
		       	return newcontrol;
		       	
	       	}
	       	return new Label();
	   	}
	}
	public class app_display : Form
	{	 
		public Control getTopbar(Control ctrl)
		{
			return topbar;
		}
		public static Panel topbar;
	    public app_display()
	    {
	    	// InitializeComponent();
	    	Application.EnableVisualStyles();

	    	Graphics.into = this;
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
	    	this.Size = new Size(480, 640);
            this.StartPosition = FormStartPosition.CenterScreen;
            // this.Location = new Point(this.Location.X, this.Location.Y-25);
            // this.CenterToScreen();

            
            Graphics.into.Load   += (sender, EventArgs) => { Interpreter.loadEvent("Window.OnCreate", Graphics.src_file, Graphics.app_path); };
            Graphics.into.Closed += (sender, EventArgs) => { Interpreter.loadEvent("Window.OnDestroy", Graphics.src_file, Graphics.app_path); };
            
            // Program prgm = new Program();

            topbar = Program.getTopbar().Clone();
			topbar.Name = "topbar";

            Controls.Add(topbar);

	    }
	    public void OnConfigFinished()
	    {
	    	// app_display app = new app_display();
	    	// app.CenterToScreen();
	    }
	}
}