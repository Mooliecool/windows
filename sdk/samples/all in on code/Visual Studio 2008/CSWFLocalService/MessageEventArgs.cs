/****************************** Module Header ******************************\
* Module Name:  Program.cs
* Project:		CSWFLocalService
* Copyright (c) Microsoft Corporation.
* 
* A event arg used for message transmission
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 11/2/2009 8:50 PM Andrew Zhu Created
\***************************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Workflow.Activities;

namespace CSWFLocalService
{
    [Serializable]
    public class MessageEventArgs : ExternalDataEventArgs
    {
        public String Message { get; set; }
        public MessageEventArgs(Guid instanceId, string message)
            : base(instanceId)
        {
            this.Message = message;
        }
    }
}
