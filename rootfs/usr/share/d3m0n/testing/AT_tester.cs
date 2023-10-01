// set pin
// AT+CPIN=“2608“


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

    Thread.Sleep(5000);

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
      Console.Write(inputstring.length());
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
        Serial1.Write(intNumber); //can only write values between 0-255
      }

      //----------------------------------------------------------------//
      //send a number as dec

      else if(inputstring.StartsWith("#D") || inputstring.StartsWith("#d")) { //allows you to send dec value
        Console.WriteLine(inputstring);
        string Substring = inputstring.Substring(2);  //remove "#h" part
        var intNumber = Substring.toInt();
        Serial1.Write(intNumber); //can only write values between 0-255
      }

      //----------------------------------------------------------------//
      //send the CR character

      else if(inputstring.StartsWith("<cr>") || inputstring.StartsWith("<CR>")) { //allows you to send dec value
        Serial1.Write(0xD); //can only write values between 0-255
      }

      //----------------------------------------------------------------//
      //send the line feed or newline charcater

      else if(inputstring.StartsWith("<lf>") || inputstring.StartsWith("<LF>")) { //allows you to send dec value
        Serial1.Write(0xA); //can only write values between 0-255
      }

      //----------------------------------------------------------------//
      //sends out a string without '$' or CR, LF characters

      else if(inputstring.StartsWith("$")) { //allows you to send text
        Console.WriteLine(inputstring); 
        string Substring = inputstring.Substring(1);  //remove the $ char
        Serial1.Write(Substring);
        upperCaseStatus = false;
      }

      //----------------------------------------------------------------//
      //at command

      else if(inputstring.StartsWith("at") || inputstring.StartsWith("AT")){
        inputstring.ToUpper();
        Console.WriteLine("AT command: "+inputstring);
    
        Serial1.Write(inputstring);
        Serial1.Write(0xD); //carriage return : important --> \r
        Serial1.Write(0xA); //newline                     --> \n
        upperCaseStatus = true;
      }

      //----------------------------------------------------------------//
      //at command
      
      else if(inputstring.StartsWith(">")){
        inputstring = "AT+" + inputstring.Substring(1);
        inputstring.ToUpper();
        Console.WriteLine(inputstring);
    
        Serial1.Write(inputstring);
        Serial1.Write(0xD); //carriage return : important
        // Serial1.Write(0xA); //newline
        upperCaseStatus = true;
      }

      //----------------------------------------------------------------//
    }

    listenToGsm(upperCaseStatus);
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
      Serial1.WriteLine("AT+CMGF=1"); //access text mode
      Thread.Sleep(500);
      listenToGsm(true);  //we must wait after each command
      Console.WriteLine();
      Console.WriteLine("-- Input --");
      Console.WriteLine("AT+CMGS=\"+33687476125\"");
      Serial1.WriteLine("AT+CMGS=\"+33687476125\""); //set phone number
      Thread.Sleep(500);
      listenToGsm(true);
      Console.WriteLine();
      Console.WriteLine("-- Input --");
      Console.WriteLine("Je t'aime papa ❤");
      Console.WriteLine("\n");
      Serial1.Write("Je t'aime papa ❤"); //sms content
      Serial1.Write(26);  //substitute char to end message
      Thread.Sleep(500);
      listenToGsm(false);  //wait for the confirmation
      Console.WriteLine("");
    }
  }

  //================================================================//
  //listen to GSM module port

  public static void listenToGsm(bool ToUpper) {
    if(Serial1.IsOpen()) {  //monitor the serial 1 interface
      inputstring = Serial1.ReadLine();  //read the contents of serial buffer as string
      Console.WriteLine();
      Console.Write("-- Response (");
      Console.Write(inputstring.length());
      Console.WriteLine(") --");
      if(ToUpper)
        inputstring.ToUpper();
      Console.Write(inputstring); 
    }
  }
}