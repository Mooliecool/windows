/****************************** Module Header ******************************\
* Module Name:                 MainPage.xaml.cs
* Project:                     CSSL3LocalMessage
* Copyright (c) Microsoft Corporation.
* 
* Local message whiteboard's code behind file.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 9/22/2009 6:34 PM Mog Liang Created
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
using System.Windows.Messaging;
using System.Runtime.Serialization.Json;
using System.IO;
using System.Windows.Ink;

namespace CSSL3LocalMessage
{
    public partial class MainPage : UserControl
    {
        // Preassign two names for LocalMessageReceiver.
        readonly string[] clientnames = { "{36BF6178-53A3-4d0b-8E0D-1E7BD9FBBE62}", "{E62BF131-B5EA-4f1d-B80D-DC083C7F0851}" };
        // Preassign two colors for distinguishing two applications.
        readonly Color[] colors = { Colors.Red, Colors.Green };
        int clientid;

        LocalMessageReceiver _localreceiver;
        LocalMessageSender _localsender;
        DataContractJsonSerializer _jsonserializer;

        public MainPage()
        {
            InitializeComponent();

            _jsonserializer =new DataContractJsonSerializer(inkP.Strokes.GetType());

            // First try.
            // Create receiver with one preset name.
            _localreceiver = new LocalMessageReceiver(clientnames[clientid]);
            _localreceiver.MessageReceived += localreceiver_MessageReceived;
            try
            {
                _localreceiver.Listen();
            }
            catch(Exception e)
            {
                // Second try.
                // Create receiver with another preset name.
                _localreceiver.MessageReceived -= localreceiver_MessageReceived;
                clientid = 1;
                _localreceiver = new LocalMessageReceiver(clientnames[clientid]);
                _localreceiver.MessageReceived += localreceiver_MessageReceived;
                try
                {
                    _localreceiver.Listen();
                }
                catch (Exception e1)
                {
                    // Already has two apllication instance.
                    // Pop messagebox, disable drawing area.
                    MessageBox.Show("Only allow 2 clients.");
                    disablegrid.Visibility = Visibility.Visible;
                    return;
                }
            }

            // Create sender targeting to another application's receiver
            _localsender = new LocalMessageSender(clientnames[(clientid + 1) % 2]);
            _localsender.SendCompleted += localsender_SendCompleted;

            drawbr.Background = new SolidColorBrush(colors[clientid]);
        }

        void localsender_SendCompleted(object sender, SendCompletedEventArgs e)
        {
            // If send message got error, pop messagebox.
            if(e.Error!=null)
                MessageBox.Show("Cannot connect to another client.");
        }

        void localreceiver_MessageReceived(object sender, MessageReceivedEventArgs e)
        {
            // Deserialize json string to stroke object.
            var stream = new MemoryStream();
            var streamwriter = new StreamWriter(stream);
            streamwriter.Write(e.Message);
            streamwriter.Flush();
            var receivedstroke = _jsonserializer.ReadObject(stream) as Stroke;
            stream.Close();

            // Add received stroke to inkPresenter.
            inkP.Strokes.Add(receivedstroke);
        }

        // Attaching MouseLeftButtonDown/MouseMove/MouseLeftButtonUp event
        // to implement drawing functionality.
        Stroke _newStroke;
        private void InkPresenter_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            inkP.CaptureMouse();
            _newStroke = new System.Windows.Ink.Stroke();
            _newStroke.DrawingAttributes.Color = colors[clientid];
            _newStroke.StylusPoints.Add(e.StylusDevice.GetStylusPoints(inkP));
            inkP.Strokes.Add(_newStroke);
        }

        // Sending message when finishing one stroke.
        private void InkPresenter_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            inkP.ReleaseMouseCapture();

            // Serialize stroke object to string.
            var stream = new MemoryStream();
            _jsonserializer.WriteObject(stream, _newStroke);
            stream.Flush();
            stream.Position = 0;
            var obstring = new StreamReader(stream).ReadToEnd();
            stream.Close();

            // Send Serialized stroke.
            if (_localsender != null)
            {
                try
                {
                    _localsender.SendAsync(obstring);
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message);
                }
            }

            _newStroke = null;
        }

        private void InkPresenter_MouseMove(object sender, MouseEventArgs e)
        {
            if (_newStroke != null)
            {
                _newStroke.StylusPoints.Add(e.StylusDevice.GetStylusPoints(inkP));
            }
        }
    }
}
