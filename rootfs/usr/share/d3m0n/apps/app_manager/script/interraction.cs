using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Collections.Generic;
using d3m0n;

namespace d3m0n
{
	public partial class d3m0n_src
	{
		public static void interraction(string command, string[] value)
		{
			try
			{
				switch(command)
				{
					case "alert":
						MessageBox.Show(script.getString(value[0]));
						break;
					case "console.logn":
						utils.logn(script.getString(value[0]), layout.getConsoleColor(script.getString(value[1])));
						break;
					case "console.log":
						utils.log(script.getString(value[0]), layout.getConsoleColor(script.getString(value[1])));
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