using System;
using display;
using System.IO;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace d3m0n
{
	public partial class layout
	{
		public static Control Rect(Dictionary<string, string> args, Graphics graphics, Panel control=null)
		{
			if(control==null) {
				control = new Panel();
			}
				

			layout temp = new layout();
			Task globalArgs = Task.Run(() => temp.setGlobalArgs(control, args, graphics));
            globalArgs.Wait();

			// infos
			// if(args.ContainsKey("scroll")) { if(args["scroll"] == "true") { control.AutoScroll = true; } }

			return control;
		}
	}
}