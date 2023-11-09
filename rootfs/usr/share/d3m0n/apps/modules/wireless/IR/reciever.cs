using System;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
// using d3m0n;
using RaspberryPiDotNet;

namespace d3m0n_modules
{
    public partial class IR
    {
        //Initialize gpio 
        GPIOMem reciever = new GPIOMem(GPIOPins.GPIO_17, GPIODirection.In);


        // public static string read()
        public static void Main()
        {
            Console.WriteLine(reciever.Read().ToString());
            Console.WriteLine("waiting for signal...");

            while(true)
            {
                if(reciever.Read().ToString() != "Low")
                {
                    Console.Write("1");
                }
            }
            
        }
    }
}