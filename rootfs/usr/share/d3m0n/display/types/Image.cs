using System;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Drawing;

namespace display
{
    
    public class Image: Control
    {
        public System.Drawing.Image Img;
        public ushort BackColor = Colors.Black;
        public bool Center = false;
        public bool reverse = false;


        // images built different (inversed)
        public DataTypes.Rect getRect()
        {
            int x1 = 240-this.Location.X;
            int y1 = this.Location.Y;

            int x2 = x1+this.Size.Width;
            int y2 = y1+this.Size.Height;

            return new DataTypes.Rect(x2, y1, x1, y2);
        }

        public void draw()
        {
            int x1 = 240-this.Location.X;
            int y1 = this.Location.Y;

            int x2 = x1-this.Size.Width;
            int y2 = y1+this.Size.Height;

            x1-=2*this.Size.Width;

            Bitmap bmp2 = (Bitmap)Img;
            // resize to width 
            Bitmap bmp = utils.ResizeBitmap(bmp2, Size.Width, Size.Height);
            for (int ii = 0; ii < bmp.Width; ii++)
            {
                for (int jj = 0; jj < bmp.Height; jj++)
                {
                    Color pixelColor = bmp.GetPixel(ii, jj);
                    ili9340.lcdDrawPixel((ushort)(x2-ii), (ushort)(y1+jj), Colors.getColor(pixelColor));
                }
            }

            // debug top-right
            ili9340.lcdDrawPixel((ushort)x2, (ushort)y1, Colors.Yellow);
            // and bottom-left pixels
            ili9340.lcdDrawPixel((ushort)x1, (ushort)y2, Colors.Green);
        }
    }
}