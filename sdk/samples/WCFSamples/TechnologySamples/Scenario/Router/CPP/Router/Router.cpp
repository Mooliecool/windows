// ----------------------------------------------------------------------------
// Copyright (C) 2003-2005 Microsoft Corporation, All rights reserved.
// ----------------------------------------------------------------------------

#include "Contracts.h"
#include "Router.h"
#include "RouterBinding.h"

#include <msclr/lock.h>

using namespace msclr;

using namespace System;
using namespace System::ServiceModel;
using namespace System::ServiceModel::Description;
using namespace System::ServiceModel::Channels;
using namespace System::ServiceModel::Dispatcher;
using namespace System::Diagnostics;
using namespace System::Configuration;

using namespace Microsoft::ServiceModel::Samples;

//===========================

SoapRouterExtension::SoapRouterExtension()
{

    this->routingTable = gcnew Microsoft::ServiceModel::Samples::RoutingTable();
    this->bindings = gcnew Dictionary<System::String^,Binding^>( 3 );

    this->simplexDatagramChannels = gcnew Dictionary<EndpointAddress ^, ISimplexDatagramRouter^>();
    this->requestReplyDatagramChannels = gcnew Dictionary<EndpointAddress^, IRequestReplyDatagramRouter^>();


    this->bindings->Add(L"http", gcnew RouterBinding(RouterTransport::Http));
    this->bindings->Add(L"net.tcp", gcnew RouterBinding(RouterTransport::Tcp));
    this->bindings->Add(L"net.pipe", gcnew RouterBinding(RouterTransport::NamedPipe));
}

void SoapRouterExtension::Attach(System::ServiceModel::ServiceHostBase ^ owner)
{
}

void SoapRouterExtension::Detach(System::ServiceModel::ServiceHostBase ^ owner)
{
}

System::Collections::Generic::IDictionary<System::String ^, System::ServiceModel::Channels::Binding ^> ^ SoapRouterExtension::Bindings::get()
{
    return this->bindings;
}

System::Collections::Generic::IDictionary<System::ServiceModel::EndpointAddress ^, Microsoft::ServiceModel::Samples::IRequestReplyDatagramRouter ^> ^ SoapRouterExtension::RequestReplyDatagramChannels::get()
{
    return this->requestReplyDatagramChannels;
}

Microsoft::ServiceModel::Samples::RoutingTable ^ SoapRouterExtension::RoutingTable::get()
{
    return this->routingTable;
}

System::Collections::Generic::IDictionary<System::ServiceModel::EndpointAddress ^, Microsoft::ServiceModel::Samples::ISimplexDatagramRouter ^> ^ SoapRouterExtension::SimplexDatagramChannels::get()
{
    return this->simplexDatagramChannels;
}

//===========================

SoapRouterServiceBehavior::SoapRouterServiceBehavior()
{
}

void SoapRouterServiceBehavior::AddBindingParameters(System::ServiceModel::Description::ServiceDescription ^ description, System::ServiceModel::ServiceHostBase ^ serviceHostBase, System::Collections::ObjectModel::Collection<System::ServiceModel::Description::ServiceEndpoint ^> ^ endpoints, System::ServiceModel::Channels::BindingParameterCollection ^ parameters)
{
}

void SoapRouterServiceBehavior::ApplyDispatchBehavior(System::ServiceModel::Description::ServiceDescription ^ description, System::ServiceModel::ServiceHostBase ^ serviceHostBase)
{
    SoapRouterExtension ^ extension = gcnew SoapRouterExtension();
    serviceHostBase->Extensions->Add(extension);
}

void SoapRouterServiceBehavior::Validate(System::ServiceModel::Description::ServiceDescription ^ description, System::ServiceModel::ServiceHostBase ^ serviceHostBase)
{
}

//===========================

SoapRouter::SoapRouter()
{
    this->duplexSessionChannel = nullptr;
    this->simplexSessionChannel = nullptr;

    this->sessionSyncRoot = gcnew Object();
    ServiceHostBase ^ host = OperationContext::Current->Host;
    this->extension = host->Extensions->Find<SoapRouterExtension^>();
}

SoapRouter::~SoapRouter()
{
    IClientChannel ^ channel = nullptr;

    if ( this->simplexSessionChannel != nullptr )
    {
        channel = safe_cast<IClientChannel^>(this->simplexSessionChannel);
    }
    else if ( this->duplexSessionChannel != nullptr )
    {
        channel = safe_cast<IClientChannel^>(this->duplexSessionChannel);
    }

    if ( channel != nullptr && channel->State != CommunicationState::Closed)
    {
        try
        {
            // Close will abort the channel should an exception be thrown.
            channel->Close();
        }
        catch (CommunicationException ^)
        { }
    }
}

void SoapRouter::DuplexSessionRouterProcessMessage(System::ServiceModel::Channels::Message ^ message)
{
    // One router service instance exists for each sessionful channel. If a channel hasn't been created yet, create one.
    if (this->duplexSessionChannel == nullptr)
    {
        try
        {
            lock::lock(this->sessionSyncRoot);

            if (this->duplexSessionChannel == nullptr)
            {
                EndpointAddress ^ forwardingAddress = this->extension->RoutingTable->SelectDestination(message);
                if (forwardingAddress == nullptr)
                {
                    message->Close();
                    return;
                }

                ISimplexSessionRouter ^ callbackChannel = OperationContext::Current->GetCallbackChannel<ISimplexSessionRouter^>();
                // Don't register the forwarding channel with the service instance. That way, the service instance can get disposed when the incoming channel closes, and then dispose of the forwarding channel.

                OperationContextScope ^ scope = gcnew OperationContextScope(safe_cast<OperationContext^>(nullptr));
                try
                {
                    ChannelFactory<IDuplexSessionRouter^> ^ factory = gcnew DuplexChannelFactory<IDuplexSessionRouter^>(gcnew InstanceContext(nullptr, gcnew ReturnMessageHandler(callbackChannel)),this->extension->Bindings[forwardingAddress->Uri->Scheme], forwardingAddress);
                    // Add a channel behavior that will turn off validation of @mustUnderstand on the headers belonging to messages flowing the opposite direction.											
                    factory->Endpoint->Behaviors->Add(gcnew MustUnderstandBehavior(false));
                    this->duplexSessionChannel = factory->CreateChannel();
                    scope = nullptr;
                }
                finally
                {	
                    delete scope;
                }
            }
        }
        catch ( System::Exception ^ exception )
        {
            Console::WriteLine( exception->Message );
        }
    }

    Console::WriteLine(L"Forwarding message {0}...", message->Headers->Action );
    this->duplexSessionChannel->DuplexSessionRouterProcessMessage(message);
}

void SoapRouter::SimplexSessionRouterProcessMessage(System::ServiceModel::Channels::Message ^ message)
{
    // One router service instance exists for each sessionful channel. If a channel hasn't been created yet, create one.
    if (this->simplexSessionChannel == nullptr)
    {
        try
        {
            lock::lock(this->sessionSyncRoot);

            if (this->simplexSessionChannel == nullptr)
            {
                EndpointAddress ^ forwardingAddress = this->extension->RoutingTable->SelectDestination(message);
                if (forwardingAddress == nullptr)
                {
                    message->Close();
                    return;
                }

                OperationContextScope ^ scope = gcnew OperationContextScope(safe_cast<OperationContext^>(nullptr));
                // Don't register the forwarding channel with the service instance. That way, the service instance can get disposed when the incoming channel closes, and then dispose of the forwarding channel.					
                try
                {				
                    ChannelFactory<ISimplexSessionRouter^> ^ factory = gcnew ChannelFactory<ISimplexSessionRouter^>(this->extension->Bindings[forwardingAddress->Uri->Scheme], forwardingAddress);
                    this->simplexSessionChannel = factory->CreateChannel();
                    scope = nullptr;
                }
                finally
                {
                    delete scope;
                }
            }
        }
        catch ( System::Exception ^ exception )
        {
            Console::WriteLine( exception->Message );
        }
    }

    Console::WriteLine(L"Forwarding message {0}...", message->Headers->Action );
    this->simplexSessionChannel->SimplexSessionRouterProcessMessage(message);
}

System::ServiceModel::Channels::Message ^ SoapRouter::RequestReplyDatagramRouterProcessMessage(System::ServiceModel::Channels::Message ^ message)
{
    EndpointAddress ^ to = this->extension->RoutingTable->SelectDestination(message);
    if (to == nullptr)
    {
        message->Close();
        return nullptr;
    }

    // If the router doesn't already have a two-way datagram channel to the 'to' EPR or if that channel is no longer opened, create one.
    IRequestReplyDatagramRouter ^ forwardingChannel;

    if (!this->extension->RequestReplyDatagramChannels->TryGetValue(to, forwardingChannel) || (safe_cast<IClientChannel^>(forwardingChannel))->State != CommunicationState::Opened)
    {
        try
        {
            lock::lock(this->sessionSyncRoot);

            if (!this->extension->RequestReplyDatagramChannels->TryGetValue(to, forwardingChannel) || (safe_cast<IClientChannel^>(forwardingChannel))->State != CommunicationState::Opened)
            {
                ChannelFactory<IRequestReplyDatagramRouter^> ^ factory = gcnew ChannelFactory<IRequestReplyDatagramRouter^>(this->extension->Bindings[to->Uri->Scheme], to);
                // Add a channel behavior that will turn off validation of @mustUnderstand on the reply's headers.
                factory->Endpoint->Behaviors->Add(gcnew MustUnderstandBehavior(false));
                forwardingChannel = factory->CreateChannel();

                this->extension->RequestReplyDatagramChannels[to] = forwardingChannel;
            }
        }
        catch ( System::Exception ^ exception )
        {
            Console::WriteLine( exception->Message );
        }
    }

    Console::WriteLine(L"Forwarding request {0}...", message->Headers->Action );
    Message ^ response = forwardingChannel->RequestReplyDatagramRouterProcessMessage(message);

    Console::WriteLine(L"Forwarding response {0}...", response->Headers->Action );
    return response;
}

void SoapRouter::SimplexDatagramRouterProcessMessage(System::ServiceModel::Channels::Message ^ message)
{
    EndpointAddress ^ to = this->extension->RoutingTable->SelectDestination(message);
    if (to == nullptr)
    {
        message->Close();
        return;
    }

    // If the router doesn't already have a one-way datagram channel to the 'to' EPR or if that channel is no longer opened, create one.
    ISimplexDatagramRouter ^ forwardingChannel;
    if (!this->extension->SimplexDatagramChannels->TryGetValue(to, forwardingChannel) || (safe_cast<IClientChannel^>(forwardingChannel))->State != CommunicationState::Opened)
    {
        try
        {
            lock::lock(this->sessionSyncRoot);

            if (!this->extension->SimplexDatagramChannels->TryGetValue(to, forwardingChannel) || (safe_cast<IClientChannel^>(forwardingChannel))->State != CommunicationState::Opened)
            {
                forwardingChannel = (gcnew ChannelFactory<ISimplexDatagramRouter^>(this->extension->Bindings[to->Uri->Scheme], to))->CreateChannel();
                this->extension->SimplexDatagramChannels[to] = forwardingChannel;
            }
        }
        catch ( System::Exception ^ exception )
        {
            Console::WriteLine( exception->Message );
        }
    }

    Console::WriteLine(L"Forwarding message {0}...", message->Headers->Action );
    forwardingChannel->SimplexDatagramRouterProcessMessage(message);
}

SoapRouter::ReturnMessageHandler::ReturnMessageHandler(Microsoft::ServiceModel::Samples::ISimplexSessionRouter ^ returnChannel)
{
    this->returnChannel = returnChannel;
}

void SoapRouter::ReturnMessageHandler::SimplexSessionRouterProcessMessage(System::ServiceModel::Channels::Message ^ message)
{
    Console::WriteLine(L"Forwarding return message {0}..." , message->Headers->Action );
    this->returnChannel->SimplexSessionRouterProcessMessage(message);
}

//===========================

int main(array<System::String ^> ^args)
{

    ServiceHost ^ serviceHost = gcnew ServiceHost( Microsoft::ServiceModel::Samples::SoapRouter::typeid );

    serviceHost->Open();

    for (int i = 0; i < serviceHost->ChannelDispatchers->Count; i++)
    {
        ChannelDispatcher ^ channelDispatcher = safe_cast<ChannelDispatcher^>( serviceHost->ChannelDispatchers[ i ] );

        for ( int j = 0; j < channelDispatcher->Endpoints->Count; j++ )
        {
            EndpointDispatcher ^ endpointDispatcher = channelDispatcher->Endpoints[ j ];
            Console::WriteLine( L"Listening on {0}...", endpointDispatcher->EndpointAddress->ToString() );
        }        
    }

    Console::WriteLine();
    Console::WriteLine(L"Press Enter to exit...");
    Console::ReadLine();
    serviceHost->Close();

    return 0;
}

