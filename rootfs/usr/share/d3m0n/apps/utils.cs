using System;
using System.Windows.Forms;
using System.IO;
using System.Drawing;

namespace d3m0n
{
   public class utils
   {
      public static string getPath()
      {
         // release
         // return "/usr/share/d3m0n"
         return "/root/d3m0n_os_debian/rootfs/usr/share/d3m0n";
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
         frm.Size = new Size(480, (640));
         ctl.Controls.Add(frm);
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
   }
}

