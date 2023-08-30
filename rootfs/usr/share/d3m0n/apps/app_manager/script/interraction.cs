using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Collections.Generic;

namespace d3m0n
{
	public partial class script
	{
		public static void interraction(string command, string[] value)
		{
			try
			{
				// Console.WriteLine("("+command+") 0: "+value[0]);
				// Console.WriteLine("("+command+") 1: "+value[1]);
				switch(command)
				{
					case "alert":
						MessageBox.Show(getString(value[0]));
						break;
					case "logn":
						utils.logn(getString(value[0]), layout.getConsoleColor(getString(value[1])));
						break;
					case "log":
						utils.log(getString(value[0]), layout.getConsoleColor(getString(value[1])));
						break;
				}
			}
			catch(Exception)
			{}
			
			// alert (text)
			// log (text)
			// use > path
			// keyboard.show()
			// keyboard.hide()
            	
		}
	}
}