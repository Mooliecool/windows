/****************************** Module Header ******************************\
* Module Name:	SampleServiceInsert.xaml.cs
* Project:		CSADONETDataServiceSL3Client
* Copyright (c) Microsoft Corporation.
* 
* SampleServiceInsert.cs demonstrates how to call ASP.NET Data Service
* to select and insert records in Silverlight.
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
using CSADONETDataServiceSL3Client.SampleService;
using System.Data.Services.Client;

namespace CSADONETDataServiceSL3Client
{
    public partial class SampleServiceInsert : UserControl
    {
        // The URL of ADO.NET Data Service
        private const string _sampleUri =
             "http://localhost:8888/Samples.svc";
        // returnedCategory => _context ={via async REST call}=> ADO.NET Data Service
        private SampleProjects _context;
        public SampleServiceInsert()
        {
            InitializeComponent();
            this.Loaded += new RoutedEventHandler(SampleServiceInsert_Loaded);
        }

        void SampleServiceInsert_Loaded(object sender, RoutedEventArgs e)
        {
            LoadData();
        }

        /// <summary>
        /// In this method we send a REST request to ADO.NET Data Service to request Category
        /// records. 
        /// </summary>
        private void LoadData()
        {
            _context = new SampleProjects(new Uri(_sampleUri));
            DataServiceQuery<Category> query = (DataServiceQuery<Category>)(
                from c in _context.Categories
                select c);

            query.BeginExecute(OnCategoryQueryComplete, query);
        }

        /// <summary>
        /// Callback method of the query to get Category records.
        /// </summary>
        /// <param name="result"></param>
        private void OnCategoryQueryComplete(IAsyncResult result)
        {
            Dispatcher.BeginInvoke(() =>
            {

                DataServiceQuery<Category> query =
                       result.AsyncState as DataServiceQuery<Category>;
                try
                {
                    var returnedCategory =
                        query.EndExecute(result);

                    if (returnedCategory != null)
                    {

                        this.mainDataGrid.ItemsSource = returnedCategory.ToList();
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
        /// In this event handler, we begin to send a REST request to ADO.NET
        /// Data Service to insert a new Category.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void InsertButton_Click(object sender, RoutedEventArgs e)
        {
            string categorynameforinsert = this.categoryTextBox.Text;
            _context.AddToCategories(new Category() { CategoryName = categorynameforinsert });
            _context.BeginSaveChanges(OnChangesSaved, _context);
        } 
        
        /// <summary>
        /// Callback method of the insert operation.
        /// </summary>
        /// <param name="result"></param>
        private void OnChangesSaved(IAsyncResult result)
        {
            Dispatcher.BeginInvoke(() =>
            {
                SampleProjects svcContext = result.AsyncState as SampleProjects;

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
