using System;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Windows.Forms;

namespace display
{
    
    public class RoundButton : Control
    {
        

        public string Text = "";
        // grey bg (30, 30, 30)
        public ushort BackColor = Colors.getRGB565(30, 30, 30);
        public ushort ForeColor = Colors.White;
        public int FontSize = 10;
        public bool Center = false;
        public int FontPadding = 0;
        public int Radius = 10;
        public int Padding = 5;

        public void draw()
        {
            int x1 = 240-this.Location.X;
            int y1 = this.Location.Y;

            int x2 = x1-this.Size.Width;
            int y2 = y1+this.Size.Height;

            ili9340.lcdDrawRoundRect((ushort)(x2-Padding), (ushort)(y1-Padding), (ushort)(x1+Padding), (ushort)(y2+Padding), (ushort)this.Radius, (ushort)this.BackColor);
            //                 text  size pad  x   y  center
            font.WriteString(this.Text, this.FontSize, this.FontPadding, (ushort)x1, (ushort)y1, this.ForeColor, this.Center, (ushort)this.Size.Width, (ushort)this.Size.Height);
        }
        
    }
}