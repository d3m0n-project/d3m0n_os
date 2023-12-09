using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Collections.Generic;
using System.Windows;

namespace d3m0n
{
	public partial class layout
	{
		public static void getTextAlignement(dynamic control, string align)
		{
			// get control type
			if(control.GetType() == typeof(Label)) { control = control as Label; }
			if(control.GetType() == typeof(RadioButton)) { control = control as RadioButton; }
			if(control.GetType() == typeof(Button)) { control = control as Button; }
			if(control.GetType() == typeof(CheckBox)) { control = control as CheckBox; }
			if(control.GetType() == typeof(TextBox)) { control = control as TextBox; }

			switch(align)
			{
				case "top":
					control.TextAlign = ContentAlignment.TopCenter;
					break;
				case "top_left":
					control.TextAlign = ContentAlignment.TopLeft;
					break;
				case "top_right":
					control.TextAlign = ContentAlignment.TopRight;
					break;
				case "bottom":
					control.TextAlign = ContentAlignment.BottomCenter;
					break;
				case "bottom_left":
					control.TextAlign = ContentAlignment.BottomLeft;
					break;
				case "bottom_right":
					control.TextAlign = ContentAlignment.BottomLeft;
					break;
				case "left":
					control.TextAlign = ContentAlignment.MiddleLeft;
					break;
				case "right":
					control.TextAlign = ContentAlignment.MiddleRight;
					break;
				case "center":
					control.TextAlign = ContentAlignment.MiddleCenter;
					break;
			}
			
		}
		public static System.Drawing.Point getPosition(int control_width, int control_height, string pos, Graphics graphics)
		{
			int window_width  = graphics.into.Width;
			int window_height = graphics.into.Height;

			switch(pos)
			{
				case "top":
					return new System.Drawing.Point(((window_width/2)-(control_width/2)), 0);
					break;
				case "top_left":
					return new System.Drawing.Point(0, 0);
					break;
				case "top_right":
					return new System.Drawing.Point((window_width-control_width), 0);
					break;

				case "bottom":
					return new System.Drawing.Point(((window_width/2)-(control_width/2)), (window_height-control_height));
					break;
				case "bottom_left":
					return new System.Drawing.Point(0, (window_height-control_height));
					break;
				case "bottom_right":
					return new System.Drawing.Point((window_width-control_width), (window_height-control_height));
					break;


				case "center":
					return new System.Drawing.Point(((window_width/2)-(control_width/2)), ((window_height/2)-(control_height/2)));
					break;
				case "left":
					return new System.Drawing.Point(0, ((window_height/2)-(control_height/2)));
					break;
				case "right":
					return new System.Drawing.Point(((window_width)-(control_width)), ((window_height/2)-(control_height/2)));
					break;
				default:
					try
					{
						string[] position = pos.Split(",");
						int x = Int32.Parse(position[0].Replace(" ", ""));
						int y = Int32.Parse(position[1].Replace(" ", ""));

						Console.WriteLine("x: "+x.ToString()+" y: "+y.ToString());

						return new System.Drawing.Point(x, y);
					}
					catch(Exception e)
					{
						// MessageBox.Show(e.ToString());
						return new System.Drawing.Point(0, 0);
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
				            form.Location = new System.Drawing.Point(scrn.Bounds.Right - form.Width, scrn.Bounds.Top);
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
				            form.Location = new System.Drawing.Point(scrn.Bounds.Right - form.Width, scrn.Bounds.Bottom - form.Height);
				            return;
				        }
				    }
				    break;
			}
		}
	}
}