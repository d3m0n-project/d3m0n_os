using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using System.Diagnostics;


namespace d3m0n
{
	class embed
	{

		[DllImport("user32.dll", SetLastError = true)]
	    private static extern bool MoveWindow(IntPtr hwnd, int x, int y, int cx, int cy, bool repaint);
	    [DllImport("user32.dll", SetLastError = true)]
	    static extern IntPtr SetParent(IntPtr hWndChild, IntPtr hWndNewParent);

	    public static Process process = new Process();

		public static void embed_process(string path, Control ctrl)
		{
			process.StartInfo.FileName = path;

	        process.Start();

	        // IntPtr ptr = IntPtr.Zero;
	        // while ((ptr = process.MainWindowHandle) == IntPtr.Zero) ;
	        // SetParent(process.MainWindowHandle, ctrl.Handle);
	        // MoveWindow(process.MainWindowHandle, 0, 25, 480, (640 - 25), true);
		}
		public static void embed_stop()
		{
			// SetParent(process.MainWindowHandle, IntPtr.Zero);
			process.Close();

		}
	}
}