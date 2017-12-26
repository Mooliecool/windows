'********************************* Module Header ***********************************\
' Module Name:  MainForm.vb
' Project:      VBCreateLowIntegrityProcess
' Copyright (c) Microsoft Corporation.
' 
' The code sample demonstrates how to start a low-integrity process. The application
' launches itself at the low integrity level when you click the "Launch myself at low
' integrity level" button on the application. Low integrity processes can only write 
' to low integrity locations, such as the %USER PROFILE%\AppData\LocalLow folder or 
' the HKEY_CURRENT_USER\Software\AppDataLow key. If you attempt to gain write access 
' to objects at a higher integrity levels, you will get an access denied error even 
' though the user's SID is granted write access in the discretionary access control 
' list (DACL). 
' 
' By default, child processes inherit the integrity level of their parent process. To 
' start a low-integrity process, you must start a new child process with a low-
' integrity access token by using CreateProcessAsUser. Please refer to the 
' CreateLowIntegrityProcess sample function for details.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
' EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
' MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***********************************************************************************/

#Region "Imports directives"

Imports System.ComponentModel
Imports System.Runtime.InteropServices
Imports System.IO

#End Region


Public Class MainForm

    Private Sub MainForm_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) _
        Handles MyBase.Load
        ' The sample application must be run on Windows Vista or any newer operating
        ' systems that support User Account Control (UAC). 
        If (Environment.OSVersion.Version.Major < 6) Then
            MessageBox.Show( _
                "The sample application must be run on Windows Vista or any newer " & _
                "operating systems that support User Account Control (UAC). The " & _
                "application will exit.")
            Close()
        End If

        Try
            Dim IL As Integer = Me.GetProcessIntegrityLevel
            Select Case IL
                Case NativeMethod.SECURITY_MANDATORY_UNTRUSTED_RID
                    Me.lbIntegrityLevel.Text = "Untrusted"
                Case NativeMethod.SECURITY_MANDATORY_LOW_RID
                    Me.lbIntegrityLevel.Text = "Low"
                Case NativeMethod.SECURITY_MANDATORY_MEDIUM_RID
                    Me.lbIntegrityLevel.Text = "Medium"
                Case NativeMethod.SECURITY_MANDATORY_HIGH_RID
                    Me.lbIntegrityLevel.Text = "High"
                Case NativeMethod.SECURITY_MANDATORY_SYSTEM_RID
                    Me.lbIntegrityLevel.Text = "System"
                Case Else
                    Me.lbIntegrityLevel.Text = "Unknown"
            End Select
        Catch ex As Exception
            Me.lbIntegrityLevel.Text = "N/A"
            MessageBox.Show(ex.Message, "GetProcessIntegrityLevel Error", _
                MessageBoxButtons.OK, MessageBoxIcon.Hand)
        End Try
    End Sub


    Private Sub btnCreateLowProcess_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
        Handles btnCreateLowProcess.Click
        Try
            ' Try to launch a new instance of the current application at the low 
            ' integrity level.
            CreateLowIntegrityProcess(Application.ExecutablePath)
        Catch ex As Exception
            MessageBox.Show(ex.Message, "CreateLowIntegrityProcess Error", _
                    MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try
    End Sub


    Private Sub btnWriteLocalAppData_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
        Handles btnWriteLocalAppData.Click
        Try
            ' Test file path: %USERPROFILE%\AppData\Local\testfile.txt
            Dim filePath As String = KnownFolder.LocalAppData & "\testfile.txt"

            ' Try to create and write to the test file.
            Using sw As StreamWriter = New StreamWriter(filePath)
                sw.Write("VBCreateLowIntegrityProcess Test File")
            End Using

            MessageBox.Show("Successfully write to the test file: " & filePath, _
                "Test Result", MessageBoxButtons.OK, MessageBoxIcon.Information)
        Catch ex As Exception
            MessageBox.Show(ex.Message, "Writing to LocalAppData Failed", _
                MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try
    End Sub


    Private Sub btnWriteLocalAppDataLow_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
        Handles btnWriteLocalAppDataLow.Click
        Try
            ' Test file path: %USERPROFILE%\AppData\LocalLow\testfile.txt
            Dim filePath As String = KnownFolder.LocalAppDataLow & "\testfile.txt"

            ' Try to create and write to the test file.
            Using sw As StreamWriter = New StreamWriter(filePath)
                sw.Write("VBCreateLowIntegrityProcess Test File")
            End Using

            MessageBox.Show("Successfully write to the test file: " & filePath, _
                "Test Result", MessageBoxButtons.OK, MessageBoxIcon.Information)
        Catch ex As Exception
            MessageBox.Show(ex.Message, "Writing to LocalAppDataLow Failed", _
                MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try
    End Sub


#Region "Helper Functions related to Process Integrity Level"

    ''' <summary>
    ''' The function launches an application at low integrity level. 
    ''' </summary>
    ''' <param name="commandLine">
    ''' The command line to be executed. The maximum length of this string is 32K 
    ''' characters. 
    ''' </param>
    ''' <remarks>
    ''' To start a low-integrity process, 
    ''' 1) Duplicate the handle of the current process, which is at medium integrity 
    '''    level.
    ''' 2) Use SetTokenInformation to set the integrity level in the access token to 
    '''    Low.
    ''' 3) Use CreateProcessAsUser to create a new process using the handle to the 
    '''    low integrity access token.
    ''' </remarks>
    Friend Sub CreateLowIntegrityProcess(ByVal commandLine As String)
        Dim hToken As SafeTokenHandle = Nothing
        Dim hNewToken As SafeTokenHandle = Nothing
        Dim pIntegritySid As IntPtr = IntPtr.Zero
        Dim cbTokenInfo As Integer = 0
        Dim pTokenInfo As IntPtr = IntPtr.Zero
        Dim si As New STARTUPINFO
        Dim pi As New PROCESS_INFORMATION

        Try
            ' Open the primary access token of the process.
            If (Not NativeMethod.OpenProcessToken(Process.GetCurrentProcess.Handle, _
                NativeMethod.TOKEN_DUPLICATE Or NativeMethod.TOKEN_ADJUST_DEFAULT Or _
                NativeMethod.TOKEN_QUERY Or NativeMethod.TOKEN_ASSIGN_PRIMARY, _
                hToken)) Then
                Throw New Win32Exception
            End If

            ' Duplicate the primary token of the current process.
            If (Not NativeMethod.DuplicateTokenEx(hToken, 0, IntPtr.Zero, _
                SECURITY_IMPERSONATION_LEVEL.SecurityImpersonation, _
                TOKEN_TYPE.TokenPrimary, hNewToken)) Then
                Throw New Win32Exception
            End If

            ' Create the low integrity SID.
            If Not NativeMethod.AllocateAndInitializeSid( _
                NativeMethod.SECURITY_MANDATORY_LABEL_AUTHORITY, 1, _
                NativeMethod.SECURITY_MANDATORY_LOW_RID, _
                0, 0, 0, 0, 0, 0, 0, pIntegritySid) Then
                Throw New Win32Exception
            End If

            Dim tml As TOKEN_MANDATORY_LABEL
            tml.Label.Attributes = NativeMethod.SE_GROUP_INTEGRITY
            tml.Label.Sid = pIntegritySid

            ' Marshal the TOKEN_MANDATORY_LABEL struct to the native memory.
            cbTokenInfo = Marshal.SizeOf(tml)
            pTokenInfo = Marshal.AllocHGlobal(cbTokenInfo)
            Marshal.StructureToPtr(tml, pTokenInfo, False)

            ' Set the integrity level in the access token to low.
            If (Not NativeMethod.SetTokenInformation(hNewToken, _
                TOKEN_INFORMATION_CLASS.TokenIntegrityLevel, pTokenInfo, _
                (cbTokenInfo + NativeMethod.GetLengthSid(pIntegritySid)))) Then
                Throw New Win32Exception
            End If

            ' Create the new process at the Low integrity level.
            si.cb = Marshal.SizeOf(si)
            If (Not NativeMethod.CreateProcessAsUser(hNewToken, Nothing, commandLine, _
                IntPtr.Zero, IntPtr.Zero, False, 0, IntPtr.Zero, Nothing, (si), pi)) Then
                Throw New Win32Exception
            End If

        Finally
            ' Centralized cleanup for all allocated resources. 
            If (Not hToken Is Nothing) Then
                hToken.Close()
                hToken = Nothing
            End If
            If (Not hNewToken Is Nothing) Then
                hNewToken.Close()
                hNewToken = Nothing
            End If
            If (pIntegritySid <> IntPtr.Zero) Then
                NativeMethod.FreeSid(pIntegritySid)
                pIntegritySid = IntPtr.Zero
            End If
            If (pTokenInfo <> IntPtr.Zero) Then
                Marshal.FreeHGlobal(pTokenInfo)
                pTokenInfo = IntPtr.Zero
                cbTokenInfo = 0
            End If
            If (pi.hProcess <> IntPtr.Zero) Then
                NativeMethod.CloseHandle(pi.hProcess)
                pi.hProcess = IntPtr.Zero
            End If
            If (pi.hThread <> IntPtr.Zero) Then
                NativeMethod.CloseHandle(pi.hThread)
                pi.hThread = IntPtr.Zero
            End If
        End Try
    End Sub


    ''' <summary>
    ''' The function gets the integrity level of the current process. Integrity 
    ''' level is only available on Windows Vista and newer operating systems, thus 
    ''' GetProcessIntegrityLevel throws a C++ exception if it is called on systems 
    ''' prior to Windows Vista.
    ''' </summary>
    ''' <returns>
    ''' Returns the integrity level of the current process. It is usually one of 
    ''' these values:
    ''' 
    '''    SECURITY_MANDATORY_UNTRUSTED_RID - means untrusted level. It is used by 
    '''    processes started by the Anonymous group. Blocks most write access.
    '''    (SID: S-1-16-0x0)
    '''    
    '''    SECURITY_MANDATORY_LOW_RID - means low integrity level. It is used by 
    '''    Protected Mode Internet Explorer. Blocks write acess to most objects 
    '''    (such as files and registry keys) on the system. (SID: S-1-16-0x1000)
    ''' 
    '''    SECURITY_MANDATORY_MEDIUM_RID - means medium integrity level. It is used 
    '''    by normal applications being launched while UAC is enabled. 
    '''    (SID: S-1-16-0x2000)
    '''    
    '''    SECURITY_MANDATORY_HIGH_RID - means high integrity level. It is used by 
    '''    administrative applications launched through elevation when UAC is 
    '''    enabled, or normal applications if UAC is disabled and the user is an 
    '''    administrator. (SID: S-1-16-0x3000)
    '''    
    '''    SECURITY_MANDATORY_SYSTEM_RID - means system integrity level. It is used 
    '''    by services and other system-level applications (such as Wininit, 
    '''    Winlogon, Smss, etc.)  (SID: S-1-16-0x4000)
    ''' 
    ''' </returns>
    ''' <exception cref="System.ComponentModel.Win32Exception">
    ''' When any native Windows API call fails, the function throws a Win32Exception 
    ''' with the last error code.
    ''' </exception>
    Friend Function GetProcessIntegrityLevel() As Integer
        Dim IL As Integer = -1
        Dim hToken As SafeTokenHandle = Nothing
        Dim cbTokenIL As Integer = 0
        Dim pTokenIL As IntPtr = IntPtr.Zero

        Try
            ' Open the access token of the current process with TOKEN_QUERY.
            If (Not NativeMethod.OpenProcessToken(Process.GetCurrentProcess.Handle, _
                NativeMethod.TOKEN_QUERY, hToken)) Then
                Throw New Win32Exception
            End If

            ' Then we must query the size of the integrity level information 
            ' associated with the token. Note that we expect GetTokenInformation to 
            ' return False with the ERROR_INSUFFICIENT_BUFFER error code because we 
            ' have given it a null buffer. On exit cbTokenIL will tell the size of 
            ' the group information.
            If (Not NativeMethod.GetTokenInformation(hToken, _
                TOKEN_INFORMATION_CLASS.TokenIntegrityLevel, _
                IntPtr.Zero, 0, cbTokenIL)) Then
                Dim err As Integer = Marshal.GetLastWin32Error
                If (err <> NativeMethod.ERROR_INSUFFICIENT_BUFFER) Then
                    ' When the process is run on operating systems prior to Windows 
                    ' Vista, GetTokenInformation returns false with the 
                    ' ERROR_INVALID_PARAMETER error code because TokenIntegrityLevel 
                    ' is not supported on those OS's.
                    Throw New Win32Exception(err)
                End If
            End If

            ' Now we allocate a buffer for the integrity level information.
            pTokenIL = Marshal.AllocHGlobal(cbTokenIL)
            If (pTokenIL = IntPtr.Zero) Then
                Throw New Win32Exception
            End If

            ' Now we ask for the integrity level information again. This may fail if 
            ' an administrator has added this account to an additional group between 
            ' our first call to GetTokenInformation and this one.
            If (Not NativeMethod.GetTokenInformation(hToken, _
                TOKEN_INFORMATION_CLASS.TokenIntegrityLevel, _
                pTokenIL, cbTokenIL, cbTokenIL)) Then
                Throw New Win32Exception
            End If

            ' Marshal the TOKEN_MANDATORY_LABEL struct from native to .NET object.
            Dim tokenIL As TOKEN_MANDATORY_LABEL = Marshal.PtrToStructure( _
            pTokenIL, GetType(TOKEN_MANDATORY_LABEL))

            ' Integrity Level SIDs are in the form of S-1-16-0xXXXX. (e.g. 
            ' S-1-16-0x1000 stands for low integrity level SID). There is one and 
            ' only one subauthority.
            Dim pIL As IntPtr = NativeMethod.GetSidSubAuthority(tokenIL.Label.Sid, 0)
            IL = Marshal.ReadInt32(pIL)

        Finally
            ' Centralized cleanup for all allocated resources. 
            If (Not hToken Is Nothing) Then
                hToken.Close()
                hToken = Nothing
            End If
            If (pTokenIL <> IntPtr.Zero) Then
                Marshal.FreeHGlobal(pTokenIL)
                pTokenIL = IntPtr.Zero
                cbTokenIL = 0
            End If
        End Try

        Return IL
    End Function

#End Region

End Class
