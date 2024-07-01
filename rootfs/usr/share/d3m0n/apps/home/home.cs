using System;
// using System.Drawing;
// using System.Windows.Forms;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using display;

namespace d3m0n
{
	public partial class Program
	{
	    public static void draw_home(Window into)
	    {
			// loads apps
			foreach (string file in Directory.EnumerateFiles(utils.getPath(), "*.d3m0n", SearchOption.AllDirectories))
			{
				string raw = Interpreter.loadApp(file);
				// Console.WriteLine("file: "+file);
				// Console.WriteLine("raw: "+raw);
				if(raw.StartsWith("can't load app"))
				{
					continue;
				}
			}
            
            System.Drawing.Image wallpaper;
            try
            {
            	wallpaper = System.Drawing.Image.FromFile(utils.getPath()+"/wallpapers/"+utils.getSetting("wallpaper", utils.getConfigPath())+".png");
            	
				Image bg = new Image();
				bg.Img = wallpaper;
				bg.Location = new DataTypes.Point(0, 0);
				bg.Size = new DataTypes.Size(240, 320);
				bg.Name="wallpaper";
				bg.draw();
        	}
            catch(Exception)
            {
            	utils.logn("[x] wallpaper not found ("+utils.getPath()+"/wallpapers/"+utils.getSetting("wallpaper", utils.getConfigPath())+".png"+")", ConsoleColor.Red);
            }
    		
			int currentPage=1;
			int totalPages=0;


			// try displaying apps
			desktop_manager.sort_apps(into);
	    }
	}
}