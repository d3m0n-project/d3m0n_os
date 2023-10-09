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
				switch(command)
				{
					case "alert":
						MessageBox.Show(getString(value[0]));
						break;
					case "console.logn":
						utils.logn(getString(value[0]), layout.getConsoleColor(getString(value[1])));
						break;
					case "console.log":
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