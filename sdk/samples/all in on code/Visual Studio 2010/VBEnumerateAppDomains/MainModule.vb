'*************************** Module Header ******************************\
' Module Name:  MainModule.vb
' Project:	    VBEnumerateAppDomains
' Copyright (c) Microsoft Corporation.
' 
' This source file is used to handle the input command. If this application
' starts with an argument, process the command directly and then exit, else 
' show the help text to let user choose a command.
'  
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'**************************************************************************

Imports Microsoft.Samples.Debugging.CorDebug

Module MainModule

    Sub Main(ByVal args() As String)

        ' Create new AppDomain in current process.
        AppDomain.CreateDomain("Hello world!")

        Try
            ' If this application starts without any argument, show the help text to
            ' let user choose a command.
            If args.Length = 0 Then
                ' Application will not exit until user types the exit command.
                ' If the command is not correct, it will show the help text in the
                ' next loop. 
                Do
                    Console.WriteLine(
                        ControlChars.CrLf _
                        & "Please choose a command:" & ControlChars.CrLf _
                        & "1: Show AppDomains in current process." & ControlChars.CrLf _
                        & "2: List all managed processes." & ControlChars.CrLf _
                        & "3: Show help text." & ControlChars.CrLf _
                        & "4: Exit this application." & ControlChars.CrLf _
                        & "To show the AppDomains in a specified process, " _
                        & "please type ""PID"" and" & ControlChars.CrLf _
                        & "the ID of the process directly, like PID1234.")

                    Dim cmd As String = Console.ReadLine()
                    Dim cmdID As Integer = 0
                    If Integer.TryParse(cmd, cmdID) Then
                        Select Case cmdID
                            Case 1
                                ProcessCommand("CurrentProcess")
                            Case 2
                                ProcessCommand("ListAllManagedProcesses")
                            Case 4
                                Environment.Exit(0)
                            Case Else

                                ' Show the help text in the next loop.

                        End Select
                    ElseIf cmd.StartsWith("PID", StringComparison.OrdinalIgnoreCase) Then
                        ProcessCommand(cmd)
                    End If

                Loop
            ElseIf args.Length = 1 Then
                ProcessCommand(args(0))
            End If
        Catch ex As Exception
            Console.WriteLine(ex.Message)

            ' The exit code 100 means that this application does not run successfully.           
            Environment.Exit(100)
        End Try
    End Sub

    Private Sub ProcessCommand(ByVal arg As String)
        ' List AppDomains in current process.
        If arg.Equals("CurrentProcess", StringComparison.OrdinalIgnoreCase) Then
            Console.WriteLine("List AppDomains in current process...")
            ShowAppDomainsInCurrentProcess()

            ' List all managed processes.
        ElseIf arg.Equals("ListAllManagedProcesses", StringComparison.OrdinalIgnoreCase) Then
            Console.WriteLine("List all managed processes...")
            ListAllManagedProcesses()

            ' Show the AppDomains in a specified process, arg must starts with "PID".
        ElseIf arg.StartsWith("PID", StringComparison.OrdinalIgnoreCase) Then
            Dim pid As Integer = 0
            Integer.TryParse(arg.Substring(3), pid)
            Console.WriteLine(String.Format("List AppDomains in the process {0} ...", pid))
            ShowAppDomains(pid)

        Else
            Throw New ArgumentException("Please type a valid command.")
        End If

    End Sub

    ''' <summary>
    ''' Show AppDomains in Current Process.
    ''' </summary>
    Private Sub ShowAppDomainsInCurrentProcess()

        ' GetAppDomainsInCurrentProcess is a static method of the class ManagedProcess.
        ' This method is used to get all AppDomains in Current Process.
        Dim appDomains = ManagedProcess.GetAppDomainsInCurrentProcess()

        For Each appDomain In appDomains
            Console.WriteLine("AppDomain Id={0}, Name={1}",
                              appDomain.Id, appDomain.FriendlyName)
        Next appDomain
    End Sub

    ''' <summary>
    ''' Show AppDomains in a specified process.
    ''' </summary>
    ''' <param name="pid"> The ID of the Process.</param>
    Private Sub ShowAppDomains(ByVal pid As Integer)
        If pid <= 0 Then
            Throw New ArgumentException("Please type a valid PID.")
        End If

        Dim process As ManagedProcess = Nothing
        Try
            ' GetManagedProcessByID is a static method of the class ManagedProcess.
            ' This method is used to get an instance of ManagedProcessInfo. If there is
            ' no managed process with this PID, an ArgumentException will be thrown.
            process = ManagedProcess.GetManagedProcessByID(pid)

            For Each appDomain As CorAppDomain In process.AppDomains
                Console.WriteLine("AppDomain Id={0}, Name={1}", appDomain.Id, appDomain.Name)
            Next appDomain

        Catch _argumentException As ArgumentException
            Console.WriteLine(_argumentException.Message)
        Catch _applicationException As ApplicationException
            Console.WriteLine(_applicationException.Message)
        Catch ex As Exception
            Console.WriteLine("Cannot get the process. " _
                              & " Make sure the process exists and it's a managed process")
        Finally
            If process IsNot Nothing Then
                process.Dispose()
            End If
        End Try
    End Sub

    ''' <summary>
    ''' List all managed processes.
    ''' </summary>
    Private Sub ListAllManagedProcesses()

        ' GetManagedProcesses is a static method of the class ManagedProcess.
        ' This method is used to get a list that contains all managed processes 
        ' in current machine.
        Dim processes = ManagedProcess.GetManagedProcesses()

        For Each process In processes
            Console.WriteLine("ID={0}" & vbTab & "Name={1}",
                              process.ProcessID, process.ProcessName)
            Console.Write("Loaded Runtimes: ")
            For Each _runtime In process.LoadedRuntimes
                Console.Write(_runtime.GetVersionString() & vbTab)
            Next _runtime
            Console.WriteLine(vbLf)
        Next process

    End Sub

End Module
