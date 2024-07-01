using System;
using System.Threading;
using System.IO;
 
namespace d3m0n
{
    public static class pin_manager
    {

        public static void setPinMode(int pin, bool input)
        {
            if(input)
            {
               File.WriteAllText("/sys/class/gpio/gpio"+pin.ToString()+"/direction", "in");
            }
            else
            {
                File.WriteAllText("/sys/class/gpio/gpio"+pin.ToString()+"/direction", "out");
            }
            
        }

        public static int getPinValue(int pin)
        {
            return Int32.Parse(File.ReadAllText("/sys/class/gpio/gpio"+pin.ToString()+"/value"));
        }
        public static void setPinValue(int pin, int input)
        {
            if(input == 0 || input == 1)
            {
                File.WriteAllText("/sys/class/gpio/gpio"+pin.ToString()+"/value", input.ToString());
            }
            else
            {
                throw new Exception("Use a binary value (0 for off and 1 for on)");
            }
        }

        public static void sleep(int time)
        {
            Thread.Sleep(time);
        }
    }
}