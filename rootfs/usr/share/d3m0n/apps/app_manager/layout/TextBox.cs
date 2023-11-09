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
		public static Control TextBox(Dictionary<string, string> args, Graphics graphics, TextBox control=null)
		{
			if(control==null) {
				control = new TextBox();
			}
				

			layout temp = new layout();
			Task globalArgs = Task.Run(() => temp.setGlobalArgs(control, args, graphics));
            globalArgs.Wait();

			// infos
			if(args.ContainsKey("font_size")) {control.Font = new Font( control.Font.FontFamily, Int32.Parse(args["font_size"]) ); }
			if(args.ContainsKey("content")) {control.Text = args["content"];}
            if(args.ContainsKey("type")) {  
                if(args["type"] == "password")
                {
                    control.PasswordChar = '*';
                }
            }

			return control;
		}
	}
}