#pragma once

using namespace System;
using namespace System::ServiceModel;


[System::Runtime::Serialization::DataContract(Namespace=L"http://Microsoft.ServiceModel.Samples/")]
public ref class AddRequestBody
{
public:
	AddRequestBody()
	{
	}

	AddRequestBody(int number)
	{
		this->number = number;
	}

	[System::Runtime::Serialization::DataMember(Order=0)]
	int number;
};

[System::ServiceModel::MessageContract(IsWrapped=false)]
public ref class AddRequest
{
public:
    AddRequest()
	{
		this->Body = gcnew AddRequestBody();
	}

    AddRequest(AddRequestBody ^ Body)
	{
		this->Body = Body;
	}

	[System::ServiceModel::MessageBodyMember(Name=L"Add", Namespace=L"http://Microsoft.ServiceModel.Samples/", ProtectionLevel=System::Net::Security::ProtectionLevel::EncryptAndSign, Order=0)]
    AddRequestBody ^ Body;
};

[System::Runtime::Serialization::DataContract(Namespace=L"http://Microsoft.ServiceModel.Samples/")]
public ref class AddResponseBody
{
public:
    AddResponseBody()
	{

	
	}

    AddResponseBody(int AddResult)
	{
		this->AddResult = AddResult;
	}

    [System::Runtime::Serialization::DataMember(Order=0)]
    int AddResult;
};

[System::ServiceModel::MessageContract(IsWrapped=false)]
public ref class AddResponse
{
public:
    AddResponse()
	{
		this->Body = gcnew AddResponseBody();
	}

    AddResponse(AddResponseBody ^ Body)
	{
		this->Body = Body;
	}

    [System::ServiceModel::MessageBodyMember(Name=L"AddResponse", Namespace=L"http://Microsoft.ServiceModel.Samples/", ProtectionLevel=System::Net::Security::ProtectionLevel::EncryptAndSign, Order=0)]
    AddResponseBody ^ Body;
};

[System::Runtime::Serialization::DataContract(Namespace=L"http://Microsoft.ServiceModel.Samples/")]
public ref class SubtractRequestBody
{
public:
    SubtractRequestBody()
	{
	
	}

    SubtractRequestBody(int number)
	{
		this->number = number;
	}

    [System::Runtime::Serialization::DataMember(Order=0)]
    int number;
};

[System::ServiceModel::MessageContract(IsWrapped=false)]
public ref class SubtractRequest
{
public:
    SubtractRequest()
	{
		this->Body = gcnew SubtractRequestBody();
	}

    SubtractRequest(SubtractRequestBody ^ Body)
	{
		this->Body = Body;
	}

    [System::ServiceModel::MessageBodyMember(Name=L"Subtract", Namespace=L"http://Microsoft.ServiceModel.Samples/", ProtectionLevel=System::Net::Security::ProtectionLevel::EncryptAndSign, Order=0)]
    SubtractRequestBody ^ Body;
};

[System::Runtime::Serialization::DataContract(Namespace=L"http://Microsoft.ServiceModel.Samples/")]
public ref class SubtractResponseBody
{
public:
    SubtractResponseBody()
	{
	
	}

    SubtractResponseBody(int SubtractResult)
	{
		this->SubtractResult = SubtractResult;
	}

    [System::Runtime::Serialization::DataMember(Order=0)]
    int SubtractResult;
};

[System::ServiceModel::MessageContract(IsWrapped=false)]
public ref class SubtractResponse
{
public:
    SubtractResponse()
	{
		this->Body = gcnew SubtractResponseBody();
	}

    SubtractResponse(SubtractResponseBody ^ Body)
	{
		this->Body = Body;
	}

    [System::ServiceModel::MessageBodyMember(Name=L"SubtractResponse", Namespace=L"http://Microsoft.ServiceModel.Samples/", ProtectionLevel=System::Net::Security::ProtectionLevel::EncryptAndSign, Order=0)]
    SubtractResponseBody ^ Body;
};


[System::CodeDom::Compiler::GeneratedCode(L"System.ServiceModel", L"3.0.0.0"), System::ServiceModel::ServiceContract(Namespace=L"http://Microsoft.ServiceModel.Samples/", SessionMode=System::ServiceModel::SessionMode::Required)]
public interface class ICalculatorService
{
public:
    [System::ServiceModel::OperationContract(Action=L"http://Microsoft.ServiceModel.Samples/ICalculatorService/Add", ReplyAction=L"http://Microsoft.ServiceModel.Samples/ICalculatorService/AddResponse")]
    AddResponse ^ Add(AddRequest ^ request);
    
	[System::ServiceModel::OperationContract(Action=L"http://Microsoft.ServiceModel.Samples/ICalculatorService/Subtract", ReplyAction=L"http://Microsoft.ServiceModel.Samples/ICalculatorService/SubtractResponse")]
    SubtractResponse ^ Subtract(SubtractRequest ^ request);
};

[System::CodeDom::Compiler::GeneratedCode(L"System.ServiceModel", L"3.0.0.0")]
public interface class ICalculatorServiceChannel : public ICalculatorService, public System::ServiceModel::IClientChannel
{
};

[System::CodeDom::Compiler::GeneratedCode(L"System.ServiceModel", L"3.0.0.0")]
public ref class CalculatorServiceClient : public System::ServiceModel::ClientBase<ICalculatorService ^>, public ICalculatorService
{
public:
	CalculatorServiceClient() : 
		System::ServiceModel::ClientBase<ICalculatorService ^>()
	{
	}

	CalculatorServiceClient(System::String ^ endpointConfigurationName) : 
		System::ServiceModel::ClientBase<ICalculatorService ^>( endpointConfigurationName )
	{
	}
    
	CalculatorServiceClient(System::ServiceModel::Channels::Binding ^ binding, System::ServiceModel::EndpointAddress ^ remoteAddress) : 
		System::ServiceModel::ClientBase<ICalculatorService ^>( binding, remoteAddress )
	{
	}
    
	CalculatorServiceClient(System::String ^ endpointConfigurationName, System::ServiceModel::EndpointAddress ^ remoteAddress) : 
		System::ServiceModel::ClientBase<ICalculatorService ^>( endpointConfigurationName, remoteAddress )
	{
	}
    
	CalculatorServiceClient(System::String ^ endpointConfigurationName, System::String ^ remoteAddress) : 
		System::ServiceModel::ClientBase<ICalculatorService ^>( endpointConfigurationName, remoteAddress )
	{
	}
    
	int Add(int number)
	{
        AddRequest ^ inValue = gcnew AddRequest();
        inValue->Body = gcnew AddRequestBody();
        inValue->Body->number = number;
        AddResponse ^ retVal = (static_cast<ICalculatorService^>(this))->Add(inValue);
        return retVal->Body->AddResult;
	}
    
	virtual AddResponse ^ Add(AddRequest ^ request)
	{
		return ClientBase<ICalculatorService^>::Channel->Add(request);
		
	}
    
	virtual SubtractResponse ^ Subtract(SubtractRequest ^ request)
	{
		return ClientBase<ICalculatorService^>::Channel->Subtract(request);
	}
    
	int Subtract(int number)
	{
        SubtractRequest ^ inValue = gcnew SubtractRequest();
        inValue->Body = gcnew SubtractRequestBody();
        inValue->Body->number = number;
        SubtractResponse ^ retVal = (static_cast<ICalculatorService^>(this))->Subtract(inValue);
        return retVal->Body->SubtractResult;
	}
};


