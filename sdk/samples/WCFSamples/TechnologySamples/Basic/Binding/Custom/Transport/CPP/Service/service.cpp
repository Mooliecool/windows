// This is the main DLL file.

#include "service.h"
#using <System.ServiceModel.dll>

using namespace System;
using namespace System::Configuration;
using namespace System::ServiceModel;
using namespace Microsoft::JScript;

namespace Microsoft {
namespace ServiceModel {
namespace Samples {
    public ref class CalculatorService : public Microsoft::ServiceModel::Samples::ICalculator
    {
    public: 
            CalculatorService()
            {
            }

            virtual double Add(double n1, double n2)
            {
				double result = n1 + n2;
				Console::WriteLine(L"Received Add(" + n1 + "," + n2 + ")");
				Console::WriteLine(L"Return: " + result);
				return result;
			}

            virtual double Subtract(double n1, double n2)
            {
				double result = n1 - n2;
				Console::WriteLine(L"Received Subtract(" + n1 + "," + n2 + ")");
				Console::WriteLine(L"Return: " + result);
				return result;
            }

            virtual double Multiply(double n1, double n2)
            {
				double result = n1 * n2;
				Console::WriteLine(L"Received Multiply(" + n1 + "," + n2 + ")");
				Console::WriteLine(L"Return: " + result);
				return result;
            }

            virtual double Divide(double n1, double n2)
            {
				double result = n1 / n2;
				Console::WriteLine(L"Received Divide(" + n1 + "," + n2 + ")");
				Console::WriteLine(L"Return: " + result);
				return result;
            }
    };
}
}
}

void main()
{
    // Get base address from app settings in configuration
	Uri^ baseHttpAddress = gcnew Uri(ConfigurationManager::AppSettings::get()->Get(L"baseHttpAddress"));
	Uri^ baseTcpAddress = gcnew Uri(ConfigurationManager::AppSettings::get()->Get(L"baseTcpAddress"));
	Uri^ basePipeAddress = gcnew Uri(ConfigurationManager::AppSettings::get()->Get(L"basePipeAddress"));

	// Create a ServiceHost for the CalculatorService type and provide the base address.
	ServiceHost^ serviceHost = gcnew ServiceHost(Microsoft::ServiceModel::Samples::CalculatorService::typeid, baseHttpAddress,baseTcpAddress,basePipeAddress);
	serviceHost->Description->Behaviors->Add(gcnew System::ServiceModel::Description::ServiceMetadataBehavior());
	serviceHost->AddServiceEndpoint(System::ServiceModel::Description::ServiceMetadataBehavior::MexContractName, 
		System::ServiceModel::Description::MetadataExchangeBindings::CreateMexHttpBinding(),
		L"mex");
    // Open the ServiceHostBase to create listeners and start listening for messages.
    serviceHost->Open();

    // The service can now be accessed.
    Console::WriteLine(L"The service is ready.");
    Console::WriteLine(L"Press <ENTER> to terminate service.");
    Console::WriteLine();
    Console::ReadLine();

    // Close the ServiceHostBase to shutdown the service.
    serviceHost->Close();
}

