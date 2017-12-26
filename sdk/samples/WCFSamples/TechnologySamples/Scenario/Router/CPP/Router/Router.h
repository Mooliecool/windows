
#include "RoutingTable.h"

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

private ref class SoapRouterExtension : public System::ServiceModel::IExtension<System::ServiceModel::ServiceHostBase ^>
{
public: 
	SoapRouterExtension();

	virtual void Attach(System::ServiceModel::ServiceHostBase ^ owner);

	virtual void Detach(System::ServiceModel::ServiceHostBase ^ owner);

	property System::Collections::Generic::IDictionary<System::ServiceModel::EndpointAddress ^, Microsoft::ServiceModel::Samples::ISimplexDatagramRouter ^> ^ SimplexDatagramChannels
	{
		System::Collections::Generic::IDictionary<System::ServiceModel::EndpointAddress ^, Microsoft::ServiceModel::Samples::ISimplexDatagramRouter ^> ^ get();
	}

	property Microsoft::ServiceModel::Samples::RoutingTable ^ RoutingTable
	{
		Microsoft::ServiceModel::Samples::RoutingTable ^ get();
	}

	property System::Collections::Generic::IDictionary<System::ServiceModel::EndpointAddress ^, Microsoft::ServiceModel::Samples::IRequestReplyDatagramRouter ^> ^ RequestReplyDatagramChannels
	{
		System::Collections::Generic::IDictionary<System::ServiceModel::EndpointAddress ^, Microsoft::ServiceModel::Samples::IRequestReplyDatagramRouter ^> ^ get();       
	}

	property System::Collections::Generic::IDictionary<System::String ^, System::ServiceModel::Channels::Binding ^> ^ Bindings
	{
		System::Collections::Generic::IDictionary<System::String ^, System::ServiceModel::Channels::Binding ^> ^ get();
	}

private: 
	System::Collections::Generic::IDictionary<System::String ^, System::ServiceModel::Channels::Binding ^> ^ bindings;
	System::Collections::Generic::IDictionary<System::ServiceModel::EndpointAddress ^, Microsoft::ServiceModel::Samples::IRequestReplyDatagramRouter ^> ^ requestReplyDatagramChannels;
	Microsoft::ServiceModel::Samples::RoutingTable ^ routingTable;
	System::Collections::Generic::IDictionary<System::ServiceModel::EndpointAddress ^, Microsoft::ServiceModel::Samples::ISimplexDatagramRouter ^> ^ simplexDatagramChannels;

};


 
private ref class SoapRouterServiceBehavior : public System::Attribute, public System::ServiceModel::Description::IServiceBehavior
{
public: 
	SoapRouterServiceBehavior();

	virtual void AddBindingParameters(System::ServiceModel::Description::ServiceDescription ^ description, System::ServiceModel::ServiceHostBase ^ serviceHostBase, System::Collections::ObjectModel::Collection<System::ServiceModel::Description::ServiceEndpoint ^> ^ endpoints, System::ServiceModel::Channels::BindingParameterCollection ^ parameters);
	virtual void ApplyDispatchBehavior(System::ServiceModel::Description::ServiceDescription ^ description, System::ServiceModel::ServiceHostBase ^ serviceHostBase);
	virtual void Validate(System::ServiceModel::Description::ServiceDescription ^ description, System::ServiceModel::ServiceHostBase ^ serviceHostBase);
};


[Microsoft::ServiceModel::Samples::SoapRouterServiceBehavior, System::ServiceModel::ServiceBehavior(InstanceContextMode=System::ServiceModel::InstanceContextMode::PerSession, ConcurrencyMode=System::ServiceModel::ConcurrencyMode::Multiple, ValidateMustUnderstand=false)]
public ref class SoapRouter : public Microsoft::ServiceModel::Samples::ISimplexDatagramRouter, public Microsoft::ServiceModel::Samples::ISimplexSessionRouter, public Microsoft::ServiceModel::Samples::IRequestReplyDatagramRouter, public Microsoft::ServiceModel::Samples::IDuplexSessionRouter, public System::IDisposable
{
public: 
	SoapRouter();

	virtual void SimplexSessionRouterProcessMessage(System::ServiceModel::Channels::Message ^ message) sealed;
	
	virtual void DuplexSessionRouterProcessMessage(System::ServiceModel::Channels::Message ^ message) sealed;
	
	virtual System::ServiceModel::Channels::Message ^ RequestReplyDatagramRouterProcessMessage(System::ServiceModel::Channels::Message ^ message) sealed;
	
	virtual void SimplexDatagramRouterProcessMessage(System::ServiceModel::Channels::Message ^ message) sealed;

	~SoapRouter();

private: 

	Microsoft::ServiceModel::Samples::IDuplexSessionRouter ^ duplexSessionChannel;
	Microsoft::ServiceModel::Samples::SoapRouterExtension ^ extension;
	System::Object ^ sessionSyncRoot;
	Microsoft::ServiceModel::Samples::ISimplexSessionRouter ^ simplexSessionChannel;   

   

	ref class ReturnMessageHandler : public Microsoft::ServiceModel::Samples::ISimplexSessionRouter
	{

	public:

		ReturnMessageHandler(Microsoft::ServiceModel::Samples::ISimplexSessionRouter ^ returnChannel);
		
		virtual void SimplexSessionRouterProcessMessage(System::ServiceModel::Channels::Message ^ message);

		Microsoft::ServiceModel::Samples::ISimplexSessionRouter ^ returnChannel;

	};

;};

}
}
}