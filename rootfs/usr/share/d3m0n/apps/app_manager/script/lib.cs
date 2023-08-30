using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Collections.Generic;

namespace d3m0n
{
	public partial class script
	{
		public static void importLib(string path)
		{
			// MessageBox.Show(path);
			while(path.StartsWith(' ') || path.EndsWith(' '))
			{
				path=path.TrimStart(' ').TrimEnd(' ');
			}
			if(File.Exists(path))
			{
				foreach(string line in File.ReadAllLines(path))
				{
					execute(line);
				}
			}
			else
			{
				utils.logn("[x] lib file '"+path+"' don't exists", ConsoleColor.Red);
			}
		}
	}
}