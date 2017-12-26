//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------

#region Using directives

using System;
using System.Drawing;
using System.Collections;
using System.Windows.Forms;
using System.Data;
using System.Threading;
using System.Messaging;
using MessagingDemo.Common;
using System.Net;
using System.Net.Sockets;


#endregion

namespace Microsoft.Samples.MsmqSample
{
    /// <summary>
    /// Main device application from which CD Orders are sent
    /// </summary>
    public partial class OrderSender : System.Windows.Forms.Form
    {
        // Since this is a private queue we follow the naming convention
        //  MachineName\Private$\QueueName 
        // "." represents the local machine name
            
        private const string QueueName = @".\private$\mqdemoAck";
        // Delegate to updates the ListBox
        private delegate void UpdateListBoxDelegate(CDOrder order);

        public OrderSender()
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();
            
            // Create a MessageQueue where messages sent to the device
            // will be stored. 

           
                if (!MessageQueue.Exists(QueueName))
                {
                    mqdemoAck = MessageQueue.Create(QueueName);
                }
                else
                {
                    mqdemoAck = new MessageQueue(QueueName);
                }

                //Initialize the MessageQueue Formatter so that the objects can 
                //serialized
                System.Type[] types = new Type[3];
                types[0] = typeof(CDOrder);
                types[1] = typeof(CompactDisc);
                types[2] = typeof(Customer);

                mqdemoAck.Formatter = new XmlMessageFormatter(types);

                //Initiate the asynchronous receive operation by telling the Message
                // Queue to begin receiving messages and notify the event handler 
                // when finished
                mqdemoAck.ReceiveCompleted += 
                       new ReceiveCompletedEventHandler(QueueReceiveCompleted);
                mqdemoAck.BeginReceive();
           
        }

        private void menuItemExit_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        /// <summary>
        /// Event handler for the ReceiveCompleted event
        /// - The messages received asynchronously are processed
        /// </summary>
        private void QueueReceiveCompleted(Object source, 
                        ReceiveCompletedEventArgs asyncResult)
        {
            try
            {
                // End the Asynchronous Receive Operation
                System.Messaging.Message message = 
                    this.mqdemoAck.EndReceive(asyncResult.AsyncResult);
                
                //Only Process Messages that contain the CD Orders
                if (message.Body is CDOrder)
                {
                    CDOrder cdOrder = message.Body as CDOrder;
                    object[] orders = new object[1];
                    orders[0] = cdOrder;
                    //call to modify listbox UI on main thread
                    this.listBoxProc.Invoke(new UpdateListBoxDelegate(UpdateListBox), orders);
                }
                else
                {
                    MessageBox.Show("Message in Queue is not a CD Order");
                }
            }
            catch (MessageQueueException e)
            {
                MessageBox.Show
                    (String.Format(System.Globalization.CultureInfo.CurrentCulture,
                                            "Failed to receive Message: {0} ", e.ToString()));
            }
            //Begin the next Asynchronous Receive Operation
            this.mqdemoAck.BeginReceive();
        }

        
        /// <summary>
        /// Function to update the ListBox with incoming CD Orders
        /// </summary>
        private void UpdateListBox(CDOrder order)
        {
            listBoxProc.Items.Add(order);
            this.BackColor = Color.Green;

        }
        /// <summary>
        /// Click event handler for the Submit button
        /// </summary>
        private void btnSubmit_Click(object sender, EventArgs e)
        {
            try
            {
                this.btnSubmit.Enabled = false;
                //Check if server name has been specified
                if (textBoxServer.Text == String.Empty)
                {
                    MessageBox.Show("Enter a name for the Server");
                    return;
                }

                //Retrieve the IP Address of the device.
                string deviceIP = String.Empty;
                IPAddress[] addresses = Dns.GetHostByName(Dns.GetHostName()).AddressList;
                for (int i = 0; i < addresses.Length; ++i)
                {
                    IPAddress addr = addresses[i];
                    if (addr.AddressFamily == AddressFamily.InterNetwork)
                    {
                        deviceIP = addr.ToString();
                        break;
                    }   
                }
                if (deviceIP == String.Empty)
                {
                    MessageBox.Show("Unable to retrieve Device IP");
                    return;
                }

                //Create a CD object that will be sent to the Server's queue
                CDOrder cdOrder = new CDOrder();
                cdOrder.Quantity = Convert.ToInt32(tBQty.Text, 
                        System.Globalization.CultureInfo.CurrentUICulture);
                cdOrder.Product = new CompactDisc(this.comboCD.SelectedItem.ToString());
                cdOrder.Customer = new Customer(tBCompName.Text, tBContact.Text);
                cdOrder.OrderTime = dateTimePicker1.Value;

                // Send the CD object to a Queue on the server

                //Create an instance of the MessageQueue class that abstracts a 
                //connection to a queue specified on the server
                MessageQueue orderQueue = new MessageQueue
                    (String.Format(System.Globalization.CultureInfo.InvariantCulture,
                        @"FORMATNAME:DIRECT=OS:{0}\private$\mqsample", textBoxServer.Text));

                //A recoverable message is created and the priority of the message
                //is set accordingly.
                System.Messaging.Message message = new System.Messaging.Message(cdOrder);
                message.Recoverable = true;
                
                //The response Queue is specified to receive acknowledgement from the
                // server
                message.ResponseQueue = new MessageQueue
                    (String.Format(System.Globalization.CultureInfo.InvariantCulture,
                        @"FORMATNAME:DIRECT=TCP:{0}\private$\mqdemoAck",deviceIP));

                if (chkFast.Checked)
                {
                    message.Priority = MessagePriority.High;
                }

                // Send the Message to the Queue
                orderQueue.Send
                    (message,String.Format(System.Globalization.CultureInfo.InvariantCulture,
                                            "CD Order ({0})",cdOrder.Customer.Contact));
                this.ClearControls();
            }
            catch (FormatException ex)
            {
                MessageBox.Show(ex.Message, "Conversion Error");
                this.ClearControls();
            }
            catch (System.Net.Sockets.SocketException ex)
            {
                MessageBox.Show
                    (String.Format(System.Globalization.CultureInfo.CurrentCulture,
                                    "Failed to resolve Host Name:\n {0}", ex.Message));
                this.ClearControls();
            }
            catch (MessageQueueException ex)
            {
                MessageBox.Show(ex.Message, "Error sending Message");
                this.ClearControls();
            }
            finally
            {
                this.btnSubmit.Enabled = true;
                
            }

        }
        private void ClearControls()
        {
            //Clear the controls
            this.tBCompName.Text = this.tBContact.Text = this.tBQty.Text = String.Empty;
            this.comboCD.SelectedIndex = -1;
            this.chkFast.Checked = false;
        }
        /// <summary>
        /// Click event handler for the Display button
        ///  - Displays the Details about the CDs that have been
        ///    processed by the server.
        /// </summary>
        private void btnDisplay_Click(object sender, EventArgs e)
        {
            CDOrder cdOrder = (CDOrder)listBoxProc.SelectedItem;

            //nothing selected
            if (cdOrder == null)
                return;

            string display = "Title :" + cdOrder.Product.Title + "\n"+
                "Quantity: " + cdOrder.Quantity.ToString(System.Globalization.CultureInfo.CurrentUICulture) 
                + "\n" +
                "Company: " + cdOrder.Customer.Company 
                + "\n" +
                "Contact: " + cdOrder.Customer.Contact 
                + "\n" +
                "Shipped: " + cdOrder.OrderTime.ToString
                ("g", System.Globalization.CultureInfo.CurrentUICulture);

            MessageBox.Show(display);
           listBoxProc.Items.Remove(cdOrder);
            if (listBoxProc.Items.Count == 0)
                this.BackColor = Color.Red;
        }
    }
}

