
#include "Service.h"

using namespace System::Configuration;
using namespace System::ServiceModel;

namespace Microsoft{
namespace ServiceModel{
namespace Samples 
{
    PurchaseOrderLineItem::PurchaseOrderLineItem(System::String ^productId, int quantity)
    {
        this->ProductId = productId;
        this->Quantity = quantity;
        this->UnitCost = (float)r->Next(10000);
    }

    String^ PurchaseOrderLineItem::ToString()
    {
        String ^displayString = L"Order LineItem: " + Quantity + L" of " + ProductId + L" @unit price: $" + UnitCost + L"\n";
        return displayString;
    }

    Single PurchaseOrderLineItem::TotalCost::get()
    {
        return this->UnitCost * this->Quantity; 
    }

    PurchaseOrder::PurchaseOrder(System::String ^customerID)
    {
        this->CustomerId = customerID;
        this->PONumber = Guid::NewGuid().ToString();
        this->orderLineItems = gcnew LinkedList<PurchaseOrderLineItem ^>;
    }

    void PurchaseOrder::AddProductLineItem(System::String ^productId, int quantity)
    {
        this->orderLineItems->AddLast(gcnew PurchaseOrderLineItem(productId,quantity));
    }

    Single PurchaseOrder::TotalCost::get()
    {
        float totalCost = 0;
        for each (PurchaseOrderLineItem ^lineItem in orderLineItems)
        {
            totalCost += lineItem->TotalCost;
        }
        return totalCost;
    }

    String^ PurchaseOrder::Status::get()
    {
        return L"Pending";
    }

    String^ PurchaseOrder::ToString()
    {
        StringBuilder ^strbuf = gcnew StringBuilder(L"Purchase Order: " + this->PONumber + L"\n");
        strbuf->Append(L"\tCustomer: " + CustomerId + L"\n");
        strbuf->Append(L"\tOrderDetails\n");
        for each (PurchaseOrderLineItem ^lineItem in orderLineItems)
        {
            strbuf->Append(L"\t\t" + lineItem->ToString());
        }

        strbuf->Append(L"\tTotal cost of this order: $" + TotalCost + L"\n");
        strbuf->Append(L"\tOrder status: " + Status + L"\n");
        return strbuf->ToString();
    }

    void OrderTakerService::OpenPurchaseOrder(System::String ^customerId)
    {
        Console::WriteLine(L"Creating purchase order");
        po = gcnew PurchaseOrder(customerId);
    }

    void OrderTakerService::AddProductLineItem(System::String ^productId, int quantity)
    {
        po->AddProductLineItem(productId, quantity);
        Console::WriteLine(L"Product " + productId + L" quantity " + quantity + L" added to purchase order");
    }

    void OrderTakerService::EndPurchaseOrder()
    {
        Console::WriteLine(L"Purchase Order Completed");
        Console::WriteLine();
        Console::WriteLine(po->ToString());
    }
}
}
}

int main(array<System::String ^> ^args)
{
    // Get MSMQ queue name from app settings in configuration
    String^ queueName = ConfigurationManager::AppSettings[L"queueName"];

    // Create the transacted MSMQ queue if necessary.
    if (!MessageQueue::Exists(queueName))
        MessageQueue::Create(queueName, true);

    // Create a ServiceHost for the OrderTakerService type.
    ServiceHost ^serviceHost;
    serviceHost = gcnew ServiceHost(Microsoft::ServiceModel::Samples::OrderTakerService::typeid);
    // Open the ServiceHostBase to create listeners and start listening for messages.
    serviceHost->Open();

    // The service can now be accessed.
    Console::WriteLine(L"The service is ready.");
    Console::WriteLine(L"Press <ENTER> to terminate service.");
    Console::WriteLine();
    Console::ReadLine();

    // Close the ServiceHostBase to shutdown the service.
    serviceHost->Close(); 
    delete serviceHost;
}
