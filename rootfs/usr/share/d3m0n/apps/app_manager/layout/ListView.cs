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
		public static Control ListView(Dictionary<string, string> args)
		{
			ListView control = new ListView();

			layout temp = new layout();
			Task globalArgs = Task.Run(() => temp.setGlobalArgs(control, args));
            globalArgs.Wait();

			return control;
		}
	}
}