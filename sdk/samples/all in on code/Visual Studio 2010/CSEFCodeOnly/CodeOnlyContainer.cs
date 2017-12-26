/****************************** Module Header ******************************\
* Module Name:    CodeOnlyContainer.cs
* Project:        CSEFCodeOnly
* Copyright (c) Microsoft Corporation.
*
* This code file contains the POCO entity classes and their custom 
* ObjectContext object.  The POCO entity classes include Type-per-Hierarchy
* inheritance entities, Type-per-Table inheritance entities, other relational 
* entities, and Complex Type classes.  
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
using System.Linq;
using System.Text;
using System.Data.Objects;
using System.Data.EntityClient;
#endregion

namespace CSEFCodeOnly
{
    // The custom ObjectContext for the POCO entities
    public class CodeOnlyContainer : ObjectContext
    {
        #region Constructor
        public CodeOnlyContainer(EntityConnection connection)
            : base(connection)
        {
            // Set the DefaultContainerName
            DefaultContainerName = "CodeOnlyContainer";
        }
        #endregion

        #region IObjectSet<> collections
        public IObjectSet<PersonTPT> PeopleTPT
        {
            get { return base.CreateObjectSet<PersonTPT>(); }
        }

        public IObjectSet<Course> Courses
        {
            get { return base.CreateObjectSet<Course>(); }
        }

        public IObjectSet<Department> Departments
        {
            get { return base.CreateObjectSet<Department>(); }
        }

        public IObjectSet<CourseStudent> CourseStudents
        {
            get { return base.CreateObjectSet<CourseStudent>(); }
        }

        public IObjectSet<PersonTPH> PeopleTPH
        {
            get { return base.CreateObjectSet<PersonTPH>(); }
        }
        #endregion
    }

    #region Type-per-Table inheritance POCO entities
    public class PersonTPT
    {
        // PK property
        public int PersonID { get; set; }

        // Complex Type properties
        public Name Name { get; set; }
        public Address Address { get; set; }
    }

    public class InstructorTPT : PersonTPT 
    {
        public DateTime HireDate { get; set; }

        // Navigation property
        public ICollection<Course> Courses { get; set; }
    }

    public class AdminTPT : PersonTPT
    {
        public DateTime AdminDate { get; set; }
    }

    public class StudentTPT : PersonTPT 
    {
        public DateTime EnrollmentDate { get; set; }
        public int? Degree { get; set; }
        public int? Credits { get; set; }

        // Navigation property
        public ICollection<CourseStudent> CourseStudents { get; set; }
    }

    public class BusinessStudentTPT : StudentTPT
    {
        public int? BusinessCredits { get; set; }
    }
    #endregion

    #region Other relational POCO entities
    public class Department
    {
        // PK property
        public int DepartmentID { get; set; }

        public string Name { get; set; }
        public decimal? Budget { get; set; }
        public DateTime StartDate { get; set; }

        // Navigation property
        public ICollection<Course> Courses { get; set; }
    }

    public class Course
    {
        // PK property
        public int CourseID { get; set; }

        public string Title { get; set; }
        public int Credits { get; set; }

        // Navigation properties
        public Department Department { get; set; }
        public ICollection<InstructorTPT> Instructors { get; set; }
        public ICollection<CourseStudent> CourseStudents { get; set; }
        
        // FK Association property
        public int DepartmentID { get; set; }
    }

    public class CourseStudent
    {
        // Composite PK properties 
        public int PersonID { get; set; }
        public int CourseID { get; set; }
        
        public int? Score { get; set; }

        // Navigation properties
        public StudentTPT Student { get; set; }
        public Course Course { get; set; }
    }
    #endregion

    #region Type-per-Hierarchy inheritance POCO entities
    public class PersonTPH
    {
        // PK property
        public int PersonID { get; set; }

        // Complex Type properties
        public Name Name { get; set; }
        public Address Address { get; set; }
    }

    public class InstructorTPH : PersonTPH
    {
        public DateTime? HireDate { get; set; }
    }

    public class StudentTPH : PersonTPH
    {
        public DateTime? EnrollmentDate { get; set; }
    }

    public class AdminTPH : PersonTPH
    {
        public DateTime? AdminDate { get; set; }
    }
    #endregion

    #region Complex Type entities
    public class Name
    {
        public string FirstName { get; set; }
        public string LastName { get; set; }

        // Override the ToString method
        public override string ToString()
        {
            return this.FirstName + " " + this.LastName;
        }
    }

    public class Address
    {
        public string City { get; set; }
        public string Country { get; set; }
        public string Zipcode { get; set; }
    }
    #endregion
}
