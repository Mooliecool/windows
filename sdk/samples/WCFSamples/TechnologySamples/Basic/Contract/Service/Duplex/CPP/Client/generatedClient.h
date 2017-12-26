#pragma once

#using <mscorlib.dll>
#using <System.ServiceModel.dll>
#using <System.Runtime.Serialization.dll>
#using <System.dll>

using namespace System::Security::Permissions;
[assembly:SecurityPermissionAttribute(SecurityAction::RequestMinimum, SkipVerification=false)];

using namespace System;
interface class ICalculatorDuplex;
interface class ICalculatorDuplexCallback;
interface class ICalculatorDuplexChannel;
ref class CalculatorDuplexClient;


[System::CodeDom::Compiler::GeneratedCodeAttribute(L"System.ServiceModel", L"3.0.0.0"), 
System::ServiceModel::ServiceContractAttribute(Namespace=L"http://Microsoft.ServiceModel.Samples", ConfigurationName=L"ICalculatorDuplex", 
CallbackContract=ICalculatorDuplexCallback::typeid, SessionMode=System::ServiceModel::SessionMode::Required)]
public interface class ICalculatorDuplex
{
    
    [System::ServiceModel::OperationContractAttribute(IsOneWay=true, Action=L"http://Microsoft.ServiceModel.Samples/ICalculatorDuplex/AddTo")]
    System::Void AddTo(System::Double n);
    
    [System::ServiceModel::OperationContractAttribute(IsOneWay=true, Action=L"http://Microsoft.ServiceModel.Samples/ICalculatorDuplex/Clear")]
    System::Void Clear();
    
    [System::ServiceModel::OperationContractAttribute(IsOneWay=true, Action=L"http://Microsoft.ServiceModel.Samples/ICalculatorDuplex/DivideBy")]
    System::Void DivideBy(System::Double n);
    
    [System::ServiceModel::OperationContractAttribute(IsOneWay=true, Action=L"http://Microsoft.ServiceModel.Samples/ICalculatorDuplex/MultiplyBy")]
    System::Void MultiplyBy(System::Double n);
    
    [System::ServiceModel::OperationContractAttribute(IsOneWay=true, Action=L"http://Microsoft.ServiceModel.Samples/ICalculatorDuplex/SubtractFrom")]
    System::Void SubtractFrom(System::Double n);
};

[System::CodeDom::Compiler::GeneratedCodeAttribute(L"System.ServiceModel", L"3.0.0.0")]
public interface class ICalculatorDuplexCallback
{
    
    [System::ServiceModel::OperationContractAttribute(IsOneWay=true, Action=L"http://Microsoft.ServiceModel.Samples/ICalculatorDuplex/Equals")]
    System::Void Equals(System::Double result);
    
    [System::ServiceModel::OperationContractAttribute(IsOneWay=true, Action=L"http://Microsoft.ServiceModel.Samples/ICalculatorDuplex/Equation")]
    System::Void Equation(System::String^  eqn);
};

[System::CodeDom::Compiler::GeneratedCodeAttribute(L"System.ServiceModel", L"3.0.0.0")]
public interface class ICalculatorDuplexChannel : public ICalculatorDuplex, public System::ServiceModel::IClientChannel
{
};

[System::Diagnostics::DebuggerStepThroughAttribute, 
System::CodeDom::Compiler::GeneratedCodeAttribute(L"System.ServiceModel", L"3.0.0.0")]
public ref class CalculatorDuplexClient : public System::ServiceModel::DuplexClientBase<ICalculatorDuplex^ >, public ICalculatorDuplex
{
    
    public: CalculatorDuplexClient(System::ServiceModel::InstanceContext^  callbackInstance);
    public: CalculatorDuplexClient(System::ServiceModel::InstanceContext^  callbackInstance, System::String^  endpointConfigurationName);
    public: CalculatorDuplexClient(System::ServiceModel::InstanceContext^  callbackInstance, System::String^  endpointConfigurationName, 
                System::String^  remoteAddress);
    public: CalculatorDuplexClient(System::ServiceModel::InstanceContext^  callbackInstance, System::String^  endpointConfigurationName, 
                System::ServiceModel::EndpointAddress^  remoteAddress);
    public: CalculatorDuplexClient(System::ServiceModel::InstanceContext^  callbackInstance, System::ServiceModel::Channels::Binding^  binding, 
                System::ServiceModel::EndpointAddress^  remoteAddress);
    public: virtual System::Void AddTo(System::Double n) sealed;
    
    public: virtual System::Void Clear() sealed;
    
    public: virtual System::Void DivideBy(System::Double n) sealed;
    
    public: virtual System::Void MultiplyBy(System::Double n) sealed;
    
    public: virtual System::Void SubtractFrom(System::Double n) sealed;
};












inline CalculatorDuplexClient::CalculatorDuplexClient(System::ServiceModel::InstanceContext^  callbackInstance) : 
        System::ServiceModel::DuplexClientBase<ICalculatorDuplex^ >(callbackInstance)
{
}

inline CalculatorDuplexClient::CalculatorDuplexClient(System::ServiceModel::InstanceContext^  callbackInstance, System::String^  endpointConfigurationName) : 
        System::ServiceModel::DuplexClientBase<ICalculatorDuplex^ >(callbackInstance, endpointConfigurationName)
{
}

inline CalculatorDuplexClient::CalculatorDuplexClient(System::ServiceModel::InstanceContext^  callbackInstance, System::String^  endpointConfigurationName, 
            System::String^  remoteAddress) : 
        System::ServiceModel::DuplexClientBase<ICalculatorDuplex^ >(callbackInstance, endpointConfigurationName, remoteAddress)
{
}

inline CalculatorDuplexClient::CalculatorDuplexClient(System::ServiceModel::InstanceContext^  callbackInstance, System::String^  endpointConfigurationName, 
            System::ServiceModel::EndpointAddress^  remoteAddress) : 
        System::ServiceModel::DuplexClientBase<ICalculatorDuplex^ >(callbackInstance, endpointConfigurationName, remoteAddress)
{
}

inline CalculatorDuplexClient::CalculatorDuplexClient(System::ServiceModel::InstanceContext^  callbackInstance, System::ServiceModel::Channels::Binding^  binding, 
            System::ServiceModel::EndpointAddress^  remoteAddress) : 
        System::ServiceModel::DuplexClientBase<ICalculatorDuplex^ >(callbackInstance, binding, remoteAddress)
{
}

inline System::Void CalculatorDuplexClient::AddTo(System::Double n)
{
    __super::Channel->AddTo(n);
}

inline System::Void CalculatorDuplexClient::Clear()
{
    __super::Channel->Clear();
}

inline System::Void CalculatorDuplexClient::DivideBy(System::Double n)
{
    __super::Channel->DivideBy(n);
}

inline System::Void CalculatorDuplexClient::MultiplyBy(System::Double n)
{
    __super::Channel->MultiplyBy(n);
}

inline System::Void CalculatorDuplexClient::SubtractFrom(System::Double n)
{
    __super::Channel->SubtractFrom(n);
}
