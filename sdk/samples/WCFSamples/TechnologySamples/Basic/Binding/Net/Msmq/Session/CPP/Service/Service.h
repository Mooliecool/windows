using namespace System;
using namespace System::ServiceModel;
using namespace System::ServiceModel::Channels;
using namespace System::Configuration;
using namespace System::Messaging;
using namespace System::Transactions;
using namespace System::Text;
using namespace System::Collections::Generic;

namespace Microsoft{
namespace ServiceModel{
namespace Samples 
{
    interface class IOrderTaker;

    [System::ServiceModel::ServiceContractAttribute(Namespace=L"http://Microsoft.ServiceModel.Samples", SessionMode=System::ServiceModel::SessionMode::Required)]
    public interface class IOrderTaker
    {
        [System::ServiceModel::OperationContractAttribute(IsOneWay = true)]
        void OpenPurchaseOrder(String ^customerId);

        [System::ServiceModel::OperationContractAttribute(IsOneWay = true)]
        void AddProductLineItem(String ^productId, int quantity);

        [System::ServiceModel::OperationContractAttribute(IsOneWay = true)]
        void EndPurchaseOrder();
    };

    public ref class PurchaseOrderLineItem
    {
        static Random ^r = gcnew Random(137);
        String^ ProductId;
        Single UnitCost;
        int Quantity;

    public:
        PurchaseOrderLineItem(String^ productId, int quantity);
        virtual String^ ToString() override;
        property Single TotalCost { Single get();};
    };

    public ref class PurchaseOrder
    {
        String^ PONumber;
        String^ CustomerId;
        LinkedList<PurchaseOrderLineItem ^> ^ orderLineItems; 
    public:
        PurchaseOrder(String^ customerID);
        void AddProductLineItem(String^ productId, int quantity);
        property Single TotalCost { Single get();};
        property String^ Status { String^ get();};
        virtual String^ ToString() override;
    };

    [System::ServiceModel::ServiceBehaviorAttribute(InstanceContextMode=System::ServiceModel::InstanceContextMode::PerSession)]
    public ref class OrderTakerService : IOrderTaker
    {
        PurchaseOrder ^po;
    public:
        [System::ServiceModel::OperationBehaviorAttribute(TransactionScopeRequired = true, TransactionAutoComplete = false)]
        virtual void OpenPurchaseOrder(String ^customerId);
        [System::ServiceModel::OperationBehaviorAttribute(TransactionScopeRequired = true, TransactionAutoComplete = false)]
        virtual void AddProductLineItem(String ^productId, int quantity);
        [System::ServiceModel::OperationBehaviorAttribute(TransactionScopeRequired = true, TransactionAutoComplete = true)]
        virtual void EndPurchaseOrder();
    };

}
}
}
