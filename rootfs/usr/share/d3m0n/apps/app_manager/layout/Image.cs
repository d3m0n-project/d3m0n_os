using System;
using System.IO;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Net;
using System.Net.Security;
using display;

namespace d3m0n
{
	public partial class layout
	{
		public static Control Image(Dictionary<string, string> args, Graphics graphics, display.Image control=null)
		{
			if(control==null)
			{
				control = new display.Image();
			}
				

			layout temp = new layout();
			Task globalArgs = Task.Run(() => temp.setGlobalArgs(control, args, graphics));
            globalArgs.Wait();

			// infos
			// if(args.ContainsKey("mode")) {
			// 	if(args["mode"] == "stretch") {
			// 		control.SizeMode = PictureBoxSizeMode.StretchImage;
			// 	}
			// 	else if(args["mode"] == "center") {
			// 		control.SizeMode = PictureBoxSizeMode.CenterImage;
			// 	}
			// 	else if(args["mode"] == "zoom") {
			// 		control.SizeMode = PictureBoxSizeMode.Zoom;
			// 	}
			// 	else if(args["mode"] == "auto_size") {
			// 		control.SizeMode = PictureBoxSizeMode.AutoSize;
			// 	}
			// }
            

			if(args.ContainsKey("src")) {
				// display theme icon
				// MessageBox.Show("src");
				if(theme_manager.checkIconExists(args["src"]))
				{
					control.Img = theme_manager.get_icon(args["src"]);
				}
				// display online stored image
                else if(args["src"].StartsWith("http"))
                {
					ServicePointManager.ServerCertificateValidationCallback = new RemoteCertificateValidationCallback(delegate { return true; });
					var request = WebRequest.Create(args["src"]);
					using (var response = request.GetResponse())
					using (var stream = response.GetResponseStream())
					{
						control.Img = System.Drawing.Bitmap.FromStream(stream);
					}
                }
				// display image file
				// can be $ressources/myImage.png for exemple
                else
                {
                    control.Img = System.Drawing.Bitmap.FromFile(script.getString(args["src"]));
                }
            }
			return control;
		}
	}
}