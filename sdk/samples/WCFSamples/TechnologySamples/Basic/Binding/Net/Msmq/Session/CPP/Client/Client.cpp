// Client.cpp : main project file.

#include "generatedClient.h"

using namespace System;
using namespace System::Configuration;
using namespace System::Messaging;
using namespace System::ServiceModel;
using namespace System::Transactions;

int main(array<System::String ^> ^args)
{
	//Create a transaction scope.
	TransactionScope ^scope = gcnew TransactionScope(TransactionScopeOption::Required);
    // Create a client with given client endpoint configuration
    OrderTakerClient ^ client = gcnew OrderTakerClient(L"OrderTakerEndpoint");

    try
    {
        // Open a purchase order
        client->OpenPurchaseOrder(L"somecustomer.com");
        Console::WriteLine(L"Purchase Order created");

        // Add product line items
        Console::WriteLine(L"Adding 10 quantities of blue widget");
        client->AddProductLineItem("Blue Widget", 10);

        Console::WriteLine(L"Adding 23 quantities of red widget");
        client->AddProductLineItem(L"Red Widget", 23);

        // Close the purchase order
        Console::WriteLine(L"Closing the purchase order");
        client->EndPurchaseOrder();

        //Closing the client gracefully closes the connection and cleans up resources
        client->Close();                

        // Complete the transaction.
        scope->Complete();
    }
    finally
    {
        delete scope;
    }

    Console::WriteLine();
    Console::WriteLine(L"Press <ENTER> to terminate client.");
    Console::ReadLine();
}