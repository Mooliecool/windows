'----------------------------------------------------------------
' Copyright (c) Microsoft Corporation.  All rights reserved.
'----------------------------------------------------------------

Imports Microsoft.VisualBasic
Imports System.Configuration
Imports System.Globalization
Imports System.ServiceModel
Imports System.ServiceModel.Channels
Imports System.ServiceModel.Configuration
Imports System.Xml

Namespace Microsoft.Samples.ByteStreamEncoder

    Public Class ByteStreamHttpBinding
        Inherits Binding

        Private httpTransport As HttpTransportBindingElement
        Private byteStreamEncoding As ByteStreamMessageEncodingBindingElement

        Public Sub New()
            Me.httpTransport = New HttpTransportBindingElement()
            Me.byteStreamEncoding = New ByteStreamMessageEncodingBindingElement()
        End Sub

        Public Sub New(ByVal configurationName As String)
            If configurationName IsNot Nothing Then
                ApplyConfiguration(configurationName)
            Else
                Throw New System.Configuration.ConfigurationErrorsException("The configuration name provided is null ")
            End If
        End Sub

        Public Property HostNameComparisonMode() As HostNameComparisonMode
            Get
                Return httpTransport.HostNameComparisonMode
            End Get
            Set(ByVal value As HostNameComparisonMode)
                httpTransport.HostNameComparisonMode = value
            End Set
        End Property

        Public Property MaxBufferSize() As Integer
            Get
                Return httpTransport.MaxBufferSize
            End Get
            Set(ByVal value As Integer)
                httpTransport.MaxBufferSize = value
            End Set
        End Property

        Public Property MaxBufferPoolSize() As Long
            Get
                Return httpTransport.MaxBufferPoolSize
            End Get
            Set(ByVal value As Long)
                httpTransport.MaxBufferPoolSize = value
            End Set
        End Property

        Public Property MaxReceivedMessageSize() As Long
            Get
                Return httpTransport.MaxReceivedMessageSize
            End Get
            Set(ByVal value As Long)
                httpTransport.MaxReceivedMessageSize = value
            End Set
        End Property

        Public Property TransferMode() As TransferMode
            Get
                Return httpTransport.TransferMode
            End Get
            Set(ByVal value As TransferMode)
                httpTransport.TransferMode = value
            End Set
        End Property

        Public Property ReaderQuotas() As XmlDictionaryReaderQuotas
            Get
                Return byteStreamEncoding.ReaderQuotas
            End Get
            Set(ByVal value As XmlDictionaryReaderQuotas)
                If value IsNot Nothing Then
                    value.CopyTo(byteStreamEncoding.ReaderQuotas)
                End If
            End Set
        End Property

        Public ReadOnly Property EnvelopeVersion() As EnvelopeVersion
            Get
                Return EnvelopeVersion.None
            End Get
        End Property

        Public Overrides ReadOnly Property Scheme() As String
            Get
                Return httpTransport.Scheme
            End Get
        End Property

        Private Sub ApplyConfiguration(ByVal configurationName As String)
            Dim bindings As BindingsSection = (CType(ConfigurationManager.GetSection("system.serviceModel/bindings"), BindingsSection))
            Dim section As ByteStreamHttpBindingCollectionElement = CType(bindings("byteStreamHttpBinding"), ByteStreamHttpBindingCollectionElement)
            Dim element As ByteStreamHttpBindingElement = section.Bindings(configurationName)

            If element Is Nothing Then
                Throw New System.Configuration.ConfigurationErrorsException(String.Format(CultureInfo.CurrentCulture, "There is no binding named {0} at {1}.", configurationName, section.BindingName))
            Else
                element.ApplyConfiguration(Me)
            End If
        End Sub

        Public Overrides Function CreateBindingElements() As BindingElementCollection
            Dim bindingElements As New BindingElementCollection()

            bindingElements.Add(Me.byteStreamEncoding)
            bindingElements.Add(Me.httpTransport)

            Return bindingElements.Clone()
        End Function

    End Class
End Namespace
