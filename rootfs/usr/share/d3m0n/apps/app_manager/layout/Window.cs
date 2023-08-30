using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Net;
using System.Net.Security;


namespace d3m0n
{
	public partial class layout
	{
		public static void Window(Form into, Dictionary<string, string> args)
		{
			// MessageBox.Show("loaded window");
			// infos
			if(args.ContainsKey("start_pos")) { setStartPostition(into, args["start_pos"]); }
            
            layout temp = new layout();
            Task globalArgs = Task.Run(() => temp.setGlobalArgs(into, args));
            globalArgs.Wait();
			

			if(args.ContainsKey("title")) { into.Text = args["title"]; }
			if(args.ContainsKey("topbar")) { Graphics.into.Controls.Find("topbar",true)[0].Visible = false; } 
			
			if(args.ContainsKey("bg_image")) { 
				if(args["bg_image"].StartsWith("http"))
                {
					ServicePointManager.ServerCertificateValidationCallback = new RemoteCertificateValidationCallback(delegate { return true; });
					var request = WebRequest.Create(args["bg_image"]);
					using (var response = request.GetResponse())
					using (var stream = response.GetResponseStream())
					{
						into.BackgroundImage = Bitmap.FromStream(stream);
					}
                }
                else
                {
                    into.BackgroundImage = new Bitmap(args["bg_image"]);
                }
			} 
			


		}
	}
}