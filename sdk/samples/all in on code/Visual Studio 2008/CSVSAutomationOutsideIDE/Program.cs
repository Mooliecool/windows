/***************************************** Module Header *****************************************\
* Module Name:  Program.cs
* Project:      CSVSAutomationOutsideIDE
* Copyright (c) Microsoft Corporation.
* 
* Demostrate how to automate Visual Studio outside the IDE. One can use 
* existing Visual Studio instance or create a new one to execute specified 
* command or automate by DTE object, like macro or add-in.
* 
* In this sample, we first find an existing Visual Studio instance or create 
* a new one and navigate its browser to All-In-One homepage.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 11/26/2009 0:35 AM Hongye Sun Created
\*************************************************************************************************/
using System;

// Step1. Reference EnvDTE object which is located at GAC and Visual Studio
// PublicAssembly folder.
using EnvDTE;
using System.Runtime.InteropServices;

namespace CSVSAutomationOutsideIDE
{
    class Program
    {
        static void Main(string[] args)
        {
            // Step2. Find an existing IDE instance and obtain its DTE object.
            DTE dte = (DTE) Marshal.GetActiveObject("VisualStudio.DTE.9.0");

            // Step3. If there is no existing one, create a new one.
            if (dte == null)
            {
                Type vsType = Type.GetTypeFromProgID("VisualStudio.DTE.9.0");
                dte = Activator.CreateInstance(vsType) as DTE;
            }

            // Step4. Display the UI of the IDE by setting its visiblity property.
            dte.MainWindow.Visible = true;

            // Step5. Using DTE object to execute a command: View.URL to navigate Visual 
            // Studio embeded browser to All-In-One homepage.
            dte.ExecuteCommand("View.URL", "cfx.codeplex.com");

            Console.WriteLine("Press any key to exit...");
            Console.Read();
        }
    }
}
