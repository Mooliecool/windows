// ----------------------------------------------------------------------------
// Copyright (C) 2003-2005 Microsoft Corporation, All rights reserved.
// ----------------------------------------------------------------------------

#include "RouterBinding.h"

using namespace System;
using namespace System::ServiceModel;
using namespace System::ServiceModel::Channels;
using namespace System::Diagnostics;
using namespace System::Configuration;

using namespace Microsoft::ServiceModel::Samples;

RouterBinding::RouterBinding() : System::ServiceModel::Channels::Binding()
{
	this->Initialize();	
}

RouterBinding::RouterBinding(Microsoft::ServiceModel::Samples::RouterTransport transport) : 
	System::ServiceModel::Channels::Binding()
{
	this->Initialize();	
	this->transport = transport;

	if ( transport == RouterTransport::NamedPipe || transport == RouterTransport::Tcp)
	{
		this->MessageEncoding = Microsoft::ServiceModel::Samples::MessageEncoding::Binary;
	}
}

RouterBinding::RouterBinding(System::String ^ configurationName) : System::ServiceModel::Channels::Binding()
{
	this->Initialize();	
	this->ApplyConfiguration( configurationName );
}

System::ServiceModel::Channels::BindingElementCollection ^ RouterBinding::CreateBindingElements()
{
	BindingElementCollection ^ elements = gcnew BindingElementCollection();
	elements->Add(this->EncodingElement);
	elements->Add(this->TransportElement);

	return elements;
}

void RouterBinding::ApplyConfiguration(System::String ^ configurationName)
{
	RouterBindingCollectionElement ^ bindingCollectionElement = RouterBindingCollectionElement::GetBindingCollectionElement();
    RouterBindingElement ^ element = bindingCollectionElement->Bindings[configurationName];

    if (element == nullptr)
    {
		throw gcnew ConfigurationErrorsException(String::Format("ConfigInvalidBindingConfigurationName", configurationName, bindingCollectionElement->BindingName));
    }
    else
    {
        element->ApplyConfiguration(this);
    }
}

void RouterBinding::Initialize()
{
	this->httpTransport = gcnew HttpTransportBindingElement();
	this->tcpTransport = gcnew TcpTransportBindingElement();
	this->namedPipeTransport = gcnew NamedPipeTransportBindingElement();

	this->textEncoding = gcnew TextMessageEncodingBindingElement();
	this->mtomEncoding = gcnew MtomMessageEncodingBindingElement();
	this->binaryEncoding = gcnew BinaryMessageEncodingBindingElement();

	this->httpTransport->ManualAddressing = true;
	this->tcpTransport->ManualAddressing = true;
	this->namedPipeTransport->ManualAddressing = true;

	this->transport = Microsoft::ServiceModel::Samples::RouterTransport::Http;
	this->messageEncoding = Microsoft::ServiceModel::Samples::MessageEncoding::Text;
}

System::ServiceModel::TransferMode RouterBinding::TransferMode::get()
{
	return this->tcpTransport->TransferMode;
}

void RouterBinding::TransferMode::set( System::ServiceModel::TransferMode value )
{
	this->tcpTransport->TransferMode = value;
	this->namedPipeTransport->TransferMode = value;
	this->httpTransport->TransferMode = value;
}

Microsoft::ServiceModel::Samples::RouterTransport RouterBinding::Transport::get()
{
	return this->transport;
}

void RouterBinding::Transport::set( Microsoft::ServiceModel::Samples::RouterTransport value )
{
	this->transport = value;
}

System::ServiceModel::HostNameComparisonMode RouterBinding::HostNameComparisonMode::get()
{
	return this->tcpTransport->HostNameComparisonMode;
}

void RouterBinding::HostNameComparisonMode::set( System::ServiceModel::HostNameComparisonMode value )
{
	this->tcpTransport->HostNameComparisonMode = value;
	this->namedPipeTransport->HostNameComparisonMode = value;
	this->httpTransport->HostNameComparisonMode = value;
}

int RouterBinding::ListenBacklog::get()
{
	return this->tcpTransport->ListenBacklog;
}

void RouterBinding::ListenBacklog::set( int value )
{
	this->tcpTransport->ListenBacklog = value;
}

long RouterBinding::MaxBufferPoolSize::get()
{
	return (long)this->tcpTransport->MaxBufferPoolSize;
}

void RouterBinding::MaxBufferPoolSize::set( long value )
{
	this->tcpTransport->MaxBufferPoolSize = value;
	this->namedPipeTransport->MaxBufferPoolSize = value;
}

int RouterBinding::MaxBufferSize::get()
{
	return this->tcpTransport->MaxBufferSize;
}

void RouterBinding::MaxBufferSize::set( int value )
{
	this->tcpTransport->MaxBufferSize = value;
	this->namedPipeTransport->MaxBufferSize = value;
}

int RouterBinding::MaxConnections::get()
{
	return this->tcpTransport->ConnectionPoolSettings->MaxOutboundConnectionsPerEndpoint;
}

void RouterBinding::MaxConnections::set( int value )
{
	this->tcpTransport->MaxPendingConnections = value;
	this->namedPipeTransport->MaxPendingConnections = value;
	this->tcpTransport->ConnectionPoolSettings->MaxOutboundConnectionsPerEndpoint = value;
	this->namedPipeTransport->ConnectionPoolSettings->MaxOutboundConnectionsPerEndpoint = value;
}

long RouterBinding::MaxReceivedMessageSize::get()
{
	return (long)this->tcpTransport->MaxReceivedMessageSize;
}

void RouterBinding::MaxReceivedMessageSize::set( long value )
{
	this->tcpTransport->MaxReceivedMessageSize = value;
	this->namedPipeTransport->MaxReceivedMessageSize = value;
	this->httpTransport->MaxReceivedMessageSize = value;
}

Microsoft::ServiceModel::Samples::MessageEncoding RouterBinding::MessageEncoding::get()
{
	return this->messageEncoding;
}

void RouterBinding::MessageEncoding::set( Microsoft::ServiceModel::Samples::MessageEncoding value )
{
	this->messageEncoding = value;
}

bool RouterBinding::PortSharingEnabled::get()
{
	return this->tcpTransport->PortSharingEnabled;
}

void RouterBinding::PortSharingEnabled::set( bool value )
{
	this->tcpTransport->PortSharingEnabled = value;
}

TransportBindingElement ^ RouterBinding::TransportElement::get()
{
	switch ( (this->transport) )
	{
	case RouterTransport::Http:
		return this->httpTransport;
	case RouterTransport::Tcp:
		return this->tcpTransport;
	case RouterTransport::NamedPipe:
		return this->namedPipeTransport;
	}

	return nullptr;
}

System::String ^ RouterBinding::Scheme::get()
{
	return this->TransportElement->Scheme;
}

System::ServiceModel::Channels::MessageEncodingBindingElement ^ RouterBinding::EncodingElement::get()
{
	switch ( (this->messageEncoding) )
	{
	case Microsoft::ServiceModel::Samples::MessageEncoding::Binary:
		return this->binaryEncoding;
	case Microsoft::ServiceModel::Samples::MessageEncoding::Mtom:
		return this->mtomEncoding;
	case Microsoft::ServiceModel::Samples::MessageEncoding::Text:
		return this->textEncoding;
	}

	return nullptr;
}

bool RouterBinding::ReceiveSynchronously::get()
{
	return false;
}

//=====================================



RouterBindingElement::RouterBindingElement() : StandardBindingElement()
{
}


RouterBindingElement::RouterBindingElement( System::String ^ name ) : StandardBindingElement( name )
{
}

void RouterBindingElement::OnApplyConfiguration(System::ServiceModel::Channels::Binding ^ binding)
{
    RouterBinding ^ routerBinding = safe_cast<RouterBinding^>(binding);
	
    routerBinding->Transport = this->Transport;
    routerBinding->MessageEncoding = this->MessageEncoding;
    routerBinding->HostNameComparisonMode = this->HostNameComparisonMode;
    routerBinding->ListenBacklog = this->ListenBacklog;
    routerBinding->MaxBufferPoolSize = this->MaxBufferPoolSize;
    routerBinding->MaxBufferSize = this->MaxBufferSize;
    routerBinding->MaxConnections = this->MaxConnections;
    routerBinding->MaxReceivedMessageSize = this->MaxReceivedMessageSize;
    routerBinding->PortSharingEnabled = this->PortSharingEnabled;
    routerBinding->TransferMode = this->TransferMode;
}

void RouterBindingElement::InitializeFrom( Binding ^ binding )
{
	StandardBindingElement::InitializeFrom( binding );
    RouterBinding ^ routerBinding = safe_cast<RouterBinding^>(binding);

    this->Transport = routerBinding->Transport;
    this->MessageEncoding = routerBinding->MessageEncoding;
    this->HostNameComparisonMode = routerBinding->HostNameComparisonMode;
    this->ListenBacklog = routerBinding->ListenBacklog;
    this->MaxBufferPoolSize = routerBinding->MaxBufferPoolSize;
    this->MaxBufferSize = routerBinding->MaxBufferSize;
    this->MaxConnections = routerBinding->MaxConnections;
    this->MaxReceivedMessageSize = routerBinding->MaxReceivedMessageSize;
    this->PortSharingEnabled = routerBinding->PortSharingEnabled;
    this->TransferMode = routerBinding->TransferMode;
}


System::Type ^ RouterBindingElement::BindingElementType::get()
{
	return RouterBinding::typeid;
}

System::ServiceModel::HostNameComparisonMode RouterBindingElement::HostNameComparisonMode::get()
{
	
	return safe_cast<System::ServiceModel::HostNameComparisonMode>( this[L"hostNameComparisonMode"] );
}

void RouterBindingElement::HostNameComparisonMode::set( System::ServiceModel::HostNameComparisonMode value )
{
	this[L"hostNameComparisonMode"] = value;
}


int RouterBindingElement::ListenBacklog::get()
{
    return safe_cast<int>( this[L"listenBacklog"] );
}

void RouterBindingElement::ListenBacklog::set( int value )
{
	this[L"listenBacklog"] = value;
}

long RouterBindingElement::MaxBufferPoolSize::get()
{
    return safe_cast<long>( this[L"maxBufferPoolSize"] );
}

void RouterBindingElement::MaxBufferPoolSize::set( long value )
{
    this[L"maxBufferPoolSize"] = value;
}

int RouterBindingElement::MaxBufferSize::get()
{
    return safe_cast<int>( this[L"maxBufferSize"] );
}

void RouterBindingElement::MaxBufferSize::set( int value )
{
    this[L"maxBufferSize"] = value;
}

int RouterBindingElement::MaxConnections::get()
{
    return safe_cast<int>( this[L"maxConnections"] );
}

void RouterBindingElement::MaxConnections::set( int value )
{
    this[L"maxConnections"] = value;
}

long RouterBindingElement::MaxReceivedMessageSize::get()
{
    return safe_cast<long>( this[L"maxReceivedMessageSize"] );
}

void RouterBindingElement::MaxReceivedMessageSize::set( long value )
{
    this[L"maxReceivedMessageSize"] = value;
}

Microsoft::ServiceModel::Samples::MessageEncoding RouterBindingElement::MessageEncoding::get()
{
    return safe_cast<Microsoft::ServiceModel::Samples::MessageEncoding>( this[L"messageEncoding"] );
}

void RouterBindingElement::MessageEncoding::set( Microsoft::ServiceModel::Samples::MessageEncoding value )
{
    this[L"messageEncoding"] = value;
}

System::ServiceModel::Channels::MessageVersion ^ RouterBindingElement::MessageVersion::get()
{
	System::String ^ content = safe_cast<System::String^> (this[ L"messageVersion" ] );
	System::Reflection::PropertyInfo ^ propertyInfo = (System::ServiceModel::Channels::MessageVersion::typeid)->GetProperty(content);
    return safe_cast<System::ServiceModel::Channels::MessageVersion^>( propertyInfo->GetValue( nullptr,nullptr ) );
}

void RouterBindingElement::MessageVersion::set( System::ServiceModel::Channels::MessageVersion ^ value )
{
	this[L"messageVersion"] = value->ToString();
}

bool RouterBindingElement::PortSharingEnabled::get()
{
    return safe_cast<bool>( this[L"portSharingEnabled"] );
}

void RouterBindingElement::PortSharingEnabled::set( bool value )
{
    this[L"portSharingEnabled"] = value;
}

System::ServiceModel::TransferMode RouterBindingElement::TransferMode::get()
{
    return safe_cast<System::ServiceModel::TransferMode>( this[L"transferMode"] );
}

void RouterBindingElement::TransferMode::set( System::ServiceModel::TransferMode value )
{
    this[L"transferMode"] = value;
}

Microsoft::ServiceModel::Samples::RouterTransport RouterBindingElement::Transport::get()
{
    return safe_cast<Microsoft::ServiceModel::Samples::RouterTransport>( this[L"transport"] );
}

void RouterBindingElement::Transport::set( Microsoft::ServiceModel::Samples::RouterTransport value )
{
    this[L"transport"] = value;
}

System::Configuration::ConfigurationPropertyCollection ^ RouterBindingElement::Properties::get()
{

	if (this->properties == nullptr)
	{
		ConfigurationPropertyCollection ^ properties = System::ServiceModel::Configuration::StandardBindingElement::Properties;
		
		properties->Add(gcnew ConfigurationProperty(L"transport", Microsoft::ServiceModel::Samples::RouterTransport::typeid, Microsoft::ServiceModel::Samples::RouterTransport::Http, nullptr, nullptr, System::Configuration::ConfigurationPropertyOptions::None));			
		properties->Add(gcnew ConfigurationProperty(L"messageVersion", System::String::typeid, L"Soap12WSAddressing10", nullptr, nullptr, System::Configuration::ConfigurationPropertyOptions::None));		
		properties->Add(gcnew ConfigurationProperty(L"messageEncoding", Microsoft::ServiceModel::Samples::MessageEncoding::typeid, Microsoft::ServiceModel::Samples::MessageEncoding::Text, nullptr, nullptr, System::Configuration::ConfigurationPropertyOptions::None));
		properties->Add(gcnew ConfigurationProperty(L"hostNameComparisonMode", System::ServiceModel::HostNameComparisonMode::typeid, System::ServiceModel::HostNameComparisonMode::StrongWildcard, nullptr, nullptr, System::Configuration::ConfigurationPropertyOptions::None));
		properties->Add(gcnew ConfigurationProperty(L"listenBacklog", System::Int32::typeid, 10, nullptr, nullptr, System::Configuration::ConfigurationPropertyOptions::None));
		properties->Add(gcnew ConfigurationProperty(L"maxBufferPoolSize", long::typeid, (long) 524288, nullptr, nullptr, System::Configuration::ConfigurationPropertyOptions::None));
		properties->Add(gcnew ConfigurationProperty(L"maxBufferSize", System::Int32::typeid, 65536, nullptr, nullptr, System::Configuration::ConfigurationPropertyOptions::None));
		properties->Add(gcnew ConfigurationProperty(L"maxConnections", System::Int32::typeid, 10, nullptr, nullptr, System::Configuration::ConfigurationPropertyOptions::None));
		properties->Add(gcnew ConfigurationProperty(L"maxReceivedMessageSize", long::typeid, (long) 65536, nullptr, nullptr, System::Configuration::ConfigurationPropertyOptions::None));
		properties->Add(gcnew ConfigurationProperty(L"portSharingEnabled", System::Boolean::typeid, (Object ^) false, nullptr, nullptr, System::Configuration::ConfigurationPropertyOptions::None));
		properties->Add(gcnew ConfigurationProperty(L"transferMode", System::ServiceModel::TransferMode::typeid, System::ServiceModel::TransferMode::Buffered, nullptr, nullptr, System::Configuration::ConfigurationPropertyOptions::None));

		this->properties = properties;
	}

	return this->properties;
}

//=================================

Microsoft::ServiceModel::Samples::RouterBindingCollectionElement ^ RouterBindingCollectionElement::GetBindingCollectionElement()
{
	RouterBindingCollectionElement ^ retval = nullptr;

	BindingsSection ^ bindingsSection = safe_cast<BindingsSection^> (ConfigurationManager::GetSection(L"system.serviceModel/bindings") );
	
	if (nullptr != bindingsSection)
	{
		retval = safe_cast<RouterBindingCollectionElement^>( bindingsSection[L"routerBinding"] );
	}

	return retval;

}


