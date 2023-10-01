using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Collections.Generic;

namespace d3m0n
{
	public partial class script
	{
		public static void callFunction(string command, string[] value)
		{
			string app_path = Interpreter.currentRunningAppPath;

			while(command.StartsWith(' ') || command.EndsWith(' '))
			{
				command=command.TrimStart(' ').TrimEnd(' ');
			}

			// 1. Functions to read data from a user (input) or display data to the user (output).
			// Variables and Data Types:

			// 2. Functions to declare variables and specify their data types (e.g., integers, floating-point numbers, strings, etc.).
			// Control Flow:

			// 3. Functions for controlling the flow of program execution, including conditionals (if, else if, else), loops (for, while, do-while), and branching (switch/case).
			// Functions and Methods:

			// 4. Functions to define reusable blocks of code, often with parameters and return values.
			// Error Handling:

			// 5. Functions and constructs to handle errors and exceptions that may occur during program execution.
			// Arrays and Collections:

			// 6. Functions for creating and manipulating collections of data, such as arrays, lists, dictionaries, etc.
			// Strings:

			// 7. Functions for manipulating and formatting strings of text, including concatenation, substring, length, etc.
			// Mathematical Operations:

			// 8. Functions for performing mathematical operations like addition, subtraction, multiplication, division, etc.
			// File Handling:

			// 9. Functions for reading from and writing to files, including functions to open, close, read, and write data to files.
			// Object-Oriented Concepts:

			// 10. Functions for creating and working with classes, objects, inheritance, polymorphism, and encapsulation.
			// Memory Management:

			// 11. Functions for managing memory allocation and deallocation, especially in languages without automatic garbage collection.
			// Concurrency and Threading:

			// 12. Functions for creating and managing concurrent processes or threads for parallel execution.
			// Sorting and Searching:

			// 13. Functions for sorting and searching data efficiently, using various algorithms and techniques.
			// Networking:

			// 14. Functions for creating network connections, sending and receiving data over networks.
			// Time and Date:

			// 15. Functions for manipulating and formatting dates and times.
			// Debugging and Logging:

			// 16. Functions for logging information for debugging and monitoring the application.
			// Type Casting and Conversion:

			// 17. Functions for converting between different data types (e.g., integer to string, string to integer).


			
			// try every type of embeded in functions
			interraction(command, value);
			system(command, value);
			pin(command, value);
			window(command, value);
			application(command, value);


			// call a user def
			if(File.Exists(app_path+"/defs/"+command))
			{
				foreach(string line in File.ReadAllText(app_path+"/defs/"+command).Split(";"))
				{
					execute(line+";");
				}	
				
			}
		}
	}
}