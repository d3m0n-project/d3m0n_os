using System;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Diagnostics;

namespace display
{
    public class Label : Control
    {
        public string Text = "";
        public ushort BackColor = Colors.Black;
        public ushort ForeColor = Colors.White;
        public int FontSize = 10;
        public bool Center = false;
        public int FontPadding = 0;

        public void draw()
        {
            int x1 = 240-this.Location.X;
            int y1 = this.Location.Y;

            int x2 = x1-this.Size.Width;
            int y2 = y1+this.Size.Height;


            font.WriteString(this.Text, this.FontSize, this.FontPadding, (ushort)x1, (ushort)y1, this.ForeColor, this.Center, (ushort)this.Size.Width, (ushort)this.Size.Height);
        }
        
    }
}