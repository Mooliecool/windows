

#include "RoutingTable.h"

using namespace System;
using namespace System::ServiceModel;
using namespace System::ServiceModel::Dispatcher;
using namespace System::ServiceModel::Channels;
using namespace System::Configuration;
using namespace System::Xml;
using namespace System::Xml::Serialization;
using namespace System::Runtime::Serialization;
using namespace System::Collections::Generic;

using namespace Microsoft::ServiceModel::Samples;

//=======================================

RoutingTableData::RoutingTableData()
{
	this->namespaces = gcnew Microsoft::ServiceModel::Samples::RouterNamespacesSection();
	this->routes = gcnew Microsoft::ServiceModel::Samples::RoutesSection();
}

Microsoft::ServiceModel::Samples::RouterNamespacesSection ^ RoutingTableData::RouterNamespacesSection::get()
{
    return this->namespaces;
}

void RoutingTableData::RouterNamespacesSection::set( Microsoft::ServiceModel::Samples::RouterNamespacesSection ^ value )
{
    this->namespaces = value;
}

Microsoft::ServiceModel::Samples::RoutesSection ^ RoutingTableData::RoutesSection::get()
{
    return this->routes;
}

void RoutingTableData::RoutesSection::set( Microsoft::ServiceModel::Samples::RoutesSection ^ value )
{
    this->routes = value;
}

//=======================================

RouterNamespacesSection::RouterNamespacesSection()
{
	this->namespaces = gcnew System::Collections::Generic::List<Microsoft::ServiceModel::Samples::RouterNamespace ^>();
}


System::Collections::Generic::List<Microsoft::ServiceModel::Samples::RouterNamespace ^> ^ RouterNamespacesSection::RouterNamespaces::get()
{
    return this->namespaces;
}

void RouterNamespacesSection::RouterNamespaces::set( System::Collections::Generic::List<Microsoft::ServiceModel::Samples::RouterNamespace^> ^ value )
{
    this->namespaces = value;
}

//=======================================


Route::Route()
{
}

Route::Route(System::String ^ xPath, System::String ^ uri)
{
	this->xPath = xPath;
	this->uri = uri;
}

System::String ^ Route::Uri::get()
{
    return this->uri;
}

void Route::Uri::set( System::String ^ value )
{
    this->uri = value;
}


System::String ^ Route::XPath::get()
{
    return this->xPath;
}

void Route::XPath::set( System::String ^ value )
{
    this->xPath = value;
}


//=======================================

RouterNamespace::RouterNamespace()
{
}

RouterNamespace::RouterNamespace(System::String ^ prefix, System::String ^ ns)
{
	this->prefix = prefix;
	this->ns = ns;
}

System::String ^ RouterNamespace::NamespaceUri::get()
{
	return this->ns;
}

void RouterNamespace::NamespaceUri::set( System::String ^ value )
{
	this->ns = value;
}

System::String ^ RouterNamespace::Prefix::get()
{
	return this->prefix;
}

void RouterNamespace::Prefix::set( System::String ^ value )
{
	this->prefix = value;
}

//=============================

RoutesSection::RoutesSection()
{
	this->Routes = gcnew System::Collections::Generic::List<Microsoft::ServiceModel::Samples::Route ^>();
}

System::Collections::Generic::List<Microsoft::ServiceModel::Samples::Route ^> ^ RoutesSection::Routes::get()
{
    return this->routes;
}

void RoutesSection::Routes::set( System::Collections::Generic::List<Microsoft::ServiceModel::Samples::Route ^> ^ value )

{
    this->routes = value;
}


//================

RoutingTable::RoutingTable()
{
	this->filterTable = gcnew XPathMessageFilterTable<EndpointAddress^>();
	this->randomNumberGenerator = gcnew Random();

    XmlNamespaceManager ^ manager = gcnew XPathMessageContext();

	XmlReader ^ routingTableDataFileReader = XmlReader::Create(ConfigurationManager::AppSettings[L"routingTableXmlFile"]);
	RoutingTableData ^ routingTableData = safe_cast<RoutingTableData^>((gcnew XmlSerializer(RoutingTableData::typeid))->Deserialize(routingTableDataFileReader));

	for ( int i = 0; i < routingTableData->RouterNamespacesSection->RouterNamespaces->Count; i++ )
    {
		RouterNamespace ^ ns = routingTableData->RouterNamespacesSection->RouterNamespaces[ i ];
        manager->AddNamespace(ns->Prefix, ns->NamespaceUri);
    }

	for ( int i = 0; i < routingTableData->RoutesSection->Routes->Count; i++ )
    {
		Route ^ route = routingTableData->RoutesSection->Routes[ i ];
        this->filterTable->Add(gcnew XPathMessageFilter(route->XPath, manager), gcnew EndpointAddress(route->Uri));
    }
	
}

System::ServiceModel::EndpointAddress ^ RoutingTable::SelectDestination(System::ServiceModel::Channels::Message ^ message)
{
	EndpointAddress ^ selectedAddress = nullptr;

	IList<EndpointAddress^> ^ results = gcnew List<EndpointAddress^>();
	this->filterTable->GetMatchingValues(message, results);

	if (results->Count == 1)
	{
		selectedAddress = results[0];
	}
	else if (results->Count > 0)
	{
		selectedAddress = results[this->randomNumberGenerator->Next(0, results->Count)];
	}

	return selectedAddress;
}

System::ServiceModel::Dispatcher::XPathMessageFilterTable<System::ServiceModel::EndpointAddress ^> ^ RoutingTable::FilterTable::get()
{
	return this->filterTable;
}
