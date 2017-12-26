/****************************** Module Header ******************************\
 Module Name:  WorkItemLinkInfoDetails.cs
 Project:      CSTFSWorkItemLinkInfoDetails
 Copyright (c) Microsoft Corporation.
 
 The details of the WorkItemLinkInfoentry class. 
 
 
 This source is subject to the Microsoft Public License.
 See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
 All other rights reserved.
 
 THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
 EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
 WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.TeamFoundation.WorkItemTracking.Client;

namespace CSTFSWorkItemLinkInfoDetails
{
    public class WorkItemLinkInfoDetails
    {

        public WorkItemLinkInfo LinkInfo { get; private set; }

        public WorkItem SourceWorkItem { get; private set; }

        public WorkItem TargetWorkItem { get; private set; }

        public WorkItemLinkType LinkType { get; private set; }

        public WorkItemLinkInfoDetails(WorkItemLinkInfo linkInfo, WorkItem sourceWorkItem,
            WorkItem targetWorkItem,WorkItemLinkType linkType)
        {
            this.LinkInfo = linkInfo;
            this.SourceWorkItem = sourceWorkItem;
            this.TargetWorkItem = targetWorkItem;
            this.LinkType = linkType;
        }

        /// <summary>
        /// Display the link as 
        /// Source:[Source title] ==> LinkType:[Link Type] ==> Target:[Target title]
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            return string.Format(
                "Source:{0} ==> LinkType:{1} ==> Target:{2}",
                SourceWorkItem.Title,
                LinkType.ForwardEnd.Name, 
                TargetWorkItem.Title);
        }      
    }
}
