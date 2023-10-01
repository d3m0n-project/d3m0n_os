using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Collections.Generic;

namespace d3m0n
{
	public partial class layout
	{
		public static Point getPosition(int control_width, int control_height, string pos)
		{
			int window_width  = Graphics.into.Width;
			int window_height = Graphics.into.Height;

			switch(pos)
			{
				case "top":
					return new Point(((window_width/2)-(control_width/2)), 0);
					break;
				case "top_left":
					return new Point(0, 0);
					break;
				case "top_right":
					return new Point((window_width-control_width), 0);
					break;

				case "bottom":
					return new Point(((window_width/2)-(control_width/2)), (window_height-control_height));
					break;
				case "bottom_left":
					return new Point(0, (window_height-control_height));
					break;
				case "bottom_right":
					return new Point((window_width-control_width), (window_height-control_height));
					break;


				case "center":
					return new Point(((window_width/2)-(control_width/2)), ((window_height/2)-(control_height/2)));
					break;
				case "left":
					return new Point(0, ((window_height/2)-(control_height/2)));
					break;
				case "right":
					return new Point(((window_width)-(control_width)), ((window_height/2)-(control_height/2)));
					break;
				default:
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
					break;
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