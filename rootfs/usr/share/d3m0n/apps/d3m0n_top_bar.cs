using System;
using System.Windows.Forms;
using System.Drawing;

namespace d3m0n
{
    class complete_topbar {
        private static Panel panel1;
        private static Label hour;
        private static PictureBox settings;
        private static Label date;
        private static PictureBox top_bar_stretcher;
        private static bool AreEventsAlreadyDef = false;

        public static void update(Control T, DateTime date) {
            
            string date_hour = date.ToString("HH:mm");
            string date_day = date.ToString("MMMM dd yyyy");
            Label hour_text = new Label();
            Label date_text = new Label();
            PictureBox top_bar_stretcher = new PictureBox();

            // slot1 (Wifi)
            Panel wifi = new Panel();
            Label wifi_text = new Label();
            PictureBox wifi_pic = new PictureBox();
            // slot2 (bluetooth)
            Panel bluetooth = new Panel();
            Label bluetooth_text = new Label();
            PictureBox bluetooth_pic = new PictureBox();



            try
            {
                hour_text = T.Controls.Find("hour",true)[0] as Label;
                date_text = T.Controls.Find("date",true)[0] as Label;
                top_bar_stretcher = T.Controls.Find("top_bar_stretcher", true)[0] as PictureBox;

                // wifi
                wifi = T.Controls.Find("wifi",true)[0] as Panel;
                wifi_text = wifi.Controls.Find("wifi_text",true)[0] as Label;
                wifi_pic = wifi.Controls.Find("wifi_pic",true)[0] as PictureBox;

                // bluetooth
                bluetooth = T.Controls.Find("bluetooth",true)[0] as Panel;
                bluetooth_text = bluetooth.Controls.Find("bluetooth_text",true)[0] as Label;
                bluetooth_pic = bluetooth.Controls.Find("bluetooth_pic",true)[0] as PictureBox;
                
            }
            catch(Exception e)
            {
                Console.WriteLine(e.ToString());
            }
            
			hour_text.Text = date_hour;
            date_text.Text = date_day;
            
            if(!AreEventsAlreadyDef)
            {
                // add events
                top_bar_stretcher.Click    += (sender, EventArgs) => { T.Visible = false; };

                wifi_pic.Click             += (sender, EventArgs) => { MessageBox.Show("wifi"); };
                bluetooth_pic.Click        += (sender, EventArgs) => { MessageBox.Show("bluetooth"); };
            }
            AreEventsAlreadyDef=true;
        }

        public static Control Get()
        {
            panel1 = new Panel();
            hour = new Label();
            date = new Label();
            settings = new PictureBox();
            top_bar_stretcher = new PictureBox();

            
            panel1.Controls.Add(top_bar_stretcher);
            panel1.Controls.Add(settings);
            panel1.Controls.Add(date);
            panel1.Controls.Add(hour);
            

            panel1.Location = new System.Drawing.Point(0, 0);
            panel1.Name = "top_bar_expand";
            panel1.BackColor = Color.FromArgb(23, 23, 23);
            panel1.Size = new System.Drawing.Size(480, 284);
            panel1.TabIndex = 0;

            panel1.Controls.Add(getIcon("wifi", "internet-on-white", 1, panel1.Width));
            panel1.Controls.Add(getIcon("bluetooth", "bluetooth-white", 2, panel1.Width));
            panel1.Controls.Add(getIcon("wifi3", "internet-on-white", 3, panel1.Width));
            panel1.Controls.Add(getIcon("wifi4", "internet-on-white", 4, panel1.Width));
            panel1.Controls.Add(getIcon("wifi5", "internet-on-white", 5, panel1.Width));
            panel1.Controls.Add(getIcon("wifi6", "internet-on-white", 6, panel1.Width));
            panel1.Controls.Add(getIcon("wifi7", "internet-on-white", 7, panel1.Width));
            panel1.Controls.Add(getIcon("wifi8", "internet-on-white", 8, panel1.Width));
            panel1.Controls.Add(getIcon("wifi9", "internet-on-white", 9, panel1.Width));
            panel1.Controls.Add(getIcon("wifi10", "internet-on-white", 10, panel1.Width));
            











            // 
            // hour
            // 
            hour.AutoSize = true;
            hour.Font = new System.Drawing.Font("Microsoft Sans Serif", 19.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            hour.Location = new System.Drawing.Point(12, 15);
            hour.Name = "hour";
            hour.ForeColor = Color.White;
            hour.Size = new System.Drawing.Size(102, 39);
            hour.TabIndex = 1;
            hour.Text = "0:0";
            // 
            // date
            // 
            date.AutoSize = true;
            date.ForeColor = Color.White;
            date.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            date.Location = new System.Drawing.Point(15, 54);
            date.Name = "date";
            date.Size = new System.Drawing.Size(182, 25);
            date.TabIndex = 2;
            date.Text = "january 1 2000";
            // 
            // settings
            // 
            settings.SizeMode = PictureBoxSizeMode.StretchImage;
            settings.Location = new System.Drawing.Point(panel1.Width-60, 10);
            settings.Image = theme_manager.get_icon("settings");
            settings.Name = "settings";
            settings.Size = new System.Drawing.Size(50, 50);
            settings.TabIndex = 1;
            settings.TabStop = false;
            // 
            // top_bar_stretcher
            // 
            top_bar_stretcher.SizeMode = PictureBoxSizeMode.Zoom;
            top_bar_stretcher.Image = theme_manager.get_icon("chevron-white", 180);
            top_bar_stretcher.Location = new System.Drawing.Point(0, 265);
            top_bar_stretcher.Name = "top_bar_stretcher";
            top_bar_stretcher.Size = new System.Drawing.Size(panel1.Width, 20);
            top_bar_stretcher.TabIndex = 11;
            top_bar_stretcher.TabStop = false;
            
        
            return panel1;
        }
        public static Panel getIcon(string name, string picture, int number, int width)
        {
            PictureBox pictureBox1 = new PictureBox();
            Label label1 = new Label();
            Panel panel2 = new Panel();

            pictureBox1.Location = new System.Drawing.Point(0, 0);
            pictureBox1.Name = name+"_pic";
            pictureBox1.Size = new System.Drawing.Size(50, 50);
            pictureBox1.Image = theme_manager.get_icon(picture);
            pictureBox1.SizeMode = PictureBoxSizeMode.StretchImage;
            pictureBox1.TabIndex = 1;
            pictureBox1.TabStop = false;

            panel2.Controls.Add(label1);
            panel2.Controls.Add(pictureBox1);

            if(number>5)
            {
                // second line y=171
                if(number==6)
                {
                    panel2.Location = new System.Drawing.Point(((width/5)-50)/2, 180);
                }
                else {
                    panel2.Location = new System.Drawing.Point((number-5)*(width/5)-50-((width/5)-50)/2, 180);
                }
            }
            else
            {
                // first line y=95
                // formula for spaces ==> width/icons number in a line - icon width
                if(number==1)
                {
                    panel2.Location = new System.Drawing.Point(((width/5)-50)/2, 95);
                }
                else {
                    panel2.Location = new System.Drawing.Point(number*(width/5)-50-((width/5)-50)/2, 95);
                }
            }

            panel2.Name = name;
            panel2.Size = new System.Drawing.Size(50, 70);
            panel2.TabIndex = 0;

            label1.Anchor = ((AnchorStyles)((((AnchorStyles.Top | AnchorStyles.Bottom) 
            | AnchorStyles.Left) 
            | AnchorStyles.Right)));
            label1.ForeColor = Color.White;
            label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            label1.Location = new System.Drawing.Point(0, 50);
            label1.Margin = new Padding(0);
            label1.Name = name+"_text";
            label1.Size = new System.Drawing.Size(50, 20);
            label1.TabIndex = 2;
            label1.Text = name;
            label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
        
            return panel2;
        }
    }
    
}