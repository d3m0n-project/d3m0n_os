using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Collections.Generic;

namespace d3m0n
{
	public partial class layout
	{
		public static Color getLayoutColor(string color)
		{
			switch(color)
			{
				case "green":
					return Color.Green;
					break;
				case "red":
					return Color.Red;
					break;
        		case "blue":
					return Color.Blue;
					break;
        		case "yellow":
					return Color.Yellow;
					break;
        		case "black":
					return Color.Black;
					break;
        		case "dark_blue":
					return Color.DarkBlue;
					break;
        		case "dark_cyan":
					return Color.DarkCyan;
					break;
        		case "dark_red":
					return Color.DarkRed;
					break;
       			case "dark_magenta":
					return Color.Black;
					break;
        		case "orange":
					return Color.Orange;
					break;
        		case "grey":
					return Color.Gray;
					break;
				case "dark_grey":
					return Color.DarkGray;
					break;
        		case "lime":
					return Color.Lime;
					break;
       		 	case "cyan":
					return Color.Cyan;
					break;
        		case "magenta":
					return Color.Magenta;
					break;
        		case "white":
					return Color.White;
					break;
				default:
					try
					{
						string[] colors = color.Split(", ");
						int r = Int32.Parse(colors[0]);
						int g = Int32.Parse(colors[1]);
						int b = Int32.Parse(colors[2]);
						return Color.FromArgb(r, g, b);
					}
					catch(Exception)
					{
						return Color.Transparent;
					}
					break;
			}
		}
		public static ConsoleColor getConsoleColor(string color)
		{
			switch(color)
			{
				case "green":
					return ConsoleColor.Green;
					break;
				case "red":
					return ConsoleColor.Red;
					break;
        		case "blue":
					return ConsoleColor.Blue;
					break;
        		case "yellow":
					return ConsoleColor.Yellow;
					break;
        		case "black":
					return ConsoleColor.Black;
					break;
        		case "dark_blue":
					return ConsoleColor.DarkBlue;
					break;
        		case "dark_cyan":
					return ConsoleColor.DarkCyan;
					break;
        		case "dark_red":
					return ConsoleColor.DarkRed;
					break;
       			case "dark_magenta":
					return ConsoleColor.Black;
					break;
        		case "orange":
					return ConsoleColor.DarkYellow;
					break;
        		case "magenta":
					return ConsoleColor.Magenta;
					break;
        		case "white":
					return ConsoleColor.White;
					break;
				default:
					return ConsoleColor.White;
					break;
			}
		}
	}
}