======================================================================================
              Windows APPLICATION: CSRunProcessAsUser Overview                        
======================================================================================

//////////////////////////////////////////////////////////////////////////////////////
Summary:

The code sample demonstrates how to run a process as a different user. 


//////////////////////////////////////////////////////////////////////////////////////
Demo:

Step1. Build this project in Visual Studio 2010. 

Step2. Run CSRunProcessAsUser.exe.

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
Implementation:

1. The sample uses the "Process.Start" function to implement running programs with 
   different users. 

            try
            {
                // Check the parameters.
                if (!string.IsNullOrEmpty(tbUserName.Text) &&
                    !string.IsNullOrEmpty(tbPassword.Text) &&
                    !string.IsNullOrEmpty(tbCommand.Text))
                {
                    SecureString password = StringToSecureString(tbPassword.Text);
                    Process proc = Process.Start(
                        this.tbCommand.Text,
                        this.tbUserName.Text,
                        password,
                        this.tbDomain.Text);

                    ProcessStarted(proc.Id);

                    proc.EnableRaisingEvents = true;
                    proc.Exited += new EventHandler(ProcessExited);
                }
                else
                {
                    MessageBox.Show("Please fill in the user name, password and command");
                }
            }
            catch (Win32Exception w32e)
            {
                ProcessStartFailed(w32e.Message);
            }
   
2. The sample uses the C++/CLI library "Kerr.Credentials" that wraps the call of the 
   native API CredUIPromptForCredentials to gather the user credential. The 
   Kerr.Credentials library provided by Kenny Kerr is downloaded from this MSDN 
   article: http://www.microsoft.com/indonesia/msdn/credmgmt.aspx.
   
            try
            {
                using (Kerr.PromptForCredential dialog = new Kerr.PromptForCredential())
                {
                    dialog.Title = "Please specify the user";
                    dialog.DoNotPersist = true;
                    dialog.ShowSaveCheckBox = false;
                    dialog.TargetName = Environment.MachineName;
                    dialog.ExpectConfirmation = true;

                    if (DialogResult.OK == dialog.ShowDialog(this))
                    {
                        tbPassword.Text = SecureStringToString(dialog.Password);
                        string[] strSplit = dialog.UserName.Split('\\');
                        if (strSplit.Length == 2)
                        {
                            this.tbUserName.Text = strSplit[1];
                            this.tbDomain.Text = strSplit[0];
                        }
                        else
                        {
                            this.tbUserName.Text = dialog.UserName;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString(), "Error");
            }

3. After the new process is started, we register its Exited event so that we can be 
   notified when the process exits.

        proc.EnableRaisingEvents = true;
        proc.Exited += new EventHandler(ProcessExited);

        /// <summary>
        /// Triggered when the target process is exited.
        /// </summary>
        private void ProcessExited(object sender, EventArgs e)
        {
            Process proc = sender as Process;
            if (proc != null)
            {
                MessageBox.Show("Process " + proc.Id.ToString() + " exited");
            }
        }


//////////////////////////////////////////////////////////////////////////////////////
References:

Process.Start
http://msdn.microsoft.com/en-us/library/system.diagnostics.process.start.aspx

Credential Management with the .NET Framework 2.0
http://www.microsoft.com/indonesia/msdn/credmgmt.aspx


//////////////////////////////////////////////////////////////////////////////////////


