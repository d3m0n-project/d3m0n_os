using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Collections.Generic;

namespace d3m0n
{
	public partial class layout
	{
		public static DockStyle getDockPosition(string pos)
		{
			switch(pos)
			{
				case "top":
					return DockStyle.Top;
					break;
				case "bottom":
					return DockStyle.Bottom;
					break;
				case "left":
					return DockStyle.Left;
					break;
				case "right":
					return DockStyle.Right;
					break;
				case "fill":
					return DockStyle.Fill;
					break;
				default:
					return DockStyle.None;
					break;
			}
		}
		public static Point getPosition(string pos)
		{
			try
			{
				string[] position = pos.Split(",");
				int x = Int32.Parse(position[0].Replace(" ", ""));
				int y = Int32.Parse(position[1].Replace(" ", ""));

				Console.WriteLine("x: "+x.ToString()+" y: "+y.ToString());

				return new Point(x, y);
			}
			catch(Exception e)
			{
				// MessageBox.Show(e.ToString());
				return new Point(0, 0);
			}
		}
		public static void setStartPostition(Form form, string pos)
		{
			switch(pos)
			{
				case "center":
					form.StartPosition = FormStartPosition.CenterScreen;
					break;
				case "center_parent": 
					form.StartPosition = FormStartPosition.CenterParent;
					break;
				case "top":
					form.StartPosition = FormStartPosition.Manual;
				    foreach (var scrn in Screen.AllScreens)
				    {
				        if (scrn.Bounds.Contains(form.Location))
				        {
				            form.Location = new Point(scrn.Bounds.Right - form.Width, scrn.Bounds.Top);
				            return;
				        }
				    }
				    break;
				case "bottom":
					form.StartPosition = FormStartPosition.Manual;
				    foreach (var scrn in Screen.AllScreens)
				    {
				        if (scrn.Bounds.Contains(form.Location))
				        {
				            form.Location = new Point(scrn.Bounds.Right - form.Width, scrn.Bounds.Bottom - form.Height);
				            return;
				        }
				    }
				    break;
			}
		}
	}
}