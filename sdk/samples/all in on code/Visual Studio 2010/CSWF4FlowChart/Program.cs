/****************************** Module Header ******************************\
* Module Name:  Program.cs
* Project:      CSWF4FlowChart
* Copyright (c) Microsoft Corporation.
* 
* The file defines the entry point of the program. It invokes the 
* GuessNumberGameInFlowChart workflow synchronously.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System.Activities;


namespace CSWF4FlowChart
{
    class Program
    {
        static void Main(string[] args)
        {
            WorkflowInvoker.Invoke(new GuessNumberGameInFlowChart());
        }
    }
}
