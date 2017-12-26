/****************************** Module Header ******************************\
* Module Name:  ReadNumberActivity.cs
* Project:		CSWF4SequenceWF
* Copyright (c) Microsoft Corporation.
* 
* The class defines the ReadNumber activity.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Activities;


namespace CSWF4SequenceWF
{
    public sealed class ReadNumberActivity : CodeActivity
    {
        // Define an activity out argument of type int
        public OutArgument<int> playerInputNumber { get; set; }

        protected override void Execute(CodeActivityContext context)
        {
            playerInputNumber.Set(context,Int32.Parse(Console.ReadLine()));
        }
    }
}
