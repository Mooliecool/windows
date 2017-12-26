' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.Collections.Generic
Imports System.IO
Imports System.Net
Imports System.Runtime.Serialization
Imports System.ServiceModel
Imports System.ServiceModel.Channels
Imports System.Text
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Data
Imports System.Windows.Documents
Imports System.Windows.Input
Imports System.Windows.Media
Imports System.Windows.Media.Imaging
Imports System.Windows.Shapes
Imports System.Xml

Imports Microsoft.WorkflowServices.Samples

Namespace Microsoft.WorkflowServices.Samples

    Partial Public Class CalculatorWindow
        Inherits Window

        Const contextFile As String = "client.ctx"
        Private onBrush As Brush = Brushes.LightSteelBlue
        Private offBrush As Brush = Brushes.GhostWhite

        Private currentBinding As MenuItem
        Private m_client As CalculatorClient = Nothing
        Private m_context As IDictionary(Of String, String) = Nothing
        Private m_powerOn As Boolean = False
        Private displayNew As Boolean = True
        Private contextApplied As Boolean = False

        Public Sub New()
            InitializeComponent()
            currentBinding = ContextOverHttp
            Context = ContextManager.DepersistContext(contextFile)
            If Context IsNot Nothing Then
                PowerOn = True
            End If
        End Sub

        Private ReadOnly Property Client() As CalculatorClient
            Get
                If m_client Is Nothing Then
                    m_client = New CalculatorClient(currentBinding.Name)
                End If
                Return m_client
            End Get
        End Property

        Private Property Context() As IDictionary(Of String, String)
            Get
                Return m_context
            End Get
            Set(ByVal value As IDictionary(Of String, String))
                Status.Text = [String].Empty
                m_context = value
                If m_context IsNot Nothing Then
                    For Each item As KeyValuePair(Of String, String) In m_context
                        Status.Text += item.Key + ":" + item.Value
                    Next
                End If
            End Set
        End Property

        Private Property PowerOn() As Boolean
            Get
                Return m_powerOn
            End Get
            Set(ByVal value As Boolean)
                If m_powerOn <> value Then
                    m_powerOn = value
                    If m_powerOn Then
                        DoOperation(ButtonEqual, Nothing)
                        Display.Background = onBrush
                        ButtonC.Background = Brushes.DarkSeaGreen
                    Else
                        Display.Text = [String].Empty
                        Display.Background = offBrush
                        ButtonC.Background = Brushes.Tomato
                    End If
                End If
            End Set
        End Property

        Private Sub UpdateDisplay(ByVal sender As Object, ByVal e As RoutedEventArgs)
            Dim input As String = DirectCast(sender, Button).Content.ToString()
            If displayNew Then
                Display.Text = input
                Display.Background = offBrush
            Else
                Display.Text += input
            End If
            displayNew = False
        End Sub

        Private Sub DoOperation(ByVal sender As Object, ByVal e As RoutedEventArgs)
            Dim value As Integer = 0
            Dim operation As String = DirectCast(sender, Button).Content.ToString()
            If Not operation.StartsWith("=") AndAlso Not Integer.TryParse(Display.Text, value) Then
                MessageBox.Show("Invalid input! Try again.")
                Display.Text = [String].Empty
            End If

            Try
                If Not PowerOn Then
                    PowerSwitch(ButtonC, Nothing)
                End If

                Using New OperationContextScope(DirectCast(Client.InnerChannel, IContextChannel))
                    If Not contextApplied Then
                        ContextManager.ApplyContextToChannel(Context, Client.InnerChannel)
                        contextApplied = True
                    End If

                    Select Case operation
                        Case "="
                            value = Client.Add(0)
                            Exit Select
                        Case "+"
                            value = Client.Add(value)
                            Exit Select
                        Case "-"
                            value = Client.Subtract(value)
                            Exit Select
                        Case "x"
                            value = Client.Multiply(value)
                            Exit Select
                        Case "/"
                            value = Client.Divide(value)
                            Exit Select
                    End Select
                End Using
            Catch ce As CommunicationException
                MessageBox.Show(ce.Message)
                Client.Abort()
                m_client = Nothing
            End Try

            Display.Background = onBrush
            Display.Text = value.ToString()
            displayNew = True
        End Sub

        Private Sub PowerSwitch(ByVal sender As Object, ByVal e As RoutedEventArgs)
            Try
                Using New OperationContextScope(DirectCast(Client.InnerChannel, IContextChannel))
                    If PowerOn Then
                        PowerOn = False
                        If Not contextApplied Then
                            ContextManager.ApplyContextToChannel(Context, Client.InnerChannel)
                            contextApplied = True
                        End If
                        Context = Nothing
                        ContextManager.DeleteContext(contextFile)
                        Client.PowerOff()
                        Client.Close()
                        m_client = Nothing
                    Else
                        Client.PowerOn()
                        ' At this point the channel has the context from the operation called
                        contextApplied = True
                        ' extract and persist context
                        Context = ContextManager.ExtractContextFromChannel(m_client.InnerChannel)
                        ContextManager.PersistContext(Context, contextFile)
                        PowerOn = True
                    End If
                End Using
            Catch ce As CommunicationException
                MessageBox.Show(ce.Message)
                Client.Abort()
                m_client = Nothing
            End Try
        End Sub

        Private Sub ChangeBinding(ByVal sender As Object, ByVal e As RoutedEventArgs)
            Dim item As MenuItem = DirectCast(sender, MenuItem)
            item.IsChecked = True
            If Not ReferenceEquals(item, currentBinding) Then
                ' change Binding
                Client.Close()
                m_client = Nothing
                currentBinding.IsChecked = False
                currentBinding = item
            End If
        End Sub

        Private Sub OnExit(ByVal sender As Object, ByVal e As RoutedEventArgs)
            Me.Close()
            Application.Current.Shutdown()
        End Sub

#Region "Context Utilities"
        '
        '        Uri ccUri = new Uri("http://private/cookiecontainer");
        '        bool ApplyContext(IDictionary<XmlQualifiedName, string> context, IClientChannel channel)
        '        {   
        '            if (context != null)
        '            {
        '                IContextManager cm = channel.GetProperty<IContextManager>();
        '                if (cm != null && cm.GetContext() == null)
        '                {   // apply context to ContextChannel
        '                    cm.SetContext(context);
        '                    return true;
        '                }
        '                else if (OperationContext.Current != null)
        '                {   // apply context as HttpCookie
        '                    CookieContainer cookies = new CookieContainer();
        '                    foreach (KeyValuePair<XmlQualifiedName, string> item in context)
        '                    {
        '                        cookies.Add(ccUri,new Cookie(item.Key.ToString(),item.Value));
        '                    }
        '                    if (cookies.Count > 0)
        '                    {
        '                        HttpRequestMessageProperty httpRequest = new HttpRequestMessageProperty();
        '                        OperationContext.Current.OutgoingMessageProperties.Add(HttpRequestMessageProperty.Name, httpRequest);
        '                        httpRequest.Headers.Add(HttpRequestHeader.Cookie, cookies.GetCookieHeader(ccUri));
        '                        return true;
        '                    }
        '                }
        '            }
        '            return false;
        '        }
        '
        '        IDictionary<XmlQualifiedName, string> ExtractContext(IClientChannel channel)
        '        {   // extract context from channel
        '            IContextManager cm = channel.GetProperty<IContextManager>();
        '            if (cm != null)
        '            {   // attempt to extract context from channel
        '                return cm.GetContext();
        '            }
        '            else if (OperationContext.Current != null)
        '            {   // attempt to extract context from HttpCookie
        '                CookieContainer cookies = new CookieContainer();
        '                if (OperationContext.Current.IncomingMessageProperties.ContainsKey(HttpResponseMessageProperty.Name))
        '                {
        '                    HttpResponseMessageProperty httpResponse = (HttpResponseMessageProperty)OperationContext.Current.IncomingMessageProperties[HttpResponseMessageProperty.Name];
        '                    cookies.SetCookies(ccUri,httpResponse.Headers[HttpResponseHeader.SetCookie]);
        '                }
        '                if (cookies.Count > 0)
        '                {   // parse into dictionary
        '                    Dictionary<XmlQualifiedName, string> newContext = new Dictionary<XmlQualifiedName, string>();
        '                    foreach(Cookie cookie in cookies.GetCookies(ccUri))
        '                    {
        '                        string[] ns = cookie.Name.Split(':');
        '                        newContext.Add(new XmlQualifiedName(ns[2], ns[0]+":"+ns[1]), cookie.Value);
        '                    }
        '                    return newContext;
        '                }
        '            }
        '            return null;
        '        }
        '
        '        IDictionary<XmlQualifiedName, string> DepersistContext(string fileName)
        '        {   // retrieve context from file
        '            if (File.Exists(contextFile))
        '            {
        '                DataContractSerializer serializer = new DataContractSerializer(typeof(Dictionary<XmlQualifiedName, string>));
        '                FileStream stream = new FileStream(fileName, FileMode.Open);
        '                return (Dictionary<XmlQualifiedName, string>)serializer.ReadObject(stream);
        '            }
        '            return null;
        '        }
        '
        '        void PersistContext(IDictionary<XmlQualifiedName, string> context, string fileName)
        '        {   // persist context to file
        '            if (context != null)
        '            {   
        '                DataContractSerializer serializer = new DataContractSerializer(typeof(Dictionary<XmlQualifiedName, string>));
        '                FileStream stream = new FileStream(fileName, FileMode.Create);
        '                serializer.WriteObject(stream, context);
        '            }
        '        }
        '
        '        void DeleteContext(string fileName)
        '        {
        '            if (File.Exists(contextFile))
        '                File.Delete(contextFile);
        '        }
        '

#End Region

    End Class


End Namespace