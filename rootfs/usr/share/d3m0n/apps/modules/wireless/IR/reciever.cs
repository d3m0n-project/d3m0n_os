using System;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using display.bcm2835;
// using d3m0n;

namespace d3m0n_modules
{
    public partial class IR
    {
        public static int pin = 27;
        


        // public static string read()
        public static void Recieve()
        {
            //Initialize gpio 
            bcm2835_init();
            bcm2835_delay(100);
            bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
            bcm2835_delay(100);

            Console.WriteLine("waiting for signal...");

            while (true)
            {
                // Read the value of the pin
                int value = bcm2835_gpio_lev(pin);
                    
                // Print the received value
                Console.WriteLine($"IR Signal Value: {value}");

                // Add a delay to control the reading frequency
                System.Threading.Thread.Sleep(100);
            }
            bcm2835_close();
            
        }
    }
}