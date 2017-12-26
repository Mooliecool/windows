

#pragma once



using namespace System::Security::Permissions;
[assembly:SecurityPermissionAttribute(SecurityAction::RequestMinimum, SkipVerification=false)];

using namespace System;
interface class IEchoService;
interface class IEchoServiceChannel;


[System::CodeDom::Compiler::GeneratedCodeAttribute(L"System.ServiceModel", L"3.0.0.0"), 
System::ServiceModel::ServiceContractAttribute(Namespace=L"http://Microsoft.ServiceModel.Samples/", ConfigurationName=L"IEchoService")]
public interface class IEchoService
{
    
    [System::ServiceModel::OperationContractAttribute(Action=L"http://Microsoft.ServiceModel.Samples/IEchoService/Echo", ReplyAction=L"http://Microsoft.ServiceModel.Samples/IEchoService/EchoResponse")]
    System::String^  Echo(System::String^  message);
};

[System::CodeDom::Compiler::GeneratedCodeAttribute(L"System.ServiceModel", L"3.0.0.0")]
public interface class IEchoServiceChannel : public IEchoService, public System::ServiceModel::IClientChannel
{
};


[System::CodeDom::Compiler::GeneratedCode(L"System.ServiceModel", L"3.0.0.0")]
public ref class EchoServiceClient : public System::ServiceModel::ClientBase<IEchoService ^>, public IEchoService
{
public: 
	EchoServiceClient() :
		System::ServiceModel::ClientBase<IEchoService ^>()	
	{
	}

	EchoServiceClient(System::String ^ endpointConfigurationName) : 
		System::ServiceModel::ClientBase<IEchoService ^>( endpointConfigurationName )
	{
	}

	EchoServiceClient(System::ServiceModel::Channels::Binding ^ binding, System::ServiceModel::EndpointAddress ^ remoteAddress) : 
		System::ServiceModel::ClientBase<IEchoService ^>( binding, remoteAddress)
	{

	}
    
	EchoServiceClient(System::String ^ endpointConfigurationName, System::ServiceModel::EndpointAddress ^ remoteAddress) : 
		System::ServiceModel::ClientBase<IEchoService ^>( endpointConfigurationName, remoteAddress )
	{
	}
    
	EchoServiceClient(System::String ^ endpointConfigurationName, System::String ^ remoteAddress) : 
		System::ServiceModel::ClientBase<IEchoService ^>( endpointConfigurationName, remoteAddress )
	{
	}
    
	virtual System::String ^ Echo(System::String ^ message)
	{
		return ClientBase<IEchoService^>::Channel->Echo( message );
	}
};

