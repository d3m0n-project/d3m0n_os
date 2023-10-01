using System;
using System.IO;
using System.Windows.Forms;
using System.Drawing;
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

        public static void display(Form into)
        {
            if(column_nbr > 4)
            {
               column_nbr=1;
                line_nbr++;
                if(line_nbr > 4)
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


            // 10px + 100px*4 + 10px   width
            // 20px + 100px*4 + 20px   height
            Panel app = new Panel();
            app.Name = name;
            // app.BackColor = Color.Lime;
            app.BackColor = Color.Transparent;
            app.Size = new Size(100, 120);
            app.Location = new Point((10*2*(column_nbr-1) + 100*(column_nbr-1)), 40+(line_nbr-1)*120);
            // app.FlowDirection = FlowDirection.TopDown;
            // app.WrapContents = false;
            app.Padding = new Padding(0);

            

            // sets app path
            Label app_path = new Label();
            app_path.Text = temp_path;
            app_path.Visible = false;
            app_path.Name = "temp_path";
            app.Controls.Add(app_path);

            // sets package
            Label app_package = new Label();
            app_package.Text = package;
            app_package.Visible = false;
            app_package.Name = "app_package";
            app.Controls.Add(app_package);

            // define if that's a folder
            CheckBox isFolder = new CheckBox();
            isFolder.Visible = false;
            isFolder.Checked = folder;

            app.Controls.Add(isFolder);
            into.Controls.Add(app);




            // application icon
            PictureBox app_icon = new PictureBox();
            app_icon.Name = name+"_app_icon";
            app_icon.Size = new Size(100, 100);
            app_icon.Location = new Point(0, -10);
            if(File.Exists(icon))
            {
                Image image = Image.FromFile(icon);
                app_icon.Image = image;
            }
            else
            {
                app_icon.Image = theme_manager.get_icon(icon);
            }
            app_icon.SizeMode = PictureBoxSizeMode.StretchImage;

            // actions for drag and drop
            app_icon.MouseDown += (sender, e) => {  Task.Run(() => mouseDown(sender, e, into));  };
            app_icon.MouseMove += (sender, e) => {  Task.Run(() => mouseMove(sender, e, into));  };
            app_icon.MouseUp += (sender, e) => {  Task.Run(() => mouseUp(sender, e, into));  };

            app.Controls.Add(app_icon);


            // application name
            Label app_name = new Label();
            app_name.Name = name+"_app_name";
            app_name.Size = new Size(100, 20);
            // app_name.BackColor = Color.Red;
            app_name.Location = new Point(0, 90);
            app_name.TextAlign = ContentAlignment.TopCenter;
            app_name.Text = name;
            // app_name.AutoSize = true;
            // app_name.Dock = DockStyle.None;
            app_name.Font = new Font("Arial", 10);
            app_name.ForeColor = Color.White;


            app.Controls.Add(app_name);



            
            column_nbr++;
        }

        private static async void openFolder(object sender, EventArgs e, Form into)
        {            
            MessageBox.Show("a");
            // Show the folder customview
            Graphics.displayCustomView(d3m0nViews.FolderView(""));
        }
        private static Point MouseDownLocation;
        private static bool HasMouseMoved = false;
        private static async void mouseMove(object sender, MouseEventArgs e, Form into)
        {
            if (e.Button == System.Windows.Forms.MouseButtons.Left)
            {
                HasMouseMoved = true;

                Control caller = sender as Control;
                
                caller.Parent.Left = e.X + caller.Parent.Left - MouseDownLocation.X;
                caller.Parent.Top = e.Y + caller.Parent.Top - MouseDownLocation.Y;
            }
        }
        private static async void mouseUp(object sender, MouseEventArgs e, Form into)
        {
            if (e.Button == System.Windows.Forms.MouseButtons.Left)
            {
                Control caller = sender as Control;
                string current_package = "";
                
                try
                {
                    // getting current app package
                    current_package=caller.Parent.Controls.Find("app_package",true)[0].Text;
                }
                catch(Exception)
                {}

                int slot_nbr_lin=0;
                int slot_nbr_col=0;


                bool alreadyExecuted=false;

                for(int tmp=0; tmp<3; tmp++)
                {
                    // Console.WriteLine("X: "+caller.Parent.Left.ToString()+", Y: "+caller.Parent.Top.ToString());

                    // check if it has already been executed
                    if(alreadyExecuted)
                    {
                        break;
                    }


                    // checked if user has moved this app
                    if(HasMouseMoved)
                    {
                        // that's an app move
                        // lines
                        if(caller.Parent.Left <= 1*100)
                        {
                            slot_nbr_col=1;
                            caller.Parent.Left = (20*(0) + 100*(0));
                        } 
                        else if(caller.Parent.Left <= 2*100)
                        {
                            slot_nbr_col=2;
                            caller.Parent.Left = (20*(1) + 100*(1));
                        }  
                        else if(caller.Parent.Left <= 3*100)
                        {
                            slot_nbr_col=3;
                            caller.Parent.Left = (20*(2) + 100*(2));
                        } 
                        else
                        {
                            slot_nbr_col=4;
                            caller.Parent.Left = (20*(3) + 100*(3));
                        } 

                        // columns
                        if(caller.Parent.Top <= 20+1*120)
                        {
                            slot_nbr_lin=1;
                            caller.Parent.Top = 40+(0)*120;
                        }
                        else if(caller.Parent.Top <= 20+2*120)
                        {
                            slot_nbr_lin=2;
                            caller.Parent.Top = 40+(1)*120;
                        }
                        else if(caller.Parent.Top <= 20+3*120)
                        {
                            slot_nbr_lin=3;
                            caller.Parent.Top = 40+(2)*120;
                        }
                        else if(caller.Parent.Top <= 20+4*120)
                        {
                            slot_nbr_lin=4;
                            caller.Parent.Top = 40+(3)*120;
                        }
                        else
                        {
                            slot_nbr_lin=5;
                            caller.Parent.Top = 40+(2)*120;
                        }

                        // getting slot number from column and line nbr
                        int final_slot = (slot_nbr_lin-1)*4+slot_nbr_col;

                        // creating a bool in case of no folder, no null and no app
                        bool isExecuted = false;

                        // getting final_slot current app (if exists)
                        foreach(string line in File.ReadAllLines(utils.getPath()+"/apps/desktop.config"))
                        {
                            // getting package name and slot number
                            string package = line.Split(": ")[0].TrimStart(' ').TrimEnd(' ');
                            string slot = line.Split(": ")[1].TrimStart(' ').TrimEnd(' ');

                            Console.WriteLine(current_package+"  =>  "+final_slot.ToString());

                            if(Int32.Parse(slot) == final_slot)
                            {
                                // check if slot already occuped by a folder
                                if(package.Split(": ")[0].TrimStart(' ').TrimEnd(' ').EndsWith("/"))
                                {
                                    Console.WriteLine("slot "+final_slot.ToString()+" is already cointains a folder");
                                    string foldername = package.Split(": ")[0].TrimStart(' ').TrimEnd(' ').Replace("/", "");
                                    string folderpath = utils.getPath()+"/folders/"+foldername+".config";
                                    if(Directory.Exists(folderpath) && current_package != "")
                                    {
                                        string currentConfig = File.ReadAllText(folderpath);
                                        File.WriteAllText(currentConfig+"\r\n"+folderpath, current_package+": "+final_slot.ToString());
                                    }
                                    else
                                    {
                                        File.WriteAllText(folderpath, current_package+": "+final_slot.ToString());
                                    }

                                    isExecuted=true;
                                    break;
                                }
                                // check if slot not occuped
                                else if(package == "null")
                                {
                                    // will be executed after the foreach loop
                                    break;
                                }
                                // already occuped by an app => create a folder
                                else
                                {
                                    // check if position not changed
                                    if(utils.getSetting(package, utils.getPath()+"/apps/desktop.config") == final_slot.ToString())
                                    {
                                        isExecuted=true;
                                        break;
                                    }
                                    MessageBox.Show("create a folder for '"+current_package+"' at "+final_slot.ToString());
                                    isExecuted=true;
                                    break;
                                }

                                // for null and "" (no entry)
                                if(!isExecuted)
                                {
                                    Console.WriteLine("slot "+final_slot.ToString()+" is not occupped");
                                    string current_content = File.ReadAllText(utils.getPath()+"/apps/desktop.config");
                                    File.WriteAllText(utils.getPath()+"/apps/destkop.config", current_content+"\r\n"+current_package+": "+final_slot.ToString());
                                    MessageBox.Show("added '"+current_package+"' to "+final_slot.ToString());

                                    isExecuted=true;
                                }
                            }
                        }

                    }
                    else
                    {
                        // that's a click
                        // checked if that's a folder or an app
                        bool isFolder = false;

                        try
                        {
                            CheckBox control = caller.Parent.Controls.Find("isFolder",true)[0] as CheckBox;
                            isFolder = control.Checked;
                        }
                        catch(Exception)
                        {}
                        
                        if(isFolder)
                        {
                            // MessageBox.Show(clikedElement.Parent.Name+" => folder");
                            Task.Run(() => openFolder(sender, e, into));
                        }
                        else
                        {
                            launch(sender);
                        }
                        
                    }

                    // MessageBox.Show(((slot_nbr_lin-1)*4+slot_nbr_col).ToString());

                    // set alreadyExecuted to true to open an app one time/click
                    alreadyExecuted = true;
                }
            }
            
        }
        private static async void mouseDown(object sender, MouseEventArgs e, Form into)
        {
            // on click release
            if (e.Button == System.Windows.Forms.MouseButtons.Left)
            {
                HasMouseMoved = false;
                MouseDownLocation = e.Location;
                Control caller = sender as Control;
                caller.Parent.Parent.Controls.SetChildIndex(caller.Parent, 0);
            }
        }

        public static bool appopened = false;
        public static void launch(object sender)
        {
            Control caller = sender as Control;
            var controls = caller.Parent.Controls;

            string path="";

            for(int i=0; i<controls.Count; i++)
            {
                if(controls[i].Name.StartsWith("temp_path"))
                {
                    path = controls[i].Text;
                }
            }
            
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