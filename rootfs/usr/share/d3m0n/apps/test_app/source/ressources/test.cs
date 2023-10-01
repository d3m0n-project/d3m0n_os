using System;
using System.Windows.Forms;

namespace myNamespace
{
	public class myclass
	{
		public static void myFunct(string text, int integ)
		{
			MessageBox.Show(text+(50+integ).ToString());
		}
	}
}