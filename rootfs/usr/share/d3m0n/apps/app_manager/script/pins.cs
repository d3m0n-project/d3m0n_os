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
		public static void pin(string command, string[] value)
		{
			try
			{
				switch(command)
				{
					case "pin.setMode":
                        if(value[1] == "low")
                        {
                            pin_manager.setPinMode(Int32.Parse(value[0]), true);
                        }
                        else if(value[1] == "high")
                        {
                            pin_manager.setPinMode(Int32.Parse(value[0]), false);
                        }
						break;
                    case "pin.setValue":
                        if(value[1] == "low")
                        {
                            pin_manager.setPinValue(Int32.Parse(value[0]), 0);
                        }
                        else if(value[1] == "high")
                        {
                            pin_manager.setPinValue(Int32.Parse(value[0]), 1);
                        }
						break;
                    case "pin.getValue":
                        MessageBox.Show("pin"+value[0]+" => "+pin_manager.getPinValue(Int32.Parse(value[0])).ToString());
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