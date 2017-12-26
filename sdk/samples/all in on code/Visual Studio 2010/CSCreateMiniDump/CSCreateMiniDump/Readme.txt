========================================================================
    CONSOLE APPLICATION : CSCreateMiniDump Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The code sample demonstrates how to programmatically create a minidump when a 
.NET application has an unhandled exception and is about to crash.

When an application runs, it will launch a Watchdog process that can debug the application
and subscribe the OnException event of the application. If there is an unhandled exception,
the Watchdog process will create a minidump for the application.

The reason why we need a watch dog process is that MiniDumpWriteDump should be called 
from a separate process if at all possible, rather than from within the target process
being dumped. This is especially true when the target process is already not stable. 
For example, if it just crashed. A loader deadlock is one of many potential side effects 
of calling MiniDumpWriteDump from within the target process. For more detailed information, 
see http://msdn.microsoft.com/en-us/library/ms680360(VS.85).aspx.

NOTE: 
1. This sample works for managed processes only, because it uses MDbg to debug the target
   process.
2. The watch dog process must be built with the same platform as the main application. 

/////////////////////////////////////////////////////////////////////////////
Demo:

Step 1. Build this solution in Visual Studio 2010, and you will get CSApplicationToCrash.exe
        and CSCreateMiniDump.exe in the output folder.

Step 2. Run the application CSApplicationToCrash.exe. It will launch a Watchdog process
        CSCreateMiniDump.exe, and then show following message:
            "Press ENTER to throw an unhandled exception..."

Step 3. Press <Enter> in the CSApplicationToCrash, and then you will get following 
        notification in the CSCreateMiniDump.exe:
           Start to handle exception...
           Getting exception information...
           Creating Minidump...
           The minidump file is  <dump file path>
           Done...
           Press ENTER to continue...

Step 4. Press <Enter> in the CSCreateMiniDump.exe and CSApplicationToCrash.exe. Both 
        CSApplicationToCrash.exe and CSCreateMiniDump.exe will shut down, and then you 
        will get a minidump named CSApplicationToCrash_<TimeStamp>.dmp in the same folder
	    as CSApplicationToCrash.exe. 

Step 5. Open the .dmp file in VS2010, and debug with Mixed. Then you can get the call stack
        and other information when the minidump was created.

/////////////////////////////////////////////////////////////////////////////
Code logic

1 Create an application CSApplicationToCrash.exe that could crash itself. When it runs, it
  will launch a Watchdog process CSCreateMiniDump.exe.

        static void Main(string[] args)
        {
            Process demoProcess = Process.GetCurrentProcess();
            Console.WriteLine("The ID of this Demo Process is " + demoProcess.Id);

            LaunchWatchdog(demoProcess);

            for (int i = 1; i < 10; i++)
            {
                Console.WriteLine(i);
                System.Threading.Thread.Sleep(1000);
            }

            // Rethrow the exception, will cause an unhandled exception.
            try
            {
                int zero = 0;
                Console.WriteLine(1 / zero);
            }
            catch
            {     
                throw;
            }
        }



2 Create CSCreateMiniDump.exe that could create minidump.

  2.1 Wrap the MiniDumpWriteDump function in the dbghelp.dll, and design MiniDumpCreator
      class that could create a minidump for a given process.

           [DllImport("dbghelp.dll", CharSet = CharSet.Auto, SetLastError = true)]
           public  static extern bool MiniDumpWriteDump(IntPtr hProcess,
               int processId,
               SafeFileHandle hFile,
               MINIDUMP_TYPE dumpType,
               IntPtr exceptionParam,
               IntPtr userStreamParam,
               IntPtr callbackParam);

  2.2 Design ManagedProcess class that represents a managed process. It could attach
      a debugger to the managed process and subscribe the PostDebugEvent event of the process.


            void AttachDebuggerToProcess()
            {
                string version =
                    MdbgVersionPolicy.GetDefaultAttachVersion(this.DiagnosticsProcess.Id);          
                if (string.IsNullOrEmpty(version))
                {
                    throw new ApplicationException ("Can't determine what version of the CLR to " +
                        "attach to the process.");
                }
                this.MDbgProcess = this.Debugger.Attach(this.DiagnosticsProcess.Id, null, version);
                       
                bool result = this.MDbgProcess.Go().WaitOne();
           
                if (!result)
                {
                    throw new ApplicationException(
                        string.Format(@"The process with an ID {0} could not be "
                        + "attached. Operation time out.", this.DiagnosticsProcess.Id));
                }
           
                this.MDbgProcess.PostDebugEvent += 
                    new PostCallbackEventHandler(MDbgProcess_PostDebugEvent);
            }

      And then start to watch the target process. If the target application stops with an unhandled
      exception, raise an UnhandledExceptionOccurred event.

            public void StartWatch()
            {
                while (true)
                {             
                    this.MDbgProcess.Go().WaitOne();
           
                    if (this.stopReason == ManagedCallbackType.OnException
                        && this.isExceptionUnhandled)
                    {
                        this.HandleException();
                        break;
                    }
                    else if (this.stopReason == ManagedCallbackType.OnProcessExit)
                    {
                        break;
                    }
                }
            }
           
            private void HandleException()
            {
                MDbgValue ex = this.MDbgProcess.Threads.Active.CurrentException;
                if (ex.IsNull)
                {
                    // No current exception is available.  Perhaps the user switched to a different
                    // thread which was not throwing an exception.
                    return;
                }
           
                Console.WriteLine("Exception=" + ex.GetStringValue(0));
           
                IntPtr exceptionPointers = IntPtr.Zero;
           
                foreach (MDbgValue f in ex.GetFields())
                {
                    if (f.Name == "_xptrs")
                    {
                        string outputValue = f.GetStringValue(0);
                        exceptionPointers = (IntPtr)int.Parse(outputValue);
                    }
                }
           
                if (exceptionPointers == IntPtr.Zero)
                {
                    // Get the Exception Pointer in the target process
                    MDbgValue value = FunctionEval(
                        "System.Runtime.InteropServices.Marshal.GetExceptionPointers");
                    if (value != null)
                    {
                        exceptionPointers = (IntPtr)int.Parse(value.GetStringValue(1));
                    }
                }
           
                this.OnUnhandledExceptionOccurred(
                    new ManagedProcessUnhandledExceptionOccurredEventArgs
                    {
                        ProcessID = this.MDbgProcess.CorProcess.Id,
                        ThreadID = this.MDbgProcess.Threads.Active.Id,
                        ExceptionPointers = exceptionPointers
                    });
            }
      
  
  2.3 When CSCreateMiniDump.exe is launched, attach the debugger to CSApplicationToCrash.exe, and 
      create a minidump for the application CSApplicationToCrash.exe if it encounters an unhandled
	  exception.

            static void process_UnhandledExceptionOccurred(object sender,
                 ManagedProcessUnhandledExceptionOccurredEventArgs e)
             {
                 Console.WriteLine("Creating Minidump...");
                 MiniDump.MiniDumpCreator.CreateMiniDump(e.ProcessID, 
                     e.ThreadID, e.ExceptionPointers);
                 Console.WriteLine("Done...");
             }    
         


/////////////////////////////////////////////////////////////////////////////
References:

MINIDUMP_CALLBACK_INPUT Structure:
http://msdn.microsoft.com/en-us/library/ms680362(v=VS.85).aspx


MINIDUMP_CALLBACK_TYPE Enumeration:
http://msdn.microsoft.com/en-us/library/ms680364(v=VS.85).aspx

MINIDUMP_CALLBACK_OUTPUT Structure:
http://msdn.microsoft.com/en-us/library/ms680363(v=VS.85).aspx

Effective MiniDumps:
http://www.debuginfo.com/articles/effminidumps.html

MSDN try-except statement:
http://msdn.microsoft.com/en-us/library/s58ftw19(VS.80).aspx


/////////////////////////////////////////////////////////////////////////////