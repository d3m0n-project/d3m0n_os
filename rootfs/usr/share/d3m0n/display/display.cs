using System;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Windows.Forms;
using System.Drawing;

namespace display
{

    class Display
    {
        public static Window window = new Window();

        public static void processClick(int x, int y)
        {
            Console.WriteLine("x=" + x.ToString() + " y=" + y.ToString());

            // check if clicked rect
            foreach (Control item in window.Controls)
            {
                if (utils.checkPosition(item.getRect(), new DataTypes.Point(x, y)))
                {
                    Console.Write($"CLICKED {item.Name}!!!!");
                    ili9340.lcdDrawFillCircle((ushort)x, (ushort)y, (ushort)10, Colors.Yellow);
                    break; // Exit the loop after handling the click
                }
            }
        }

        
        public static void init()
        {
            // enable touchscreen
            // Task.Run(() => touchscreen.touchInit());

            if(window == null)
            {
                Console.WriteLine("is null!!!!");
            }
            int screenWidth = 240;
            int screenHeight = 320;
            int offsetx = 0;
            int offsety = 0;


            ili9340.lcdInit(screenWidth, screenHeight, offsetx, offsety);
            ili9340.lcdReset();
            ili9340.lcdSetup();

            // bcm2835_gpio_fsel(T_IRQ,BCM2835_GPIO_FSEL_INPT);

            ili9340.lcdFillScreen(Colors.Black);
            // ili9340.lcdDrawFillCircle(10, 10, 10, Color.Yellow);



            Button btn1 = new Button();
            btn1.Text = "test";
            btn1.Location = new DataTypes.Point(50, 50);
            btn1.BackColor = Colors.Red;
            btn1.Center = true;
            btn1.ForeColor = Colors.White;
            // btn1.OnClick += Btn1_Click;
            // btn1.InvokeCustomEvent("Custom event invoked!");
            btn1.Name="btn1_8";
            btn1.draw();
            window.Add(btn1);

            Image img1 = new Image();
            img1.Img = System.Drawing.Image.FromFile("/home/d3m0n/test.png");
            img1.Location = new DataTypes.Point(75, 75);
            img1.Size = new DataTypes.Size(50, 50);
            img1.Name="img1_7";
            img1.draw();
            window.Add(img1);

            Console.WriteLine(Colors.getRGB565(255, 0, 0).ToString("X4"));

            // enable touchscreen
            touchscreen.touchInit();
        }
    }

    static class ili9340
    {
            public static ushort DIRECTION0 = 0;
            public static ushort DIRECTION90 = 1;
            public static ushort DIRECTION180 = 2;
            public static ushort DIRECTION270 = 3;

            // Importing ili9340 functions from the C library

            [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            public static extern void lcdWriteCommandByte(byte c);

            [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            public static extern void lcdWriteDataByte(byte c);

            [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            public static extern void lcdWriteDataWord(ushort w);

            [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            public static extern void lcdWriteColor(ushort color, IntPtr size);

            [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            public static extern void lcdInit(int width, int height, int offsetx, int offsety);

            [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            public static extern void lcdReset();

            [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            public static extern void lcdSetup();

            [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            public static extern void lcdDrawPixel(ushort x, ushort y, ushort color);

            [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            public static extern void lcdDrawFillRect(ushort x1, ushort y1, ushort x2, ushort y2, ushort color);

            [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            public static extern void lcdDisplayOff();

            [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            public static extern void lcdDisplayOn();

            [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            public static extern void lcdInversionOn();

            [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            public static extern void lcdFillScreen(ushort color);

            [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            public static extern void lcdDrawLine(ushort x1, ushort y1, ushort x2, ushort y2, ushort color);

            [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            public static extern void lcdDrawRect(ushort x1, ushort y1, ushort x2, ushort y2, ushort color);

            [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            public static extern void lcdDrawCircle(ushort x0, ushort y0, ushort r, ushort color);

            [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            public static extern void lcdDrawFillCircle(ushort x0, ushort y0, ushort r, ushort color);

            [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            public static extern void lcdDrawRoundRect(ushort x1, ushort y1, ushort x2, ushort y2, ushort r, ushort color);

            [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            public static extern void lcdDrawArrow(ushort x0, ushort y0, ushort x1, ushort y1, ushort w, ushort color);

            [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            public static extern void lcdDrawFillArrow(ushort x0, ushort y0, ushort x1, ushort y1, ushort w, ushort color);

            [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            public static extern int lcdDrawSJISChar(IntPtr fx, ushort x, ushort y, ushort sjis, ushort color);

            // [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            // public static extern int lcdDrawUTF8Char(FontxFile[] fx, ushort x, ushort y, string utf8, ushort color);

            // [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            // public static extern int lcdDrawUTF8String(FontxFile[] fx, ushort x, ushort y, string utfs, ushort color);

            [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            public static extern void lcdSetFontDirection(ushort direction);

            [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            public static extern void lcdSetFontFill(ushort color);

            [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            public static extern void lcdUnsetFontFill();

            [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            public static extern void lcdSetFontUnderLine(ushort color);

            [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
            public static extern void lcdUnsetFontUnderLine();
        }
	
}

        

// }