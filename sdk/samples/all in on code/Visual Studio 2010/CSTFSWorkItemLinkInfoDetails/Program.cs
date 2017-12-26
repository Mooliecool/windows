/****************************** Module Header ******************************\
 Module Name:  Program.cs
 Project:      CSTFSWorkItemLinkInfoDetails
 Copyright (c) Microsoft Corporation.
 
 The main entry of the application. To run this application, use following command
 arguments:
 
    CSTFSWorkItemLinkInfoDetails.exe <CollectionUrl> <WorkItemID>
 
 
 This source is subject to the Microsoft Public License.
 See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
 All other rights reserved.
 
 THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
 EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
 WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Net;
using Microsoft.TeamFoundation.Client;
using Microsoft.TeamFoundation.WorkItemTracking.Client;

namespace CSTFSWorkItemLinkInfoDetails
{
    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                // There are 2 arguments.
                if (args.Length == 2)
                {

                    // Get CollectionUrl and WorkItemID from the arguments.
                    Uri collectionUri = new Uri(args[0]);
                    int workitemID = int.Parse(args[1]);

                    using (WorkItemLinkQuery query = new WorkItemLinkQuery(collectionUri))
                    {

                        // Get the WorkItemLinkInfoDetails list of a work item.
                        var detailsList = query.GetWorkItemLinkInfos(workitemID);

                        foreach (WorkItemLinkInfoDetails details in detailsList)
                        {
                            Console.WriteLine(details.ToString());
                        }
                    }
                }
                else
                {
                    Console.WriteLine("Use following command arguments to use this application:");
                    Console.WriteLine("CSTFSWorkItemLinkInfoDetails.exe <CollectionUrl> <WorkItemID>");
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }
    }
}
