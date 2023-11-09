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
		public static Control RoundButton(Dictionary<string, string> args, Graphics graphics, RoundedButton control=null)
		{
			if(control==null)
            {
                control = new RoundedButton();
            }
			

			layout temp = new layout();
			Task globalArgs = Task.Run(() => temp.setGlobalArgs(control, args, graphics));
            globalArgs.Wait();

			// infos
			if(args.ContainsKey("font_size")) {control.Font = new Font( control.Font.FontFamily, Int32.Parse(args["font_size"]) ); }
            if(args.ContainsKey("radius")) {control.radius = Int32.Parse(args["radius"]);}
			if(args.ContainsKey("content")) {control.Text = args["content"];}
			if(args.ContainsKey("image")) {
				if(args["image"].StartsWith("http"))
                {
					ServicePointManager.ServerCertificateValidationCallback = new RemoteCertificateValidationCallback(delegate { return true; });
					var request = WebRequest.Create(args["image"]);
					using (var response = request.GetResponse())
					using (var stream = response.GetResponseStream())
					{
						control.Image = Bitmap.FromStream(stream);
					}
                }
                else
                {
                    control.Image = new Bitmap(script.getString(args["image"]));
                }
			}



			return control;
		}
	}
}