using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Collections.Generic;
using System.Diagnostics;

namespace d3m0n
{
	public partial class d3m0n_src
	{
		public static void system(string command, string[] value)
		{
			try
			{
				switch(command)
				{
					case "system.exec":
						string filename = script.getString(value[0].Split(" ")[0]);
						string commandname  = script.getString(value[0].Replace(value[0].Split(" ")[0], ""));
						// MessageBox.Show(filename+" => "+commandname);
						Process process = new Process();
						ProcessStartInfo processStartInfo = new ProcessStartInfo();
						processStartInfo.WindowStyle = ProcessWindowStyle.Hidden;
						processStartInfo.FileName = filename;
						processStartInfo.Arguments = commandname;
						processStartInfo.UseShellExecute = false;
						process.StartInfo = processStartInfo;
						process.Start();
						break;
				}
			}
			catch(Exception e)
			{
				MessageBox.Show(e.ToString());
			}          	
		}
	}
}