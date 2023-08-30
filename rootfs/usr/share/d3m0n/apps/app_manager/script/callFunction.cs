using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Collections.Generic;

namespace d3m0n
{
	public partial class script
	{
		public static void callFunction(string command, string[] value)
		{
			string app_path = Interpreter.currentRunningAppPath;

			while(command.StartsWith(' ') || command.EndsWith(' '))
			{
				command=command.TrimStart(' ').TrimEnd(' ');
			}
			
			interraction(command, value);
			system(command, value);

			// call a def
			if(File.Exists(app_path+"/defs/"+command))
			{
				foreach(string line in File.ReadAllText(app_path+"/defs/"+command).Split(";"))
				{
					execute(line+";");
				}	
				
			}
		}
	}
}