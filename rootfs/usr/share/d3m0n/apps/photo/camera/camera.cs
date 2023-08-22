using System;
using System.Windows.Forms;
using System.Drawing;

class Program
{
	public static void Main(string[] args)
	{
		Application.EnableVisualStyles();
		Application.Run(new test());
	}
}
public class test : Form
{	 
	public test()
	{
	    Application.EnableVisualStyles();


	    // this.WindowState = FormWindowState.Maximized;
	    this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
	    // this.Bounds = Screen.PrimaryScreen.Bounds;

	    this.BackColor = Color.LimeGreen;
		this.TransparencyKey = Color.LimeGreen;
	    this.Name = "d3m0n_app_launcher";
	    this.Font = new Font("Arial", 15);
	    this.Text = "d3m0n app launcher";
	    this.Size = new Size(480, (640 - 25));
	    // this.StartPosition = FormStartPosition.CenterScreen;
	    this.Location = new Point(100, 100);
		// this.SuspendLayout();
		// this.FormBorderStyle = FormBorderStyle.FixedDialog;


		Label topbar_time = new Label();
		topbar_time.Font = new Font("Arial", 15);
		topbar_time.ForeColor = Color.White;
		topbar_time.Text = "test";
		topbar_time.Size = new Size(100, 100);
		topbar_time.Location = new Point(0, 0);

		Controls.Add(topbar_time);  
	}
}