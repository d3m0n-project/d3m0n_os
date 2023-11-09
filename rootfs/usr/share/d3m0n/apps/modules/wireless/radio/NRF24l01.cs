// using System;
// using System.IO;
// using System.Text;
// using System.Threading;
// using System.Runtime.InteropServices;
// using System.Threading.Tasks;


// namespace d3m0n_modules
// {
//     public partial class wireless
//     {
//         public static void main()
//         {

//             int CE_PIN = 22;
//             int CSN_PIN = 8;
//             RF24 nrf24 = new RF24(CE_PIN, CSN_PIN);

//             int num_channels = 126;
//             byte[] values = new byte[num_channels];

//             int num_reps = 100;
//             int reset_array = 0;


//             nrf24.begin();
//             nrf24.setAutoAck(false);

//             // Get into standby mode
//             nrf24.startListening();
//             nrf24.stopListening();

//             nrf24.printDetails();

//             // Print out header, high then low digit
//             int i = 0;

//             while (i < num_channels) {
//                 Console.WriteLine(i.ToString());
//                 //Console.WriteLine(i >> 4);
//                 i++;
//             }
//             Console.WriteLine("");

//             i = 0;
//             while (i < num_channels) {
//                 Console.WriteLine("aaa !");
//                 //Console.WriteLine("%x", i & 0xf);
//                 i++;
//             }
//             Console.WriteLine("");

//             // forever loop
//             while (true) {
//                 // Clear measurement values
//                 //memset(values, 0, sizeof(values));
//                 values = new byte[num_channels];

//                 // Scan all channels num_reps times
//                 int rep_counter = num_reps;
//                 while (rep_counter-- > 0) {

//                     int i2 = num_channels;
//                     while (i2--  > 0) {

//                         // Select this channel
//                         nrf24.setChannel(i2);

//                         // Listen for a little
//                         nrf24.startListening();
//                         System.Threading.Thread.Sleep(128);
//                         nrf24.stopListening();

//                         // Did we get a carrier?
//                         if (nrf24.testCarrier()) {
//                             values[i2]++;
//                         }
//                     }
//                 }

//                 // Print out channel measurements, clamped to a single hex digit
//                 i = 0;
//                 while (i < num_channels) {
//                     if (values[i] != null) {
//                         //Console.WriteLine("%x", min(0xf, (values[i] & 0xf)));
//                         Console.WriteLine(values[i].ToString());
//                     } else {
//                         Console.WriteLine("-");
//                     }
//                     i++;
//                 }
//                 Console.WriteLine("");
//             }
//         }
//     }
//     class RF24 {
//         [DllImport("/usr/lib/librf24.so", SetLastError = true)]
//         public extern RF24(int ce, int cs);

//         [DllImport("/usr/lib/librf24.so", SetLastError = true)]
//         public static extern void begin();

//         [DllImport("/usr/lib/librf24.so", SetLastError = true)]
//         public static extern void setAutoAck(bool value);

//         [DllImport("/usr/lib/librf24.so", SetLastError = true)]
//         public static extern void startListening();

//         [DllImport("/usr/lib/librf24.so", SetLastError = true)]
//         public static extern void stopListening();

//         [DllImport("/usr/lib/librf24.so", SetLastError = true)]
//         public static extern void printDetails();

//         [DllImport("/usr/lib/librf24.so", SetLastError = true)]
//         public static extern void setChannel(int channel);
        
//         [DllImport("/usr/lib/librf24.so", SetLastError = true)]
//         public static extern bool testCarrier();

//     }
// }