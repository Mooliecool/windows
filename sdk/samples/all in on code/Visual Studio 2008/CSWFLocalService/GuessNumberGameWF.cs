/****************************** Module Header ******************************\
* Module Name:  Program.cs
* Project:		CSWFLocalService
* Copyright (c) Microsoft Corporation.
* 
* Guess Number Game Workflow
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 11/2/2009 8:50 PM Andrew Zhu Created
\***************************************************************************/
using System;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Drawing;
using System.Workflow.ComponentModel.Compiler;
using System.Workflow.ComponentModel.Serialization;
using System.Workflow.ComponentModel;
using System.Workflow.ComponentModel.Design;
using System.Workflow.Runtime;
using System.Workflow.Activities;
using System.Workflow.Activities.Rules;

namespace CSWFLocalService
{
    public sealed partial class GuessNumberGameWF 
        :SequentialWorkflowActivity 
    {

        public string OutMessage = "";
        public int InNumber = 0;
        public int RandomNumber;

        public GuessNumberGameWF() 
        {
            InitializeComponent();
            OutMessage = "Please input a number(1~10):";
            Random random = new Random();
            RandomNumber = random.Next(1, 10);
        }

        private void handleExternalEventActivity1_Invoked(object sender, 
            ExternalDataEventArgs e) 
        {
            InNumber = Int32.Parse(((MessageEventArgs)e).Message);
        }

        private void codeActivity_GameCor_ExecuteCode(object sender, 
            EventArgs e) 
        {
            if (InNumber < RandomNumber) 
            {
                OutMessage = "please try a larger Number";
            } else if (InNumber > RandomNumber) 
            {
                OutMessage = "Please try a smaller number";
            } else 
            {
                OutMessage = "right";
            }
        }
    }
}
