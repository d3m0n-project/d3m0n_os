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
		public static void basics(string command, string[] value)
		{
            switch(command)
            {
                case "sleep":
                    System.Threading.Thread.Sleep(Int32.Parse(value[0]));
                    break;
            }
        }
    }
}