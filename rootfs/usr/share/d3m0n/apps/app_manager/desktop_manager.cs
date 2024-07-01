using System;
using System.IO;
using System.Collections.Generic;
// using System.Windows.Forms;
// using System.Drawing;
using System.Text.RegularExpressions;
using System.Linq;
using display;

namespace d3m0n
{
    class desktop_manager
    {
        public static void sort_apps(Window into)
        {
            string[] apps = Interpreter.getApps();
            Dictionary<string, string> apps_dict = new Dictionary<string, string>();
            foreach(string raw in apps)
            {
                // sets package=raw
                apps_dict.Add(raw.Split("//!//")[6], raw);
            }


            string d3m0n_desktop_conf = utils.getPath()+"/apps/desktop.config";
            if(!File.Exists(d3m0n_desktop_conf))
            {
                string bottom_bar = @"
com.4re5.d3m0n.phone
com.4re5.d3m0n.messages
com.4re5.d3m0n.camera
com.4re5.d3m0n.gallery";
                string file_content="";
                int slotnbr=0;
                foreach(string raw in apps)
                {
                    if(bottom_bar.Contains(raw.Split("//!//")[6]))
                    {}
                    else
                    {
                        slotnbr++;
                        if(slotnbr==1)
                        {
                            file_content=raw.Split("//!//")[6];
                        }
                        else
                        {
                            file_content=file_content+"\r\n"+raw.Split("//!//")[6];
                        }
                    }
                }
                // add null slots
                // max slots = 20
                int null_slots = 20 - file_content.Split("\n").Length - bottom_bar.Split("\n").Length;
                for(int i=0; i <= null_slots; i++)
                {
                    file_content+="\nnull";
                }
                File.WriteAllText(d3m0n_desktop_conf, file_content+bottom_bar);
            }
            Dictionary<string, int> id_by_package = new Dictionary<string, int>();

            // check if new package apears in folders but not in desktop > then add it to desktop
            foreach(string raw in apps)
            {
                string package=raw.Split("//!//")[6];
                if(!File.ReadAllText(d3m0n_desktop_conf).Contains(package))
                {
                    // replace first null line by a package
                    var regex = new Regex(Regex.Escape("null"));
                    var newText = regex.Replace(File.ReadAllText(d3m0n_desktop_conf), package, 1);
                                
                    File.WriteAllText(d3m0n_desktop_conf, newText);

                    // MessageBox.Show("package '"+package+"' added to desktop");
                } 
            }
            




            string[] lines = File.ReadAllLines(d3m0n_desktop_conf);
            int slot_id=0;
            foreach(string line in lines)
            {
                slot_id++;
                // if dictionnary already have same package name
                if(id_by_package.ContainsKey(line))
                {
                    id_by_package[line+"_"+utils.RandomString(3)] = slot_id;
                }
                else
                {
                    id_by_package[line] = slot_id;
                }
            }

            // order packages by id
            var id_by_package_final = from entry in id_by_package orderby entry.Value ascending select entry;


            foreach(var content in id_by_package_final)
            {
                int id = content.Value;
                string package = content.Key.Split("_")[0].TrimStart(' ').TrimEnd(' ');

                
                
                // Console.WriteLine("'"+package+"'");
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