
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

#include "generatedClient.h"

using namespace System;
using namespace System::ServiceModel;

namespace Microsoft{
namespace ServiceModel {
namespace Samples
{
    // The service contract is defined in generatedClient.h, generated from the service by the svcutil tool.

    // Define class which implements callback interface of duplex contract
	public ref class CallbackHandler : public ICalculatorDuplexCallback
	{
    public: 
        virtual void Equals(double result)
        {
            System::Console::WriteLine(L"Equals({0})", result);
        }

        virtual void Equation(System::String ^ eqn)
        {
            System::Console::WriteLine(L"Equation({0})", eqn);
        }
	};
}
}
}

int main(array<System::String ^> ^args)
{			
	// Construct InstanceContext to handle messages on callback interface
	System::ServiceModel::InstanceContext ^ instanceContext = gcnew System::ServiceModel::InstanceContext(gcnew Microsoft::ServiceModel::Samples::CallbackHandler());

    // Create a client
    CalculatorDuplexClient ^ client = gcnew CalculatorDuplexClient(instanceContext);

	System::Console::WriteLine(L"Press <ENTER> to terminate client once the output is displayed.");
	System::Console::WriteLine();

	// Call the AddTo service operation.
	double num1 = 100;
	client->AddTo(num1);

	// Call the SubtractFrom service operation.
	num1 = 50;
	client->SubtractFrom(num1);

	// Call the MultiplyBy service operation.
	num1 = 17.65;
	client->MultiplyBy(num1);


	// Call the DivideBy service operation.
	num1 = 2;
	client->DivideBy(num1);

	// Complete equation
	client->Clear();

	System::Console::ReadLine();
	client->Close();
	return 0;
}
