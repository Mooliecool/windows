/****************************** Module Header ******************************\
* Module Name:  ScoreCardForSchoolLinqToEntities.cs
* Project:      CSADONETDataServiceSL3Client
* Copyright (c) Microsoft Corporation.
* 
* ScoreCardForSchoolLinqToEntities.cs demonstrates how to create a class as a real
* data source for UI. It's a bridge between data retrieved from auto generated ADO.NET 
* Data Service client side code and the UI. Some tricks are also shown here such as 
* Display and Editable attribute.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 8/17/2009 4:00 PM Allen Chen Created
\***************************************************************************/

using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.ComponentModel;
using System.ComponentModel.DataAnnotations;
using CSADONETDataServiceSL3Client.SchoolLinqToEntitiesService;

namespace CSADONETDataServiceSL3Client
{
    // The collection of ScoreCardForSchoolLinqToEntities objects is the data source
    // of DataGrid control.
    public class ScoreCardForSchoolLinqToEntities 
    {   
        // Maintain a reference to CourseGrade for update  purpose
        [Display(AutoGenerateField = false)]
        public CourseGrade CourseGrade
        { 
            get; 
            set; 
        }
        [Editable(false)]
        public string PersonName
        {
            get;
            set;
        }
        [Editable(false)]
        public string Course
        {
            get;
            set;
        }
        private decimal? _grade;
        public decimal? Grade
        {
            get { return _grade; }
            set {
                // When the Grade gets changed, update Grade property of the CourseGrade in memory.
                if (CourseGrade != null) 
                {
                CourseGrade.Grade = value;
                }
            _grade = value;
            }
        }

       
    }
}
