using System;
using display;
using System.IO;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Net;
using System.Net.Security;


namespace d3m0n
{
	public partial class layout
	{
		public static void Window(Window into, Dictionary<string, string> args, Graphics graphics)
		{
			// infos
			if(args.ContainsKey("start_pos")) { setStartPostition(into, args["start_pos"]); }
            
            // layout temp = new layout();
            // Task globalArgs = Task.Run(() => temp.setGlobalArgs(into, args, graphics));
            // globalArgs.Wait();
			

			if(args.ContainsKey("title")) { into.Title = args["title"]; }
			try
			{
				if(args.ContainsKey("topbar")) { into.Get("topbar").Visible = false; } 
			}
			catch(Exception)
			{}
			
			// try
			// {
			// 	if(args.ContainsKey("bg_image")) { 
			// 		if(args["bg_image"].StartsWith("http"))
			// 		{
			// 			ServicePointManager.ServerCertificateValidationCallback = new RemoteCertificateValidationCallback(delegate { return true; });
			// 			var request = WebRequest.Create(args["bg_image"]);
			// 			using (var response = request.GetResponse())
			// 			using (var stream = response.GetResponseStream())
			// 			{
			// 				into.BackgroundImage = Bitmap.FromStream(stream);
			// 			}
			// 		}
			// 		else
			// 		{
			// 			into.BackgroundImage = new Bitmap(script.getString(args["bg_image"]));
			// 		}
			// 	}
			// }
			// catch(Exception)
			// {}
			 
			


		}
	}
}