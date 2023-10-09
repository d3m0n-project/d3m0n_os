using System;
using System.IO.Ports;
using d3m0n;

namespace d3m0n_modules
{
    public partial class gsm
    {
        static SerialPort serialPort;


        private static string portName = "/dev/ttyAMA0";
        private static int baudRate = 115200;

        public static void init()
        {
            serialPort = new SerialPort(portName, baudRate);
        }

        public static void println(string text)
        {
            if(serialPort == null)
            {
                utils.logn("[x] error module => gsm.cs: use d3m0n_modules.gsm.init() first", ConsoleColor.Red);
                return;
            }

            try
            {
                serialPort.Open();
                serialPort.WriteLine(text);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error: "+ex.Message);
            }
            finally
            {
                if (serialPort.IsOpen)
                    serialPort.Close();
            }
        }
        public static void print(string text)
        {
            if(serialPort == null)
            {
                utils.logn("[x] error module => gsm.cs: use d3m0n_modules.gsm.init() first", ConsoleColor.Red);
                return;
            }

            try
            {
                serialPort.Open();
                serialPort.Write(text);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error: "+ex.Message);
            }
            finally
            {
                if (serialPort.IsOpen)
                    serialPort.Close();
            }
        }
        public static void listenToGsm(bool toUpperCase) {
            if(serialPort.IsOpen) {  //monitor the serial 1 interface
                string inputString = serialPort.ReadExisting();  //read the contents of serial buffer as string
                Console.WriteLine();
                Console.Write("-- Response (");
                Console.Write(inputString.Length);
                Console.WriteLine(") --");
                if(toUpperCase)
                {
                    inputString.ToUpper();
                    Console.Write(inputString); 
                }
            }
        }
    }
}