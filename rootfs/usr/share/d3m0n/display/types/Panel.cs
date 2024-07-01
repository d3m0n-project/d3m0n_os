using System;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Collections.Generic;
using System.Drawing;

namespace display
{
    
    public class Panel : Control
    {
        public ushort BackColor = Colors.Red;

        public void draw()
        {
            // draw backcolor if it's not transparent
            Console.WriteLine("DEBUG: Panel has been displayed");
            int x1 = 240-this.Location.X;
            int y1 = this.Location.Y;

            int x2 = x1-this.Size.Width;
            int y2 = y1+this.Size.Height;

            ili9340.lcdDrawFillRect((ushort)(x2), (ushort)(y1), (ushort)(x1), (ushort)(y2), this.BackColor);
            // nothing to do => that's a container
        }
    }
}