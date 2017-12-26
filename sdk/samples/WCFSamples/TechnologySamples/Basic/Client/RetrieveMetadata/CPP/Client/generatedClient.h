#pragma once

#using <mscorlib.dll>
#using <System.ServiceModel.dll>
#using <System.Runtime.Serialization.dll>
#using <System.dll>

using namespace System::Security::Permissions;
[assembly:SecurityPermissionAttribute(SecurityAction::RequestMinimum, SkipVerification=false)];

using namespace System;
interface class ICalculator;
interface class ICalculatorChannel;
ref class CalculatorClient;


[System::CodeDom::Compiler::GeneratedCodeAttribute(L"System.ServiceModel", L"3.0.0.0"), 
System::ServiceModel::ServiceContractAttribute(Namespace=L"http://Microsoft.ServiceModel.Samples", ConfigurationName=L"ICalculator")]
public interface class ICalculator
{
    
    [System::ServiceModel::OperationContractAttribute(Action=L"http://Microsoft.ServiceModel.Samples/ICalculator/Add", ReplyAction=L"http://Microsoft.ServiceModel.Samples/ICalculator/AddResponse")]
    System::Double Add(System::Double n1, System::Double n2);
    
    [System::ServiceModel::OperationContractAttribute(Action=L"http://Microsoft.ServiceModel.Samples/ICalculator/Divide", ReplyAction=L"http://Microsoft.ServiceModel.Samples/ICalculator/DivideResponse")]
    System::Double Divide(System::Double n1, System::Double n2);
    
    [System::ServiceModel::OperationContractAttribute(Action=L"http://Microsoft.ServiceModel.Samples/ICalculator/Multiply", ReplyAction=L"http://Microsoft.ServiceModel.Samples/ICalculator/MultiplyResponse")]
    System::Double Multiply(System::Double n1, System::Double n2);
    
    [System::ServiceModel::OperationContractAttribute(Action=L"http://Microsoft.ServiceModel.Samples/ICalculator/Subtract", ReplyAction=L"http://Microsoft.ServiceModel.Samples/ICalculator/SubtractResponse")]
    System::Double Subtract(System::Double n1, System::Double n2);
};

[System::CodeDom::Compiler::GeneratedCodeAttribute(L"System.ServiceModel", L"3.0.0.0")]
public interface class ICalculatorChannel : public ICalculator, public System::ServiceModel::IClientChannel
{
};

[System::Diagnostics::DebuggerStepThroughAttribute, 
System::CodeDom::Compiler::GeneratedCodeAttribute(L"System.ServiceModel", L"3.0.0.0")]
public ref class CalculatorClient : public System::ServiceModel::ClientBase<ICalculator^ >, public ICalculator
{
    
    public: CalculatorClient();
    public: CalculatorClient(System::String^  endpointConfigurationName);
    public: CalculatorClient(System::String^  endpointConfigurationName, System::String^  remoteAddress);
    public: CalculatorClient(System::String^  endpointConfigurationName, System::ServiceModel::EndpointAddress^  remoteAddress);
    public: CalculatorClient(System::ServiceModel::Channels::Binding^  binding, System::ServiceModel::EndpointAddress^  remoteAddress);
    public: virtual System::Double Add(System::Double n1, System::Double n2) sealed;
    
    public: virtual System::Double Divide(System::Double n1, System::Double n2) sealed;
    
    public: virtual System::Double Multiply(System::Double n1, System::Double n2) sealed;
    
    public: virtual System::Double Subtract(System::Double n1, System::Double n2) sealed;
};








inline CalculatorClient::CalculatorClient()
{
}

inline CalculatorClient::CalculatorClient(System::String^  endpointConfigurationName) : 
        System::ServiceModel::ClientBase<ICalculator^ >(endpointConfigurationName)
{
}

inline CalculatorClient::CalculatorClient(System::String^  endpointConfigurationName, System::String^  remoteAddress) : 
        System::ServiceModel::ClientBase<ICalculator^ >(endpointConfigurationName, remoteAddress)
{
}

inline CalculatorClient::CalculatorClient(System::String^  endpointConfigurationName, System::ServiceModel::EndpointAddress^  remoteAddress) : 
        System::ServiceModel::ClientBase<ICalculator^ >(endpointConfigurationName, remoteAddress)
{
}

inline CalculatorClient::CalculatorClient(System::ServiceModel::Channels::Binding^  binding, System::ServiceModel::EndpointAddress^  remoteAddress) : 
        System::ServiceModel::ClientBase<ICalculator^ >(binding, remoteAddress)
{
}

inline System::Double CalculatorClient::Add(System::Double n1, System::Double n2)
{
    return __super::Channel->Add(n1, n2);
}

inline System::Double CalculatorClient::Divide(System::Double n1, System::Double n2)
{
    return __super::Channel->Divide(n1, n2);
}

inline System::Double CalculatorClient::Multiply(System::Double n1, System::Double n2)
{
    return __super::Channel->Multiply(n1, n2);
}

inline System::Double CalculatorClient::Subtract(System::Double n1, System::Double n2)
{
    return __super::Channel->Subtract(n1, n2);
}
