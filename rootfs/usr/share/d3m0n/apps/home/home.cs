using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;

namespace d3m0n
{
	public class home : Form
	{	 
		public FlowLayoutPanel app_embed;
	    public home()
	    {
	    	// Application.EnableVisualStyles();

            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;

            this.BackColor = Color.FromArgb(0, 0, 0);
            
            Image wallpaper;
            try
            {
            	wallpaper = Image.FromFile(utils.getPath()+"/wallpapers/"+utils.getSetting("wallpaper", utils.getConfigPath())+".png");
            	this.BackgroundImage = wallpaper;
        	}
            catch(Exception)
            {
            	MessageBox.Show("wallpaper not found");
            }
    		
    		// this.Dock = DockStyle.Fill; 
    		this.BackgroundImageLayout = ImageLayout.Stretch;

            this.Size = new Size(480, 640);
            this.Name = "d3m0n_home";
            this.Text = "d3m0n home";
			// this.SuspendLayout();
			// this.FormBorderStyle = FormBorderStyle.FixedDialog;


            app_embed = new FlowLayoutPanel();
			app_embed.Name = "embed_app";
			// app_embed.BackColor = Color.LimeGreen;
			app_embed.Size = new Size(480, (640 - 25));
			app_embed.Visible = false;
			app_embed.Location = new Point(0, 25);
			app_embed.FlowDirection = FlowDirection.TopDown;
			app_embed.WrapContents = false;
			Controls.Add(app_embed);





			foreach (string file in Directory.EnumerateFiles(utils.getPath(), "*.d3m0n", SearchOption.AllDirectories))
			{
				string raw = Interpreter.loadApp(file);
				Console.WriteLine("file: "+file);
				Console.WriteLine("raw: "+raw);
				if(raw.StartsWith("can't load app"))
				{
					MessageBox.Show(raw);
					continue;
				}

				string name = raw.Split("//!//")[0];
	            string icon = raw.Split("//!//")[1];
	            string temp_path = raw.Split("//!//")[2];
	            string perms = raw.Split("//!//")[3];
	            string start_path = raw.Split("//!//")[4];
	            string category = raw.Split("//!//")[5];
	            string package = raw.Split("//!//")[6];

	            d3m0nApplication.name = name;
	            d3m0nApplication.icon = icon;
	            d3m0nApplication.temp_path = temp_path;
	            d3m0nApplication.perms = perms;
	            d3m0nApplication.start_path = start_path;
	            d3m0nApplication.category = category;
	            d3m0nApplication.package = package;

	            d3m0nApplication.display(this);
			}
	    }
	}
}