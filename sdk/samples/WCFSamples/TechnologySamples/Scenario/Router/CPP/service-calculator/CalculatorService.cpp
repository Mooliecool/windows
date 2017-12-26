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

    interface class ICalculatorService;

    [ServiceContract(Namespace = L"http://Microsoft.ServiceModel.Samples/", SessionMode=SessionMode::Required)]
    public interface class ICalculatorService
    {
        [OperationContract(IsOneWay = false)]
        int Add(int number);

        [OperationContract(IsOneWay = false)]
        int Subtract(int number);
    };


    [ServiceBehavior(InstanceContextMode = InstanceContextMode::PerSession)]
    public ref class CalculatorService : public ICalculatorService
    {
    private:

        int total;

    public:
        CalculatorService()
        {
            this->total = 0;
        }

        virtual int Add(int number)
        {
            this->total += number;
            return this->total;
        }

        virtual int Subtract(int number)
        {
            this->total -= number;
            return this->total;
        }
    };
}
}
}



int main(array<System::String ^> ^args)
{
    ServiceHost ^ serviceHost = gcnew ServiceHost( Microsoft::ServiceModel::Samples::CalculatorService::typeid );

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