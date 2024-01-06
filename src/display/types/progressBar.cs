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
    
    public class ProgressBar : Control
    {
        // bg color grey
        public ushort BackColor = Colors.getRGB565(30, 30, 30);
        public ushort ForeColor = Colors.White;
        public DataTypes.Size Size  = new DataTypes.Size(100, 25);
        public int Percentage = 0;
        public int Min = 0;
        public int Max = 100;

        // enable percentage edition
        public override void Clicked(int x, int y)
        {
            int x1 = 240-this.Location.X;
            int y1 = this.Location.Y;

            int x2 = x1-this.Size.Width;
            int y2 = y1+this.Size.Height;

            // origin = (x2,y1)
            // calc touch position since origin (only x needed => that's a progressbar)
            // add it width to get to the left and no right side of the Control
            int X_pos_from_origin = x2-x+this.Size.Width;
            int new_percentage = (X_pos_from_origin*(this.Max-this.Min))/this.Size.Width;
            this.Percentage = new_percentage;

            int circlePosX = x1-(this.Percentage*this.Size.Width)/(this.Max-this.Min);
            ili9340.lcdDrawFillCircle((ushort)circlePosX, (ushort)y1, (ushort)5, Colors.Red);

            Console.Write(new_percentage.ToString());

            draw();

            
            OnClick(EventArgs.Empty);
        }
        
        public void draw()
        {
            int x1 = 240-this.Location.X;
            int y1 = this.Location.Y;

            int x2 = x1-this.Size.Width;
            int y2 = y1+this.Size.Height;
            
            // circle of progressbar
            ili9340.lcdDrawLine((ushort)x1, (ushort)y1, (ushort)x2, (ushort)y1, this.BackColor);
            // get circle pos from start (x1) and this.Percentage
            int circlePosX = x1-(this.Percentage*this.Size.Width)/(this.Max-this.Min);
            ili9340.lcdDrawFillCircle((ushort)circlePosX, (ushort)y1, (ushort)5, this.ForeColor);
        }
        
    }
}