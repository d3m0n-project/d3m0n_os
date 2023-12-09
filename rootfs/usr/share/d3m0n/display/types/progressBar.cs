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
    
    public class ProgressBar
    {
        

        public ushort BackColor = Colors.Black;
        public ushort ForeColor = Colors.White;
        public DataTypes.Point Location = new DataTypes.Point(0, 0);
        public DataTypes.Size Size  = new DataTypes.Size(50, 25);
        public int Percentage = 0;
        public int Min = 0;
        public int Max = 100;

        public void draw()
        {
            int x1 = 240-this.Location.X;
            int y1 = this.Location.Y;

            int x2 = x1-this.Size.Width;
            int y2 = this.Location.Y-this.Size.Height;
            
            // circle of progressbar
            ili9340.lcdDrawLine((ushort)x1, (ushort)y1, (ushort)x2, (ushort)y1, this.BackColor);
            // get circle pos from start (x1) and this.Percentage
            int circlePosX = x1-(this.Percentage/(this.Max-this.Min))*this.Size.Width;
            ili9340.lcdDrawFillCircle((ushort)circlePosX, (ushort)y1, (ushort)10, Colors.Yellow);
        }
        
    }
}