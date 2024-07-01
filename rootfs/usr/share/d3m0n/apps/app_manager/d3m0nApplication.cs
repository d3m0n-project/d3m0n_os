using System;
using System.IO;
// using System.Windows.Forms;
// using System.Drawing;
using display;
using System.Threading.Tasks;
using System.Collections.Generic;
using System.Timers;

namespace d3m0n
{
    public class d3m0nApplication
    {
        private static int column_nbr = 1;
        private static int line_nbr = 1;
        private static int page_number = 1;

        public static string name { get; set;}
        public static string package {get; set;}
        public static string temp_path { get; set;}
        public static string icon { get; set;}
        public static string perms { get; set;}
        public static string start_path { get; set;}
        public static string category { get; set;}
        public static bool empty { get; set;}
        public static bool folder { get; set;}

        public static void display(Window into)
        {
            if(column_nbr > 4)
            {
                column_nbr=1;
                line_nbr++;
                
                if(line_nbr > 5)
                {
                    line_nbr=1;
                    page_number++;
                }
            }
            
            if(empty)
            {
                column_nbr++;
                return;
            }



            // 5px + 50px*4 + 5px   width
            // 11px + 50px*4 + 10px   height
            Panel app = new Panel();
            app.Name = name;
            app.Size = new DataTypes.Size(50, 60);
            app.Location = new DataTypes.Point((5*2*(column_nbr-1) + 50*(column_nbr-1)), 20+(line_nbr-1)*60);
            into.Add(app);

            // sets app path
            Label app_path = new Label();
            app_path.Text = temp_path;
            app_path.Visible = false;
            app_path.Name = "temp_path";
            app.Add(app_path);

            // sets package
            Label app_package = new Label();
            app_package.Text = package;
            app_package.Visible = false;
            app_package.Name = "app_package";
            app.Add(app_package);

            // define if that's a folder
            CheckBox isFolder = new CheckBox();
            isFolder.Visible = false;
            isFolder.Checked = folder;
            app.Add(isFolder);




            // application icon
            Image app_icon = new Image();
            app_icon.Name = name+"_app_icon";
            app_icon.Size = new DataTypes.Size(40, 40);
            app_icon.Location = new DataTypes.Point(app.Location.X+5, app.Location.Y+5);
            if(File.Exists(icon))
            {
                System.Drawing.Image image = System.Drawing.Image.FromFile(icon);
                app_icon.Img = image;
            }
            else
            {
                app_icon.Img = theme_manager.get_icon(icon);
            }
            app_icon.draw();
            app.Add(app_icon);

            // actions for drag and drop
            app.Click += new ClickEventHandler(launch);
            // app_icon.MouseDown += (sender, e) => {  Task.Run(() => mouseDown(sender, e, into));  };
            // app_icon.MouseMove += (sender, e) => {  Task.Run(() => mouseMove(sender, e, into));  };
            // app_icon.MouseUp += (sender, e) => {  Task.Run(() => mouseUp(sender, e, into));  };


            // application name
            Label app_name = new Label();
            app_name.Name = name+"_app_name";
            app_name.FontSize = 7;
            app_name.Size = new DataTypes.Size(50, 10);
            app_name.Location = new DataTypes.Point(app.Location.X+0, app.Location.Y+45);
            app_name.Center = true;
            app_name.Text = name;
            app_name.ForeColor = Colors.White;
            app_name.draw();
            app.Add(app_name);



            
            column_nbr++;
        }

        private static async void openFolder(object sender, EventArgs e, Window into)
        {           
            Console.WriteLine("folder opened"); 
            // MessageBox.Show("a");
            // Show the folder customview
            // Graphics.displayCustomView(d3m0nViews.FolderView(""));
        }
        private static DataTypes.Point MouseDownLocation;
        private static bool HasMouseMoved = false;
        private static async void mouseMove(object sender, EventArgs e, Window into)
        {
            // if (e.Button == System.Windows.Forms.MouseButtons.Left)
            // {
            //     HasMouseMoved = true;

            //     Control caller = sender as Control;
                
            //     caller.Parent.Left = e.X + caller.Parent.Left - MouseDownLocation.X;
            //     caller.Parent.Top = e.Y + caller.Parent.Top - MouseDownLocation.Y;
            // }
        }
        private static async void mouseUp(object sender, EventArgs e, Window into)
        {
            // if (e.Button == System.Windows.Forms.MouseButtons.Left)
            // {
            //     Control caller = sender as Control;
            //     string current_package = "";
                
            //     try
            //     {
            //         // getting current app package
            //         current_package=caller.Parent.Controls.Find("app_package",true)[0].Text;
            //     }
            //     catch(Exception)
            //     {}

            //     int slot_nbr_lin=0;
            //     int slot_nbr_col=0;


            //     bool alreadyExecuted=false;

            //     for(int tmp=0; tmp<3; tmp++)
            //     {
            //         // Console.WriteLine("X: "+caller.Parent.Left.ToString()+", Y: "+caller.Parent.Top.ToString());

            //         // check if it has already been executed
            //         if(alreadyExecuted)
            //         {
            //             break;
            //         }


            //         // checked if user has moved this app
            //         if(HasMouseMoved)
            //         {
            //             // that's an app move
            //             // lines
            //             if(caller.Parent.Left <= 1*100)
            //             {
            //                 slot_nbr_col=1;
            //                 caller.Parent.Left = (20*(0) + 100*(0));
            //             } 
            //             else if(caller.Parent.Left <= 2*100)
            //             {
            //                 slot_nbr_col=2;
            //                 caller.Parent.Left = (20*(1) + 100*(1));
            //             }  
            //             else if(caller.Parent.Left <= 3*100)
            //             {
            //                 slot_nbr_col=3;
            //                 caller.Parent.Left = (20*(2) + 100*(2));
            //             } 
            //             else
            //             {
            //                 slot_nbr_col=4;
            //                 caller.Parent.Left = (20*(3) + 100*(3));
            //             } 

            //             // columns
            //             if(caller.Parent.Top <= 20+1*120)
            //             {
            //                 slot_nbr_lin=1;
            //                 caller.Parent.Top = 40+(0)*120;
            //             }
            //             else if(caller.Parent.Top <= 20+2*120)
            //             {
            //                 slot_nbr_lin=2;
            //                 caller.Parent.Top = 40+(1)*120;
            //             }
            //             else if(caller.Parent.Top <= 20+3*120)
            //             {
            //                 slot_nbr_lin=3;
            //                 caller.Parent.Top = 40+(2)*120;
            //             }
            //             else if(caller.Parent.Top <= 20+4*120)
            //             {
            //                 slot_nbr_lin=4;
            //                 caller.Parent.Top = 40+(3)*120;
            //             }
            //             else
            //             {
            //                 slot_nbr_lin=5;
            //                 caller.Parent.Top = 40+(4)*120;
            //             }

            //             // getting slot number from column and line nbr
            //             int final_slot = (slot_nbr_lin-1)*4+slot_nbr_col;

            //             // creating a bool in case of no folder, no null and no app
            //             bool isExecuted = false;

            //             // getting final_slot current app (if exists)
            //             int slot=0;
            //             int current_slot=0;
            //             foreach(string line in File.ReadAllLines(utils.getPath()+"/apps/desktop.config"))
            //             {
            //                 string package = line.TrimStart(' ').TrimEnd(' ');
            //                 slot++;

            //                 if(package==current_package)
            //                 {
            //                     current_slot=slot;
            //                     Console.WriteLine("current_slot = "+current_slot.ToString());
            //                 }
            //             }
            //             slot=0;
            //             foreach(string line in File.ReadAllLines(utils.getPath()+"/apps/desktop.config"))
            //             {
            //                 // getting package name and slot number
            //                 string package = line.TrimStart(' ').TrimEnd(' ');
            //                 slot++;

                            
            //                 if(slot == final_slot)
            //                 {
            //                     // Console.WriteLine(current_package+"  =>  "+final_slot.ToString());
            //                     // Console.WriteLine(package+" => destination slot");

            //                     // check if slot already occuped by a folder
            //                     if(package.Split(": ")[0].TrimStart(' ').TrimEnd(' ').EndsWith("/"))
            //                     {
            //                         Console.WriteLine("slot "+final_slot.ToString()+" is already cointains a folder");
            //                         string foldername = package.Split(": ")[0].TrimStart(' ').TrimEnd(' ').Replace("/", "");
            //                         string folderpath = utils.getPath()+"/folders/"+foldername+".config";
            //                         if(Directory.Exists(folderpath) && current_package != "")
            //                         {
            //                             string currentConfig = File.ReadAllText(folderpath);
            //                             File.WriteAllText(currentConfig+"\r\n"+folderpath, current_package+": "+final_slot.ToString());
            //                         }
            //                         else
            //                         {
            //                             File.WriteAllText(folderpath, current_package+": "+final_slot.ToString());
            //                         }

            //                         isExecuted=true;
            //                         break;
            //                     }
            //                     // check if slot not occuped
            //                     else if(package == "null")
            //                     {
            //                         // will be executed after the foreach
            //                         break;
            //                     }
            //                     // already occuped by an app => create a folder
            //                     else
            //                     {
            //                         // check if position not changed
            //                         if(current_slot == final_slot)
            //                         {
            //                             isExecuted=true;
            //                             break;
            //                         }
            //                         MessageBox.Show("create a folder for '"+current_package+"' at "+final_slot.ToString());
            //                         isExecuted=true;
            //                         break;
            //                     }


            //                 }
            //             }
            //             if(!isExecuted)
            //             {
            //                 // null and empty slots
            //                 Console.WriteLine(current_package+" at line "+final_slot.ToString());
            //                 Console.WriteLine("null  at line "+current_slot.ToString());
                            
            //                 utils.changeLine(current_package, utils.getPath()+"/apps/desktop.config", final_slot);
            //                 utils.changeLine("null", utils.getPath()+"/apps/desktop.config", current_slot);
            //                 // Console.WriteLine("slot "+final_slot.ToString()+" is not occupped");
            //             }
                        

            //         }
            //         else
            //         {
            //             // that's a click
            //             // checked if that's a folder or an app
            //             bool isFolder = false;

            //             try
            //             {
            //                 CheckBox control = caller.Parent.Controls.Find("isFolder",true)[0] as CheckBox;
            //                 isFolder = control.Checked;
            //             }
            //             catch(Exception)
            //             {}
                        
            //             if(isFolder)
            //             {
            //                 // MessageBox.Show(clikedElement.Parent.Name+" => folder");
            //                 Task.Run(() => openFolder(sender, e, into));
            //             }
            //             else
            //             {
            //                 launch(sender);
            //             }
                        
            //         }

            //         // MessageBox.Show(((slot_nbr_lin-1)*4+slot_nbr_col).ToString());

            //         // set alreadyExecuted to true to open an app one time/click
            //         alreadyExecuted = true;
            //     }
            // }
            
        }
        private static async void mouseDown(object sender, EventArgs e, Window into)
        {
            // on click release
            // if (e.Button == System.Windows.Forms.MouseButtons.Left)
            // {
            //     HasMouseMoved = false;
            //     MouseDownLocation = e.Location;
            //     Control caller = sender as Control;
            //     caller.Parent.Parent.Controls.SetChildIndex(caller.Parent, 0);
            // }
        }

        public static bool appopened = false;
        public static void launch(object sender, EventArgs args)
        {
            utils.logn("[o] LAUNCHED APP!!!", ConsoleColor.Red);
            Control caller = sender as Control;

            string path="";

            Label temp_path_control = caller.Get("temp_path") as Label;

            path = temp_path_control.Text;
            
            if(!appopened)
            {
                appopened=true;
                
                Interpreter.runApp(path);
                while(Graphics.running)
                {}
                appopened=false;
            }
            
        }
    }
    
}