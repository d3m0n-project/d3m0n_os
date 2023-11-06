using System;
using System.IO;
using System.Runtime.InteropServices;
using RaspberryPiDotNet.dll;


namespace d3m0n_touchscreen
{
    class manager
    {
        const string LibPath = "/usr/share/d3m0n/apps/lib/libbcm2835.so";

        [DllImport(LibPath, EntryPoint = "bcm2835_spi_begin")]
        static extern void spi_begin();

        [DllImport(LibPath, EntryPoint = "bcm2835_spi_end")]
        static extern void spi_end();

        [DllImport(LibPath, EntryPoint = "bcm2835_spi_transfer")]
        static extern byte spi_transfer(byte val);

        [DllImport(LibPath, EntryPoint = "bcm2835_spi_chipSelect")]
        static extern byte spi_chipSelect(GPIOPins pin);

        [DllImport(LibPath, EntryPoint = "bcm2835_spi_setClockDivider")]
        static extern byte spi_setClockDivider(int val);

        [DllImport(LibPath, EntryPoint = "bcm2835_spi_setDataMode")]
        static extern byte spi_setDataMode(int val);

        [DllImport(LibPath, EntryPoint = "bcm2835_spi_setChipSelectPolarity")]
        static extern byte spi_setChipSelectPolarity(GPIOPins pin, bool activeHigh);


        // BCM2835_GPIO_FSEL_INPT
        // BCM2835_GPIO_FSEL_OUTP
        // LOW
        // HIGH





        private int T_CS = 26;
        private int RESET = ;
        private int T_IRQ = 22;


        public static void Main(string[] args){
                int x, y;
                int pen_irq;

                if (bcm2835_init() != 1) return;
                bcm2835_gpio_fsel(T_IRQ,BCM2835_GPIO_FSEL_INPT); // T_IRQ
                bcm2835_gpio_fsel(RES,BCM2835_GPIO_FSEL_OUTP); // RES
                bcm2835_gpio_write(RES, LOW);
                bcm2835_delay(100);
                bcm2835_gpio_write(RES, HIGH);
                bcm2835_delay(100);

                while(true) 
                {
                    usleep(10000);        /* do it anyway ; settle time when pen goes up */
                    pen_irq = bcm2835_gpio_lev(T_IRQ);
                    if (pen_irq == LOW)     { /* P1.22 == PenIRQ is LOW : touch! pen is down */
                            xptGetxy(T_CS, x, y);
                            Console.WriteLine("x="+x.ToString()+" y="+y.ToString());
                    } else { 
                        /* pen is up */
                    }
                } // end for

                bcm2835_close();
        }
    }
}

// #define T_CS BCM2835_SPI_CS1
// #define RES RPI_V2_GPIO_P1_11
// #define T_IRQ RPI_V2_GPIO_P1_22

