using System;
using System.IO;
using System.Windows.Forms;
using System.Drawing;

namespace d3m0n
{
    public class d3m0nApplication
    {

        private static int column_nbr = 1;
        private static int line_nbr   = 1;
        private static int page_number= 1;

        public static string name { get; set;}
        public static string package {get; set;}
        public static string temp_path { get; set;}
        public static string icon { get; set;}
        public static string perms { get; set;}
        public static string start_path { get; set;}
        public static string category { get; set;}


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
            




            // 10px + 100px*4 + 10px   w
            // 20px + 100px*4 + 20px   h
            Panel app = new Panel();
            app.Name = name;
            app.BackColor = Color.Transparent;
            app.Size = new Size(100, 120);
            app.Location = new Point((10*2*(column_nbr-1) + 100*(column_nbr-1)), 40+(line_nbr-1)*120);
            // app.FlowDirection = FlowDirection.TopDown;
            // app.WrapContents = false;
            app.Padding = new Padding(0);
            app.Click += (sender, EventArgs) => { launch(temp_path, sender, EventArgs); };
            into.Controls.Add(app);

            PictureBox app_icon = new PictureBox();
            app_icon.Name = name+"_app_icon";
            app_icon.Size = new Size(100, 100);
            app_icon.Location = new Point(0, -10);
            app_icon.Click += (sender, EventArgs) => { launch(temp_path, sender, EventArgs); };
            if(File.Exists(icon))
            {
                Image image = Image.FromFile(icon);
                app_icon.Image = image;
            }
            else
            {
                app_icon.Image = theme_manager.get_icon(name);
            }
            app_icon.SizeMode = PictureBoxSizeMode.StretchImage;
            app.Controls.Add(app_icon);

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
            app_name.Click += (sender, EventArgs) => { launch(temp_path, sender, EventArgs); };


            app.Controls.Add(app_name);



            
            column_nbr++;
            // Console.WriteLine("column_nbr: "+column_nbr);
            // Console.WriteLine("line_nbr: "+line_nbr);
        }

        public static bool appopened = false;
        public static void launch(string path, object sender, EventArgs e)
        {
            if(!appopened)
            {
                appopened=true;
                // MessageBox.Show(name+": App Clicked!");
                // app_embed.Visible = true;
                // MessageBox.Show(path);
                Interpreter.runApp(path);
                while(Graphics.running)
                {}
                appopened=false;
            }
            
        }
    }
    
}

