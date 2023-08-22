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
            	wallpaper = Image.FromFile(utils.getSetting("wallpaper", utils.getConfigPath()));
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





			// name+"//!//"+icon+"//!//"executable+"//!//"+perms+"//!//"+start_path+"//!//"+category+"//!//"+package;
			int column_nbr = 1;
			int line_nbr   = 1;
			int page_number= 1;
			foreach(string raw in app_manager.getAllApps())
			{
				Console.WriteLine(raw);
				if(column_nbr >= 4)
				{
					column_nbr=1;
					line_nbr++;
					if(line_nbr >= 4)
					{
						line_nbr=1;
						page_number++;
					}
				}
				string name = raw.Split("//!//")[0];
			    string icon = raw.Split("//!//")[1];
			    string executable = raw.Split("//!//")[2];
			    string perms = raw.Split("//!//")[3];
				string start_path = raw.Split("//!//")[4];
				string category = raw.Split("//!//")[5];
				string package = raw.Split("//!//")[6];




				// 10px + 100px*4 + 10px   w
				// 20px + 100px*4 + 20px   h
				FlowLayoutPanel app = new FlowLayoutPanel();
				app.Name = name;
				app.BackColor = Color.Transparent;
				app.Size = new Size(100, 100);
				app.Location = new Point((10*2*column_nbr-10 + 100*(column_nbr-1)), 20*line_nbr + line_nbr*100);
				app.FlowDirection = FlowDirection.TopDown;
				app.WrapContents = false;
				app.Click += (sender, EventArgs) => { App_Click(executable, sender, EventArgs); };
				Controls.Add(app);

				PictureBox app_icon = new PictureBox();
				app_icon.Name = name+"_app_icon";
				app_icon.Size = new Size(80, 80);
				app_icon.Click += (sender, EventArgs) => { App_Click(executable, sender, EventArgs); };
				if(File.Exists(icon))
				{
					Image image = Image.FromFile(icon);
					app_icon.Image = image;
				}
				else
				{
				    app_icon.Image = theme_manager.get_icon(name);
				}
				app_icon.SizeMode = PictureBoxSizeMode.StretchImage;
				app.Controls.Add(app_icon);

				Label app_name = new Label();
				app_name.Name = name+"_app_name";
				app_name.Size = new Size(100, 20);
				app_name.Text = name;
				app_name.AutoSize = false;
				app_name.Dock = DockStyle.None;
				app_name.Font = new Font("Arial", 10);
				app_name.ForeColor = Color.FromArgb(255, 255, 255);
				app_name.Click += (sender, EventArgs) => { App_Click(executable, sender, EventArgs); };
				app.Controls.Add(app_name);
			
				column_nbr++;
				Console.WriteLine("column_nbr: "+column_nbr);
				Console.WriteLine("line_nbr: "+line_nbr);
			}
	    }
	 
	    private void App_Click(string path, object sender, EventArgs e)
	    {
	        MessageBox.Show(name+": App Clicked!");
	        app_embed.Visible = true;
	        System.Reflection.Assembly.LoadFile, System.Reflection.Assembly.LoadFrom(path);
	        d3m0n_app app = new d3m0n_app();
			utils.ShowFormInContainerControl(app_embed, app);

	        // embed.embed_process("/root/d3m0n_os_debian/rootfs/usr/share/d3m0n/apps/photo/camera/camera.exe", app_embed);
	    	// System.Threading.Thread.Sleep(2);
	    	// embed.embed_stop();
	    }
	}
}