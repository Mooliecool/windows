/****************************** Module Header ******************************\
* Module Name:	Program.cs
* Project:		CSPowershell
* Copyright (c) Microsoft Corporation.
* 
* This sample indicates how to call Powershell from C# language. It first
* creats a Runspace object in System.Management.Automation namespace. Then 
* it creats a Pipeline from Runspace. The Pipeline is used to host a line of
* commands which are supposed to be executed. The example call Get-Process 
* command to get all processes whose name are started with "C"
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Text;
using System.Management.Automation;
using System.Management.Automation.Runspaces;
using System.Diagnostics;
using System.Collections.ObjectModel;
#endregion


namespace CSPowerShell
{
    class Program
    {
        static void Main(string[] args)
        {
            // Create a RunSpace to host the Powershell script enviroment 
            // using RunspaceFactory.CreateRunSpace
            Runspace runSpace = RunspaceFactory.CreateRunspace();
            runSpace.Open();

            // Create a Pipeline to host commands to be executed using 
            // Runspace.CreatePipeline
            Pipeline pipeLine = runSpace.CreatePipeline();

            // Create a Command object by passing the command to the constructor
            Command getProcessCStarted = new Command("Get-Process");

            // Add parameters to the Command. 
            getProcessCStarted.Parameters.Add("name", "C*");

            // Add the commands to the Pipeline
            pipeLine.Commands.Add(getProcessCStarted);

            // Run all commands in the current pipeline by calling Pipeline.Invoke. 
            // It returns a System.Collections.ObjectModel.Collection object. 
            // In this example, the executed script is "Get-Process -name C*".
            Collection<PSObject> cNameProcesses = pipeLine.Invoke();

            foreach (PSObject psObject in cNameProcesses)
            {
                Process process = psObject.BaseObject as Process;
                Console.WriteLine("Process Name: {0}", process.ProcessName);
            }
        }
    }
}
