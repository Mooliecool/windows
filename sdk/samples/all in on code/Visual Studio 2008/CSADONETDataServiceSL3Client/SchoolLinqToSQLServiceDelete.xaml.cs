/****************************** Module Header ******************************\
* Module Name:	SchoolLinqToSQLServiceDelete.xaml.cs
* Project:		CSADONETDataServiceSL3Client
* Copyright (c) Microsoft Corporation.
* 
* SchoolLinqToSQLServiceDelete.cs demonstrates how to call ASP.NET Data Service
* to select and delete records in Silverlight.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using CSADONETDataServiceSL3Client.SchoolLinqToSQLService;
using System.Data.Services.Client;

namespace CSADONETDataServiceSL3Client
{
    public partial class SchoolLinqToSQLServiceDelete : UserControl
    {
        // The data source of DataGrid control
        private List<ScoreCardForSchoolLinqToSQL> _collection = new List<ScoreCardForSchoolLinqToSQL>();
        // The URL of ADO.NET Data Service
        private const string _schoolLinqToSQLUri =
            "http://localhost:8888/SchoolLinqToSQL.svc";
        // collection => returnedCourseGrade => _context ={via async REST call}=> ADO.NET Data Service
        private SchoolLinqToSQLDataContext _context;

        public SchoolLinqToSQLServiceDelete()
        {
            InitializeComponent();
            this.Loaded += new RoutedEventHandler(SchoolLinqToSQLServiceDelete_Loaded);
        }

        void SchoolLinqToSQLServiceDelete_Loaded(object sender, RoutedEventArgs e)
        {
            LoadData();
        } 
        
        /// <summary>
        /// In this method we send a REST request to ADO.NET Data Service to request CourseGrade
        /// records. We expand Person and Course so the foreign records will be returned as well
        /// </summary>
        private void LoadData()
        {
            _context = new SchoolLinqToSQLDataContext(new Uri(_schoolLinqToSQLUri));
            DataServiceQuery<CourseGrade> query = (DataServiceQuery<CourseGrade>)(
                from c in _context.CourseGrades.Expand("Person").Expand("Course")
                select c);

            query.BeginExecute(OnCourseGradeQueryComplete, query);
        }

        /// <summary>
        /// Callback method of the query to get CourseGrade records.
        /// </summary>
        /// <param name="result"></param>
        private void OnCourseGradeQueryComplete(IAsyncResult result)
        {
            Dispatcher.BeginInvoke(() =>
            {
                DataServiceQuery<CourseGrade> query =
                       result.AsyncState as DataServiceQuery<CourseGrade>;
                try
                {
                    var returnedCourseGrade =
                        query.EndExecute(result);

                    if (returnedCourseGrade != null)
                    {
                        _collection = (from c in returnedCourseGrade.ToList()
                                      select new ScoreCardForSchoolLinqToSQL()
                                      {
                                          CourseGrade = c,
                                          Course = c.Course.Title,
                                          Grade = c.Grade,
                                          PersonName = string.Format("{0} {1}",
                                          c.Person.FirstName, c.Person.LastName)
                                      }).ToList();

                        this.mainDataGrid.ItemsSource = _collection;
                    }
                }
                catch (DataServiceQueryException ex)
                {
                    this.messageTextBlock.Text = string.Format("Error: {0} - {1}",
                        ex.Response.StatusCode.ToString(), ex.Response.Error.Message);
                }

            });

        }

        /// <summary>
        /// This event handler handles the Click event of the Delete Button control
        /// It deletes the record that is on the same row of the Delete Button,
        /// via an async REST call to ADO.NET Data Service.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void DeleteButton_Click(object sender, RoutedEventArgs e)
        {
            Button b = (Button)sender;
            ScoreCardForSchoolLinqToSQL s = b.DataContext as ScoreCardForSchoolLinqToSQL;
            if (s != null) {
                CourseGrade recordfordelete = s.CourseGrade;
                _context.DeleteObject(recordfordelete);
                _context.BeginSaveChanges(SaveChangesOptions.None,
                    OnChangesSaved, _context);
            }
        }  
        
        /// <summary>
        /// Callback method of the delete operation.
        /// </summary>
        /// <param name="result"></param>
        private void OnChangesSaved(IAsyncResult result)
        {
            Dispatcher.BeginInvoke(() =>
            {
                SchoolLinqToSQLDataContext svcContext = result.AsyncState as SchoolLinqToSQLDataContext;

                try
                {
                    // Complete the save changes operation and display the response.
                    WriteOperationResponse(svcContext.EndSaveChanges(result));
                }
                catch (DataServiceRequestException ex)
                {
                    // Display the error from the response.
                    WriteOperationResponse(ex.Response);
                }
                catch (InvalidOperationException ex)
                {
                    messageTextBlock.Text = ex.Message;
                }
                finally
                {
                    // Reload the binding collection to refresh Grid to see if it's successfully updated. 
                    // You can also remove the following line. To see the update effect, just refresh the page or check out database directly.
                    LoadData();
                }
            });

        }
        
        /// <summary>
        /// This method shows details information of the response from ADO.NET Data Service.
        /// </summary>
        /// <param name="response"></param>
        private void WriteOperationResponse(DataServiceResponse response)
        {
            messageTextBlock.Text = string.Empty;
            int i = 1;

            if (response.IsBatchResponse)
            {
                messageTextBlock.Text = string.Format("Batch operation response code: {0}\n",
                    response.BatchStatusCode);
            }
            foreach (ChangeOperationResponse change in response)
            {
                messageTextBlock.Text +=
                    string.Format("\tChange {0} code: {1}\n",
                    i.ToString(), change.StatusCode.ToString());
                if (change.Error != null)
                {
                    string.Format("\tChange {0} error: {1}\n",
                        i.ToString(), change.Error.Message);
                }
                i++;
            }
        }
    }
}
