using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Threading.Tasks;

namespace d3m0n
{
	class Program
	{
		public static void Main(string[] args)
		{
			Application.EnableVisualStyles();
			Application.Run(new d3m0n());
		}
	}
	public class d3m0n : Form
	{	 
		Label topbar_time;
	    public d3m0n()
	    {
	    	Application.EnableVisualStyles();


	    	// this.WindowState = FormWindowState.Maximized;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            // this.Bounds = Screen.PrimaryScreen.Bounds;

            this.BackColor = Color.FromArgb(0, 0, 0);
            this.Name = "d3m0n_app_launcher";
            this.Font = new Font("Arial", 15);
            this.Text = "d3m0n app launcher";
            this.Size = new Size(480, 640);
            this.StartPosition = FormStartPosition.CenterScreen;
			// this.SuspendLayout();
			// this.FormBorderStyle = FormBorderStyle.FixedDialog;


			FlowLayoutPanel topbar = new FlowLayoutPanel();
			topbar.Size = new Size(this.Width, 25);
			topbar.Anchor = AnchorStyles.Right;  
			topbar.FlowDirection = FlowDirection.LeftToRight;
			topbar.Location = new Point(0, 0);
			topbar.WrapContents = false;
			topbar.BackColor = Color.FromArgb(255, 255, 255);


			topbar_time = new Label();
			topbar_time.Font = new Font("Arial", 10);

			DateTime currentDateTime = DateTime.Now;
			string date = currentDateTime.ToString("HH:mm");
			topbar_time.Text = date;

			topbar.Controls.Add(topbar_time);




			FlowLayoutPanel embeding = new FlowLayoutPanel();
			embeding.Size = new Size(this.Width, this.Height);
			embeding.AutoScroll = false;
			embeding.Location = new Point(0, 0);
			embeding.FlowDirection = FlowDirection.TopDown;
			embeding.WrapContents = false;
			embeding.BackColor = Color.FromArgb(0, 0, 0);

			home home_embed = new home();
			utils.ShowFormInContainerControl(embeding, home_embed);


	        // add top and bottom bars
	        Controls.Add(topbar);
	        Controls.Add(embeding);


	        Task.Run(() => Update());
	        
	    }
	 
	    private async void Update()
	    {
	    	while(true)
	    	{
	    		DateTime currentDateTime = DateTime.Now;
				string date = currentDateTime.ToString("HH:mm");
				topbar_time.Text = date;
	    		await Task.Delay(2);
	    	}
	        // topbar time
	        
	    }
	}
}