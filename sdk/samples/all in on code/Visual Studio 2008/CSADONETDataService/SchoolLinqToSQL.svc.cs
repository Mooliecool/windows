/****************************** Module Header ******************************\
* Module Name:	SchoolLinqToSQL.svc.cs
* Project:		CSADONETDataService
* Copyright (c) Microsoft Corporation.
* 
* SchoolLinqToSQL.svc demonstrates the ADO.NET Data Service for Linq to SQL
* Data Classes.  The Linq to SQL Data Class connects to the SQL Server
* database deployed by SQLServer2005DB.  It contains data tables: Person, 
* Course, CourseGrade, and CourseInstructor. The service also exports a 
* service operation SearchCourses to let the client search course objects via
* SQL commands.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directive
using System;
using System.Collections.Generic;
using System.Data.Services;
using System.Linq;
using System.ServiceModel.Web;
using System.Web;
using CSADONETDataService.LinqToSQL;
#endregion


namespace CSADONETDataService
{
    public class SchoolLinqToSQL : DataService<SchoolLinqToSQLDataContext>
    {
        // This method is called only once to initialize service-wide policies.
        public static void InitializeService(IDataServiceConfiguration config)
        {
            // Set rules to indicate which entity sets and service operations 
            // are visible, updatable, etc.
            config.UseVerboseErrors = true;
            config.SetEntitySetAccessRule("*", EntitySetRights.All);
            config.SetServiceOperationAccessRule("SearchCourses", 
                ServiceOperationRights.All);
        }

        /// <summary>
        /// A service operation that searches the courses via SQL commands
        /// and returns an IQueryable collection of Course objects
        /// </summary>
        /// <param name="searchText">The query SQL commands</param>
        /// <returns>An IQueryable collection contains Course objects
        /// </returns>
        [WebGet]
        public IQueryable<Course> SearchCourses(string searchText)
        {
            // Call DataContext.ExecuteQuery to call the search SQL commands
            return this.CurrentDataSource.ExecuteQuery<Course>(searchText).
                AsQueryable();
        }
    }
}
