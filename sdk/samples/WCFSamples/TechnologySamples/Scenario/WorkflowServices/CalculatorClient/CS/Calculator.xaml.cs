//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Runtime.Serialization;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Xml;
using Microsoft.WorkflowServices.Samples;

namespace Microsoft.WorkflowServices.Samples
{
    /// <summary>
    /// Interaction logic for CalculatorWindow.xaml
    /// </summary>
    


    public partial class CalculatorWindow : Window
    {
        const string contextFile = "client.ctx";
        Brush onBrush = Brushes.LightSteelBlue;
        Brush offBrush = Brushes.GhostWhite;

        MenuItem currentBinding;
        CalculatorClient client = null;
        IDictionary<string, string> context = null;
        bool powerOn = false;
        bool displayNew = true;
        bool contextApplied = false;

        public CalculatorWindow()
        {
            InitializeComponent();
            currentBinding = ContextOverHttp;
            Context = ContextManager.DepersistContext(contextFile);
            if (Context != null)
                PowerOn = true;
        }

        CalculatorClient Client
        {
            get
            {
                if (client == null)
                    client = new CalculatorClient(currentBinding.Name);
                return client;
            }
        }

        IDictionary<string, string> Context
        {
            get { return context; }
            set
            {
                Status.Text = String.Empty;
                context = value;
                if (context != null)
                {
                    foreach (KeyValuePair<string, string> item in context)
                        Status.Text += item.Key + ":" + item.Value;
                }
            }
        }

        bool PowerOn
        {
            get { return powerOn; }
            set
            {
                if (powerOn != value)
                {
                    powerOn = value;
                    if (powerOn)
                    {
                        DoOperation(ButtonEqual, null);
                        Display.Background = onBrush;
                        ButtonC.Background = Brushes.DarkSeaGreen;
                    }
                    else
                    {
                        Display.Text = String.Empty;
                        Display.Background = offBrush;
                        ButtonC.Background = Brushes.Tomato;
                    }
                }
            }
        }

        void UpdateDisplay(object sender, RoutedEventArgs e)
        {
            string input = ((Button)sender).Content.ToString();
            if (displayNew)
            {
                Display.Text = input;
                Display.Background = offBrush;
            }
            else
                Display.Text += input;
            displayNew = false;
        }

        void DoOperation(object sender, RoutedEventArgs e)
        {
            int value = 0;
            string operation = ((Button)sender).Content.ToString();
            if (!operation.StartsWith("=") && !int.TryParse(Display.Text,out value))
            {
                MessageBox.Show("Invalid input! Try again.");
                Display.Text = String.Empty;
            }

            try
            {
                if (!PowerOn)
                    PowerSwitch(ButtonC, null);

                using (new OperationContextScope((IContextChannel)Client.InnerChannel))
                {
                    if (!contextApplied)
                    {
                        ContextManager.ApplyContextToChannel(Context, Client.InnerChannel);
                        contextApplied = true;
                    }

                    switch (operation)
                    {
                        case "=": { value = Client.Add(0); break; }
                        case "+": { value = Client.Add(value); break; }
                        case "-": { value = Client.Subtract(value); break; }
                        case "x": { value = Client.Multiply(value); break; }
                        case "/": {
                                    if (value == 0)
                                    {
                                        MessageBox.Show("Divide By Zero is not allowed");
                                        value = client.Add(0);
                                        break;
                                    }
                                    else
                                    {
                                        value = Client.Divide(value);
                                        break;
                                    }
                                  }
                    }
                }
            }
            catch (CommunicationException ce)
            {
                MessageBox.Show(ce.Message);
                Client.Abort();
                client = null;
            }

            Display.Background = onBrush;
            Display.Text = value.ToString();
            displayNew = true;
        }

        void PowerSwitch(object sender, RoutedEventArgs e)
        {
            try
            {
                using (new OperationContextScope((IContextChannel)Client.InnerChannel))
                {
                    if (PowerOn)
                    {
                        PowerOn = false;
                        if (!contextApplied)
                        {
                            ContextManager.ApplyContextToChannel(Context, Client.InnerChannel);
                            contextApplied = true;
                        }
                        Context = null;
                        ContextManager.DeleteContext(contextFile);
                        Client.PowerOff();
                        Client.Close();
                        client = null;
                    }
                    else
                    {
                        Client.PowerOn();

                        // At this point the channel has the context from the operation called
                        contextApplied = true;
                        // extract and persist context
                        Context = ContextManager.ExtractContextFromChannel(client.InnerChannel);
                        ContextManager.PersistContext(Context, contextFile);
                        PowerOn = true;
                    }
                }
            }
            catch (CommunicationException ce)
            {
                MessageBox.Show(ce.Message);
                Client.Abort();
                client = null;
            }
        }

        void ChangeBinding(object sender, RoutedEventArgs e)
        {
            MenuItem item = (MenuItem)sender;
            item.IsChecked = true;
            if (item != currentBinding)
            {   // change Binding
                Client.Close();
                client = null;
                currentBinding.IsChecked = false;
                currentBinding = item;
            }
        }

        void OnExit(object sender, RoutedEventArgs e)
        {
            this.Close();
            Application.Current.Shutdown();
        }

        #region Context Utilities 
/*
        Uri ccUri = new Uri("http://private/cookiecontainer");
        bool ApplyContext(IDictionary<XmlQualifiedName, string> context, IClientChannel channel)
        {   
            if (context != null)
            {
                IContextManager cm = channel.GetProperty<IContextManager>();
                if (cm != null && cm.GetContext() == null)
                {   // apply context to ContextChannel
                    cm.SetContext(context);
                    return true;
                }
                else if (OperationContext.Current != null)
                {   // apply context as HttpCookie
                    CookieContainer cookies = new CookieContainer();
                    foreach (KeyValuePair<XmlQualifiedName, string> item in context)
                    {
                        cookies.Add(ccUri,new Cookie(item.Key.ToString(),item.Value));
                    }
                    if (cookies.Count > 0)
                    {
                        HttpRequestMessageProperty httpRequest = new HttpRequestMessageProperty();
                        OperationContext.Current.OutgoingMessageProperties.Add(HttpRequestMessageProperty.Name, httpRequest);
                        httpRequest.Headers.Add(HttpRequestHeader.Cookie, cookies.GetCookieHeader(ccUri));
                        return true;
                    }
                }
            }
            return false;
        }

        IDictionary<XmlQualifiedName, string> ExtractContext(IClientChannel channel)
        {   // extract context from channel
            IContextManager cm = channel.GetProperty<IContextManager>();
            if (cm != null)
            {   // attempt to extract context from channel
                return cm.GetContext();
            }
            else if (OperationContext.Current != null)
            {   // attempt to extract context from HttpCookie
                CookieContainer cookies = new CookieContainer();
                if (OperationContext.Current.IncomingMessageProperties.ContainsKey(HttpResponseMessageProperty.Name))
                {
                    HttpResponseMessageProperty httpResponse = (HttpResponseMessageProperty)OperationContext.Current.IncomingMessageProperties[HttpResponseMessageProperty.Name];
                    cookies.SetCookies(ccUri,httpResponse.Headers[HttpResponseHeader.SetCookie]);
                }
                if (cookies.Count > 0)
                {   // parse into dictionary
                    Dictionary<XmlQualifiedName, string> newContext = new Dictionary<XmlQualifiedName, string>();
                    foreach(Cookie cookie in cookies.GetCookies(ccUri))
                    {
                        string[] ns = cookie.Name.Split(':');
                        newContext.Add(new XmlQualifiedName(ns[2], ns[0]+":"+ns[1]), cookie.Value);
                    }
                    return newContext;
                }
            }
            return null;
        }

        IDictionary<XmlQualifiedName, string> DepersistContext(string fileName)
        {   // retrieve context from file
            if (File.Exists(contextFile))
            {
                DataContractSerializer serializer = new DataContractSerializer(typeof(Dictionary<XmlQualifiedName, string>));
                FileStream stream = new FileStream(fileName, FileMode.Open);
                return (Dictionary<XmlQualifiedName, string>)serializer.ReadObject(stream);
            }
            return null;
        }

        void PersistContext(IDictionary<XmlQualifiedName, string> context, string fileName)
        {   // persist context to file
            if (context != null)
            {   
                DataContractSerializer serializer = new DataContractSerializer(typeof(Dictionary<XmlQualifiedName, string>));
                FileStream stream = new FileStream(fileName, FileMode.Create);
                serializer.WriteObject(stream, context);
            }
        }

        void DeleteContext(string fileName)
        {
            if (File.Exists(contextFile))
                File.Delete(contextFile);
        }
*/
        #endregion

    }


}