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
						if(File.Exists(script.getString(value[0])) || Interpreter.get_path_from_name.ContainsKey(script.getString(value[0])))
                        {
                            Interpreter.runApp(script.getString(value[0]));
                            utils.logn("[o] Started running app '"+script.getString(value[0])+"'", ConsoleColor.Green);
                        }
                        else
                        {
                            utils.logn("[x] App '"+script.getString(value[0])+"' can't be loaded: don't exists", ConsoleColor.Red);
                        }
						break;
                    case "app.close":
						utils.logn("[o] App '"+script.getString(value[0])+"' is closed", ConsoleColor.Green);
						// Graphics.destroy(script.getString(value[0]));
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