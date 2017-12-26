/************************************* Module Header **************************************\
* Module Name:  OptionPageGrid.cs
* Project:      CSVSPackageState
* Copyright (c) Microsoft Corporation.
* 
* The Visual Studio package state sample demostrate the state persisting for
* application options and show object states in properties window.
*
* The sample doesn't include the state management for solution and project,
* which will be included in project package sample.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 9/6/2009 1:00 PM Hongye Sun Created
\******************************************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.VisualStudio.Shell;
using System.Runtime.InteropServices;
using System.ComponentModel;

namespace AllInOne.CSVSPackageState
{
    // ClassInterface attribute creates a COM dual interface 
    // that lets Visual Studio Automation use GetAutomationObject 
    // to access the public members of the class programmatically.
    [ClassInterface(ClassInterfaceType.AutoDual)]
    public class OptionPageGrid : DialogPage
    {
        private int optionValue = 256;

        /// <summary>
        /// This is the property which will be shown in the tool
        /// options page. Its user settings will be persisted 
        /// in the user registry settings.
        /// </summary>
        [Category("CSVSPakcageState Category")]
        [DisplayName("Option Interger Property")]
        [Description("An interger property displayed in tools option page")]
        public int OptionInteger
        {
            get { return optionValue; }
            set { optionValue = value; }
        }

    }
}
