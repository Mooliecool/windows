//-----------------------------------------------------------------------------
//
//  Win32
//
//-----------------------------------------------------------------------------

using System;
using System.Runtime.InteropServices;
using System.Security;
using System.Security.Permissions;

public class Win32
{
    //
    //  Structures
    //
    
    [StructLayout(LayoutKind.Sequential)]
    public struct SIZE
    {
        public int cx;
        public int cy;
    };

    [StructLayout(LayoutKind.Sequential)]
    public struct RECT
    {
        public int left;
        public int top;
        public int right;
        public int bottom;
    };


    //
    //  Functions
    //

    [DllImport("user32.dll")]
    public static extern IntPtr GetDC(IntPtr hWnd);

    [DllImport("user32.dll")]
    public static extern void ReleaseDC(IntPtr hWnd, IntPtr hdc);



    [DllImport("user32.dll")]
    public static extern int DrawCaption(
    IntPtr hwnd,    // handle to window
    IntPtr hdc,      // handle to device context
    ref RECT lprc, // rectangle to draw into
     int uFlags   // drawing options
    );

    [DllImport("user32.dll")]
    public static extern int GetClientRect(
        IntPtr hwnd,
        ref RECT rect);

    [DllImport("gdi32.dll")]
    public static extern IntPtr CreateRectRgn(
        int left,
        int top,
        int right,
        int bottom);

    [DllImport("gdi32.dll")]
    public static extern IntPtr CreateRoundRectRgn(
        int left,
        int top,
        int right,
        int bottom,
        int widthOfEllipse,   // width of the ellipse used to create the rounded corners
        int heightOfEllipse); 

    [DllImport("user32.dll")]
    public static extern int SetWindowRgn(
        IntPtr hwnd,
        IntPtr hrgn,
        bool redrawWindow);

}
