using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Net;
using System.Net.Security;
// using Mono.WebBrowser;


namespace d3m0n
{
	public partial class layout
	{
		public static Control RawHtml(Dictionary<string, string> args)
		{
			// IWebBrowser control = Mono.WebBrowser.Manager.GetNewInstance();

            // layout temp = new layout();
			// Task globalArgs = Task.Run(() => temp.setGlobalArgs(control as Control, args));
            // globalArgs.Wait();
			

			// if(args.ContainsKey("value")) {
			// 	string finalvalue = script.getString(args["value"]);

			// 	if(File.Exists(finalvalue))
			// 	{
			// 		MessageBox.Show("file exists");
			// 		MessageBox.Show(File.ReadAllText(finalvalue));
			// 		control.Navigation.Go( "https://google.com" );
			// 	}
			// 	else
			// 	{
			// 		// control.DocumentText =  WebUtility.HtmlEncode(finalvalue);
			// 	}
			// 	MessageBox.Show(args["value"]);
			// 	MessageBox.Show(finalvalue);
			// 	// MessageBox.Show(control.DocumentText);
				
			// }
			

            // return control as Control;
			return new Label();
		}
	}
}