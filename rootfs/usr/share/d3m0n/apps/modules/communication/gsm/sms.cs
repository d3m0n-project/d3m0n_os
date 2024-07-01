using System;
using System.IO.Ports;
using System.Threading.Tasks;

namespace d3m0n_modules
{
    public class sms
    {
        // public string PhoneNumber { get; set; }

        // public async void send(string message)
        // {
        //     gsm Serial1 = new gsm();
        //     Serial1.println("AT+CMGF=1"); //access text mode
        //     await Task.Delay(500);
        //     Serial1.listenToGsm(true);  //we must wait after each command
        //     Serial1.println("AT+CMGS=\""+PhoneNumber+"\""); //set phone number
        //     await Task.Delay(500);
        //     Serial1.listenToGsm(true);
        //     Serial1.print(message); //sms content
        //     Serial1.write(26);  //substitute char to end message
        //     await Task.Delay(500);
        //     Serial1.listenToGsm();  //wait for the confirmation
        // }
    }
}