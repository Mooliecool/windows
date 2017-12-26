=============================================================================
          APPLICATION : CSUACSelfElevation Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary: 

User Account Control (UAC) is a new security component in Windows Vista and 
newer operating systems. With UAC fully enabled, interactive administrators 
normally run with least user privileges. This example demonstrates how to 
check the privilege level of the current process, and how to self-elevate 
the process by giving explicit consent with the Consent UI. 


/////////////////////////////////////////////////////////////////////////////
Prerequisite:

You must run this sample on Windows Vista or newer operating systems.


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the UAC sample.

Step1. After you successfully build the sample project in Visual Studio 2008, 
you will get an application: CSUACSelfElevation.exe. 

Step2. Run the application as a protected administrator on a Windows Vista or 
Windows 7 system with UAC fully enabled. The application should display the 
following content on the main dialog.

  IsUserInAdminGroup:     True
  IsRunAsAdmin:           False
  IsProcessElevated:      False
  Integrity Level:        Medium

There is a UAC shield icon on the Self-elevate button.

Step3. Click on the Self-elevate button. You will see a Consent UI.

  User Account Control
  ---------------------------------- 
  Do you want to allow the following program from an unknown publisher to 
  make changes to this computer?

Step4. Click Yes to approve the elevation. The original application will then 
be started and display the following content on the main dialog.

  IsUserInAdminGroup:     True
  IsRunAsAdmin:           True
  IsProcessElevated:      True
  Integrity Level:        High

The Self-elevate button on the dialog does not have the UAC shield icon this 
time. That is, the application is running as elevated administrator. The 
elevation succeeds. If you click on the Self-elevate button again, the 
application will tell you that it is running as administrator.

Step5. Close the application. 


/////////////////////////////////////////////////////////////////////////////
Implementation:

Step1. Create a new Visual C# Windows Forms project named CSUACSelfElevation.

Step2. Add controls to the main form

  Type: Button
  ID: btnElevate
  Caption: "Self-elevate"
  
  Type: Label
  ID: lbInAdminGroup
  Use: Display whether the primary access token of the process belongs to 
  user account that is a member of the local Administrators group, even if it 
  currently is not elevated.
  
  Type: Label
  ID: lbIsRunAsAdmin
  Use: Display whether the application is run as administrator. 
  
  Type: Label
  ID: lbIsElevated
  Use: Display whether the process is elevated or not. Token elevation is 
  only available on Windows Vista and newer operating systems. The label 
  shows N/A on systems prior to Windows Vista.
 
  Type: Label
  ID: lbIntegrityLevel
  Use: Display the integrity level of the current process. Integrity level is 
  only available on Windows Vista and newer operating systems. The label 
  shows N/A on systems prior to Windows Vista.

Step3. Check and display the current process's "run as administrator" status, 
elevation information and integrity level when the application initializes 
the main dialog.

Create the following four helper functions:

    /// <summary>
    /// The function checks whether the primary access token of the process belongs 
    /// to user account that is a member of the local Administrators group, even if 
    /// it currently is not elevated.
    /// </summary>
    /// <returns>
    /// Returns true if the primary access token of the process belongs to user 
    /// account that is a member of the local Administrators group. Returns false 
    /// if the token does not.
    /// </returns>
    /// <exception cref="System.ComponentModel.Win32Exception">
    /// When any native Windows API call fails, the function throws a Win32Exception 
    /// with the last error code.
    /// </exception>
    internal bool IsUserInAdminGroup()

    /// <summary>
    /// The function checks whether the current process is run as administrator.
    /// In other words, it dictates whether the primary access token of the 
    /// process belongs to user account that is a member of the local 
    /// Administrators group and it is elevated.
    /// </summary>
    /// <returns>
    /// Returns true if the primary access token of the process belongs to user 
    /// account that is a member of the local Administrators group and it is 
    /// elevated. Returns false if the token does not.
    /// </returns>
    internal bool IsRunAsAdmin()
    
    /// <summary>
    /// The function gets the elevation information of the current process. It 
    /// dictates whether the process is elevated or not. Token elevation is only 
    /// available on Windows Vista and newer operating systems, thus 
    /// IsProcessElevated throws a C++ exception if it is called on systems prior 
    /// to Windows Vista. It is not appropriate to use this function to determine 
    /// whether a process is run as administartor.
    /// </summary>
    /// <returns>
    /// Returns true if the process is elevated. Returns false if it is not.
    /// </returns>
    /// <exception cref="System.ComponentModel.Win32Exception">
    /// When any native Windows API call fails, the function throws a Win32Exception 
    /// with the last error code.
    /// </exception>
    /// <remarks>
    /// TOKEN_INFORMATION_CLASS provides TokenElevationType to check the elevation 
    /// type (TokenElevationTypeDefault / TokenElevationTypeLimited / 
    /// TokenElevationTypeFull) of the process. It is different from TokenElevation 
    /// in that, when UAC is turned off, elevation type always returns 
    /// TokenElevationTypeDefault even though the process is elevated (Integrity 
    /// Level == High). In other words, it is not safe to say if the process is 
    /// elevated based on elevation type. Instead, we should use TokenElevation. 
    /// </remarks>
    internal bool IsProcessElevated()
    
    /// <summary>
    /// The function gets the integrity level of the current process. Integrity 
    /// level is only available on Windows Vista and newer operating systems, thus 
    /// GetProcessIntegrityLevel throws a C++ exception if it is called on systems 
    /// prior to Windows Vista.
    /// </summary>
    /// <returns>
    /// Returns the integrity level of the current process. It is usually one of 
    /// these values:
    /// 
    ///    SECURITY_MANDATORY_UNTRUSTED_RID - means untrusted level. It is used 
    ///    by processes started by the Anonymous group. Blocks most write access.
    ///    (SID: S-1-16-0x0)
    ///    
    ///    SECURITY_MANDATORY_LOW_RID - means low integrity level. It is used by
    ///    Protected Mode Internet Explorer. Blocks write acess to most objects 
    ///    (such as files and registry keys) on the system. (SID: S-1-16-0x1000)
    /// 
    ///    SECURITY_MANDATORY_MEDIUM_RID - means medium integrity level. It is 
    ///    used by normal applications being launched while UAC is enabled. 
    ///    (SID: S-1-16-0x2000)
    ///    
    ///    SECURITY_MANDATORY_HIGH_RID - means high integrity level. It is used 
    ///    by administrative applications launched through elevation when UAC is 
    ///    enabled, or normal applications if UAC is disabled and the user is an 
    ///    administrator. (SID: S-1-16-0x3000)
    ///    
    ///    SECURITY_MANDATORY_SYSTEM_RID - means system integrity level. It is 
    ///    used by services and other system-level applications (such as Wininit, 
    ///    Winlogon, Smss, etc.)  (SID: S-1-16-0x4000)
    /// 
    /// </returns>
    /// <exception cref="System.ComponentModel.Win32Exception">
    /// When any native Windows API call fails, the function throws a Win32Exception 
    /// with the last error code.
    /// </exception>
    internal int GetProcessIntegrityLevel()

Some of the methods need to P/Invoke some native Windows APIs. The P/Invoke 
signatures are defined in NativeMethod.cs.

In the constructor of the main form, check and display the "run as 
administrator" status, the elevation information, and the integrity level of 
the current process.

    // Get and display whether the primary access token of the process belongs 
    // to user account that is a member of the local Administrators group even 
    // if it currently is not elevated (IsUserInAdminGroup).
    try
    {
        bool fInAdminGroup = IsUserInAdminGroup();
        this.lbInAdminGroup.Text = fInAdminGroup.ToString();
    }
    catch (Exception ex)
    {
        this.lbInAdminGroup.Text = "N/A";
        MessageBox.Show(ex.Message, "An error occurred in IsUserInAdminGroup",
            MessageBoxButtons.OK, MessageBoxIcon.Error);
    }

    // Get and display whether the process is run as administrator or not 
    // (IsRunAsAdmin).
    try
    {
        bool fIsRunAsAdmin = IsRunAsAdmin();
        this.lbIsRunAsAdmin.Text = fIsRunAsAdmin.ToString();
    }
    catch (Exception ex)
    {
        this.lbIsRunAsAdmin.Text = "N/A";
        MessageBox.Show(ex.Message, "An error occurred in IsRunAsAdmin",
            MessageBoxButtons.OK, MessageBoxIcon.Error);
    }


    // Get and display the process elevation information (IsProcessElevated) 
    // and integrity level (GetProcessIntegrityLevel). The information is not 
    // available on operating systems prior to Windows Vista.
    if (Environment.OSVersion.Version.Major >= 6)
    {
        // Running Windows Vista or later (major version >= 6). 

        try
        {
            // Get and display the process elevation information.
            bool fIsElevated = IsProcessElevated();
            this.lbIsElevated.Text = fIsElevated.ToString();

            // Update the Self-elevate button to show the UAC shield icon on 
            // the UI if the process is not elevated.
            this.btnElevate.FlatStyle = FlatStyle.System;
            NativeMethod.SendMessage(btnElevate.Handle, 
                NativeMethod.BCM_SETSHIELD, 0, 
                fIsElevated ? IntPtr.Zero : (IntPtr)1);
        }
        catch (Exception ex)
        {
            this.lbIsElevated.Text = "N/A";
            MessageBox.Show(ex.Message, "An error occurred in IsProcessElevated",
                MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        try
        {
            // Get and display the process integrity level.
            int IL = GetProcessIntegrityLevel();
            switch (IL)
            {
            case NativeMethod.SECURITY_MANDATORY_UNTRUSTED_RID:
                this.lbIntegrityLevel.Text = "Untrusted"; break;
            case NativeMethod.SECURITY_MANDATORY_LOW_RID:
                this.lbIntegrityLevel.Text = "Low"; break;
            case NativeMethod.SECURITY_MANDATORY_MEDIUM_RID:
                this.lbIntegrityLevel.Text = "Medium"; break;
            case NativeMethod.SECURITY_MANDATORY_HIGH_RID:
                this.lbIntegrityLevel.Text = "High"; break;
            case NativeMethod.SECURITY_MANDATORY_SYSTEM_RID:
                this.lbIntegrityLevel.Text = "System"; break;
            default:
                this.lbIntegrityLevel.Text = "Unknown"; break;
            }
        }
        catch (Exception ex)
        {
            this.lbIntegrityLevel.Text = "N/A";
            MessageBox.Show(ex.Message, "An error occurred in GetProcessIntegrityLevel",
                MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
    }
    else
    {
        this.lbIsElevated.Text = "N/A";
        this.lbIntegrityLevel.Text = "N/A";
    }

Step4. Handle the click event of the Self-elevate button. When user clicks 
the button, elevate the process by restarting itself with 
ProcessStartInfo.UseShellExecute = true and ProcessStartInfo.Verb = "runas" 
if the process is not run as administrator.

    private void btnElevate_Click(object sender, EventArgs e)
    {
        // Elevate the process if it is not run as administrator.
        if (!IsRunAsAdmin())
        {
            // Launch itself as administrator
            ProcessStartInfo proc = new ProcessStartInfo();
            proc.UseShellExecute = true;
            proc.WorkingDirectory = Environment.CurrentDirectory;
            proc.FileName = Application.ExecutablePath;
            proc.Verb = "runas";

            try
            {
                Process.Start(proc);
            }
            catch
            {
                // The user refused the elevation.
                // Do nothing and return directly ...
                return;
            }

            Application.Exit();  // Quit itself
        }
        else
        {
            MessageBox.Show("The process is running as administrator", "UAC");
        }
    }

Step5. Automatically elevate the process when it's started up.

If your application always requires administrative privileges, such as during 
an installation step, the operating system can automatically prompt the user 
for privileges elevation each time your application is invoked. 

If a specific kind of resource (RT_MANIFEST) is found embedded within the 
application executable, the system looks for the <trustInfo> section and 
parses its contents. Here is an example of this section in the manifest file:

    <trustInfo xmlns="urn:schemas-microsoft-com:asm.v2">
       <security>
          <requestedPrivileges>
             <requestedExecutionLevel
                level="requireAdministrator"
             />
          </requestedPrivileges>
       </security>
    </trustInfo>

Three different values are possible for the level attribute

  a) requireAdministrator 
  The application must be started with Administrator privileges; it won't run 
  otherwise.

  b) highestAvailable 
  The application is started with the highest possible privileges.
  If the user is logged on with an Administrator account, an elevation prompt 
  appears. If the user is a Standard User, the application is started 
  (without any elevation prompt) with these standard privileges.

  c) asInvoker 
  The application is started with the same privileges as the calling 
  application.

To configure the elevation level in this Visual C# Windows Forms project, 
open the project's properties, turn to the Security tab, check the checkbox 
"Enable ClickOnce Security Settings", check "This is a fulltrust application" 
and close the application Properies page. This creates an app.manifest file 
and configures the project to embed the manifest. You can open the 
"app.manifest" file from Solution Explorer by expanding the Properies folder. 
The file has the following content by default.

    <?xml version="1.0" encoding="utf-8"?>
    <asmv1:assembly manifestVersion="1.0" xmlns="urn:schemas-microsoft-com:asm.v1" 
    xmlns:asmv1="urn:schemas-microsoft-com:asm.v1" xmlns:asmv2="urn:schemas-microsoft-com:asm.v2" 
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
      <assemblyIdentity version="1.0.0.0" name="MyApplication.app" />
      <trustInfo xmlns="urn:schemas-microsoft-com:asm.v2">
        <security>
          <requestedPrivileges xmlns="urn:schemas-microsoft-com:asm.v3">
            <!-- UAC Manifest Options
                If you want to change the Windows User Account Control level replace the 
                requestedExecutionLevel node with one of the following.

            <requestedExecutionLevel  level="asInvoker" uiAccess="false" />
            <requestedExecutionLevel  level="requireAdministrator" uiAccess="false" />
            <requestedExecutionLevel  level="highestAvailable" uiAccess="false" />

                If you want to utilize File and Registry Virtualization for backward 
                compatibility then delete the requestedExecutionLevel node.
            -->
            <requestedExecutionLevel level="asInvoker" uiAccess="false" />
          </requestedPrivileges>
          <applicationRequestMinimum>
            <PermissionSet class="System.Security.PermissionSet" version="1" 
            Unrestricted="true" ID="Custom" SameSite="site" />
            <defaultAssemblyRequest permissionSetReference="Custom" />
          </applicationRequestMinimum>
        </security>
      </trustInfo>
    </asmv1:assembly>

Here we are focusing on the line:

    <requestedExecutionLevel level="asInvoker" uiAccess="false" />

You can change it to be 

    <requestedExecutionLevel level="requireAdministrator" uiAccess="false" />

to require the application always be started with Administrator privileges.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: User Account Control
http://msdn.microsoft.com/en-us/library/aa511445.aspx

MSDN: Windows Vista Application Development Requirements for User Account 
Control Compatibility
http://msdn.microsoft.com/en-us/library/bb530410.aspx


/////////////////////////////////////////////////////////////////////////////