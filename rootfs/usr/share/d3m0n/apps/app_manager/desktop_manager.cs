using System;
using System.IO;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Drawing;
using System.Linq;

namespace d3m0n
{
    class desktop_manager
    {
        public static void sort_apps(Form into)
        {
            string[] apps = Interpreter.getApps();
            Dictionary<string, string> apps_dict = new Dictionary<string, string>();
            foreach(string raw in apps)
            {
                apps_dict.Add(raw.Split("//!//")[6], raw);
            }


            string d3m0n_desktop_conf = utils.getPath()+"/apps/desktop.config";
            if(!File.Exists(d3m0n_desktop_conf))
            {
                string file_content="";
                int slotnbr=0;
                foreach(string raw in apps)
                {
                    slotnbr++;
                    if(slotnbr==1)
                    {
                        file_content=raw.Split("//!//")[6]+": "+slotnbr.ToString();
                    }
                    else
                    {
                        file_content=file_content+"\r\n"+raw.Split("//!//")[6]+": "+slotnbr.ToString();
                    }
                }
                File.WriteAllText(d3m0n_desktop_conf, file_content);
            }
            Dictionary<string, int> id_by_package = new Dictionary<string, int>();


            string[] lines = File.ReadAllLines(d3m0n_desktop_conf);
            foreach(string line in lines)
            {
                try
                {
                    string package_name = line.Split(": ")[0];
                    string slot_id = line.Split(": ")[1];

                    id_by_package[package_name] = Int32.Parse(slot_id);
                }
                catch(Exception)
                {}
            }

            var id_by_package_final = from entry in id_by_package orderby entry.Value ascending select entry;


            foreach(var content in id_by_package_final)
            {
                int id = content.Value;
                string package = content.Key;
                
                if(package == "null")
                {
                    d3m0nApplication.name = "empty";
                    d3m0nApplication.package = "empty";
                    d3m0nApplication.temp_path = "/";
                    d3m0nApplication.icon = "null";
                    d3m0nApplication.perms = "none";
                    d3m0nApplication.start_path = "/";
                    d3m0nApplication.category = "";
                    d3m0nApplication.empty = true;
                    d3m0nApplication.folder = false;
                }
                else if(package.Split(": ")[0].TrimStart(' ').TrimEnd(' ').EndsWith("/"))
                {
                    d3m0nApplication.name = "folder!";
                    d3m0nApplication.package = "empty";
                    d3m0nApplication.temp_path = "/";
                    d3m0nApplication.icon = "folder";
                    d3m0nApplication.perms = "none";
                    d3m0nApplication.start_path = "/";
                    d3m0nApplication.category = "folder";
                    d3m0nApplication.empty = false;
                    d3m0nApplication.folder = true;
                }
                else if(apps_dict.ContainsKey(package))
                {
                    d3m0nApplication.name = apps_dict[package].Split("//!//")[0];
                    d3m0nApplication.package = apps_dict[package].Split("//!//")[6];
                    d3m0nApplication.temp_path = apps_dict[package].Split("//!//")[2];
                    d3m0nApplication.icon = apps_dict[package].Split("//!//")[1];
                    d3m0nApplication.perms = apps_dict[package].Split("//!//")[3];
                    d3m0nApplication.start_path = apps_dict[package].Split("//!//")[4];
                    d3m0nApplication.category = apps_dict[package].Split("//!//")[5];
                    d3m0nApplication.empty = false;
                    d3m0nApplication.folder = false;
                }
                
               d3m0nApplication.display(into);
                
            }
            
        }
    }
}