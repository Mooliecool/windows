===============================================================================
			Microsoft Dynamics NAV APPLICATION: CSDynamicsPowerShellAdmin
===============================================================================


/////////////////////////////////////////////////////////////////////////////
Summary:

This sample uses PowerShell to list and manage NAV Services:
  - List NAV Services
  - Start and stop services
  - Enter the name of a remote computer to manager NAV Services on that computer

The sample also uses xml to show and update CustomSettings.config.

The purpose of this sample is to illustrate how to use PowerShell and other technologies 
for managing NAV Services. It is on purpose developed to be just about useful as it is, 
but with lots of scope for further development.


The definition of a NAV Service for this purpose is a service where the name of the 
executable contains "Dynamics.Nav", and it is assumed that any such service is a 
Dynamics NAV2009 middle-tier Service for connections from either RoleTailored Client 
or NAV Web Services.


////////////////////////////////////////////////////////////////////////////////
Demo:

The solution in this sample consists of a form with the following functionality:

Step1. Click "Get NAV Services" to list the NAV Services running on the local machine.
Step2. Enter a ComputerName in "NAV Server Name" and then click the same button to list 
       NAV Services on a remote computer.
Step3. Select a NAV Service from the list, then click Start/Stop to start or stop that 
       service. Note: This part of the sample requires that you first build the project, 
       then run it as Administrator.
Step4. Select a NAV Service from the list, then click "Get Config" to get selected values 
       from CustomSettings.config belonging to that NAV Service.
Step5. After "Get Config", overwrite any of the values, then click "Update Config" to 
       save those to CustomSettings.config.


////////////////////////////////////////////////////////////////////////////////
Implementation:

Functionalities demonstrated in this sample:
Step1. List NAV Services, either on local machine or on a remote machine.
Step2. Start and stop any of the services listed.
Step3. View and update values from CustomSettings.config for the selected NAV Service.


////////////////////////////////////////////////////////////////////////////////
References:

Further information and samples for coding with NAV and PowerShell:

C# app invokes PowerShell script (CSPowerShell)
http://code.msdn.microsoft.com/CSPowerShell-1f44f80d

NAV Team Blog
http://blogs.msdn.com/b/nav
////////////////////////////////////////////////////////////////////////////////