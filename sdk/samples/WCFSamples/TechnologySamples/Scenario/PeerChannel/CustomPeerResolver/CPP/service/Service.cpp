#include <msclr\lock.h>
#using <System.ServiceModel.dll>
#using <System.dll>
using namespace System;
using namespace System::Configuration;
using namespace System::Text;
using namespace System::Threading;
using namespace System::Collections::Generic;
using namespace msclr;

// Custom peer resolver service implementation. Implements registration, unregistration, and resolution 
// of mesh ID and associated addresses. Mesh ID identifies the mesh (a named collection of nodes identified 
// by the mesh ID). An example of mesh ID is chatMesh and identifies the host name portion of an EndpointAddresss,
// net.p2p://chatMesh/servicemodesamples/chat"
// Mesh IDs are expected to be unique and if multiple applications use the same custom peer resolver service, 
// they should choose different mesh IDs to avoid conflict.

// If you are unfamiliar with new concepts used in this sample, refer to the WCF Basic\GettingStarted sample.

namespace Microsoft{
namespace ServiceModel{
namespace Samples{
    [System::ServiceModel::ServiceContract(Namespace=L"http://Microsoft.ServiceModel.Samples")]
    public interface class ICustomPeerResolver
    {
        // Methods
        [System::ServiceModel::OperationContract]
        int Register(System::String^ meshId, System::ServiceModel::PeerNodeAddress^ nodeAddresses);

        [System::ServiceModel::OperationContract]
        array<System::ServiceModel::PeerNodeAddress^>^ Resolve(System::String ^ meshId, int maxAddresses);

        [System::ServiceModel::OperationContract]
        void Unregister(int registrationId);

        [System::ServiceModel::OperationContract]
        void Update(int registrationId, System::ServiceModel::PeerNodeAddress^ updatedNodeAddress);
    };

    [System::ServiceModel::ServiceBehavior]
    public ref class CustomPeerResolverService : public Microsoft::ServiceModel::Samples::ICustomPeerResolver
    {
    private: 
        ref class Registration
        {
        public: 
            Registration(System::String ^ meshId, System::ServiceModel::PeerNodeAddress ^ nodeAddress)
            {
                this->meshId = meshId;
                this->nodeAddress = nodeAddress;
            }

            property System::String ^ meshId;
            property System::ServiceModel::PeerNodeAddress ^ nodeAddress;
        };

    public: 
        CustomPeerResolverService()
        {
            this->random = gcnew System::Random();
        }

        virtual int Register(System::String ^ meshId, System::ServiceModel::PeerNodeAddress ^ nodeAddress)
        {
            int num1;
            bool flag1 = false;
            Microsoft::ServiceModel::Samples::CustomPeerResolverService::Registration ^ registration1 = gcnew Microsoft::ServiceModel::Samples::CustomPeerResolverService::Registration(meshId, nodeAddress);
            {
                lock registrationTableLock(Microsoft::ServiceModel::Samples::CustomPeerResolverService::registrationTable); //wait infinitely
                num1 = Microsoft::ServiceModel::Samples::CustomPeerResolverService::nextRegistrationId++;
                {
                    lock meshIdTableLock(Microsoft::ServiceModel::Samples::CustomPeerResolverService::meshIdTable);
                    System::Collections::Generic::Dictionary<Int32, System::ServiceModel::PeerNodeAddress ^> ^ dictionary1;
                    if (!Microsoft::ServiceModel::Samples::CustomPeerResolverService::meshIdTable->TryGetValue(meshId, dictionary1))
                    {
                        flag1 = true;
                        dictionary1 = gcnew System::Collections::Generic::Dictionary<Int32, System::ServiceModel::PeerNodeAddress ^>();
                        Microsoft::ServiceModel::Samples::CustomPeerResolverService::meshIdTable[meshId] = dictionary1;
                    }
                    dictionary1[num1] = nodeAddress;
                    Microsoft::ServiceModel::Samples::CustomPeerResolverService::registrationTable[num1] = gcnew Microsoft::ServiceModel::Samples::CustomPeerResolverService::Registration(meshId, nodeAddress);
                    // meshIdTableLock is automatically released when it goes out of scope and its destructor is called
                }
                // registrationTableLock is automatically released when it goes out of scope and its destructor is called
            }
            if (flag1)
            {
                System::Console::WriteLine(L"Registered new meshId {0}", meshId);
            }
            return num1;
        }

        virtual array<System::ServiceModel::PeerNodeAddress^>^ Resolve(System::String ^ meshId, int maxAddresses)
        {
            array<System::ServiceModel::PeerNodeAddress^>^ addressArray1;
            if (maxAddresses <= 0)
            {
                throw gcnew System::ArgumentOutOfRangeException(L"maxAddresses");
            }
            {
                lock meshIdTableLock(Microsoft::ServiceModel::Samples::CustomPeerResolverService::meshIdTable);
                System::Collections::Generic::Dictionary<Int32, System::ServiceModel::PeerNodeAddress^>^ dictionary1;
                if (Microsoft::ServiceModel::Samples::CustomPeerResolverService::meshIdTable->TryGetValue(meshId, dictionary1))
                {
                    addressArray1 = gcnew array<System::ServiceModel::PeerNodeAddress^>(dictionary1->Count);
                    dictionary1->Values->CopyTo(addressArray1, 0);
                }
                else
                {
                    addressArray1 = gcnew array<System::ServiceModel::PeerNodeAddress^>(0);
                }
            }
            if (addressArray1->Length <= maxAddresses)
            {
                return addressArray1;
            }
            System::Collections::Generic::List<Int32>^ list1 = gcnew System::Collections::Generic::List<Int32>(maxAddresses);
            while (list1->Count < maxAddresses)
            {
                int num1 = (this->random->Next() % addressArray1->Length);
                if (!list1->Contains(num1))
                {
                    list1->Add(num1);
                }
            }
            array<System::ServiceModel::PeerNodeAddress^>^ addressArray2 = gcnew array<System::ServiceModel::PeerNodeAddress^>(maxAddresses);
            for (int num2 = 0; (num2 < addressArray2->Length); num2++)
            {
                addressArray2[num2] = addressArray1[list1[num2]];
            }
            return addressArray2;
        }

        virtual void Unregister(int registrationId)
        {
            Microsoft::ServiceModel::Samples::CustomPeerResolverService::Registration ^ registration1;
            bool flag1 = false;
            {
                lock registrationLock(Microsoft::ServiceModel::Samples::CustomPeerResolverService::registrationTable);
                registration1 = Microsoft::ServiceModel::Samples::CustomPeerResolverService::registrationTable[registrationId];
                Microsoft::ServiceModel::Samples::CustomPeerResolverService::registrationTable->Remove(registrationId);
                {
                    lock meshLock(Microsoft::ServiceModel::Samples::CustomPeerResolverService::meshIdTable);
                    System::Collections::Generic::Dictionary<Int32, System::ServiceModel::PeerNodeAddress^> ^ dictionary1 = Microsoft::ServiceModel::Samples::CustomPeerResolverService::meshIdTable[registration1->meshId];
                    dictionary1->Remove(registrationId);
                    if (dictionary1->Count == 0)
                    {
                        Microsoft::ServiceModel::Samples::CustomPeerResolverService::meshIdTable->Remove(registration1->meshId);
                        flag1 = true;
                    }
                }
            }
            if (flag1)
            {
                System::Console::WriteLine(L"Unregistered meshId {0}", registration1->meshId);
            }
        }

        virtual void Update(int registrationId, System::ServiceModel::PeerNodeAddress ^ updatedNodeAddress)
        {
            {
                lock registerLock(Microsoft::ServiceModel::Samples::CustomPeerResolverService::registrationTable);
                Microsoft::ServiceModel::Samples::CustomPeerResolverService::Registration ^ registration1 = Microsoft::ServiceModel::Samples::CustomPeerResolverService::registrationTable[registrationId];
                {
                    lock meshTableLock(Microsoft::ServiceModel::Samples::CustomPeerResolverService::meshIdTable);
                    System::Collections::Generic::Dictionary<Int32, System::ServiceModel::PeerNodeAddress^> ^ dictionary1 = Microsoft::ServiceModel::Samples::CustomPeerResolverService::meshIdTable[registration1->meshId];
                    dictionary1[registrationId] = updatedNodeAddress;
                }
                Microsoft::ServiceModel::Samples::CustomPeerResolverService::registrationTable[registrationId]->nodeAddress = updatedNodeAddress;
            }
        }

    private: 
        static System::Collections::Generic::Dictionary<String ^,Dictionary<Int32, System::ServiceModel::PeerNodeAddress ^> ^> ^ meshIdTable = gcnew System::Collections::Generic::Dictionary<String ^,Dictionary<Int32, System::ServiceModel::PeerNodeAddress ^> ^>();
        static int nextRegistrationId;
        System::Random ^ random;
        static System::Collections::Generic::Dictionary<Int32, Registration ^> ^ registrationTable = gcnew System::Collections::Generic::Dictionary<Int32, Registration ^>();
    };
}
}
}

int main(array<System::String ^> ^args)
{
	////// Create a ServiceHost for the CustomPeerResolverService type.
	System::ServiceModel::ServiceHost ^serviceHost;
	serviceHost = gcnew System::ServiceModel::ServiceHost(Microsoft::ServiceModel::Samples::CustomPeerResolverService::typeid);
	////// Open the ServiceHostBase to create listeners and start listening for messages.
	serviceHost->Open();

    // The service can now be accessed.
    System::Console::WriteLine(L"The service is ready.");
    System::Console::WriteLine(L"Press <ENTER> to terminate service.");
    System::Console::WriteLine();
    System::Console::ReadLine();

    serviceHost->Close();
	Console::WriteLine(L"The service has shutdown.");
}
