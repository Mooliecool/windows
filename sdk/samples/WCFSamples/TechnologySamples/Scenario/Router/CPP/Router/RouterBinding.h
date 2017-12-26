// ----------------------------------------------------------------------------
// Copyright (C) 2003-2005 Microsoft Corporation, All rights reserved.
// ----------------------------------------------------------------------------

using namespace System;
using namespace System::ServiceModel;
using namespace System::ServiceModel::Channels;
using namespace System::ServiceModel::Configuration;
using namespace System::Diagnostics;
using namespace System::Configuration;

namespace Microsoft{
namespace ServiceModel{
namespace Samples
{
public enum class RouterTransport
{
	Http = 0,
	NamedPipe = 2,
	Tcp = 1
};

public enum class MessageEncoding
{
	Text = 0,
	Binary = 1,
	Mtom = 2

};

public ref class RouterBinding : public System::ServiceModel::Channels::Binding, public System::ServiceModel::Channels::IBindingRuntimePreferences
{
public: 
	RouterBinding();

	RouterBinding(Microsoft::ServiceModel::Samples::RouterTransport transport);

	RouterBinding(System::String ^ configurationName);

	virtual System::ServiceModel::Channels::BindingElementCollection ^ CreateBindingElements() override;

	property System::ServiceModel::TransferMode TransferMode
	{
		System::ServiceModel::TransferMode get();
		void set( System::ServiceModel::TransferMode value );
	}

	property Microsoft::ServiceModel::Samples::RouterTransport Transport
	{
		Microsoft::ServiceModel::Samples::RouterTransport get();
		void set( Microsoft::ServiceModel::Samples::RouterTransport value );
	}

	property System::ServiceModel::HostNameComparisonMode HostNameComparisonMode
	{
		System::ServiceModel::HostNameComparisonMode get();
		void set( System::ServiceModel::HostNameComparisonMode value );
	}

	property int ListenBacklog
	{
		int get();
		void set( int value );
	}

	property int MaxBufferSize
	{
        int get();
        void set( int value );
	}

	property long MaxBufferPoolSize
	{
		long get();
		void set( long value );
	}

	property int MaxConnections
	{
		int get();
		void set( int value );
	}

	property long MaxReceivedMessageSize
	{
		long get();
		void set( long value );
	}

	property Microsoft::ServiceModel::Samples::MessageEncoding MessageEncoding
	{
		Microsoft::ServiceModel::Samples::MessageEncoding get();
		void set( Microsoft::ServiceModel::Samples::MessageEncoding value );
	}

	property bool PortSharingEnabled
	{
		bool get();
		void set( bool value );
	}

	virtual property System::String ^ Scheme
	{
        System::String ^ get() override;
	}


	
	virtual property bool ReceiveSynchronously
	{
		bool get();
	}
private: 


	void ApplyConfiguration(System::String ^ configurationName);

	void Initialize();

	property System::ServiceModel::Channels::MessageEncodingBindingElement ^ EncodingElement
	{
		System::ServiceModel::Channels::MessageEncodingBindingElement ^ get();
	}


	property System::ServiceModel::Channels::TransportBindingElement ^ TransportElement
	{
		System::ServiceModel::Channels::TransportBindingElement ^ get();
	}

	System::ServiceModel::Channels::BinaryMessageEncodingBindingElement ^	binaryEncoding;
	System::ServiceModel::Channels::HttpTransportBindingElement ^			httpTransport;
	Microsoft::ServiceModel::Samples::MessageEncoding 						messageEncoding;
	System::ServiceModel::Channels::MtomMessageEncodingBindingElement ^		mtomEncoding;
	System::ServiceModel::Channels::NamedPipeTransportBindingElement ^		namedPipeTransport;
	System::ServiceModel::Channels::TcpTransportBindingElement ^			tcpTransport;
	System::ServiceModel::Channels::TextMessageEncodingBindingElement ^		textEncoding;
	Microsoft::ServiceModel::Samples::RouterTransport						transport;
};

public ref class RouterBindingElement : public System::ServiceModel::Configuration::StandardBindingElement
{
private: 
	
	System::Configuration::ConfigurationPropertyCollection ^ properties;

public: 
	RouterBindingElement();

	RouterBindingElement(System::String ^ name);

protected: 

	virtual void OnApplyConfiguration(System::ServiceModel::Channels::Binding ^ binding) override;

	virtual void InitializeFrom( Binding ^ binding ) override;

	virtual property System::Type ^ BindingElementType
	{
		System::Type ^ get() override;
	}


public: 
	[System::Configuration::ConfigurationProperty(L"hostNameComparisonMode", DefaultValue=System::ServiceModel::HostNameComparisonMode::StrongWildcard)]
	property System::ServiceModel::HostNameComparisonMode HostNameComparisonMode
	{
		System::ServiceModel::HostNameComparisonMode get();
		void set( System::ServiceModel::HostNameComparisonMode value );
	}

	[System::Configuration::ConfigurationProperty(L"listenBacklog", DefaultValue=10)]
	property int ListenBacklog
	{
		int get();
		void set( int value );
	}


	[System::Configuration::ConfigurationProperty(L"maxBufferPoolSize", DefaultValue=524288)]
	property long MaxBufferPoolSize
	{
		long get();
		void set( long value );
	}

	[System::Configuration::ConfigurationProperty(L"maxBufferSize", DefaultValue=65536)]
	property int MaxBufferSize
	{
		int get();
		void set( int value );
	}

	[System::Configuration::ConfigurationProperty(L"maxConnections", DefaultValue=10)]
	property int MaxConnections
	{
		int get();
		void set( int value );
	}

	[System::Configuration::ConfigurationProperty(L"maxReceivedMessageSize", DefaultValue=65536)]
	property long MaxReceivedMessageSize
	{
		long get();
		void set( long value );
	}

	[System::Configuration::ConfigurationProperty(L"messageEncoding", DefaultValue=Microsoft::ServiceModel::Samples::MessageEncoding::Text)]
	property Microsoft::ServiceModel::Samples::MessageEncoding MessageEncoding
	{
		Microsoft::ServiceModel::Samples::MessageEncoding get();
		void set( Microsoft::ServiceModel::Samples::MessageEncoding value );
	}


	[System::Configuration::ConfigurationProperty(L"messageVersion", DefaultValue=L"Soap12WSAddressing10")]
	property System::ServiceModel::Channels::MessageVersion ^ MessageVersion
	{
		System::ServiceModel::Channels::MessageVersion ^ get();
		void set( System::ServiceModel::Channels::MessageVersion ^ value );
	}

	[System::Configuration::ConfigurationProperty(L"portSharingEnabled", DefaultValue=(Object ^)false)]
	property bool PortSharingEnabled
	{
		bool get();
		void set( bool value );
	}

	[System::Configuration::ConfigurationProperty(L"transport", DefaultValue=Microsoft::ServiceModel::Samples::RouterTransport::Http)]
	property Microsoft::ServiceModel::Samples::RouterTransport Transport
	{
		Microsoft::ServiceModel::Samples::RouterTransport get();
		void set( Microsoft::ServiceModel::Samples::RouterTransport value );
	}

	[System::Configuration::ConfigurationProperty(L"transferMode", DefaultValue=System::ServiceModel::TransferMode::Buffered)]
	property System::ServiceModel::TransferMode TransferMode
	{
		System::ServiceModel::TransferMode get();
		void set( System::ServiceModel::TransferMode value );
	}

protected:

	virtual property System::Configuration::ConfigurationPropertyCollection ^ Properties
	{
        System::Configuration::ConfigurationPropertyCollection ^ get() override;
	}
};

public ref class RouterBindingCollectionElement : public System::ServiceModel::Configuration::StandardBindingCollectionElement<Microsoft::ServiceModel::Samples::RouterBinding ^, Microsoft::ServiceModel::Samples::RouterBindingElement ^>
{
public: 
    static Microsoft::ServiceModel::Samples::RouterBindingCollectionElement ^ GetBindingCollectionElement();
};

 



}
}
}