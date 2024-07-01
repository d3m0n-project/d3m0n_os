using System;
using System.IO;
using System.Text;
using System.Threading;
// using Mono.Unix.Native;
using System.Threading.Tasks;
using d3m0n;

namespace d3m0n_modules
{
    public partial class usb
    {
        // if don't work = maye bad file write "File.OpenWrite instead
        public async void badUsb(string payload)
        {
            int currentline=0;
            foreach(string line in payload.Split("\r\n"))
            {
                currentline++;
                try
                {
                    if(line.ToUpper().StartsWith("GUI")) { report("GUI"); }
                    if(line.ToUpper().StartsWith("STRING ")) {
                        foreach (char c in line.Remove(0, 7))
                        {    
                            report(c.ToString()); 
                        }
                    }
                    if(line.ToUpper().StartsWith("DELAY ")) { await Task.Delay(Int32.Parse(line.Remove(0, 6))); }
                }
                catch(Exception)
                {
                    utils.logn("[x] badUSB => an error occured at line "+currentline.ToString()+" '"+line+"'", ConsoleColor.Red);
                }
                
            }
        }
        private static void report(string text)
        {
            // Open the /dev/hidg0 file for writing
            // using (var hidg0 = File.OpenWrite("/dev/hidg0"))
            // {
            //     // Define a simple HID report for a key press
            //     byte[] hidReport = new byte[8];
            //     // hidReport[0] = 0x02; // Modifier key (Shift)
            //     hidReport[2] = keycodes_fr[text];

            //     // Write the HID report to /dev/hidg0
            //     hidg0.Write(hidReport, 0, hidReport.Length);
            // }
        }
    }
    
}