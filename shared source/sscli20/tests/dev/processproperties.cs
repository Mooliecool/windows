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

// Test for properties of class Process

using System;
using System.IO;
using System.Diagnostics;
using System.Threading;

class My
{
    const int NUMIT = 5;
#if PLATFORM_UNIX
    const string CHILDPROC = "sleep";
    const string CHILDARGS = "5";
#else
    const string CHILDPROC = "ping";
    const string CHILDARGS = "127.0.0.1";
#endif

    static int ExitNotifications = 0;

    public void MyProcessExited(Object source, EventArgs e)
    {
       lock (this) { ++ExitNotifications; }      
    }

    static void Main(string[] args)
    { 
        My my = new My();

        try {
            my.Go();
        }
        catch (Exception e)
        {
            Console.WriteLine ("Unexpected exception: " + e.ToString());
            Environment.Exit(1);
        }

        Environment.Exit(0);
    }  

    void Go()
    {
        int[] pid = new int[NUMIT];
        IntPtr[] prochandles = new IntPtr[NUMIT];
        Process proc;

        for (int i=0;i<NUMIT;i++)
        {     
            Console.WriteLine ("Iteration: " + i);
            if (i == 0)
            {
                proc = Process.GetCurrentProcess();
            }
            else
            {
                ProcessStartInfo psi = new ProcessStartInfo (CHILDPROC, CHILDARGS);
                //
                // Enable stdout and stderr redirection for odd iterations
                //
                psi.RedirectStandardOutput = true;
                if ((i % 2) != 0)
                {             
                    psi.RedirectStandardError = true;
                }
                psi.UseShellExecute = false;
                proc = Process.Start(psi);
            }

            //
            // Enable events for even iterations
            //
            proc.EnableRaisingEvents = ((i%2) == 0);
            
            // 
            // Fetch exit code too early and get an exception
            //
            int ival;
            try {
               ival = proc.ExitCode;
               throw new Exception ("Expected InvalidOperationException somehow not thrown");
            }
            catch (InvalidOperationException)
            {
                // 
                // Expected InvalidOperationException suppressed
                //
            }

            // 
            // Target process should be still running
            //
            if (proc.HasExited)
                throw new Exception ("Target process exited too early");


            // 
            // Get PID and perform some minimal check on it
            //
            pid[i] = proc.Id;
            if (proc.Id == pid[(i==0) ? 1 : 0])
                throw new Exception ("Invalid PID returned");    

            // 
            // Get Process Handle and perform some minimal check on it
            //
            prochandles[i] = proc.Handle;
            if (proc.Handle == prochandles[(i==0) ? 1 : 0])
                throw new Exception ("Invalid Process Handle returned");    

            // 
            // Get machine name; here it should always be "."
            //
            if (proc.MachineName != ".")
                throw new Exception ("Invalid machine name: '" + proc.MachineName + "' (expected: '.')");

            // 
            // Play with StandardError redirection
            //
            StreamReader streamreader;
            try {
               //
               // Expect InvalidOperationException on even iterations and
               // no exception on odd ones
               //
               streamreader = proc.StandardError;
               if ((i % 2) == 0)
               {
                  throw new Exception ("Expected InvalidOperationException somehow not thrown");
               }
            }
            catch (InvalidOperationException e)
            {            
                if ((i % 2) != 0)
                {
                    //
                    // An InvalidOperationException should not have been
                    // thrown on odd iterations: rethrowing it
                    //
                    throw e;
                }
                // 
                // Expected InvalidOperationException suppressed
                //
            }


            if (i > 0)
            {
                // 
                // Get redirected StandardOutput
                //
                streamreader = proc.StandardOutput;
            }

            //
            // Play with StandardInput redirection
            //
            StreamWriter streamwriter;
            try {
               streamwriter = proc.StandardInput;
               throw new Exception ("Expected InvalidOperationException somehow not thrown");
            }
            catch (InvalidOperationException)
            {
                // 
                // Expected InvalidOperationException suppressed
                //
            }

            //
            // Retrieve ProcessStartInfo and do a basic verification
            //
            ProcessStartInfo pstartinfo = proc.StartInfo;
            if (i!=0)
            {
                if (pstartinfo.FileName != CHILDPROC)
                    throw new Exception ("Invalid pstartinfo.FileName: got '" + 
                        pstartinfo.FileName + "', expected '" + CHILDPROC + "'");
                if (pstartinfo.Arguments != CHILDARGS)
                    throw new Exception ("Invalid pstartinfo.Arguments: got '" + 
                        pstartinfo.Arguments + "', expected '" + CHILDARGS + "'");
            }

            if (i > 0)
            {
                //
                // Set an exit event handler that should be called only 
                // on even iterations, when EnableRaisingEvents is set to true
                //
                proc.Exited += new EventHandler(MyProcessExited);

                if (i > 1)
                {
                    //
                    // Kill the process on iterations > 1
                    //
                    proc.Kill();
                }

                //
                // Retrieve the redirected stdout
                //
                string output = proc.StandardOutput.ReadToEnd();          
                
                //
                // Wait for exit
                //
                proc.WaitForExit();

                if ((i%2) == 0)
                {
                    // Note: Getting any of the two following properties causes
                    //       the event handler to be called anyway, even if
                    //       EnableRaisingEvents is set to false.
                    // 
                    // Target process should be done by now
                    //
                    if (!proc.HasExited)
                         throw new Exception ("Target process should have exited");
                    // 
                    // Retrieve the exit code 
                    //            
                    ival = proc.ExitCode;
                }
            }
        }

        // Allow some time for the EventHandler to be called
        Thread.Sleep(1000);     

        lock (this) 
        { 
            if (ExitNotifications != NUMIT/2) 
                throw new Exception ("Event handler called " + ExitNotifications + " times, expected " + NUMIT/2);
        }
    }
}

