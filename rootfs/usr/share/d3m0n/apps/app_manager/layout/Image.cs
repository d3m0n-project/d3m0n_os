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
		public static Control Image(Dictionary<string, string> args, PictureBox control=null)
		{
			if(control==null)
			{
				control = new PictureBox();
			}
				

			layout temp = new layout();
			Task globalArgs = Task.Run(() => temp.setGlobalArgs(control, args));
            globalArgs.Wait();

			// infos
			if(args.ContainsKey("mode")) {
				if(args["mode"] == "stretch") {
					control.SizeMode = PictureBoxSizeMode.StretchImage;
				}
				else if(args["mode"] == "center") {
					control.SizeMode = PictureBoxSizeMode.CenterImage;
				}
				else if(args["mode"] == "zoom") {
					control.SizeMode = PictureBoxSizeMode.Zoom;
				}
				else if(args["mode"] == "auto_size") {
					control.SizeMode = PictureBoxSizeMode.AutoSize;
				}
			}
            

			if(args.ContainsKey("src")) {
				// display theme icon
				// MessageBox.Show("src");
				if(theme_manager.checkIconExists(args["src"]))
				{
					control.Image = theme_manager.get_icon(args["src"]);
				}
				// display online stored image
                else if(args["src"].StartsWith("http"))
                {
					ServicePointManager.ServerCertificateValidationCallback = new RemoteCertificateValidationCallback(delegate { return true; });
					var request = WebRequest.Create(args["src"]);
					using (var response = request.GetResponse())
					using (var stream = response.GetResponseStream())
					{
						control.Image = Bitmap.FromStream(stream);
					}
                }
				// display image file
				// can be $ressources/myImage.png for exemple
                else
                {
                    control.ImageLocation = script.getString(args["src"]);
                }
            }
			return control;
		}
	}
}