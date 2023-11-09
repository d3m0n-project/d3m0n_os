using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Net;
using System.Net.Security;


namespace d3m0n
{
	public partial class layout
	{
		public static Control Hscroll(Dictionary<string, string> args, Graphics graphics, Panel control=null)
		{
			if(control==null)
            {
                control = new Panel();
            }
				

			layout temp = new layout();
			Task globalArgs = Task.Run(() => temp.setGlobalArgs(control, args, graphics));
            globalArgs.Wait();

			// infos
            if(args.ContainsKey("bar")) {
                if(args["bar"] == "true")
                {
                    goto Default_bar;
                }
            }
            else
            {
                goto Default_bar;
            }
            Default_bar:
                    HScrollBar hScrollBar1 = new HScrollBar();
                    hScrollBar1.Location = new System.Drawing.Point(0, 0);
                    hScrollBar1.Size = new System.Drawing.Size(control.Width, 50);
                    hScrollBar1.TabIndex = 1;
                    control.Controls.Add(hScrollBar1);
			return control;
		}
	}
}