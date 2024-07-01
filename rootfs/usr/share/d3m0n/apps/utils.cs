using System;
using System.Windows.Forms;
using System.IO;
using System.Drawing;
using System.Linq;

namespace d3m0n
{
   public class utils
   {
      private static Random random = new Random();
      public static int getScreenWidth()
      {
         return Screen.PrimaryScreen.Bounds.Width;
      }
      public static int getScreenHeight()
      {
         return Screen.PrimaryScreen.Bounds.Height;
      }

      public static bool checkArgsLength(string[] args, int len)
      {
         if(len <= args.Length) {
            logn("[x] Not enough args '"+len.ToString()+"' required", ConsoleColor.Red);
         }
         return(len >= args.Length);
      }

      public static string RemoveLast(string text, string character)
      {
          if(text.Length < 1) return text;
          return text.Remove(text.ToString().LastIndexOf(character), character.Length);
      }

      public static void logn(string message, ConsoleColor color)
      {
         Console.ForegroundColor = color;
         Console.WriteLine(message);
         Console.ResetColor();
      }
      public static void log(string message, ConsoleColor color)
      {
         Console.ForegroundColor = color;
         Console.Write(message);
         Console.ResetColor();
      }

      public static string RandomString(int length)
      {
          const string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
          return new string(Enumerable.Repeat(chars, length)
              .Select(s => s[random.Next(s.Length)]).ToArray());
      }
      public static string getPath()
      {
         // release
         // return "/usr/share/d3m0n";
         return "/home/d3m0n/d3m0n-testing";
         // return "/root/d3m0n_os_debian/rootfs/usr/share/d3m0n";
      }
      public static string getConfigPath()
      {
         return getPath()+"/config";
      }
      public static void ShowFormInContainerControl(Control ctl, Form frm)
      {
         // frm.Margin = new System.Windows.Thickness(10, 0, 5, 0);
         // ctl.Padding.Left = 0;
         frm.TopLevel = false;
         frm.WindowState = FormWindowState.Maximized;
         frm.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
         frm.Visible = true;
         frm.Size = new Size(455, (640));
         frm.Location = new Point(0, 25);
         ctl.Controls.Add(frm);
      } 
      public static void changeLine(string newText, string fileName, int line_to_edit)
      {
         string[] arrLine = File.ReadAllLines(fileName);
         arrLine[line_to_edit - 1] = newText;
         File.WriteAllLines(fileName, arrLine);
      }
      public static string getSetting(string name, string path)
      {
         string to_return="not found";

         string[] file  = File.ReadAllText(path).Split("\r\n");
         foreach(string line in file)
         {
            string temp_name = line.Split(": ")[0];
            string temp_value = line.Split(": ")[1];
            if(temp_name == name)
            {
               to_return = temp_value;
               break;
            }
         }
         return to_return;
      } 

      public static void DeleteDirectory(string target_dir)
      {
          string[] files = Directory.GetFiles(target_dir);
          string[] dirs = Directory.GetDirectories(target_dir);

          foreach (string file in files)
          {
              File.SetAttributes(file, FileAttributes.Normal);
              File.Delete(file);
          }

          foreach (string dir in dirs)
          {
              DeleteDirectory(dir);
          }

          Directory.Delete(target_dir, false);
      }
   }
}

