using System;
// using System.Drawing;
// using System.Windows.Forms;
using System.IO;
using display;
using System.Collections.Generic;

namespace d3m0n
{
	public partial class layout
	{
		public static ushort getLayoutColor(string color)
		{
			switch(color)
			{
				case "green":
					return Colors.Green;
					break;
				case "red":
					return Colors.Red;
					break;
        		case "blue":
					return Colors.Blue;
					break;
        		case "yellow":
					return Colors.Yellow;
					break;
        		case "black":
					return Colors.Black;
					break;
        		case "dark_blue":
					return Colors.getRGB565(0, 20, 122);
					break;
        		case "dark_cyan":
					return Colors.getRGB565(0, 82, 130);
					break;
        		case "dark_red":
					return Colors.getRGB565(130, 0, 0);
					break;
       			case "dark_magenta":
					return Colors.getRGB565(126, 0, 130);
					break;
        		case "orange":
					return Colors.getRGB565(255, 132, 0);
					break;
        		case "grey":
					return Colors.getRGB565(82, 82, 82);
					break;
				case "dark_grey":
					return Colors.getRGB565(31, 31, 31);
					break;
        		case "lime":
					return Colors.getRGB565(10, 255, 75);
					break;
       		 	case "cyan":
					return Colors.getRGB565(0, 217, 255);
					break;
        		case "magenta":
					return Colors.getRGB565(255, 0, 255);
					break;
        		case "white":
					return Colors.White;
					break;
				default:
					try
					{
						string[] colors = color.Split(", ");
						int r = Int32.Parse(colors[0]);
						int g = Int32.Parse(colors[1]);
						int b = Int32.Parse(colors[2]);
						return Colors.getRGB565(r, g, b);
					}
					catch(Exception)
					{
						return Colors.Black;
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