========================================================================
    CONSOLE APPLICATION : VBCreateMiniDump Project Overview
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

Step 1. Build this solution in Visual Studio 2010, and you will get VBApplicationToCrash.exe
        and VBCreateMiniDump.exe in the output folder.

Step 2. Run the application VBApplicationToCrash.exe. It will launch a Watchdog process
        VBCreateMiniDump.exe, and then show following message:
            "Press ENTER to throw an unhandled exception..."

Step 3. Press <Enter> in the VBApplicationToCrash, and then you will get following 
        notification in the VBCreateMiniDump.exe:
           Start to handle exception...
           Getting exception information...
           Creating Minidump...
           The minidump file is  <dump file path>
           Done...
           Press ENTER to continue...

Step 4. Press <Enter> in the VBCreateMiniDump.exe and VBApplicationToCrash.exe. Both 
        VBApplicationToCrash.exe and VBCreateMiniDump.exe will shut down, and then you 
        will get a minidump named VBApplicationToCrash_<TimeStamp>.dmp in the same folder
	    as VBApplicationToCrash.exe. 

Step 5. Open the .dmp file in VS2010, and debug with Mixed. Then you can get the call stack
        and other information when the minidump was created.

/////////////////////////////////////////////////////////////////////////////
Code logic

1. Create an application VBApplicationToCrash.exe that could crash itself. When it runs, it
   will launch a Watchdog process VBCreateMiniDump.exe.

            Sub Main()
                Dim demoProcess As Process = Process.GetCurrentProcess()
                Console.WriteLine("The ID of this Demo Process is " & demoProcess.Id)
            
                LaunchWatchdog(demoProcess)
            
                For i As Integer = 1 To 10
                    Console.WriteLine(i)
                    System.Threading.Thread.Sleep(1000)
                Next i
            
                ' Rethrow the exception, will cause an unhandled exception.
                Try
                    Dim zero As Integer = 0
                    Console.WriteLine(1 \ zero)
                Catch
                    Throw
                End Try
            End Sub


2. Create VBCreateMiniDump.exe that could create minidump.

   2.1 Wrap the MiniDumpWriteDump function in the dbghelp.dll, and design MiniDumpCreator
       class that could create a minidump for a given process.
   
             <DllImport("dbghelp.dll", CharSet:=CharSet.Auto, SetLastError:=True)>
             Public Shared Function MiniDumpWriteDump(ByVal hProcess As IntPtr,
                                                 ByVal processId As Integer,
                                                 ByVal hFile As SafeFileHandle,
                                                 ByVal dumpType As MINIDUMP_TYPE,
                                                 ByVal exceptionParam As IntPtr,
                                                 ByVal userStreamParam As IntPtr,
                                                 ByVal callbackParam As IntPtr) As Boolean
             End Function
   
   2.2 Design ManagedProcess class that represents a managed process. It could attach
       a debugger to the managed process and subscribe the PostDebugEvent event of the process.
   
   
              Private Sub AttachDebuggerToProcess()

                   Dim version As String = MdbgVersionPolicy.GetDefaultAttachVersion(Me.DiagnosticsProcess.Id)
              
                   If String.IsNullOrEmpty(version) Then
                       Throw New ApplicationException("Can't determine what version of the CLR to " _
                                                       & "attach to the process.")
                   End If
              
                   Me.MdbgProcess = Me.Debugger.Attach(Me.DiagnosticsProcess.Id, Nothing, version)
              
                   Dim result As Boolean = Me.MdbgProcess.Go.WaitOne()
              
                   If Not result Then
                       Throw New ApplicationException(String.Format("The process with an ID {0} could not be " _
                                                                    & "attached. Operation time out.",
                                                                    Me.DiagnosticsProcess.Id))
                   End If
              
                   AddHandler MdbgProcess.PostDebugEvent, AddressOf MDbgProcess_PostDebugEvent
               End Sub
   
       And then start to watch the target process. If the target application stops with an unhandled
       exception, raise an UnhandledExceptionOccurred event.
   
               Public Sub StartWatch()
                   Do
                       Dim result As Boolean = Me.MdbgProcess.Go.WaitOne
           
                       If Me._stopReason = ManagedCallbackType.OnException _
                           AndAlso Me._isExceptionUnhandled Then
                           Me.HandleException()
                           Exit Do
                       ElseIf Me._stopReason = ManagedCallbackType.OnProcessExit Then
                           Exit Do
                       End If
                   Loop
           
               End Sub
           
               Private Sub HandleException()
           
                   Dim ex As MDbgValue = Me.MdbgProcess.Threads.Active.CurrentException
                   If ex.IsNull Then
                       ' No current exception is available.  Perhaps the user switched to a different
                       ' thread which was not throwing an exception.
                       Return
                   End If
           
                   Console.WriteLine("Exception=" & ex.GetStringValue(0))
           
                   Dim exceptionPointers As IntPtr = IntPtr.Zero
           
                   For Each f As MDbgValue In ex.GetFields()
                       If f.Name = "_xptrs" Then
                           Dim outputValue As String = f.GetStringValue(0)
                           exceptionPointers = CType(Integer.Parse(outputValue), IntPtr)
                       End If
                   Next f
           
                   If exceptionPointers = IntPtr.Zero Then
                       ' Get the Exception Pointer in the target process
                       Dim value As MDbgValue = FunctionEval(
                           "System.Runtime.InteropServices.Marshal.GetExceptionPointers")
                       If value IsNot Nothing Then
                           exceptionPointers = CType(Integer.Parse(value.GetStringValue(1)), IntPtr)
                       End If
                   End If
           
                   Me.OnUnhandledExceptionOccurred(
                       New ManagedProcessUnhandledExceptionOccurredEventArgs With _
                       {.ProcessID = Me.MdbgProcess.CorProcess.Id,
                        .ThreadID = Me.MdbgProcess.Threads.Active.Id,
                        .ExceptionPointers = exceptionPointers})
               End Sub
                  
   
   2.3 When VBCreateMiniDump.exe is launched, attach the debugger to VBApplicationToCrash.exe, and 
       create a minidump for the application VBApplicationToCrash.exe if it encounters an unhandled
	   exception.
   
               Private Sub process_UnhandledExceptionOccurred(ByVal sender As Object,
                                                              ByVal e As ManagedProcessUnhandledExceptionOccurredEventArgs)
                   Console.WriteLine("Creating Minidump...")
                   MiniDump.MiniDumpCreator.CreateMiniDump(e.ProcessID, e.ThreadID, e.ExceptionPointers)
                   Console.WriteLine("Done...")
               End Sub



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