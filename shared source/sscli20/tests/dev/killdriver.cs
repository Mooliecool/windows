// ==++==
//
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
//
// ==--==

// driver for Process.GetCurrentProcess().Kill test

using System;
using System.IO;
using System.Diagnostics;
using System.Threading;

public class MainClass
{
  public static void Main(string [] args)
  {
    string filename = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "killself.exe");

    Process p = Process.Start(filename);
    if (p.WaitForExit(Timeout.Infinite) != true) {
        Console.WriteLine("Process.WaitForExit returned unexpected result");
        Environment.Exit(1);
    }
    int code = p.ExitCode;
    Console.WriteLine("ExitCode: " + code.ToString());
    if (code != -1 && code != 255) {
        Console.WriteLine("Process.ExitCode returned unexpected result");
        Environment.Exit(1);
    }
    Environment.Exit(0);
  }
}
