using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Net;
using System.Net.Security;
using Microsoft.Web.WebView2.Core;
using Microsoft.Web.WebView2.WinForms;



namespace d3m0n
{
	public partial class layout
	{
		public static Control WebView(Dictionary<string, string> args, WebView2 control=null)
		{

			if(control==null)
            {
                control = new WebView2();
            }
				
           

			layout temp = new layout();
			Task globalArgs = Task.Run(() => temp.setGlobalArgs(control, args));
            globalArgs.Wait();

            if (control != null && control.CoreWebView2 != null)
            {
                control.CoreWebView2.Navigate("https://google.com");
            }

			// infos
            // try
            // {
            //     // don't work, args contains url but webview don't show link
            //     if(args.ContainsKey("url")) { control.Url = new Uri(args["url"]); control.Navigate(new Uri(args["url"])); }
            // }
            // catch(Exception e)
            // {
            //     MessageBox.Show(e.ToString());
            //     control.Navigate(new Uri("https://google.com"));
            // }
			

            

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