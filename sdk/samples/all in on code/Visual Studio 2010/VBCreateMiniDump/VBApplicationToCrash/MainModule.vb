'*************************** Module Header ******************************'
' Module Name:  MainModule.vb
' Project:	    VBApplicationToCrash
' Copyright (c) Microsoft Corporation.
' 
' When this application starts, it will launch a Watchdog process. It will also
' create an unhandled exception.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Imports System.Runtime.InteropServices

Module MainModule

    Sub Main()
        Dim demoProcess As Process = Process.GetCurrentProcess()
        Console.WriteLine("The ID of this Demo Process is " & demoProcess.Id)

        LaunchWatchdog(demoProcess)

        ' Wait 2 seconds, so the watch dog process can attach a debugger to this 
        ' application.
        System.Threading.Thread.Sleep(2000)

        Console.WriteLine("Press ENTER to throw an unhandled exception...")
        Console.ReadLine()

        Try
            Dim zero As Integer = 0

            ' This exception will be handled by the catch block, so the watch dog
            ' will not create a minidump at this moment.
            Console.WriteLine(1 \ zero)

        Catch ex As Exception

            Console.WriteLine(ex.GetType())

            ' Rethrowing the exception will cause an unhandled exception, and the watch
            ' dog will create a minidump now.
            Throw

        End Try
    End Sub

    ''' <summary>
    ''' Launch the Watchdog process.
    ''' </summary>
    ''' <param name="demoProcess"></param>
    Sub LaunchWatchdog(ByVal demoProcess As Process)
        Try
            Console.WriteLine("Launch Watchdog process...")
            Dim start As ProcessStartInfo = New ProcessStartInfo With _
                                            {.Arguments = demoProcess.Id.ToString(),
                                             .FileName = "VBCreateMiniDump.exe"}
            Dim miniDumpCreatorProcess As Process = Process.Start(start)
            Console.WriteLine("The Watchdog process was launched!")
        Catch ex As Exception
            Console.WriteLine("The Watchdog process was not launched!")
            Console.WriteLine(ex.Message)
        End Try
    End Sub

End Module
