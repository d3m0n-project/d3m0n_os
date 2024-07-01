using System;
// using System.Drawing;
// using System.Windows.Forms;
using System.IO;
using System.Threading.Tasks;
using System.Drawing.Imaging;
using display;

namespace d3m0n
{
	public partial class Program
	{
		public static Window into;
		public static void Main(string[] args)
		{
			// Application.EnableVisualStyles();
			into = new Window();
		
			// int screenWidth = 240;
            // int screenHeight = 320;
            int offsetx = 0;
            int offsety = 0;


            ili9340.lcdInit(width, height, offsetx, offsety);
            ili9340.lcdReset();
            ili9340.lcdSetup();

            ili9340.lcdFillScreen(Colors.Black);

			window_main();
		}
		public static Control getTopbar()
		{
			if(into != null)
			{
				return into.Get("topbar");
			}
			else
			{
				return new Label();
			}
		}

		public static int width=240;
		public static int height=320;

		public static Image splashimage;
		public static Panel topbar;
		public static Label topbar_time;
		public static Image topbar_battery_image;
		public static Label topbar_battery;
		public static Image topbar_internet;


		private static void window_main()
		{
			into.BackColor = Colors.getRGB565(0, 0, 0);


			splashimage = new Image();
            splashimage.Img = System.Drawing.Image.FromFile(utils.getPath()+"/splash/splash.png");
            splashimage.Location = new DataTypes.Point(0, 0);
            splashimage.Size = new DataTypes.Size(100, 100);
            splashimage.Name="splashimage";
            splashimage.draw();
            into.Add(splashimage);
			Task.Run(() => splash());

			
			draw_home(into);
            




			topbar = new Panel();
			topbar.Size = new DataTypes.Size(240, 25);
			topbar.Location = new DataTypes.Point(0, 0);
			topbar.BackColor = Colors.White;
			// topbar.Click += (sender, EventArgs) => { expand_topbar(); };
			topbar.draw();
            into.Add(topbar);

			// time
			topbar_time = new Label();
			topbar_time.Center = true;
			topbar_time.ForeColor = Colors.Black;
			DateTime currentDateTime = DateTime.Now;
			string date = currentDateTime.ToString("HH:mm");
			topbar_time.Text = date;
			topbar_time.Location = new DataTypes.Point(0, 3);
			topbar_time.draw();
			topbar.Add(topbar_time);



			topbar_internet = new Image();
			topbar_internet.Size = new DataTypes.Size(20, 20);
			topbar_internet.Img = theme_manager.get_icon("internet-off-black");
        	topbar_internet.Location = new DataTypes.Point(50, 0);
			topbar_internet.draw();
			topbar.Add(topbar_internet);



        	topbar_battery = new Label();
        	topbar_battery.Text = "100%";
        	topbar_battery.Center = true;
        	topbar_battery.ForeColor = Colors.Black;
        	topbar_battery.Location = new DataTypes.Point(width-70, 3);
			topbar_battery.draw();
			topbar.Add(topbar_battery);

        	topbar_battery_image = new Image();
        	topbar_battery_image.Img = theme_manager.get_icon("battery-100-black");
        	topbar_battery_image.Size = new DataTypes.Size(25, 25);
        	topbar_battery_image.Location = new DataTypes.Point(width-(25), 0);
			topbar_battery_image.draw();
        	topbar_battery.Add(topbar_battery_image);
        	

			// // getting full topbar
			// full_topbar = complete_topbar.Get();
			// full_topbar.Width = this.Width;
			// full_topbar.Visible = false;
			// this.Controls.Add(full_topbar);
			
			// // create switch bar
			// bottom_switch_bar = new Panel();
			// bottom_switch_bar.Width = this.Width;
			// bottom_switch_bar.Height = 30;
			// bottom_switch_bar.Location = new Point(0, this.Height-bottom_switch_bar.Height);


			// list_bottom_switch_bar = new PictureBox();
			// list_bottom_switch_bar.Location = new Point((this.Width/2)-(3*30)+15, 0);
			// list_bottom_switch_bar.Size = new Size(30, 30);
			// list_bottom_switch_bar.SizeMode = PictureBoxSizeMode.StretchImage;
			// list_bottom_switch_bar.Image = theme_manager.get_icon("list-white");

			// home_bottom_switch_bar = new PictureBox();
			// home_bottom_switch_bar.Size = new Size(30, 30);
			// home_bottom_switch_bar.SizeMode = PictureBoxSizeMode.StretchImage;
			// home_bottom_switch_bar.Location = new Point((this.Width/2)-(1*30)+15, 0);
			// home_bottom_switch_bar.Image = theme_manager.get_icon("home-white");

			// back_bottom_switch_bar = new PictureBox();
			// back_bottom_switch_bar.Location = new Point((this.Width/2)-(-1*30)+15, 0);
			// back_bottom_switch_bar.Size = new Size(30, 30);
			// back_bottom_switch_bar.SizeMode = PictureBoxSizeMode.StretchImage;
			// back_bottom_switch_bar.Image = theme_manager.get_icon("chevron-white", 90);

			// bottom_switch_bar.Controls.Add(list_bottom_switch_bar);
			// bottom_switch_bar.Controls.Add(home_bottom_switch_bar);
			// bottom_switch_bar.Controls.Add(back_bottom_switch_bar);




	        // // add top and bottom bars
	        // Controls.Add(splashimage);
			// Controls.Add(bottom_switch_bar);
	        // Controls.Add(topbar);
	        // Controls.Add(embeding);


	        
	        Task.Run(() => Update());

			//enable touchscreen
			touchscreen.touchInit(into);
		}
		public static void expand_topbar()
		{
			// full_topbar.Visible = true;
		}
	    private static async void Update()
	    {
	    	while(true)
	    	{
	    		// topbar time
	    		DateTime currentDateTime = DateTime.Now;
				string date = currentDateTime.ToString("HH:mm");
				topbar_time.Text = date;
				topbar_time.draw();

				// complete_topbar.update(full_topbar, currentDateTime);

	    		// topbar internet
	    		if(network.isInternetConnected())
	    		{
	    			topbar_internet.Img = theme_manager.get_icon("internet-on-black");
	    		}
	    		else
	    		{
	    			topbar_internet.Img = theme_manager.get_icon("internet-off-black");
	    		}
				topbar_internet.draw();
	    		topbar_battery.Text=battery.getPercentage();
	    		await Task.Delay(2000);
	    	}
	        
	        
	    }

	    public static async void splash()
	   	{
			Interpreter.unloadApps();
			
	   		theme_manager.PlaySound("boot");
	   		update.checkForUpdates();

	   		int splash_time = Int32.Parse(utils.getSetting("splash_time", utils.getConfigPath()));
		    
		    await Task.Delay(splash_time);

		    splashimage.Visible = false;
	   	}
	}
}