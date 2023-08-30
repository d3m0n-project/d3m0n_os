using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace d3m0n
{
	public partial class layout
	{
		public static Control Rect(Dictionary<string, string> args)
		{
			Panel control = new Panel();

			layout temp = new layout();
			Task globalArgs = Task.Run(() => temp.setGlobalArgs(control, args));
            globalArgs.Wait();

			// infos
			if(args.ContainsKey("scroll")) { if(args["scroll"] == "true") { control.AutoScroll = true; } }

			return control;
		}
	}
}