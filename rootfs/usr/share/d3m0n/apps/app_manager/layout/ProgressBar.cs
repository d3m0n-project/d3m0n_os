using System;
using display;
using System.IO;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace d3m0n
{
	public partial class layout
	{
		public static Control ProgressBar(Dictionary<string, string> args, Graphics graphics, ProgressBar control=null)
		{
			if(control==null)
				control = new ProgressBar();

			layout temp = new layout();
			Task globalArgs = Task.Run(() => temp.setGlobalArgs(control, args, graphics));
            globalArgs.Wait();

			// infos
            try
            {
                if(args.ContainsKey("value")) {control.Percentage = Int32.Parse(args["value"]);}
                if(args.ContainsKey("min")) {control.Min = Int32.Parse(args["min"]);}
                if(args.ContainsKey("max")) {control.Max = Int32.Parse(args["max"]);}
            }
			catch(Exception)
            {
                utils.logn("[x] value isn't a int", ConsoleColor.Red);
            }

			return control;
		}
	}
}