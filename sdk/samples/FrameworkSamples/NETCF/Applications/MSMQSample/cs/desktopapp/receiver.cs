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
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Messaging;
using System.Threading;
using System.Collections;

namespace Microsoft.Samples.MsmqSample
{
    /// <summary>
    /// Main server side application from which CD Orders are processed
    /// </summary>
    public partial class Receiver : Form
    {
        // ArrayList to keep track of Orders retrieved from the queue
        private ArrayList ordersRetrieved;
        // Delegate to Add Items to ListBox
        private delegate void AddListBoxDelegate(LabelIdMapping labelId);

        public Receiver()
        {
            InitializeComponent();

            //Initialize the MessageQueue Formatter so that the objects can 
            //serialized
            Type[] types = new Type[3];
            types[0] = typeof(CDOrder);
            types[1] = typeof(CompactDisc);
            types[2] = typeof(Customer);

          
            if ( ! MessageQueue.Exists( mqOrderQueue.Path ))
            {
                mqOrderQueue = MessageQueue.Create( mqOrderQueue.Path );
            }

            mqOrderQueue.Formatter = new XmlMessageFormatter(types);

            //Start a Thread which peeks at messages that come into the queue
            Thread peekThread = new Thread(new ThreadStart(PeekMessages));
            peekThread.IsBackground = true;
            peekThread.Start();
        }

        private void btnProcess_Click(object sender, EventArgs e)
        {
            //The LabelIdMapping helps to map the Label of a message with the message Id.
            //This enables each message to be uniquely identified
            LabelIdMapping labelID = (LabelIdMapping)lBOrders.SelectedItem;
            System.Messaging.Message message = mqOrderQueue.ReceiveById(labelID.Id);

            lBOrders.Items.Remove(labelID);
            lBOrders.SelectedIndex = -1;
            btnProcess.Enabled = false;

            System.Messaging.Message response = new System.Messaging.Message();

            CDOrder ackOrder = message.Body as CDOrder;
            if (null != ackOrder)
            {
                ackOrder.OrderTime = DateTime.Now;

                response.Body = ackOrder;
                response.Label = message.Label;
                response.CorrelationId = message.Id;

                message.ResponseQueue.Send(response);

                tBCDTitle.Text = String.Empty;
                tBCount.Text = String.Empty;
                tBCompany.Text = String.Empty;
                tBContact.Text = String.Empty;
                tBSent.Text = String.Empty;
                MessageBox.Show("The order was shipped");
            }
            else
            {
                MessageBox.Show("There is an unexpected item in the Queue");
            }
        }

        /// <summary>
        /// Thread function to peek at the messages in the Queue
        /// Adds newly arrived Orders in the message queue to the ListBox 
        /// </summary>
        protected void PeekMessages()
        {
            MessageEnumerator messageEnum = mqOrderQueue.GetMessageEnumerator2();
            //We keep track of Messages in List box already
            ordersRetrieved = new ArrayList();

            //The MessageEnumerator walks through the messages in the queue
            //MoveNext is set to a 4 hour time-out if no messages are in the queue

            while (messageEnum.MoveNext(new TimeSpan(4, 0, 0)))
            {
                LabelIdMapping labelID = new LabelIdMapping(messageEnum.Current.Label,
                                        messageEnum.Current.Id);
                if (!ordersRetrieved.Contains(messageEnum.Current.Id))
                {
                  //   lBOrders.Items.Add(labelID);
                    this.lBOrders.Invoke(new AddListBoxDelegate(AddListBox), labelID);
                }
                this.ordersRetrieved.Add(messageEnum.Current.Id);

            }
        }
        /// <summary>
        /// Function to Add incoming CD Orders to the ListBox
        /// </summary>
        private void AddListBox(LabelIdMapping labelID)
        {
            lBOrders.Items.Add(labelID);
        }
        /// <summary>
        /// SelectedIndex Changed event handler for ListBox
        ///  - Peeks at the Message selected in the Queue
        /// and Display information about the CD order in the queue
        /// </summary>

        private void lBOrders_SelectedIndexChanged(object sender, EventArgs e)
        {
            LabelIdMapping labelID = (LabelIdMapping)lBOrders.SelectedItem;

            if (labelID == null)
                return;

            MessagePropertyFilter filter = new MessagePropertyFilter();
            filter.SetDefaults();
            filter.Priority = true;
            mqOrderQueue.MessageReadPropertyFilter = filter;
            System.Messaging.Message message = mqOrderQueue.PeekById(labelID.Id);

            if (message.Body is CDOrder)
            {
                if (message.Priority > MessagePriority.Normal)
                {
                    lblFast.Visible = true;
                }
                else
                {
                    lblFast.Visible = false;
                }
                CDOrder cdOrder = message.Body as CDOrder;

                tBCDTitle.Text = cdOrder.Product.Title;
                tBCount.Text = cdOrder.Quantity.ToString();
                tBCompany.Text = cdOrder.Customer.Company;
                tBContact.Text = cdOrder.Customer.Contact;
                tBSent.Text = cdOrder.OrderTime.ToString("f");
                btnProcess.Enabled = true;
            }
            else
            {
                MessageBox.Show("The selected Item is not a book order");
            }

        }
        // Private class that maps the Label of the message to the ID. 
        private class LabelIdMapping
        {
            private string labelValue;
            private string idValue;
            public LabelIdMapping(string label, string id)
            {
                this.labelValue = label;
                this.idValue = id;
            }
            public override string ToString()
            {
                return labelValue;
            }

            public string Label
            {
                get
                {
                    return labelValue;
                }
            }
            public string Id
            {
                get
                {
                    return idValue;
                }
            }

        }
    }
}