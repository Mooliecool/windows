
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

#include "generatedClient.h"

using namespace System;
using namespace System::ServiceModel;

//The service contract is defined in generatedProxy.cs, generated from the service by the svcutil tool.
void DoCalculations(ICalculator^ client);

//Client implementation code.
int main(array<System::String ^> ^args)
{			
	// Create a client to default (http) endpoint configuration 
    CalculatorClient^ client = gcnew CalculatorClient(L"default");
	Console::WriteLine(L"Communicate with http endpoint.");
	// call operations
    DoCalculations(client);
    //Closing the client gracefully closes the connection and cleans up resources
    client->Close();

	// Create a client to tcp endpoint configuration
    client = gcnew CalculatorClient(L"tcp");
	Console::WriteLine(L"Communicate with tcp endpoint.");
	// call operations
	DoCalculations(client);
    client->Close();

	// Create a client to named pipe endpoint configuration
    client = gcnew CalculatorClient(L"namedpipe");
	Console::WriteLine(L"Communicate with named pipe endpoint.");
	// call operations
	DoCalculations(client);
    client->Close();

	Console::WriteLine();
	Console::WriteLine(L"Press <ENTER> to terminate client.");
	Console::ReadLine();
}

void DoCalculations(ICalculator^ client)
{
	// Call the Add service operation.
	double value1 = 100.00;
	double value2 = 15.99;
	double result = client->Add(value1, value2);
    Console::WriteLine(L"Add({0},{1}) = {2}", value1, value2, result);

	// Call the Subtract service operation.
	value1 = 145.00;
	value2 = 76.54;
	result = client->Subtract(value1, value2);
	Console::WriteLine(L"Subtract({0},{1}) = {2}", value1, value2, result);

	// Call the Multiply service operation.
	value1 = 9.00;
	value2 = 81.25;
	result = client->Multiply(value1, value2);
	Console::WriteLine(L"Multiply({0},{1}) = {2}", value1, value2, result);

	// Call the Divide service operation.
	value1 = 22.00;
	value2 = 7.00;
	result = client->Divide(value1, value2);
	Console::WriteLine(L"Divide({0},{1}) = {2}", value1, value2, result);
}


