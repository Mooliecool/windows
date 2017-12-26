
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

#include "generatedClient.h"

using namespace System;
using namespace System::ServiceModel;
using namespace System::ServiceModel::Description;
using namespace System::Configuration;

ref class Client
{
public :
    static void DoCalculations(CalculatorClient ^ client)
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
};

int main(array<System::String ^> ^args)
{			
    // Create a MetadataExchangeClient for retrieving metadata.
    String ^address = ConfigurationManager::AppSettings[L"mexAddress"];
    EndpointAddress ^mexAddress = gcnew EndpointAddress(address);
    MetadataExchangeClient ^mexClient = gcnew MetadataExchangeClient(mexAddress);

    // Retrieve the metadata for all endpoints using metadata exchange protocol (mex).
    MetadataSet ^metadataSet = mexClient->GetMetadata();

    //Convert the metadata into endpoints
    WsdlImporter ^importer = gcnew WsdlImporter(metadataSet);
    ServiceEndpointCollection ^endpoints = importer->ImportAllEndpoints();

    // Communicate with each endpoint
    for (int i = 0; i < endpoints->Count; i++)
    {
        ServiceEndpoint ^ep = endpoints[i];
        // Create a client using the endpoint address and binding.						
        CalculatorClient ^ client = gcnew CalculatorClient(ep->Binding, ep->Address);
        Console::WriteLine(L"Communicate with endpoint: ");
        Console::WriteLine(L"   AddressPath={0}", ep->Address->Uri->PathAndQuery);
        Console::WriteLine(L"   Binding={0}", ep->Binding->Name);

        // call operations
        Client::DoCalculations(client);

        //Closing the client gracefully closes the connection and cleans up resources
        client->Close();
    }

    Console::WriteLine();
    Console::WriteLine(L"Press <ENTER> to terminate client.");
    Console::ReadLine();

    return 0;
}