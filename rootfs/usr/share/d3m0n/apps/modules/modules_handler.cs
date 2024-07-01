using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Collections.Generic;

namespace d3m0n
{
	public partial class script
	{
        public static void modules_handler(string command, string[] value)
        {
            if(command.StartsWith("module.gsm."))
            {
                modules.gsm.parse(command.Replace("module.gsm.", ""), value);
            }
            else if(command.StartsWith("module.usb."))
            {
                modules.usb.parse(command.Replace("module.usb.", ""), value);
            }
        }
    }
}