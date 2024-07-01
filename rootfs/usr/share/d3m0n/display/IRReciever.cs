using System;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using display;
// using d3m0n;

namespace d3m0n_modules
{
    public partial class IR
    {
        public static int pin = 27;
        public static int[] LogList = new int[32];
        public static ulong StartTime;
        public static int EndTime, End2Time;
        public static int FlagCode = 0;
        public static bool IrState = true;


        // public static string read()
        public static void Recieve()
        {
            //Initialize gpio 
            bcm2835.bcm2835_init();

            bcm2835.bcm2835_gpio_fsel(pin, bcm2835.BCM2835_GPIO_FSEL_INPT);
            bcm2835.bcm2835_gpio_set_pud(pin, bcm2835.BCM2835_GPIO_PUD_UP);

            System.Threading.Thread.Sleep(100);

            Console.WriteLine("waiting for signal...");

            while (true)
            {
                // Read the value of the pin
                int value = bcm2835.bcm2835_gpio_lev(pin);
                    
                // Print the received value
                Console.Write($"{value}");

                // Add a delay to control the reading frequency
                System.Threading.Thread.Sleep(100);
            }
            bcm2835.bcm2835_close();
            
        }

        private static void IR_Read()
        {

        }
    }
}