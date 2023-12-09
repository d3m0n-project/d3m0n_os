using System;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Diagnostics;

namespace display
{
    
    public class Label
    {
        

        public string Text = "";
        public ushort BackColor = Colors.Black;
        public ushort ForeColor = Colors.White;
        public DataTypes.Point Location = new DataTypes.Point(0, 0);
        public DataTypes.Size Size  = new DataTypes.Size(50, 25);
        public int FontSize = 10;
        public bool Center = false;
        public int FontPadding = 0;

        public void draw()
        {
            int x1 = 240-this.Location.X;
            int y1 = this.Location.Y;

            int x2 = x1-this.Size.Width;
            int y2 = this.Location.Y-this.Size.Height;
            //                 text  size pad  x   y  center
            font.WriteString(this.Text, this.FontSize, this.FontPadding, (ushort)x1, (ushort)y1, this.ForeColor, this.Center, (ushort)this.Size.Width, (ushort)this.Size.Height);
        }
        
    }
}