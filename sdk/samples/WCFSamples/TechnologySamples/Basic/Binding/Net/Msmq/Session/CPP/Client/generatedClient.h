#pragma once

#using <mscorlib.dll>
#using <System.ServiceModel.dll>
#using <System.Runtime.Serialization.dll>
#using <System.dll>

using namespace System::Security::Permissions;
[assembly:SecurityPermissionAttribute(SecurityAction::RequestMinimum, SkipVerification=false)];

using namespace System;
interface class IOrderTaker;
interface class IOrderTakerChannel;
ref class OrderTakerClient;


[System::CodeDom::Compiler::GeneratedCodeAttribute(L"System.ServiceModel", L"3.0.0.0"), 
System::ServiceModel::ServiceContractAttribute(Namespace=L"http://Microsoft.ServiceModel.Samples", ConfigurationName=L"IOrderTaker", 
SessionMode=System::ServiceModel::SessionMode::Required)]
public interface class IOrderTaker
{
    
    [System::ServiceModel::OperationContractAttribute(IsOneWay=true, Action=L"http://Microsoft.ServiceModel.Samples/IOrderTaker/OpenPurchaseOrder")]
    System::Void OpenPurchaseOrder(System::String^  customerId);
    
    [System::ServiceModel::OperationContractAttribute(IsOneWay=true, Action=L"http://Microsoft.ServiceModel.Samples/IOrderTaker/AddProductLineItem")]
    System::Void AddProductLineItem(System::String^  productId, System::Int32 quantity);
    
    [System::ServiceModel::OperationContractAttribute(IsOneWay=true, Action=L"http://Microsoft.ServiceModel.Samples/IOrderTaker/EndPurchaseOrder")]
    System::Void EndPurchaseOrder();
};

[System::CodeDom::Compiler::GeneratedCodeAttribute(L"System.ServiceModel", L"3.0.0.0")]
public interface class IOrderTakerChannel : public IOrderTaker, public System::ServiceModel::IClientChannel
{
};

[System::Diagnostics::DebuggerStepThroughAttribute, 
System::CodeDom::Compiler::GeneratedCodeAttribute(L"System.ServiceModel", L"3.0.0.0")]
public ref class OrderTakerClient : public System::ServiceModel::ClientBase<IOrderTaker^ >, public IOrderTaker
{
    
    public: OrderTakerClient();
    public: OrderTakerClient(System::String^  endpointConfigurationName);
    public: OrderTakerClient(System::String^  endpointConfigurationName, System::String^  remoteAddress);
    public: OrderTakerClient(System::String^  endpointConfigurationName, System::ServiceModel::EndpointAddress^  remoteAddress);
    public: OrderTakerClient(System::ServiceModel::Channels::Binding^  binding, System::ServiceModel::EndpointAddress^  remoteAddress);
    public: virtual System::Void OpenPurchaseOrder(System::String^  customerId) sealed;
    
    public: virtual System::Void AddProductLineItem(System::String^  productId, System::Int32 quantity) sealed;
    
    public: virtual System::Void EndPurchaseOrder() sealed;
};







inline OrderTakerClient::OrderTakerClient()
{
}

inline OrderTakerClient::OrderTakerClient(System::String^  endpointConfigurationName) : 
        System::ServiceModel::ClientBase<IOrderTaker^ >(endpointConfigurationName)
{
}

inline OrderTakerClient::OrderTakerClient(System::String^  endpointConfigurationName, System::String^  remoteAddress) : 
        System::ServiceModel::ClientBase<IOrderTaker^ >(endpointConfigurationName, remoteAddress)
{
}

inline OrderTakerClient::OrderTakerClient(System::String^  endpointConfigurationName, System::ServiceModel::EndpointAddress^  remoteAddress) : 
        System::ServiceModel::ClientBase<IOrderTaker^ >(endpointConfigurationName, remoteAddress)
{
}

inline OrderTakerClient::OrderTakerClient(System::ServiceModel::Channels::Binding^  binding, System::ServiceModel::EndpointAddress^  remoteAddress) : 
        System::ServiceModel::ClientBase<IOrderTaker^ >(binding, remoteAddress)
{
}

inline System::Void OrderTakerClient::OpenPurchaseOrder(System::String^  customerId)
{
    __super::Channel->OpenPurchaseOrder(customerId);
}

inline System::Void OrderTakerClient::AddProductLineItem(System::String^  productId, System::Int32 quantity)
{
    __super::Channel->AddProductLineItem(productId, quantity);
}

inline System::Void OrderTakerClient::EndPurchaseOrder()
{
    __super::Channel->EndPurchaseOrder();
}
