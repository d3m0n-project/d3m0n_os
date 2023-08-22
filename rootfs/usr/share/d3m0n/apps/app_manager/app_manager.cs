using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;

namespace d3m0n
{
	public class app_manager
	{
		public static string[] hidden_apps = {};
		public static string[] getAllApps()
		{
			string[] to_return = {};
			// system apps
			foreach (string file in Directory.EnumerateFiles("../", "app", SearchOption.AllDirectories))
			{
				string name = utils.getSetting("name", file);
				string package = utils.getSetting("package", file);
			    string icon = utils.getSetting("icon", file);
			    string executable = utils.getSetting("executable", file);
			    string perms = utils.getSetting("perms", file);
				string start_path = utils.getSetting("start_path", file);
				string category = utils.getSetting("category", file);
				
				string hidden = utils.getSetting("hidden", file);
				if(hidden == "true")
				{
					Array.Resize(ref hidden_apps, hidden_apps.Length + 1);
					hidden_apps[hidden_apps.Length - 1] = name+"//!//"+icon+"//!//"+executable+"//!//"+perms+"//!//"+start_path+"//!//"+category+"//!//"+package;
					Console.WriteLine("FOUND(hidden): "+name);
				}
				else
				{
					Console.WriteLine("FOUND: "+name);
					Array.Resize(ref to_return, to_return.Length + 1);
					to_return[to_return.Length - 1] = name+"//!//"+icon+"//!//"+executable+"//!//"+perms+"//!//"+start_path+"//!//"+category+"//!//"+package;
				}
			}
			return to_return;
		}
		public static string[] getHiddenApps()
		{
			return hidden_apps;
		}
	}
	
}