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
		public static Control Button(Dictionary<string, string> args)
		{
			Button control = new Button();

			layout temp = new layout();
			Task globalArgs = Task.Run(() => temp.setGlobalArgs(control, args));
            globalArgs.Wait();

			// infos
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
                    control.Image = new Bitmap(args["image"]);
                }
			}



			return control;
		}
	}
}