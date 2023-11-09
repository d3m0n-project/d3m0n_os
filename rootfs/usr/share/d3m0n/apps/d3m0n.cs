using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Threading.Tasks;
using System.Drawing.Imaging;

namespace d3m0n
{
	class Program
	{
		public static Form into;
		public static void Main(string[] args)
		{
			Application.EnableVisualStyles();
			into = new d3m0n();
			Application.Run(into);
		}
		public static Panel getTopbar()
		{
			if(into != null)
			{
				return d3m0n.topbar;
			}
			else
			{
				return new Panel();
			}
		}
	}
	public class d3m0n : Form
	{	 
		public static Panel topbar;
		PictureBox splashimage;
		Label topbar_battery;
		PictureBox topbar_battery_image;
		Label topbar_time;
		PictureBox topbar_internet;
		public static Control full_topbar;
		public static Panel bottom_switch_bar;
		PictureBox home_bottom_switch_bar;
		PictureBox back_bottom_switch_bar;
		PictureBox list_bottom_switch_bar;

	    public d3m0n()
	    {

	    	// Application.EnableVisualStyles();


	    	// this.WindowState = FormWindowState.Maximized;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            // this.Bounds = Screen.PrimaryScreen.Bounds;

            this.BackColor = Color.FromArgb(0, 0, 0);
            this.Name = "d3m0n_app_launcher";
            this.Font = new Font("Arial", 15);
            this.Text = "d3m0n app launcher";
            this.Size = new Size(480, 640);
			// this.Size = new Size(utils.getScreenWidth(), utils.getScreenHeight());
            this.StartPosition = FormStartPosition.CenterScreen;
			// this.SuspendLayout();
			// this.FormBorderStyle = FormBorderStyle.FixedDialog;


            splashimage = new PictureBox();
            splashimage.SizeMode = PictureBoxSizeMode.StretchImage;
            splashimage.Size = new Size(this.Width, this.Height);
            splashimage.Image = Image.FromFile(utils.getPath()+"/splash/splash.png");
            Task.Run(() => splash());




			topbar = new Panel();
			topbar.Size = new Size(this.Width, 25);
			topbar.Anchor = AnchorStyles.Right;  
			// topbar.FlowDirection = FlowDirection.LeftToRight;
			topbar.Location = new Point(0, 0);
			// topbar.WrapContents = false;
			topbar.BackColor = Color.FromArgb(255, 255, 255);
			topbar.Click += (sender, EventArgs) => { expand_topbar(); };

			// time
			topbar_time = new Label();
			topbar_time.Font = new Font("Arial", 10);
			topbar_time.TextAlign = ContentAlignment.MiddleCenter;
        	// topbar_time.Dock = DockStyle.None;
			DateTime currentDateTime = DateTime.Now;
			string date = currentDateTime.ToString("HH:mm");
			topbar_time.Text = date;



			topbar_internet = new PictureBox();
			topbar_internet.Size = new Size(25, 25);
			topbar_internet.SizeMode = PictureBoxSizeMode.StretchImage;
			topbar_internet.Image = theme_manager.get_icon("internet-off-black");
        	topbar_internet.Dock = DockStyle.None;
        	topbar_internet.Location = new Point(5, 0);


        	topbar_battery = new Label();
        	topbar_battery.Text = "100%";
        	topbar_battery.TextAlign = ContentAlignment.MiddleLeft;
        	// topbar_battery.Dock = DockStyle.None;
        	// topbar_battery.Width = 10;
        	topbar_battery.Font = new Font("Arial", 10);
        	topbar_battery.ForeColor = Color.Black;
        	topbar_battery.Location = new Point(75, 0);

        	topbar_battery_image = new PictureBox();
        	topbar_battery_image.Image = theme_manager.get_icon("battery-100-black");
        	topbar_battery_image.Size = new Size(25, 25);
        	topbar_battery_image.Location = new Point(35, 0);
        	topbar_battery_image.SizeMode = PictureBoxSizeMode.StretchImage;

        	topbar_battery.Controls.Add(topbar_battery_image);


        	// add all topbar elements
        	topbar.Controls.Add(topbar_battery);
	 		topbar.Controls.Add(topbar_internet);
	 		topbar.Controls.Add(topbar_time);
        	

			// getting full topbar
			full_topbar = complete_topbar.Get();
			full_topbar.Width = this.Width;
			full_topbar.Visible = false;
			this.Controls.Add(full_topbar);
			
			// create switch bar
			bottom_switch_bar = new Panel();
			bottom_switch_bar.Width = this.Width;
			bottom_switch_bar.Height = 30;
			bottom_switch_bar.Location = new Point(0, this.Height-bottom_switch_bar.Height);


			list_bottom_switch_bar = new PictureBox();
			list_bottom_switch_bar.Location = new Point((this.Width/2)-(3*30)+15, 0);
			list_bottom_switch_bar.Size = new Size(30, 30);
			list_bottom_switch_bar.SizeMode = PictureBoxSizeMode.StretchImage;
			list_bottom_switch_bar.Image = theme_manager.get_icon("list-white");

			home_bottom_switch_bar = new PictureBox();
			home_bottom_switch_bar.Size = new Size(30, 30);
			home_bottom_switch_bar.SizeMode = PictureBoxSizeMode.StretchImage;
			home_bottom_switch_bar.Location = new Point((this.Width/2)-(1*30)+15, 0);
			home_bottom_switch_bar.Image = theme_manager.get_icon("home-white");

			back_bottom_switch_bar = new PictureBox();
			back_bottom_switch_bar.Location = new Point((this.Width/2)-(-1*30)+15, 0);
			back_bottom_switch_bar.Size = new Size(30, 30);
			back_bottom_switch_bar.SizeMode = PictureBoxSizeMode.StretchImage;
			back_bottom_switch_bar.Image = theme_manager.get_icon("chevron-white", 90);

			bottom_switch_bar.Controls.Add(list_bottom_switch_bar);
			bottom_switch_bar.Controls.Add(home_bottom_switch_bar);
			bottom_switch_bar.Controls.Add(back_bottom_switch_bar);





			FlowLayoutPanel embeding = new FlowLayoutPanel();
			embeding.Size = new Size(this.Width, this.Height);
			embeding.AutoScroll = false;
			embeding.Location = new Point(0, 0);
			embeding.FlowDirection = FlowDirection.TopDown;
			embeding.WrapContents = false;
			embeding.BackColor = Color.FromArgb(0, 0, 0);
			Interpreter.unloadApps();
			
			home home_embed = new home();
			utils.ShowFormInContainerControl(embeding, home_embed);


	        // add top and bottom bars
	        Controls.Add(splashimage);
			Controls.Add(bottom_switch_bar);
	        Controls.Add(topbar);
	        Controls.Add(embeding);


	        
	        Task.Run(() => Update());
	        
	    }
		public static void expand_topbar()
		{
			full_topbar.Visible = true;
		}
	    private async void Update()
	    {
	    	while(true)
	    	{
	    		// topbar time
	    		DateTime currentDateTime = DateTime.Now;
				string date = currentDateTime.ToString("HH:mm");
				topbar_time.Text = date;

				complete_topbar.update(full_topbar, currentDateTime);

	    		// topbar internet
	    		if(network.isInternetConnected())
	    		{
	    			topbar_internet.Image = theme_manager.get_icon("internet-on-black");
	    		}
	    		else
	    		{
	    			topbar_internet.Image = theme_manager.get_icon("internet-off-black");
	    		}
	    		topbar_battery.Text=battery.getPercentage();
	    		await Task.Delay(2000);
	    	}
	        
	        
	    }

	    private async void splash()
	   	{
	   		theme_manager.PlaySound("boot");
	   		update.checkForUpdates();

	   		int splash_time = Int32.Parse(utils.getSetting("splash_time", utils.getConfigPath()));
		    
		    await Task.Delay(splash_time);

		    splashimage.Visible = false;
	   	}
	}
}