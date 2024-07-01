using System;
using display;
using System.IO;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace d3m0n
{
	public partial class layout
	{
		public static Control Switch(Dictionary<string, string> args, Graphics graphics, CheckBox control=null)
		{
			if(control==null) {
				control = new CheckBox();
			}
			
			layout temp = new layout();
			Task globalArgs = Task.Run(() => temp.setGlobalArgs(control, args, graphics));
            globalArgs.Wait();

			// infos
			if(args.ContainsKey("font_size")) {control.FontSize = Int32.Parse(args["font_size"]); }
			if(args.ContainsKey("content")) {control.Text = args["content"];}
            if(args.ContainsKey("checked")) {
                if(args["checked"] == "true")
                {
                    control.Checked = true;
                }
            }


			return control;
		}
	}
}