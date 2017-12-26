/****************************** Module Header ******************************\
* Module Name:	SchoolLinqToEntities.svc.cs
* Project:		CSADONETDataService
* Copyright (c) Microsoft Corporation.
* 
* SchoolLinqToEntities.svc demonstrates the ADO.NET Data Service for ADO.NET
* Entity Data Model.  The ADO.NET Entity Data Model connects to the SQL 
* Server database deployed by SQLServer2005DB.  It contains data tables: 
* Person, Course, CourseGrade, and CourseInstructor. The service also exports
* a service operation CoursesByPersonID to retrieve the instructor's Course 
* list by PersonID, a service operation GetPersonUpdateException to get the 
* person update exception, a service query interceptor to filter the Course 
* objects, and a servie change interceptor to check PersonCategory value of 
* Added or Changed Person objects.
* 
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
using CSADONETDataService.LinqToEntities;
using System.Linq.Expressions;
#endregion


namespace CSADONETDataService
{
    public class SchoolLinqToEntities : DataService<SQLServer2005DBEntities>
    {
        // This method is called only once to initialize service-wide policies.
        public static void InitializeService(IDataServiceConfiguration config)
        {
            // Set rules to indicate which entity sets and service operations 
            // are visible, updatable, etc.
            config.UseVerboseErrors = true;
            config.SetEntitySetAccessRule("*", EntitySetRights.All);
            config.SetServiceOperationAccessRule("CoursesByPersonID", 
                ServiceOperationRights.All);
            config.SetServiceOperationAccessRule("GetPersonUpdateException",
                ServiceOperationRights.All);
        }

        /// <summary>
        /// Override the HandleException method to throw 400 Bad Request
        /// exception to the client side.
        /// </summary>
        /// <param name="args">The HandleException argument</param>
        protected override void HandleException(HandleExceptionArgs args)
        {
            // Check if the InnerException is null
            if (args.Exception.InnerException != null)
            {
                // Convert the InnerException to DataServiceException
                DataServiceException ex = args.Exception.InnerException as
                    DataServiceException;

                // Check if the InnerException is in type of 
                // DataServiceException and the StatusCode is 
                // 400(Bad Request)
                if (ex != null && ex.StatusCode == 400)
                {
                    // Return the DataServiceException to the client
                    args.Exception = ex;
                }
            }

            base.HandleException(args);
        }

        /// <summary>
        /// A service operation that retrieve the person update exception
        /// information
        /// </summary>
        /// <returns>An IQueryable collection contains Person objects
        /// </returns>
        [WebGet]
        public IQueryable<Person> GetPersonUpdateException()
        {
            // Throw the person update exception to the client
            throw new DataServiceException(400, 
                "The valid value of PersonCategory is 1" + 
                "(for students) or 2(for instructors)."); ;
        }

        /// <summary>
        /// A service operation that retrieve the instructor's courses list 
        /// by primary key PersonID
        /// </summary>
        /// <param name="ID">The primary key PersonID</param>
        /// <returns>An IQueryable collection contains Course objects
        /// </returns>
        [WebGet]
        public IQueryable<Course> CoursesByPersonID(int ID)
        {
            // Check whether the PersonID is valid and it is instructor's ID
            if (this.CurrentDataSource.Person.Any(i => i.PersonID == ID &&
                i.PersonCategory == 2))
            {
                // Retrieve the instructor's course list
                var courses = from p in this.CurrentDataSource.Person
                              where p.PersonID == ID
                              select p.Course;

                // Return the query result
                return courses.First().AsQueryable();
            }
            else
            {
                // Throw DataServiceException if the PersonID is invalid or
                // it is student's ID
                throw new DataServiceException(400, "Person ID is incorrect" +
                    " or the person is not instructor!");
            }
        }


        /// <summary>
        /// A service query interceptor that filters the course objects to
        /// return the course which CourseID is larger than 4000
        /// </summary>
        /// <returns>LINQ lambda expression to filter the course objects
        /// </returns>
        [QueryInterceptor("Course")]
        public Expression<Func<Course, bool>> QueryCourse()
        {
            // LINQ lambda expression to filter the course objects
            return c => c.CourseID > 4000;
        }


        /// <summary>
        /// A service change interceptor that checks the PersonCategory value
        /// of the added or changed Person object
        /// </summary>
        /// <param name="p">The added or changed Person object</param>
        /// <param name="operation">The update operation</param>
        [ChangeInterceptor("Person")]
        public void OnChangePerson(Person p, UpdateOperations operation)
        {
            // Check whether the update operation is Add or Change
            if (operation == UpdateOperations.Add || 
                operation == UpdateOperations.Change)
            {
                // Check invalid value of PersonCategory
                if (p.PersonCategory != 1 && p.PersonCategory != 2)
                {
                    // Throw DataServieException
                    throw new DataServiceException(400,
                        "The valid value of PersonCategory is 1" + 
                        "(for students) or 2(for instructors).");;
                }
            }
        }
    }
}
