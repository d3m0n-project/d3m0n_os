using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Windows;

namespace d3m0n
{
	public partial class layout
	{
		public static Control Text(Dictionary<string, string> args, Graphics graphics, Label control=null)
		{
			if(control==null)
			{
				control = new Label();
			}
				

			layout temp = new layout();
			Task globalArgs = Task.Run(() => temp.setGlobalArgs(control, args, graphics));
            globalArgs.Wait();

			// infos
			if(args.ContainsKey("text_align")) { getTextAlignement(control, args["text_align"]); }
			if(args.ContainsKey("font_size")) {control.Font = new Font( control.Font.FontFamily, Int32.Parse(args["font_size"]) ); }
			if(args.ContainsKey("content")) {control.Text = args["content"];}

			return control;
		}
	}
}