/****************************** Module Header ******************************\
* Module Name:  COMHelper.cs
* Project:      CSCOMService
* Copyright (c) Microsoft Corporation.
* 
* The CSWPFThreading sample project illustrates two WPF threading models. The
* first one divides a long-running process into many snippets of workitems.  
* Then the dispather of WPF will pick up the workitems one by one from the 
* queue by their priority. The background workitem does not affect the UI 
* operation, so it just looks like the background workitem is processed by 
* another thread. But actually, all of them are executed in the same thread. 
* This trick is very useful if you want single threaded GUI application, and 
* also want to keep the GUI responsive when doing expensive operations in  
* the UI thread. 
* 
* The second model is similar to the traditional WinForm threading model. The 
* background work item is executed in another thread and it calls the 
* Dispatcher.BeginInvoke method to update the UI.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 4/16/2009 3:23 PM Ji Zhou Created
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;
using System.Threading;
#endregion


namespace CSWPFThreading
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class Window1 : Window
    {
        public Window1()
        {
            InitializeComponent();
        }


        #region Long-Running Calculation in UI Thread

        public delegate void NextPrimeDelegate();
        private long num = 3;
        private bool continueCalculating = false;
        private bool fNotAPrime = false;

        private void btnPrimeNumber_Click(object sender, RoutedEventArgs e)
        {
            if (continueCalculating)
            {
                continueCalculating = false;
                btnPrimeNumber.Content = "Resume";
            }
            else
            {
                continueCalculating = true;
                btnPrimeNumber.Content = "Stop";
                btnPrimeNumber.Dispatcher.BeginInvoke(
                    DispatcherPriority.Normal,
                    new NextPrimeDelegate(CheckNextNumber));
            }
        }

        public void CheckNextNumber()
        {
            // Reset flag.
            fNotAPrime = false;

            for (long i = 3; i <= Math.Sqrt(num); i++)
            {
                if (num % i == 0)
                {
                    // Set not-a-prime flag to true.
                    fNotAPrime = true;
                    break;
                }
            }

            // If a prime number.
            if (!fNotAPrime)
            {
                tbPrime.Text = num.ToString();
            }

            num += 2;
            if (continueCalculating)
            {
                btnPrimeNumber.Dispatcher.BeginInvoke(
                    System.Windows.Threading.DispatcherPriority.SystemIdle,
                    new NextPrimeDelegate(this.CheckNextNumber));
            }
        }

        #endregion


        #region Blocking Operation in Worker Thread

        private delegate void NoArgDelegate();
        private delegate void OneArgDelegate(Int32[] arg);

        private void btnRetrieveData_Click(object sender, RoutedEventArgs e)
        {
            this.btnRetrieveData.IsEnabled = false;
            this.btnRetrieveData.Content = "Contacting Server";

            NoArgDelegate fetcher = new NoArgDelegate(
                this.RetrieveDataFromServer);
            fetcher.BeginInvoke(null, null);
        }

        /// <summary>
        /// Retrieve data in a worker thread.
        /// </summary>
        private void RetrieveDataFromServer()
        {
            // Simulate the delay from network access.
            Thread.Sleep(5000);

            // Generate random data to be displayed.
            Random rand = new Random();
            Int32[] data = {
                               rand.Next(1000), rand.Next(1000), 
                               rand.Next(1000), rand.Next(1000) 
                           };

            // Schedule the update function in the UI thread.
            this.Dispatcher.BeginInvoke(
                System.Windows.Threading.DispatcherPriority.Normal,
                new OneArgDelegate(UpdateUserInterface),
                data);
        }

        /// <summary>
        /// Update the UI about the new data. The function runs in the UI thread.
        /// </summary>
        /// <param name="data"></param>
        private void UpdateUserInterface(Int32[] data)
        {
            this.btnRetrieveData.IsEnabled = true;
            this.btnRetrieveData.Content = "Retrieve Data from Server";
            this.tbData1.Text = data[0].ToString();
            this.tbData2.Text = data[1].ToString();
            this.tbData3.Text = data[2].ToString();
            this.tbData4.Text = data[3].ToString();
        }

        #endregion

    }
}
