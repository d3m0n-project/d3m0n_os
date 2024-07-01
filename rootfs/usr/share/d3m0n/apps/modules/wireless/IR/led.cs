using System;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using d3m0n;
using RaspberryPiDotNet;


namespace d3m0n_modules
{
    public partial class IR
    {
        public static void play(int[] partition)
        {
            GPIOMem led = new GPIOMem(GPIOPins.GPIO_17, GPIODirection.In);
            foreach(int sleep_time in partition)
            {
                System.Threading.Thread.Sleep(sleep_time);
                led.Write(PinState.High);
                System.Threading.Thread.Sleep(1);
                led.Write(PinState.High);
            }
        }
    }
}