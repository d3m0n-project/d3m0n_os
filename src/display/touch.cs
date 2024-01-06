using System;
using System.Runtime.InteropServices;
using System.IO;

namespace display
{
    class touchscreen
    {
        public static int T_CS = bcm2835.BCM2835_SPI_CS1;
        public static int RES = bcm2835.RPI_V2_GPIO_P1_11;
        public static int T_IRQ = bcm2835.RPI_V2_GPIO_P1_22;

        public static void touchInit(Window into){
            int     x=0;
            int     y=0;
            int     pen_irq;

            if (bcm2835.bcm2835_init() != 1) return;
            bcm2835.bcm2835_gpio_fsel(T_IRQ,bcm2835.BCM2835_GPIO_FSEL_INPT); // T_IRQ
            bcm2835.bcm2835_gpio_fsel(RES,bcm2835.BCM2835_GPIO_FSEL_OUTP); // RES
            bcm2835.bcm2835_gpio_write(RES, bcm2835.LOW);
            bcm2835.bcm2835_delay(100);
            bcm2835.bcm2835_gpio_write(RES, bcm2835.HIGH);
            bcm2835.bcm2835_delay(100);

            while(true) 
            {
                //usleep(10000);        /* do it anyway ; settle time when pen goes up */
                pen_irq = bcm2835.bcm2835_gpio_lev(T_IRQ);
                if (pen_irq == bcm2835.LOW)     { /* P1.22 == PenIRQ is LOW : touch! pen is down */
                        xpt.xptGetxy(T_CS, ref x, ref y);

                        x-=150;
                        y-=150;

                        int max_X = 1750;
                        int max_Y = 1750;

                        int x2 = (x*240)/max_X;
                        int y2 = (y*320)/max_Y;

                        x2=240-x2;
                        y2=320-y2;

                        into.processClick(x2, y2);

                        // 876543210
                        //         1
                        //         2
                        //         3

                        // max x=1750 y=1015
                        // min x=0    y=0
                }
            }

            bcm2835.bcm2835_close();
        }
    }
    public static class bcm2835
    {
        // import touchscreen functions
        [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
        public static extern int bcm2835_init();

        [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
        public static extern void bcm2835_close();

        [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
        public static extern void bcm2835_gpio_fsel(int pin, int pin2);

        [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
        public static extern int bcm2835_gpio_lev(int pin);

        [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
        public static extern void bcm2835_gpio_set_pud(int pin, int state);

        

        [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
        public static extern void bcm2835_delay(int time);

        [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
        public static extern void bcm2835_gpio_write(int pin, int pin2);

        


        // import vars
        public static int RPI_V2_GPIO_P1_11;
        public static int RPI_V2_GPIO_P1_22=25;
        public static int BCM2835_GPIO_FSEL_INPT=0;
        public static int BCM2835_GPIO_FSEL_OUTP=1;
        public static int BCM2835_SPI_CS1=1;
        public static int BCM2835_GPIO_PUD_UP=2;
        public static int BCM2835_GPIO_PUD_DOWN=1;
        public static int BCM2835_GPIO_PUD_OFF=0;
        


        // low and high
        public static int HIGH=1;
        public static int LOW=0;

        

    }
}