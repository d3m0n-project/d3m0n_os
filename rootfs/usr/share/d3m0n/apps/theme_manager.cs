using System;
using System.IO;
using System.Windows.Forms;
using System.Drawing;
using System.Media;
using System.Diagnostics;

namespace d3m0n
{
	class theme_manager
	{
		public static bool checkIconExists(string name)
		{
			string theme_path = utils.getPath()+"/themes/"+utils.getSetting("theme", utils.getConfigPath())+"/icons";
			return(File.Exists(theme_path+"/"+name+".png"));
		}
		public static Image get_icon(string name, int rotate=0)
		{
			Image icon;
			byte[] bytes = Convert.FromBase64String("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");
			using (MemoryStream ms = new MemoryStream(bytes))
			{
			    icon = Image.FromStream(ms);
			}

			string theme_path = utils.getPath()+"/themes/"+utils.getSetting("theme", utils.getConfigPath())+"/icons";
			// Console.WriteLine("theme_path: "+theme_path);
			// Console.WriteLine("'"+theme_path+"/"+name+".png'");
			if(File.Exists(theme_path+"/"+name+".png"))
			{
				// Console.WriteLine("icon exists: "+theme_path+"/"+name+".png");
				icon = new Bitmap(theme_path+"/"+name+".png");
			}
			else
			{
				utils.logn("icon: "+name+".png don't exists", ConsoleColor.Red);
				// MessageBox.Show("icon: "+name+".png don't exists");
			}

			// rotate icon
			switch (rotate)
			{
				case 90:
					icon.RotateFlip(RotateFlipType.Rotate90FlipNone);
					break;
				case 180:
					icon.RotateFlip(RotateFlipType.Rotate180FlipNone);
					break;
				case 270:
					icon.RotateFlip(RotateFlipType.Rotate270FlipNone);
					break;
			}
			
			return icon;
			
		}
		public static void PlaySound(string name)
		{
			string theme_path = utils.getPath()+"/themes/"+utils.getSetting("theme", utils.getConfigPath())+"/sounds";

			try
			{
				if(File.Exists(theme_path+"/"+name+".mp3"))
				{
					Process process = new Process();
					ProcessStartInfo processStartInfo = new ProcessStartInfo();
					processStartInfo.WindowStyle = ProcessWindowStyle.Hidden;
					processStartInfo.FileName = "paplay";
					processStartInfo.Arguments = theme_path+"/"+name+".mp3";
					processStartInfo.UseShellExecute = false;
					process.StartInfo = processStartInfo;
					process.Start();
				}
				else
				{
					MessageBox.Show("sound '"+name+"' is not found");
				}
			}
			catch(Exception)
			{
				utils.logn("[x] Could not find command 'paplay' >> can't play sound file", ConsoleColor.Red);
			}
		}
	}
}