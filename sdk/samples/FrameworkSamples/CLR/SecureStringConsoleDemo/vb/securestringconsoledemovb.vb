'---------------------------------------------------------------------
'  This file is part of the Microsoft .NET Framework SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'---------------------------------------------------------------------

Imports System
Imports System.Security.Permissions
Imports System.Globalization
Imports System.Security
Imports System.Runtime.InteropServices
Imports System.Diagnostics

<Assembly: CLSCompliant(True)> 
<Assembly: ComVisible(False)> 
<Assembly: FileIOPermissionAttribute(SecurityAction.RequestMinimum)> 
Namespace Microsoft.Samples
    Public Class SecureStringConsoleDemo
        Private loginName As String
        Private mainCursorLeft As Integer
        Private mainCursorTop As Integer
        Private password As SecureString

        <STAThread()> _
        Shared Sub Main()
            Dim initialWindowWidth As Integer = Console.WindowWidth
            Dim initialWindowHeight As Integer = Console.WindowHeight
            Dim initialBufferWidth As Integer = Console.BufferWidth
            Dim initialBufferHeight As Integer = Console.BufferHeight
            Dim originalBack As ConsoleColor = Console.BackgroundColor
            Dim originalFore As ConsoleColor = Console.ForegroundColor
            Console.ForegroundColor = ConsoleColor.White
            Console.BackgroundColor = ConsoleColor.Black
            Dim sscd As New SecureStringConsoleDemo()
            sscd.Run()

            Console.SetWindowSize(1, 1)
            Console.SetBufferSize(initialBufferWidth, initialBufferHeight)
            Console.SetWindowSize(initialWindowWidth, initialWindowHeight)
            Console.ForegroundColor = originalFore
            Console.BackgroundColor = originalBack
            Console.Clear()
        End Sub

        Private Sub Run()
            ' set the buffersize to a width that allows us to move the screen...
            ' ShowPoint: SetWindowSize, SetBufferSize
            Console.SetWindowSize(1, 1)
            Console.SetBufferSize(100, 17)
            Console.SetWindowSize(45, 16)
            'Console.SetBufferSize(Console.WindowWidth * 2, Console.WindowHeight + 2)

            ' ShowPoint: Change Title, Change CursorSize
            Console.Title = "Fanatical Health Entry System"
            Console.CursorSize = 100

            Do While (True)

                PaintMainScreen()

                ' ShowPoint: get a key, asit's pressed
                Select Case Console.ReadKey().KeyChar

                    Case "1"c
                        If (LogInOut()) Then

                            Console.Clear()
                        Else

                            Console.CursorVisible = False
                            Console.Clear()
                            Console.SetCursorPosition(2, 7)
                            Console.ForegroundColor = ConsoleColor.White
                            Console.Write("Login unsuccessful: canceling application")
                            System.Threading.Thread.Sleep(2500)
                            Return
                        End If

                    Case "2"c
                        If (CheckLoggedIn()) Then
                            PaintPatientScreen()
                        End If

                    Case "3"c
                        If (CheckLoggedIn()) Then
                            OpenDoctorFile()
                        End If

                    Case "4"c
                        If (CheckLoggedIn()) Then
                            OpenNurseFile()
                        End If

                    Case "5"c
                        Return
                    Case Else
                        Console.Clear()

                End Select
            Loop
        End Sub

        Private Function CheckLoggedIn() As Boolean

            If loginName Is Nothing Then

                ' ShowPoint: Clear the screen
                Console.Clear()
                PaintBorder()
                Console.CursorVisible = False
                Console.SetCursorPosition(5, 5)
                Console.ForegroundColor = ConsoleColor.Cyan
                Console.Write("You must login first!!!")
                ' ShowPoint: beep!
                Console.Beep()
                Dim sw As New Stopwatch()
                sw.Start()
                Do While (sw.ElapsedMilliseconds < 2500)
                Loop
                Return False
            Else

                Return True
            End If
        End Function

        Private Sub PaintMainScreen()

            ' ShowPoint: Make the cursor invisible
            Console.CursorVisible = False
            Console.Clear()
            PaintBorder()
            Console.SetCursorPosition(5, 1)
            Dim top As Integer = 1
            WriteEntry("", ConsoleColor.Magenta, False, top)
            top = top + 1
            WriteEntry("Welcome to Fanatical Health!", ConsoleColor.Green, True, top)
            top = top + 1

            If loginName Is Nothing Then

                WriteEntry("", ConsoleColor.Magenta, False, top)
                top = top + 1
                WriteEntry(" You are currently not logged in", ConsoleColor.Red, True, top)
                top = top + 1
            Else
                WriteEntry("", ConsoleColor.Magenta, False, top)
                top = top + 1
                WriteEntry(" You are currently logged in as " + loginName, ConsoleColor.Red, True, top)
                top = top + 1
            End If

            WriteEntry("", ConsoleColor.Magenta, False, top)
            top = top + 1
            WriteEntry(" Options:", ConsoleColor.Yellow, False, top)
            top = top + 1
            If loginName Is Nothing Then
                WriteEntry(String.Format(CultureInfo.InvariantCulture, " 1. {0}", "Login"), ConsoleColor.White, False, top)
                top = top + 1
                WriteEntry(" 2. Patient Data", ConsoleColor.Gray, False, top)
                top = top + 1
                WriteEntry(" 3. Open Doctor Data File", ConsoleColor.Gray, False, top)
                top = top + 1
                WriteEntry(" 4. Open Nurse Data File", ConsoleColor.Gray, False, top)
                top = top + 1
                WriteEntry(" 5. Exit", ConsoleColor.White, False, top)
            Else
                WriteEntry(String.Format(CultureInfo.InvariantCulture, " 1. {0}", "Logout"), ConsoleColor.White, False, top)
                top = top + 1
                WriteEntry(" 2. Patient Data", ConsoleColor.White, False, top)
                top = top + 1
                WriteEntry(" 3. Open Doctor Data File", ConsoleColor.White, False, top)
                top = top + 1
                WriteEntry(" 4. Open Nurse Data File", ConsoleColor.White, False, top)
                top = top + 1
                WriteEntry(" 5. Exit", ConsoleColor.White, False, top)
                top = top + 1
            End If

            Console.ForegroundColor = ConsoleColor.White
            Console.SetCursorPosition(1, Console.WindowHeight - 1)
            Console.Write("Please enter a selection (1 - 5) ==> ")
            mainCursorLeft = Console.CursorLeft
            mainCursorTop = Console.CursorTop
            Console.CursorVisible = True
        End Sub

        Private Sub PaintBorder()
            Console.SetCursorPosition(0, 0)
            ' ShowPoint: Change the screen color
            Console.ForegroundColor = ConsoleColor.Magenta
            Console.Write("*********************************************")
            Console.SetCursorPosition(0, Console.WindowHeight - 3)
            Console.Write("*********************************************")
            For i As Integer = 1 To Console.WindowHeight - 2
                Console.SetCursorPosition(0, i)
                Console.Write("*")
                Console.SetCursorPosition(Console.WindowWidth - 1, i)
                Console.Write("*")
            Next
        End Sub

        Private Sub WriteEntry(ByVal entry As String, ByVal color As ConsoleColor, ByVal enterMidway As Boolean, ByVal top As Integer)

            Console.ForegroundColor = ConsoleColor.Magenta
            Console.ForegroundColor = color
            Dim firstWidth As Integer
            Dim secondWidth As Integer
            If enterMidway Then

                firstWidth = CInt(Math.Ceiling((43D - entry.Length) / 2) + entry.Length)
                secondWidth = (CInt(Math.Floor((43D - entry.Length) / 2)))

            Else

                firstWidth = entry.Length
                secondWidth = 43 - entry.Length
            End If

            Console.SetCursorPosition(1, top)
            Console.Write("{0," & firstWidth & "}{1," & secondWidth & "}", entry, "")
            Console.ForegroundColor = ConsoleColor.Magenta
        End Sub

        Private Function LogInOut() As Boolean
            If loginName Is Nothing Then
                Return LogIn()
            Else

                loginName = Nothing
                Return True
            End If
        End Function


        Private Function LogIn() As Boolean

            password = New SecureString()

            Dim name As String = Nothing
            Dim top As Integer = 5
            Dim count As Integer
            Dim prompt As String = "Login name ==> "

            Console.Clear()
            PaintBorder()
            Console.ForegroundColor = ConsoleColor.White
            Console.SetCursorPosition(3, top)
            top = top + 1

            Console.Write(prompt)
            Console.ForegroundColor = ConsoleColor.Magenta
            ' task: turn this into non-blocking code, like the code below
            name = Console.ReadLine()

            Console.ForegroundColor = ConsoleColor.White
            Console.SetCursorPosition(3, top)
            Console.Write("Password ==> ")
            Console.ForegroundColor = ConsoleColor.Magenta

            Do While (True)

                ' ShowPoint: intercept the keypress,
                ' so we can blank out the password with asterisks
                Dim cki As ConsoleKeyInfo = Console.ReadKey(True)
                'SecureString s = new SecureString();

                If cki.Key = ConsoleKey.Enter Then

                    count = count + 1
                    If (LoginValid(name, prompt)) Then

                        loginName = name
                        Return True

                    Else

                        If count >= 3 Then
                            Return False
                        End If

                        Console.ForegroundColor = ConsoleColor.White
                        Console.SetCursorPosition(3, top)
                        Console.Write("{0,41}", " ")
                        Console.SetCursorPosition(3, top)
                        Console.Write("Password ==> ")
                        Console.ForegroundColor = ConsoleColor.Magenta
                        password.Clear()
                    End If

                ElseIf cki.Key = ConsoleKey.Backspace Then

                    ' remove the last asterisk from the screen...
                    If (password.Length > 0) Then
                        ReplaceEntry(" ", Console.CursorLeft - 1, Console.CursorTop)
                        password.RemoveAt(password.Length - 1)
                    End If

                ElseIf cki.Key = ConsoleKey.Escape Then

                    Console.Clear()
                    PaintMainScreen()
                    Return True

                ElseIf Char.IsLetterOrDigit(cki.KeyChar) Or Char.IsSymbol(cki.KeyChar) Then

                    If (password.Length < 15) Then
                        password.AppendChar(cki.KeyChar)
                        Console.Write("*")
                    Else
                        Console.Beep()
                    End If
                Else
                    Console.Beep()
                End If
            Loop
        End Function

        Private Function LoginValid(ByVal name As String, ByVal prompt As String) As Boolean

            Dim tokenHandle As IntPtr = IntPtr.Zero
            Const LOGON32_PROVIDER_DEFAULT As Integer = 0
            Const LOGON32_LOGON_INTERACTIVE As Integer = 2

            Try
                tokenHandle = Marshal.SecureStringToGlobalAllocUnicode(password)
                If (NativeMethods.LogonUser(name, "", Marshal.SecureStringToGlobalAllocUnicode(password), _
                 LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, tokenHandle)) Then

                    Return True

                Else
                    Console.SetCursorPosition(3, 5)
                    Console.Write("{0,41}", " ")
                    Console.SetCursorPosition(3, 5)
                    Console.ForegroundColor = ConsoleColor.White
                    Console.Write(prompt)
                    Console.ForegroundColor = ConsoleColor.Magenta
                    Console.Write(name)
                    Console.ForegroundColor = ConsoleColor.Red
                    Console.SetCursorPosition(5, 8)
                    Console.Write("That password is invalid!")

                    Dim sw As New Stopwatch()

                    Dim count As Integer

                    Console.Beep()
                    Do While (count < 2)

                        sw.Start()

                        If sw.ElapsedMilliseconds > 100 Then

                            Console.Beep()
                            count = count + 1
                        End If

                    Loop
                    Console.SetCursorPosition(5, 8)
                    Console.Write("{0,38}", " ")
                End If
            Finally

                If tokenHandle <> IntPtr.Zero Then
                    Marshal.ZeroFreeGlobalAllocUnicode(tokenHandle)
                End If
            End Try
            Return False
        End Function

        Private Sub ReplaceEntry(ByVal replacement As String, ByVal left As Integer, ByVal top As Integer)

            Console.SetCursorPosition(left, top)
            Console.Write(replacement)
            Console.SetCursorPosition(left, top)
        End Sub

        Private Sub PaintPatientScreen()

            Console.CursorVisible = False
            MoveMainScreen(True)
            Console.SetCursorPosition(0, 0)

            Do While (True)

                PaintBorder()
                Dim top As Integer = 1
                WriteEntry("", ConsoleColor.Magenta, False, top)
                top = top + 1
                WriteEntry("Patient Maintenance Screen!", ConsoleColor.Green, True, top)
                top = top + 1

                If loginName Is Nothing Then
                    WriteEntry("", ConsoleColor.Magenta, False, top)
                    top = top + 1
                    WriteEntry(" You are currently not logged in", ConsoleColor.Red, True, top)
                    top = top + 1
                Else
                    WriteEntry("", ConsoleColor.Magenta, False, top)
                    top = top + 1
                    WriteEntry(" You are currently logged in as " + loginName, ConsoleColor.Red, True, top)
                    top = top + 1
                End If

                WriteEntry("", ConsoleColor.Magenta, False, top)
                top = top + 1
                WriteEntry(" Options:", ConsoleColor.Yellow, False, top)
                top = top + 1
                WriteEntry(" 1. New Patient", ConsoleColor.White, False, top)
                top = top + 1
                WriteEntry(" 2. View Existing Patient", ConsoleColor.White, False, top)
                top = top + 1
                WriteEntry(" 3. Patient in Trouble!", ConsoleColor.White, False, top)
                top = top + 1
                WriteEntry(" 4. Exit", ConsoleColor.White, False, top)
                top = top + 1

                WriteEntry("", ConsoleColor.Magenta, False, top)
                top = top + 1

                Console.ForegroundColor = ConsoleColor.White
                Console.SetCursorPosition(1, Console.WindowHeight - 1)
                Console.Write("Please enter a selection (1 - 4) ==> ")
                Console.CursorVisible = True

                Dim cki As ConsoleKeyInfo = Console.ReadKey()

                If cki.Key = ConsoleKey.D1 Or _
                           cki.Key = ConsoleKey.D2 Or _
                           cki.Key = ConsoleKey.D3 Then

                    ShowNotImplementedScreen("Please enter a selection (1 - 4) ==> ")

                ElseIf cki.Key = ConsoleKey.Escape Or cki.Key = ConsoleKey.D4 Then

                    MoveMainScreen(False)
                    Console.SetCursorPosition(mainCursorLeft, mainCursorTop)
                    Return
                End If
            Loop
        End Sub

        Private Sub MoveMainScreen(ByVal moveOff As Boolean)

            Dim sw As New Stopwatch()

            If moveOff Then
                For i As Integer = 0 To Console.WindowWidth

                    Console.MoveBufferArea(i, 0, Console.WindowWidth, Console.WindowHeight, i + 1, 0)
                    sw.Start()
                    Do While (sw.ElapsedMilliseconds < 50)
                    Loop
                    sw.Reset()
                Next

            Else
                For i As Integer = Console.WindowWidth To 1 Step -1
                    Console.MoveBufferArea(i, 0, Console.WindowWidth, Console.WindowHeight, i - 1, 0)
                    sw.Start()
                    Do While (sw.ElapsedMilliseconds < 50)

                    Loop
                    sw.Reset()
                Next
            End If
        End Sub

        Private Sub ShowNotImplementedScreen(ByVal line As String)

            Console.Clear()
            PaintBorder()
            Dim cursorVisible As Boolean = Console.CursorVisible
            Console.CursorVisible = False
            Dim cursorLeft As Integer = Console.CursorLeft
            Dim cursorTop As Integer = Console.CursorTop
            Console.SetCursorPosition(3, 7)
            Console.WriteLine(line)
            System.Threading.Thread.Sleep(3000)
            Console.SetCursorPosition(cursorLeft, cursorTop)
            Console.Clear()
            Console.CursorVisible = cursorVisible
        End Sub

        Private Sub OpenDoctorFile()
            ' this code attempts to open the doctor file, for the current user.
            ' if the user has access to the file, then notepad will open, and you will see the file
            ' If they DON'T have access (simulating that they aren't a doctor), then notepad
            ' will still run, but the file won't open

            ' See the instructions in the Doctor file for simulating 
            ' someone not being allowed to open the file
            Dim p As New Process()
            Dim psi As New ProcessStartInfo()
            psi.Password = password
            psi.UserName = loginName
            psi.UseShellExecute = False

            psi.FileName = "notepad"
            psi.Arguments = Environment.CurrentDirectory + "\doctorFile.txt"
            p.StartInfo = psi

            p.Start()
        End Sub

        Private Sub OpenNurseFile()
            ' this code attempts to open the doctor file, for the current user.
            ' if the user has access to the file, then notepad will open, and you will see the file
            ' If they DON'T have access (simulating that they aren't a doctor), then notepad
            ' will still run, but the file won't open

            ' See the instructions in the Nurse file for simulating someone being allowed to open the file
            Dim p As New Process()
            Dim psi As New ProcessStartInfo()
            psi.Password = password
            psi.UserName = loginName
            psi.UseShellExecute = False

            psi.FileName = "notepad"
            psi.Arguments = Environment.CurrentDirectory + "\nurseFile.txt"
            p.StartInfo = psi

            p.Start()
        End Sub
    End Class
End Namespace
