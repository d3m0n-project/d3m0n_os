using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Collections.Generic;
using System.Diagnostics;

namespace d3m0n
{
	public partial class script
	{
		public static void application_layout(string command, string[] value)
		{
			try
			{
				switch(command)
				{
					case "app.layout.display":
						if(File.Exists(getString(value[0])))
                        {
                            Interpreter.loadLayout(getString(value[0]));
                            utils.logn("[o] Loaded '"+getString(value[0])+"'", ConsoleColor.Green);
                        }
                        else
                        {
                            utils.logn("[x] File '"+value[0]+"' can't be loaded: don't exists", ConsoleColor.Red);
                        }
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