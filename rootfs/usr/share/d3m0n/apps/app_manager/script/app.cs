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
		public static void application(string command, string[] value)
		{
			try
			{
				switch(command)
				{
					default:
						application_layout(command, value);
						break;
					case "app.run":
						if(File.Exists(getString(value[0])))
                        {
                            Interpreter.loadApp(getString(value[0]));
                            utils.logn("[o] Loaded app '"+getString(value[0])+"'", ConsoleColor.Green);
                        }
                        else
                        {
                            utils.logn("[x] App '"+value[0]+"' can't be loaded: don't exists", ConsoleColor.Red);
                        }
						break;
                    case "app.close":
						// Graphics.destroy(value[0]);
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