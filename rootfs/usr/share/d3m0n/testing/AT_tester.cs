// set pin
// AT+CPIN="0000"

// +COPS: 
// +COPS: <mode><format><operator>,
// <mode>
// 0 automatic
// 1 manual
// 2 de-register from network
// 4 manual/automatic
// <operator>
// 0 long alphanumeric format
// 1 short alphanumeric format
// 2 numeric format










using System;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.IO.Ports;

class Program
{
  static SerialPort Serial1;

  private static string inputstring = "";
  private static string lastCommand = "";

  private static bool upperCaseStatus = false; //keep tracks of whether incoming string needs to be converted to upper case

  private static string portName = "/dev/ttyAMA0";
  private static int baudRate = 115200;

  public static void Main(string[] args) {
    Serial1 = new SerialPort(portName, baudRate);
    Serial1.WriteTimeout = 2000;
    Serial1.ReadTimeout = 2000;
    

    Thread.Sleep(5000);
    while(!Serial1.IsOpen) {
      Console.WriteLine("Waiting for port to open");
      Serial1.Open();
      System.Threading.Thread.Sleep(250);
    }

    SerialWriteLine("AT+CPIN=\"2608\"");

    Task.Run(() => {listenToGsm();});

    Console.WriteLine();
    Console.WriteLine("## GA6 GSM Module - AT Tester ##");
    Console.WriteLine("--------------------------------");
    Console.WriteLine();
    Console.WriteLine("-- Usage --");
    Console.WriteLine();
    Console.WriteLine("Start AT commands with \"at\" or \"AT\" (<CR> is automatically appended)");
    Console.WriteLine("Use \">\" shorthand for \"AT+\" (eg. \">CREG?\")");
    Console.WriteLine("Start string with \"$\" (eg. \"$message\")");
    Console.WriteLine("Start decimal number with \"#d\" or \"#D\" (eg. \"#D42\")");
    Console.WriteLine("Start hex number with \"#h\" or \"#H\" (eg. \"#H42\")");
    Console.WriteLine("Send carriage return with \"<cr>\" or \"<CR>\"");
    Console.WriteLine("Send newline with \"<lf>\" or \"<LF>\"");
    Console.WriteLine("Start custom commands with \"/\" (eg. \"/command\")");
    Console.WriteLine();
    Thread.Sleep(1000);
    
    while(true)
    {
      inputstring = Console.ReadLine();  //read the contents of serial buffer as string
      Console.WriteLine();
      Console.Write("-- Input (");
      Console.Write(inputstring.Length.ToString());
      Console.WriteLine(") --");

      lastCommand = inputstring;  //just a backup of the original string

      //----------------------------------------------------------------//
      //custom commands

      if(inputstring.StartsWith("/")) { //allows you to send non-AT commands
        Console.WriteLine(inputstring+" let's go"); 
        processCustomCmd();
      }
      
      //----------------------------------------------------------------//
      //send a number as hex

      else if(inputstring.StartsWith("#H") || inputstring.StartsWith("#h")) { //allows you to send hex value
        Console.WriteLine(inputstring);
        string Substring = inputstring.Substring(2);  //remove "#h" part
        byte[] hexstring = Encoding.ASCII.GetBytes(Substring); //convert string object to byte array
        var intNumber = BitConverter.ToInt32(hexstring, 16); // convert Byte[] to int
        //            intNumber
        SerialWrite(Substring); //can only write values between 0-255
      }

      //----------------------------------------------------------------//
      //send a number as dec

      else if(inputstring.StartsWith("#D") || inputstring.StartsWith("#d")) { //allows you to send dec value
        Console.WriteLine(inputstring);
        string Substring = inputstring.Substring(2);  //remove "#h" part
        var intNumber = Int32.Parse(Substring);
        SerialWrite(Substring); //can only write values between 0-255 intNumber
      }

      //----------------------------------------------------------------//
      //send the CR character

      else if(inputstring.StartsWith("<cr>") || inputstring.StartsWith("<CR>")) { //allows you to send dec value
        SerialWrite("\r"); //can only write values between 0-255 0xD
      }

      //----------------------------------------------------------------//
      //send the line feed or newline charcater

      else if(inputstring.StartsWith("<lf>") || inputstring.StartsWith("<LF>")) { //allows you to send dec value
        SerialWrite("\n"); //can only write values between 0-255 0xA
      }

      //----------------------------------------------------------------//
      //sends out a string without '$' or CR, LF characters

      else if(inputstring.StartsWith("$")) { //allows you to send text
        Console.WriteLine(inputstring); 
        string Substring = inputstring.Substring(1);  //remove the $ char
        SerialWrite(Substring);
        upperCaseStatus = false;
      }

      //----------------------------------------------------------------//
      //at command

      else if(inputstring.StartsWith("at") || inputstring.StartsWith("AT")){
        inputstring.ToUpper();
        Console.WriteLine("AT command: "+inputstring);
    
        SerialWrite(inputstring);
        SerialWrite("\r"); //carriage return : important --> \r 0xD
        SerialWrite("\n"); //newline                     --> \n 0xA
        upperCaseStatus = true; 
      }

      //----------------------------------------------------------------//
      //at command
      
      else if(inputstring.StartsWith(">")){
        inputstring = "AT+" + inputstring.Substring(1);
        inputstring.ToUpper();
        Console.WriteLine(inputstring);
    
        SerialWrite(inputstring);
        SerialWrite("\r"); //carriage return : important 0xD
        // SerialWrite(0xA); //newline
        upperCaseStatus = true;
      }

      //----------------------------------------------------------------//
    }
    ToUpper = upperCaseStatus;
    //listenToGsm(upperCaseStatus);
  }

  //================================================================//
  //custom commands start with "/"

  public static void processCustomCmd() {
    //add your custom command handles below
    Console.WriteLine("inputstring: '"+inputstring+"'");
    if(inputstring.StartsWith("/send sms")) {
      Console.WriteLine();
      Console.WriteLine("-- Input --");
      Console.WriteLine("AT+CMGF=1");
      SerialWriteLine("AT+CMGF=1"); //access text mode
      Thread.Sleep(500);
      ToUpper = true;
      //listenToGsm(true);  //we must wait after each command
      Console.WriteLine();
      Console.WriteLine("-- Input --");
      Console.WriteLine("AT+CMGS=\"phonehere\"");
      SerialWriteLine("AT+CMGS=\"phonehere\""); //set phone number
      Thread.Sleep(500);
      ToUpper = true;
      //listenToGsm(true);
      Console.WriteLine();
      Console.WriteLine("-- Input --");
      Console.WriteLine("message sent");
      Console.WriteLine("\n");
      SerialWrite("Bisous maman"); //sms content
      SerialWrite("\u001a");  //substitute char to end message 0x26
      Thread.Sleep(500);
      ToUpper = false;
      //listenToGsm(false);  //wait for the confirmation
      Console.WriteLine("");
    }
  }

  //================================================================//
  //listen to GSM module port

  public static void SerialWrite(string text)
  {
    try{
      Serial1.Write(text);
    }
    catch(Exception)
    {
      Console.Write("X");
    }
    
  }

  public static void SerialWriteLine(string text)
  {
    try{
      Serial1.WriteLine(text);
    }
    catch(Exception)
    {
      Console.Write("X");
    }
    
  }

  public static bool ToUpper = false;
  public static async void listenToGsm() {
      while (true) {
        if(Serial1.IsOpen) {  //monitor the serial 1 interface
          try {
            inputstring = Serial1.ReadLine();  //read the contents of serial buffer as string
            if(inputstring.Length>0 && inputstring.StartsWith("#H")) {
              Console.WriteLine();
              Console.Write("-- Response (");
              Console.Write(inputstring.Length.ToString());
              Console.WriteLine(") --");
              if(ToUpper)
                inputstring.ToUpper();
              Console.Write(inputstring);
            }
             
          }
          catch(Exception) {
            Console.WriteLine(@"/!\ bad wireing /!\");
          }
          
        }
      }
    }
    
}
