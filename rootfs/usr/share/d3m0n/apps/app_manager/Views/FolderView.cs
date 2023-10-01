using System;
using System.IO;
using System.Windows.Forms;
using System.Drawing;
using System.Threading.Tasks;
using System.Collections.Generic;
using System.Timers;
using d3m0n;

namespace d3m0n
{
    public class d3m0nViews
    {
        
        public static Panel FolderView(string path)
        {
            Panel view = new Panel();
			// square window
            // view.Size = new Size(380, 380);
			view.Anchor = AnchorStyles.Right;  
			// view.Location = new Point(0, 0);
            // no bg color
            view.BackColor = Color.Lime;
            view.Font = new Font("Arial", 15);

            

            // string[] apps = Interpreter.getApps();
            // foreach(string raw in apps)
            // {
            //     d3m0nApplication.name = raw.Split("//!//")[0];
            //     d3m0nApplication.package = raw.Split("//!//")[6];
            //     d3m0nApplication.temp_path = raw.Split("//!//")[2];
            //     d3m0nApplication.icon = raw.Split("//!//")[1];
            //     d3m0nApplication.perms = raw.Split("//!//")[3];
            //     d3m0nApplication.start_path = raw.Split("//!//")[4];
            //     d3m0nApplication.category = raw.Split("//!//")[5];
            //     d3m0nApplication.empty = false;
            //     d3m0nApplication.folder = false;

            //     d3m0nApplication.display(this);
            // }

            

            return view;
        }
    }
}