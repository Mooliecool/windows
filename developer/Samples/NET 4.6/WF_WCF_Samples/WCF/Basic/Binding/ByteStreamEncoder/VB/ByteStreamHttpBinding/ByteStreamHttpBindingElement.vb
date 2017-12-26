'----------------------------------------------------------------
' Copyright (c) Microsoft Corporation.  All rights reserved.
'----------------------------------------------------------------


Imports Microsoft.VisualBasic
Imports System
Imports System.Configuration
Imports System.Globalization
Imports System.ServiceModel.Channels
Imports System.ServiceModel.Configuration
Imports System.Xml

Namespace Microsoft.Samples.ByteStreamEncoder

    Public Class ByteStreamHttpBindingElement
        Inherits StandardBindingElement

        Public Sub New(ByVal configurationName As String)
            MyBase.New(configurationName)
        End Sub

        Public Sub New()
            Me.New(Nothing)
        End Sub

        <ConfigurationProperty(ByteStreamHttpConfigurationStrings.BypassProxyOnLocal, DefaultValue:=ByteStreamHttpDefaults.DefaultBypassProxyOnLocal)> _
        Public Property BypassProxyOnLocal() As Boolean
            Get
                Return (CBool(MyBase.Item(ByteStreamHttpConfigurationStrings.BypassProxyOnLocal)))
            End Get
            Set(ByVal value As Boolean)
                MyBase.Item(ByteStreamHttpConfigurationStrings.BypassProxyOnLocal) = value
            End Set
        End Property

        <ConfigurationProperty(ByteStreamHttpConfigurationStrings.HostNameComparisonMode, DefaultValue:=ByteStreamHttpDefaults.DefaultHostNameComparisonMode), ServiceModelEnumValidator(GetType(HostNameComparisonModeHelper))> _
        Public Property HostNameComparisonMode() As System.ServiceModel.HostNameComparisonMode
            Get
                Return (CType(MyBase.Item(ByteStreamHttpConfigurationStrings.HostNameComparisonMode), System.ServiceModel.HostNameComparisonMode))
            End Get
            Set(ByVal value As System.ServiceModel.HostNameComparisonMode)
                MyBase.Item(ByteStreamHttpConfigurationStrings.HostNameComparisonMode) = value
            End Set
        End Property

        <ConfigurationProperty(ByteStreamHttpConfigurationStrings.MaxBufferPoolSize, DefaultValue:=ByteStreamHttpDefaults.DefaultMaxBufferPoolSize), LongValidator(MinValue:=0)> _
        Public Property MaxBufferPoolSize() As Long
            Get
                Return (CLng(Fix(MyBase.Item(ByteStreamHttpConfigurationStrings.MaxBufferPoolSize))))
            End Get
            Set(ByVal value As Long)
                MyBase.Item(ByteStreamHttpConfigurationStrings.MaxBufferPoolSize) = value
            End Set
        End Property

        <ConfigurationProperty(ByteStreamHttpConfigurationStrings.MaxBufferSize, DefaultValue:=ByteStreamHttpDefaults.DefaultMaxBufferSize), IntegerValidator(MinValue:=1)> _
        Public Property MaxBufferSize() As Integer
            Get
                Return (CInt(Fix(MyBase.Item(ByteStreamHttpConfigurationStrings.MaxBufferSize))))
            End Get
            Set(ByVal value As Integer)
                MyBase.Item(ByteStreamHttpConfigurationStrings.MaxBufferSize) = value
            End Set
        End Property

        <ConfigurationProperty(ByteStreamHttpConfigurationStrings.MaxReceivedMessageSize, DefaultValue:=ByteStreamHttpDefaults.DefaultMaxReceivedMessageSize), LongValidator(MinValue:=1)> _
        Public Property MaxReceivedMessageSize() As Long
            Get
                Return (CLng(Fix(MyBase.Item(ByteStreamHttpConfigurationStrings.MaxReceivedMessageSize))))
            End Get
            Set(ByVal value As Long)
                MyBase.Item(ByteStreamHttpConfigurationStrings.MaxReceivedMessageSize) = value
            End Set
        End Property

        <ConfigurationProperty(ByteStreamHttpConfigurationStrings.ProxyAddress, DefaultValue:=ByteStreamHttpDefaults.DefaultProxyAddress), AddressValidator()> _
        Public Property ProxyAddress() As System.Uri
            Get
                Return (CType(MyBase.Item(ByteStreamHttpConfigurationStrings.ProxyAddress), System.Uri))
            End Get
            Set(ByVal value As System.Uri)
                MyBase.Item(ByteStreamHttpConfigurationStrings.ProxyAddress) = value
            End Set
        End Property

        <ConfigurationProperty(ByteStreamHttpConfigurationStrings.ReaderQuotas, DefaultValue:=ByteStreamHttpDefaults.DefaultReaderQuotas)> _
        Public ReadOnly Property ReaderQuotas() As XmlDictionaryReaderQuotasElement
            Get
                Return CType(MyBase.Item(ByteStreamHttpConfigurationStrings.ReaderQuotas), XmlDictionaryReaderQuotasElement)
            End Get
        End Property

        <ConfigurationProperty(ByteStreamHttpConfigurationStrings.TransferMode, DefaultValue:=ByteStreamHttpDefaults.DefaultTransferMode), ServiceModelEnumValidator(GetType(TransferModeHelper))> _
        Public Property TransferMode() As System.ServiceModel.TransferMode
            Get
                Return (CType(MyBase.Item(ByteStreamHttpConfigurationStrings.TransferMode), System.ServiceModel.TransferMode))
            End Get
            Set(ByVal value As System.ServiceModel.TransferMode)
                MyBase.Item(ByteStreamHttpConfigurationStrings.TransferMode) = value
            End Set
        End Property

        <ConfigurationProperty(ByteStreamHttpConfigurationStrings.UseDefaultWebProxy, DefaultValue:=ByteStreamHttpDefaults.DefaultUseDefaultWebProxy)> _
        Public Property UseDefaultWebProxy() As Boolean
            Get
                Return (CBool(MyBase.Item(ByteStreamHttpConfigurationStrings.UseDefaultWebProxy)))
            End Get
            Set(ByVal value As Boolean)
                MyBase.Item(ByteStreamHttpConfigurationStrings.UseDefaultWebProxy) = value
            End Set
        End Property

        Protected Overrides ReadOnly Property BindingElementType() As Type
            Get
                Return GetType(ByteStreamHttpBinding)
            End Get
        End Property

        Protected Overrides ReadOnly Property Properties() As ConfigurationPropertyCollection
            Get
                Dim PropertyColl As ConfigurationPropertyCollection = MyBase.Properties

                PropertyColl.Add(New ConfigurationProperty(ByteStreamHttpConfigurationStrings.HostNameComparisonMode, GetType(System.ServiceModel.HostNameComparisonMode), ByteStreamHttpDefaults.DefaultHostNameComparisonMode))
                PropertyColl.Add(New ConfigurationProperty(ByteStreamHttpConfigurationStrings.MaxBufferSize, GetType(Integer), ByteStreamHttpDefaults.DefaultMaxBufferSize))
                PropertyColl.Add(New ConfigurationProperty(ByteStreamHttpConfigurationStrings.MaxBufferPoolSize, GetType(Long), ByteStreamHttpDefaults.DefaultMaxBufferPoolSize))
                PropertyColl.Add(New ConfigurationProperty(ByteStreamHttpConfigurationStrings.MaxReceivedMessageSize, GetType(Long), ByteStreamHttpDefaults.DefaultMaxReceivedMessageSize))
                PropertyColl.Add(New ConfigurationProperty(ByteStreamHttpConfigurationStrings.TransferMode, GetType(System.ServiceModel.TransferMode), ByteStreamHttpDefaults.DefaultTransferMode))
                PropertyColl.Add(New ConfigurationProperty(ByteStreamHttpConfigurationStrings.ReaderQuotas, GetType(XmlDictionaryReaderQuotasElement), ByteStreamHttpDefaults.DefaultReaderQuotas))

                Return PropertyColl
            End Get
        End Property

        Protected Overrides Sub InitializeFrom(ByVal binding As Binding)
            MyBase.InitializeFrom(binding)
            Dim ByteStreamHttpBinding As ByteStreamHttpBinding = (CType(binding, ByteStreamHttpBinding))
            Me.HostNameComparisonMode = ByteStreamHttpBinding.HostNameComparisonMode
            Me.MaxBufferSize = ByteStreamHttpBinding.MaxBufferSize
            Me.MaxBufferPoolSize = ByteStreamHttpBinding.MaxBufferPoolSize
            Me.MaxReceivedMessageSize = ByteStreamHttpBinding.MaxReceivedMessageSize
            Me.TransferMode = ByteStreamHttpBinding.TransferMode

            ' Copy reader quotas over.
            Me.ReaderQuotas.MaxDepth = ByteStreamHttpBinding.ReaderQuotas.MaxDepth
            Me.ReaderQuotas.MaxStringContentLength = ByteStreamHttpBinding.ReaderQuotas.MaxStringContentLength
            Me.ReaderQuotas.MaxArrayLength = ByteStreamHttpBinding.ReaderQuotas.MaxArrayLength
            Me.ReaderQuotas.MaxBytesPerRead = ByteStreamHttpBinding.ReaderQuotas.MaxBytesPerRead
            Me.ReaderQuotas.MaxNameTableCharCount = ByteStreamHttpBinding.ReaderQuotas.MaxNameTableCharCount
        End Sub

        Protected Overrides Sub OnApplyConfiguration(ByVal binding As Binding)
            If (binding Is Nothing) Then
                Throw New System.ArgumentNullException("binding")
            End If
            If (binding.GetType() IsNot GetType(ByteStreamHttpBinding)) Then
                Throw New System.ArgumentException(String.Format(CultureInfo.CurrentCulture, "Invalid type for binding. Expected type: {0}. Type passed in: {1}.", GetType(ByteStreamHttpBinding).AssemblyQualifiedName, binding.GetType().AssemblyQualifiedName))
            End If
            Dim ByteStreamHttpBinding As ByteStreamHttpBinding = (CType(binding, ByteStreamHttpBinding))
            ByteStreamHttpBinding.HostNameComparisonMode = Me.HostNameComparisonMode
            ByteStreamHttpBinding.MaxBufferSize = Me.MaxBufferSize
            ByteStreamHttpBinding.MaxBufferPoolSize = Me.MaxBufferPoolSize
            ByteStreamHttpBinding.MaxReceivedMessageSize = Me.MaxReceivedMessageSize
            ByteStreamHttpBinding.TransferMode = Me.TransferMode

            ' Copy reader quotas over if set from config.
            If Me.ReaderQuotas.MaxDepth <> 0 Then
                ByteStreamHttpBinding.ReaderQuotas.MaxDepth = Me.ReaderQuotas.MaxDepth
            End If
            If Me.ReaderQuotas.MaxStringContentLength <> 0 Then
                ByteStreamHttpBinding.ReaderQuotas.MaxStringContentLength = Me.ReaderQuotas.MaxStringContentLength
            End If
            If Me.ReaderQuotas.MaxArrayLength <> 0 Then
                ByteStreamHttpBinding.ReaderQuotas.MaxArrayLength = Me.ReaderQuotas.MaxArrayLength
            End If
            If Me.ReaderQuotas.MaxBytesPerRead <> 0 Then
                ByteStreamHttpBinding.ReaderQuotas.MaxBytesPerRead = Me.ReaderQuotas.MaxBytesPerRead
            End If
            If Me.ReaderQuotas.MaxNameTableCharCount <> 0 Then
                ByteStreamHttpBinding.ReaderQuotas.MaxNameTableCharCount = Me.ReaderQuotas.MaxNameTableCharCount
            End If
        End Sub
    End Class


End Namespace
