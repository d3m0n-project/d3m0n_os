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
		public static void radio(string command, string[] value)
		{
			try
			{
				switch(command)
				{
					case "module.wireless.radio.hijack":
                        if(!utils.checkArgsLength(value, 4)) { break; }
                        
                        float freq = Int64.Parse(script.getString(value[0]));
                        string soundFile = script.getString(value[1]);
                        string radioName = script.getString(value[2]);
                        string radioText = script.getString(value[3]);

                        d3m0n_modules.wireless.radio_hijack(freq, soundFile, radioName, radioText);

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