using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Collections.Generic;
using System.Reflection;

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

			
			// try every type of embeded in functions
			MethodInfo[] mInfos = typeof(d3m0n_src).GetMethods();
            foreach (MethodInfo mInfo in mInfos)
            {
				try
				{
					mInfo.Invoke(mInfo, new object[]{ command, value });
				}
				catch(Exception)
				{}
            }


			// call a d3m0n system definition
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