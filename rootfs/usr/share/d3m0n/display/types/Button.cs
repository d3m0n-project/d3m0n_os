using System;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Diagnostics;

namespace display
{
    
    public class Button : Control
    {
        public string Text = "";
        public ushort BackColor = Colors.Black;
        public ushort ForeColor = Colors.White;
        public int Padding = 5;


        public int FontSize = 10;
        public bool Center = false;
        public int FontPadding = 0;


        public void draw()
        {
            int x1 = 240-this.Location.X;
            int y1 = this.Location.Y;

            int x2 = x1-this.Size.Width;
            int y2 = y1+this.Size.Height;

            // check if size not changed
            if(defaultSize.Width == this.Size.Width && defaultSize.Height == this.Size.Height)
            {
                // then define width with text size
                this.Size.Width = this.Text.Length*(this.FontSize+this.FontPadding);
            }

            //                        x   y    x1   x2 color
            //ili9340.lcdDrawFillRect((ushort)10, (ushort)10, (ushort)50, (ushort)70, this.BackColor);
            
            ili9340.lcdDrawFillRect((ushort)(x2-Padding), (ushort)(y1-Padding), (ushort)(x1+Padding), (ushort)(y2+Padding), this.BackColor);
            //                 text  size pad  x   y  center
            font.WriteString(this.Text, this.FontSize, this.FontPadding, (ushort)x1, (ushort)y1, this.ForeColor, this.Center, (ushort)this.Size.Width, (ushort)this.Size.Height);

            // debug top-right
            ili9340.lcdDrawPixel((ushort)x2, (ushort)y1, Colors.Yellow);
            // and bottom-left pixels
            ili9340.lcdDrawPixel((ushort)x1, (ushort)y2, Colors.Green);
        }
    }
}