/****************************** Module Header ******************************\
* Module Name:    SchoolDB.cs
* Project:        CSLinqToSQL
* Copyright (c) Microsoft Corporation.
*
* Manually create the object model of Student / Course / CourseGrade.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
* History:
* * 3/08/2009 9:28 PM Rongchun Zhang Created
* * 3/27/2009 5:40 PM Jialiang Ge Reviewed
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.Linq.Mapping;
using System.Data.Linq;
using System.Configuration;
#endregion


namespace CSLinqToSQL.Manual
{
    [Table(Name = "Person")]
    public class Student
    {
        public Student()
        {
            this._Grades = new EntitySet<CourseGrade>();

            // Set the default value of the column PersonCategory
            this.PersonCategory = 1;
        }

        // Refer to the ID column in database.
        [Column(IsPrimaryKey = true, Name = "PersonID", IsDbGenerated = true,
            AutoSync = AutoSync.OnInsert, DbType = "Int NOT NULL IDENTITY")]
        public int PersonID
        {
            get;
            set;
        }

        // Refer to the LastName column in database
        [Column]
        public string LastName
        {
            get;
            set;
        }

        // Refer to the FirstName column in database
        [Column]
        public string FirstName
        {
            get;
            set;
        }

        // Refer to the EnrollmentDate column in database
        [Column]
        public System.Nullable<System.DateTime> EnrollmentDate
        {
            get;
            set;
        }

        // Refer to the PersonCategory column in database
        [Column]
        public short PersonCategory
        {
            get;
            set;
        }

        // Refer to the FK of FK_CourseGrade_Student 
        private EntitySet<CourseGrade> _Grades;
        [Association(Storage = "_Grades", OtherKey = "StudentID")]
        public EntitySet<CourseGrade> CourseGrades
        {
            get { return this._Grades; }
            set { this._Grades.Assign(value); }
        }


        // Refer to the image column in database
        [Column(Name = "Picture")]
        public Byte[] Picture
        {
            get;
            set;
        }
    }

    [Table(Name = "Course")]
    public class Course
    {
        public Course()
        {
            this._Grades = new EntitySet<CourseGrade>();
        }

        // Refer to the CourseID column in database
        [Column(IsPrimaryKey = true, Name = "CourseID")]
        public int CourseID
        {
            get;
            set;
        }

        // Refer to the Title column in database
        [Column(Name = "Title")]
        public string Title
        {
            get;
            set;
        }

        // Refer to the Credits column in database
        [Column]
        public int Credits
        {
            get;
            set;
        }

        // Refer to the FK for FK_CourseGrade_Course
        private EntitySet<CourseGrade> _Grades;
        [Association(Storage = "_Grades", OtherKey = "CourseID")]
        public EntitySet<CourseGrade> Grades
        {
            get { return this._Grades; }
            set { this._Grades.Assign(value); }
        }

    }

    [Table(Name = "CourseGrade")]
    public class CourseGrade
    {
        public CourseGrade()
        {
            this._Course = default(EntityRef<Course>);
            this._Student = default(EntityRef<Student>);
        }

        // Refer to the EnrollmentID column in database
        [Column(IsPrimaryKey = true, Name = "EnrollmentID", IsDbGenerated = true)]
        public int EnrollmentID
        {
            get;
            set;
        }

        // Refer to the CourseID column in database
        [Column(Name = "CourseID")]
        public int CourseID
        {
            get;
            set;
        }

        // Refer to the StudentID column in database
        [Column(Name = "StudentID")]
        public int StudentID
        {
            get;
            set;
        }

        // Refer to the Grade column in database
        [Column]
        public decimal Grade
        {
            get;
            set;
        }

        // Refer to the FK of FK_CourseGrade_Course
        private EntityRef<Course> _Course;
        [Association(Storage = "_Course", ThisKey = "CourseID")]
        public Course Course
        {
            get { return this._Course.Entity; }
            set { this._Course.Entity = value; }
        }

        // Refer to the FK of FK_CourseGrade_Student 
        private EntityRef<Student> _Student;
        [Association(Storage = "_Student", ThisKey = "StudentID")]
        public Student Student
        {
            get { return this._Student.Entity; }
            set { this._Student.Entity = value; }
        }
    }

    public class SchoolDataContext : DataContext
    {
        public Table<Student> Students;
        public Table<Course> Courses;
        public Table<CourseGrade> CourseGrades;
        public SchoolDataContext(string connection) : base(connection) { }
    }
}
