/****************************** Module Header ******************************\
 Module Name:  WorkItemLinkQuery.cs
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
using System.Net;
using Microsoft.TeamFoundation.Client;
using Microsoft.TeamFoundation.WorkItemTracking.Client;

namespace CSTFSWorkItemLinkInfoDetails
{
    public class WorkItemLinkQuery : IDisposable
    {

        // The query format.
        const string queryFormat =
            "select * from WorkItemLinks where [Source].[System.ID] = {0}";

        bool disposed = false;
      
        Dictionary<int, WorkItemLinkType> linkTypes;

        // The dictionary to store the ID and WorkItemLinkType KeyValuePair.
        public Dictionary<int, WorkItemLinkType> LinkTypes
        {
            get
            {
                // Get all WorkItemLinkType from WorkItemStore.
                if (linkTypes == null)
                {
                    linkTypes = new Dictionary<int, WorkItemLinkType>();
                    foreach (var type in this.WorkItemStore.WorkItemLinkTypes)
                    {
                        linkTypes.Add(type.ForwardEnd.Id, type);
                    }
                }
                return linkTypes;
            }
        }

        /// <summary>
        /// TFS Team Project Collection.
        /// </summary>
        public TfsTeamProjectCollection ProjectCollection { get; private set; }

        /// <summary>
        /// WorkItemStore of the Team Project Collection.
        /// </summary>
        public WorkItemStore WorkItemStore { get; private set; }

        /// <summary>
        /// Use the default credentials to initialize this instance.
        /// </summary>
        public WorkItemLinkQuery(Uri collectionUri)
            : this(collectionUri, CredentialCache.DefaultCredentials)
        { }

        /// <summary>
        /// Initialize this instance.
        /// </summary>
        public WorkItemLinkQuery(Uri collectionUri, ICredentials credential)
        {
            if (collectionUri == null)
            {
                throw new ArgumentNullException("collectionUrl");
            }

            // If the credential failed, an UICredentialsProvider instance will be launched.
            this.ProjectCollection =
                    new TfsTeamProjectCollection(collectionUri, credential, new UICredentialsProvider());
            this.ProjectCollection.EnsureAuthenticated();

            // Get the WorkItemStore service.
            this.WorkItemStore = this.ProjectCollection.GetService<WorkItemStore>();
        }

        /// <summary>
        /// Get the WorkItemLinkInfoDetails of a work item.
        /// </summary>
        public IEnumerable<WorkItemLinkInfoDetails> GetWorkItemLinkInfos(int workitemID)
        {

            // Construct the WIQL.
            string queryStr = string.Format(queryFormat, workitemID);

            Query linkQuery = new Query(this.WorkItemStore, queryStr);

            // Get all WorkItemLinkInfo objects.
            WorkItemLinkInfo[] linkinfos = linkQuery.RunLinkQuery();

            // Get WorkItemLinkInfoDetails from the  WorkItemLinkInfo objects.
            List<WorkItemLinkInfoDetails> detailsList = new List<WorkItemLinkInfoDetails>();
            foreach (var linkinfo in linkinfos)
            {
                if (linkinfo.LinkTypeId != 0)
                {
                    WorkItemLinkInfoDetails details = GetDetailsFromWorkItemLinkInfo(linkinfo);
                    Console.WriteLine(details.ToString());
                }
            }
            return detailsList;
        }

        /// <summary>
        /// Get WorkItemLinkInfoDetails from the  WorkItemLinkInfo object.
        /// </summary>
        public WorkItemLinkInfoDetails GetDetailsFromWorkItemLinkInfo(WorkItemLinkInfo linkInfo)
        {
            if (linkInfo == null)
            {
                throw new ArgumentNullException("linkInfo");
            }

            if (this.LinkTypes.ContainsKey(linkInfo.LinkTypeId))
            {
                WorkItemLinkInfoDetails details = new WorkItemLinkInfoDetails(
                   linkInfo,
                   this.WorkItemStore.GetWorkItem(linkInfo.SourceId),
                   this.WorkItemStore.GetWorkItem(linkInfo.TargetId),
                   this.LinkTypes[linkInfo.LinkTypeId]);
                return details;
            }
            else
            {
                throw new ApplicationException("Cannot find WorkItemLinkType!");
            }
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            // Protect from being called multiple times.
            if (disposed)
            {
                return;
            }

            if (disposing)
            {
                if (this.ProjectCollection != null)
                {
                    this.ProjectCollection.Dispose();
                }
                disposed = true;
            }
        }

    }
}
