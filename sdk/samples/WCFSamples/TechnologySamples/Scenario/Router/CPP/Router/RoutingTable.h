// ----------------------------------------------------------------------------
// Copyright (C) 2003-2005 Microsoft Corporation, All rights reserved.
// ----------------------------------------------------------------------------

using namespace System;
using namespace System::ServiceModel::Dispatcher;
using namespace System::ServiceModel::Channels;
using namespace System::Configuration;
using namespace System::Xml;
using namespace System::Xml::Serialization;
using namespace System::Runtime::Serialization;
using namespace System::Collections::Generic;


namespace Microsoft{
namespace ServiceModel{
namespace Samples
{


[System::Xml::Serialization::XmlType(Namespace=L"http://Microsoft.ServiceModel.Samples/Router")]
public ref class Route
{
public: 
	Route();

	Route(System::String ^ xPath, System::String ^ uri);

	[System::Xml::Serialization::XmlElement(L"uri")]
	property System::String ^ Uri
	{
		System::String ^ get();
		void set( System::String ^ value );
	}

	
	[System::Xml::Serialization::XmlElement(L"xPath")]
	property System::String ^ XPath
	{
		System::String ^ get();
		void set( System::String ^ value );
	}


private: 
	System::String ^ uri;
	System::String ^ xPath;
};





[System::Xml::Serialization::XmlType(Namespace=L"http://Microsoft.ServiceModel.Samples/Router")]
public ref class RouterNamespace
{
public: 
	RouterNamespace();

	RouterNamespace(System::String ^ prefix, System::String ^ ns);


	[System::Xml::Serialization::XmlElement(L"namespace")]
	property System::String ^ NamespaceUri
	{
		System::String ^ get();
        void set( System::String ^ value );
	}

	[System::Xml::Serialization::XmlElement(L"prefix")]
	property System::String ^ Prefix
	{
        System::String ^ get();
        void set( System::String ^ value );
	}

private: 
	System::String ^ ns;
	System::String ^ prefix;
};

[System::Xml::Serialization::XmlType(Namespace=L"http://Microsoft.ServiceModel.Samples/Router")]
public ref class RouterNamespacesSection
{
public: 
	RouterNamespacesSection();

	[System::Xml::Serialization::XmlElement(L"namespace", Type=Microsoft::ServiceModel::Samples::RouterNamespace::typeid)]
	property System::Collections::Generic::List<Microsoft::ServiceModel::Samples::RouterNamespace ^> ^ RouterNamespaces
	{
		System::Collections::Generic::List<Microsoft::ServiceModel::Samples::RouterNamespace ^> ^ get();
		void set( System::Collections::Generic::List<Microsoft::ServiceModel::Samples::RouterNamespace ^> ^ value );
	}

private: 
	System::Collections::Generic::List<Microsoft::ServiceModel::Samples::RouterNamespace ^> ^ namespaces;
};

[System::Xml::Serialization::XmlType(Namespace=L"http://Microsoft.ServiceModel.Samples/Router")]
public ref class RoutesSection
{
public: 
	
	RoutesSection();

	[System::Xml::Serialization::XmlElement(L"route", Type=Microsoft::ServiceModel::Samples::Route::typeid)]
	property System::Collections::Generic::List<Microsoft::ServiceModel::Samples::Route ^> ^ Routes
	{
		System::Collections::Generic::List<Microsoft::ServiceModel::Samples::Route ^> ^ get();
		void set( System::Collections::Generic::List<Microsoft::ServiceModel::Samples::Route ^> ^ value );
	}

private: 
	System::Collections::Generic::List<Microsoft::ServiceModel::Samples::Route ^> ^ routes;
};


[System::Serializable, System::Xml::Serialization::XmlType(Namespace=L"http://Microsoft.ServiceModel.Samples/Router"), System::Xml::Serialization::XmlRoot(L"routingTable", Namespace=L"http://Microsoft.ServiceModel.Samples/Router", IsNullable=false)]
public ref class RoutingTableData
{

public: 
	RoutingTableData();

	[System::Xml::Serialization::XmlElement(L"namespaces", Type=Microsoft::ServiceModel::Samples::RouterNamespacesSection::typeid)]
	property Microsoft::ServiceModel::Samples::RouterNamespacesSection ^ RouterNamespacesSection
	{
		Microsoft::ServiceModel::Samples::RouterNamespacesSection ^ get();
		void set( Microsoft::ServiceModel::Samples::RouterNamespacesSection ^ value );
	}
	
	[System::Xml::Serialization::XmlElement(L"routes", Type=Microsoft::ServiceModel::Samples::RoutesSection::typeid)]
	property Microsoft::ServiceModel::Samples::RoutesSection ^ RoutesSection
	{
		Microsoft::ServiceModel::Samples::RoutesSection ^ get();
		void set( Microsoft::ServiceModel::Samples::RoutesSection ^ value );
	}
	
private: 
	Microsoft::ServiceModel::Samples::RouterNamespacesSection ^ namespaces;
	Microsoft::ServiceModel::Samples::RoutesSection ^ routes;
};

public ref class RoutingTable
{
public: 
	RoutingTable();

	System::ServiceModel::EndpointAddress ^ SelectDestination(System::ServiceModel::Channels::Message ^ message);

	property System::ServiceModel::Dispatcher::XPathMessageFilterTable<System::ServiceModel::EndpointAddress ^> ^ FilterTable
	{
		System::ServiceModel::Dispatcher::XPathMessageFilterTable<System::ServiceModel::EndpointAddress ^> ^ get();
	}

private:
	System::ServiceModel::Dispatcher::XPathMessageFilterTable<System::ServiceModel::EndpointAddress ^> ^ filterTable;
	System::Random ^ randomNumberGenerator;
};

 


}
}
}