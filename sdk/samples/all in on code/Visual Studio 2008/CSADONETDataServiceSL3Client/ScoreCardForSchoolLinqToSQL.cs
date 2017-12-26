/****************************** Module Header ******************************\
* Module Name:	ScoreCardForSchoolLinqToSQL.cs
* Project:		CSADONETDataServiceSL3Client
* Copyright (c) Microsoft Corporation.
* 
* ScoreCardForSchoolLinqToSQL.cs demonstrates how to create a class as a real
* data source for UI. It's a bridge between data retrieved from auto generated ADO.NET 
* Data Service client side code and the UI. Some tricks are also shown here such as 
* Display and Editable attribute.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 8/18/2009 2:00 PM Allen Chen Created
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
using System.ComponentModel.DataAnnotations;
using CSADONETDataServiceSL3Client.SchoolLinqToSQLService;

namespace CSADONETDataServiceSL3Client
{
    // The collection of ScoreCardForSchoolLinqToSQL objects is the data source
    // of DataGrid control.
    public class ScoreCardForSchoolLinqToSQL
    {
        // Maintain a reference to CourseGrade for delete  purpose.
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

        [Editable(false)]
        public decimal? Grade
        {
            get;
            set;
        }
    }
}
