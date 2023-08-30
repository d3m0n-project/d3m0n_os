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
		public static Control WebView(Dictionary<string, string> args)
		{
			WebBrowser control = new WebBrowser();

			layout temp = new layout();
			Task globalArgs = Task.Run(() => temp.setGlobalArgs(control, args));
            globalArgs.Wait();

			// infos
            try
            {
                // don't work, args contains url but webview don't show link
                if(args.ContainsKey("url")) { control.Url = new Uri(args["url"]); control.Navigate(new Uri(args["url"])); }
            }
            catch(Exception e)
            {
                MessageBox.Show(e.ToString());
                control.Navigate(new Uri("https://google.com"));
            }
			

            

			return control;
		}
        public static void WebView_execute(WebBrowser webview, string command)
        {
            switch(command)
            {
                case "goBack":
                    webview.GoBack();
                    break;
            }
        }
	}
}