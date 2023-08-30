using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Collections.Generic;
using System.Net;

namespace d3m0n
{
	public class update
	{
		public static float getLastVersion()
		{
			var url = "https://raw.githubusercontent.com/d3m0n-project/d3m0n_os/main/version.txt";

			var httpRequest = (HttpWebRequest)WebRequest.Create(url);


			var httpResponse = (HttpWebResponse)httpRequest.GetResponse();

			if(httpResponse.StatusCode.ToString() == "200")
			{
				using (var streamReader = new StreamReader(httpResponse.GetResponseStream()))
				{
				   return Int64.Parse(streamReader.ReadToEnd());
				}
			}
			else
			{
				return (float)0.0;
			}
		}
		public static float getCurrentVersion()
		{
			string version_path = utils.getPath()+"/version.txt";
			if(File.Exists(version_path))
			{
				return Int64.Parse(File.ReadAllText(version_path));
			}
			else
			{
				return (float)0.0;
			}
		}
		public static void checkForUpdates()
		{
			try
			{
				ServicePointManager.Expect100Continue = true; 
				ServicePointManager.SecurityProtocol = (SecurityProtocolType)(0xc00);
				if(getLastVersion() > getCurrentVersion())
				{
					utils.logn("[x] you're not up to date ("+getCurrentVersion().ToString()+" < "+getLastVersion().ToString()+" )", ConsoleColor.Red);
					if(network.isInternetConnected())
					{
						// download last
					}
					else
					{
						utils.logn("[x] can't update (no connection)", ConsoleColor.Red);
					}
				}
				else
				{
					utils.logn("[o] you're up to date ("+getCurrentVersion().ToString()+")", ConsoleColor.Green);
				}
			}
			catch(Exception)
			{
				utils.logn("[x] can't check for update", ConsoleColor.Red);
			}
		}
	}
}