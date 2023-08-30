using System;
using System.Net;
using System.IO;

namespace d3m0n
{
	class network
	{
		public static bool isInternetConnected()
		{
			try
		    {
		        var request = (HttpWebRequest)WebRequest.Create("https://google.com");
		        request.KeepAlive = false;
		        request.Timeout = 1;
		        using (var response = (HttpWebResponse)request.GetResponse())
		            return true;
		    }
		    catch
		    {
		        return false;
		    }
		}
	}
}