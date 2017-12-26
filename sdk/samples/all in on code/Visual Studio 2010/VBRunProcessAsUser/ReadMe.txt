======================================================================================
              Windows APPLICATION: VBRunProcessAsUser Overview
======================================================================================

//////////////////////////////////////////////////////////////////////////////////////
Summary:

The code sample demonstrates how to run a process as a different user. 


//////////////////////////////////////////////////////////////////////////////////////
Demo:

Step1. Build this project in Visual Studio 2010. 

Step2. Run VBRunProcessAsUser.exe.

Step3. Fill out the User Name, Domain (if it is an Active Directory user account), and 
       Password textboxes for the user that you want to run as.  Alternatively, you can 
       click the "..." button next to the User Name textbox. It will prompt a standard 
       user credential collection dialog.  You can fill out the user name and password 
       in it too.
       
Step4. Click the "Command..." button and select the program that you want to run as the 
       specified user in Step 3.

Step5. Click the "Run Command" button to run the program as the specified user.  When 
       the process is started successfully, you will see a message box saying "the 
       process xxx started".  You can verify that the process is run as the specified 
       user in Task Manager.  When you exit the new process, you will see another 
       message box saying "the process xxx exited".

//////////////////////////////////////////////////////////////////////////////////////
Code Logic:

1. The sample uses the "Process.Start" function to implement running programs with 
   different users. 

            Try
            If (Not String.IsNullOrEmpty(tbUserName.Text)) AndAlso _
                (Not String.IsNullOrEmpty(tbPassword.Text)) AndAlso _
                (Not String.IsNullOrEmpty(tbCommand.Text)) Then
                Dim password As SecureString = StringToSecureString(Me.tbPassword.Text.ToString())
                Dim proc As Process = Process.Start(tbCommand.Text.ToString(), _
                                                    tbUserName.Text.ToString(), _
                                                    password, _
                                                    tbDomain.Text.ToString())
                ProcessStarted(proc.Id)

                proc.EnableRaisingEvents = True
                AddHandler proc.Exited, AddressOf ProcessExited
            Else
                MessageBox.Show("Please fill in the user name, password and command")
                Return
            End If
        Catch w32e As System.ComponentModel.Win32Exception
            ProcessStartFailed(w32e.Message)
        End Try
   
2. The sample uses the C++/CLI library "Kerr.Credentials" that wraps the call of the 
   native API CredUIPromptForCredentials to gather the user credential. The 
   Kerr.Credentials library provided by Kenny Kerr is downloaded from this MSDN 
   article: http://www.microsoft.com/indonesia/msdn/credmgmt.aspx.
   
        Try
            Using dialog As New Kerr.PromptForCredential()
                dialog.Title = "Please Specify the user"
                dialog.DoNotPersist = True
                dialog.ShowSaveCheckBox = False
                dialog.TargetName = Environment.MachineName
                dialog.ExpectConfirmation = True

                If DialogResult.OK = dialog.ShowDialog(Me) Then
                    tbPassword.Text = SecureStringToString(dialog.Password)
                    Dim strSplit() As String = dialog.UserName.Split("\"c)
                    If (strSplit.Length = 2) Then
                        Me.tbUserName.Text = strSplit(1)
                        Me.tbDomain.Text = strSplit(0)
                    Else
                        Me.tbUserName.Text = dialog.UserName
                    End If
                End If
            End Using
        Catch ex As Exception
            MessageBox.Show(ex.ToString())
        End Try

3. After the new process is started, we register its Exited event so that we can be 
   notified when the process exits.

        proc.EnableRaisingEvents = True
        AddHandler proc.Exited, AddressOf ProcessExited

      	''' <summary>
        ''' Triggered when the target process is exited.
        ''' </summary>
        Private Sub ProcessExited(ByVal sender As Object, ByVal e As EventArgs)
        Dim proc As Process = TryCast(sender, Process)
        If proc IsNot Nothing Then
            MessageBox.Show("Process " & proc.Id.ToString() & " exited")
        End If
        End Sub

//////////////////////////////////////////////////////////////////////////////////////
References:

Process.Start
http://msdn.microsoft.com/en-us/library/system.diagnostics.process.start.aspx

Credential Management with the .NET Framework 2.0
http://www.microsoft.com/indonesia/msdn/credmgmt.aspx


//////////////////////////////////////////////////////////////////////////////////////


