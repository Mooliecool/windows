// ----------------------------------------------------------------------------
// Copyright (C) 2003-2005 Microsoft Corporation, All rights reserved.
// ----------------------------------------------------------------------------

using namespace System;
using namespace System::ServiceModel;
using namespace System::ServiceModel::Dispatcher;
using namespace System::Diagnostics;
using namespace System::Configuration;

namespace Microsoft{
namespace ServiceModel{
namespace Samples{

    [System::ServiceModel::ServiceContract(Namespace=L"http://Microsoft.ServiceModel.Samples/")]
    public interface class IEchoService
    {
        [System::ServiceModel::OperationContract(IsOneWay=false)]
        System::String ^ Echo(System::String ^ message);
    };




    [System::ServiceModel::ServiceBehavior(InstanceContextMode=System::ServiceModel::InstanceContextMode::Single)]
    public ref class EchoService : public IEchoService
    {

    public: 
        EchoService()
        {
        }

        virtual System::String ^ Echo(System::String ^ message)
        {
            return message;
        }
    };
}
}
}



int main(array<System::String ^> ^args)
{	
    ServiceHost ^ serviceHost = gcnew ServiceHost( Microsoft::ServiceModel::Samples::EchoService::typeid );

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