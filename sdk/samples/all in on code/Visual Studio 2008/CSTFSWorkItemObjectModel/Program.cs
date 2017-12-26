/****************************** Module Header ********************************\
* Module Name:  Program.cs
* Project:      CSTFSWorkItemObjectModel
* Copyright (c) Microsoft Corporation.
*
* This C# sample works in machines where Team Explorer 2008 is installed. This 
* sample uses object model from Team Explorer 2008 to access TFS. It will 
* demostrate how to programatically manage work items. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 7/28/2009 9:32 AM Bill Wang Created
\*****************************************************************************/

#region Using Directive
using System;
using System.Configuration;
using System.IO;
using System.Linq;
using System.Threading;

using Microsoft.TeamFoundation.Client;
using Microsoft.TeamFoundation.WorkItemTracking.Client;
#endregion


namespace CSTFSWorkItemObjectModel
{
    class Program
    {
        static void Main(string[] args)
        {
            // Read TFS URL from the application configuration file and 
            // initialize a TeamFoundationServer instance.
            var tfsUrl = ConfigurationManager.AppSettings["TfsUrl"];
            var tfs = TeamFoundationServerFactory.GetServer(tfsUrl);

            // Get the work item tracking service that was encapsulated in a
            // WorkItemStore instance.
            var wis = (WorkItemStore)tfs.GetService(typeof(WorkItemStore));

            // Read project name form the application configuration file
            var projectName = ConfigurationManager.AppSettings["TeamProject"];


            EnsureWITImported(wis, projectName);

            var project = wis.Projects[projectName];

            var id = CreateWorkItem(project);
            var wi = GetWorkItem(wis, id);
            EditWorkItem(wi);

            QueryWorkItems(wis, projectName);

            Guid queryGuid = EnsureWIQueryCreated(wis, project);
            QueryWorkItemAsynchronously(wis, queryGuid, project);
        }

        /// <summary>
        /// Create a work item in the specified team project.
        /// </summary>
        /// <param name="project">Team project.</param>
        /// <returns>The Id of the created work item.</returns>
        private static int CreateWorkItem(Project project)
        {
            var wit = project.WorkItemTypes["My WIT"];

            var wi = wit.NewWorkItem();
            wi.Title = "Test TFS work item object model";

            wi.Save();
            return wi.Id;
        }

        /// <summary>
        /// Get a work item from its Id.
        /// </summary>
        /// <param name="wis">A WorkItemStore instance</param>
        /// <param name="id">Work item Id.</param>
        /// <returns>A WorkItem instance.</returns>
        private static WorkItem GetWorkItem(WorkItemStore wis, int id)
        {
            var wi = wis.GetWorkItem(id);
            return wi;
        }

        /// <summary>
        /// Edit a work item.
        /// </summary>
        /// <param name="wi">The work item to be edited.</param>
        private static void EditWorkItem(WorkItem wi)
        {
            /// Set the work item history.
            wi.History = "Modify the work item";

            wi.Save();
        }

        /// <summary>
        /// Import "My WIT" work item type if it has not been imported yet.
        /// </summary>
        /// <param name="wis">A WorkItemStore instance</param>
        /// <param name="projectName">The name of the team project</param>
        private static void EnsureWITImported(WorkItemStore wis, string projectName)
        {
            Project project = wis.Projects[projectName];
            if (!project.WorkItemTypes.Contains("My WIT"))
            {
                using (var reader = new StreamReader("My WIT.xml"))
                {
                    // Read work item definition from "My WIT.xml".
                    var definition = reader.ReadToEnd();

                    // Import the work item definition.
                    project.WorkItemTypes.Import(definition);

                    // Refresh work item cache. 
                    wis.RefreshCache();

                    // Switch the WorkItemStore instance to use the latest metadata.
                    wis.SyncToCache();
                }
            }
        }

        /// <summary>
        /// Create a work item query "My WITs" if it doesn't exist. This query will 
        /// list all work items of type "My WIT" in the current team project.
        /// </summary>
        /// <param name="wis">A WorkItemStore instance.</param>
        /// <param name="project">A Project instance.</param>
        /// <returns>The GUID of the created query.</returns>
        private static Guid EnsureWIQueryCreated(WorkItemStore wis, Project project)
        {
            var storedQuery = from StoredQuery q in project.StoredQueries
                              where q.Name == "My WITs"
                              select q;

            if (storedQuery.Count<StoredQuery>() == 0)
            {
                var queryString = @"SELECT System.Id, System.Title 
                    FROM WorkItems 
                    WHERE System.TeamProject='" +
                    project.Name +
                    "' AND System.WorkItemType='My WIT'";
                var query = new StoredQuery(QueryScope.Private, "My WITs",
                    queryString, null);
                project.StoredQueries.Add(query);
                return query.QueryGuid;
            }
            return storedQuery.First<StoredQuery>().QueryGuid;
        }

        /// <summary>
        /// Query work items asynchronously. 
        /// </summary>
        /// <param name="wis">A WorkItemStore instance.</param>
        /// <param name="queryGuid">The GUID of the query to be executed.</param>
        /// <param name="project">A Project instance.</param>
        private static void QueryWorkItemAsynchronously(WorkItemStore wis,
            Guid queryGuid, Project project)
        {
            var storedQuery = project.StoredQueries[queryGuid];
            var query = new Query(wis, storedQuery.QueryText);

            var asyncResult = query.BeginQuery();

            Thread.Sleep(1000);

            // If query completes after 1 second, then read the result work 
            // items. Otherwise, cancel the query.
            if (asyncResult.IsCompleted)
            {
                var queryResult = query.EndQuery(asyncResult);
            }
            else
            {
                asyncResult.Cancel();
            }

        }

        /// <summary>
        /// Query work items without creating a query. 
        /// </summary>
        /// <param name="wis">A WorkItemStore instance.</param>
        /// <param name="projectName">Team project name.</param>
        private static void QueryWorkItems(WorkItemStore wis, string projectName)
        {
            var queryString = @"SELECT System.Id, System.Title 
                    FROM WorkItems 
                    WHERE System.TeamProject='" +
                    projectName +
                    "' AND System.WorkItemType='My WIT'";

            var wiCollection = wis.Query(queryString);
        }
    }
}
