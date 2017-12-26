// ----------------------------------------------------------------------------
// Copyright (C) 2003-2005 Microsoft Corporation, All rights reserved.
// ----------------------------------------------------------------------------

using namespace System;
using namespace System::ServiceModel;
using namespace System::ServiceModel::Channels;
using namespace System::Diagnostics;
using namespace System::Configuration;

namespace Microsoft{
namespace ServiceModel{
namespace Samples
{
	 
	[System::ServiceModel::ServiceContract(SessionMode=System::ServiceModel::SessionMode::Required)]
	public interface class ISimplexSessionRouter
	{
		  [System::ServiceModel::OperationContract(IsOneWay=true, Action=L"*")]
		  void SimplexSessionRouterProcessMessage(System::ServiceModel::Channels::Message ^ message);
	};

	[System::ServiceModel::ServiceContract(SessionMode=System::ServiceModel::SessionMode::Required, CallbackContract=Microsoft::ServiceModel::Samples::ISimplexSessionRouter::typeid)]
	public interface class IDuplexSessionRouter
	{
		  [System::ServiceModel::OperationContract(IsOneWay=true, Action=L"*")]
		  void DuplexSessionRouterProcessMessage(System::ServiceModel::Channels::Message ^ message);
	};

	[System::ServiceModel::ServiceContract(SessionMode=System::ServiceModel::SessionMode::Allowed)]
	public interface class IRequestReplyDatagramRouter
	{
		  [System::ServiceModel::OperationContract(IsOneWay=false, Action=L"*", ReplyAction=L"*")]
		  System::ServiceModel::Channels::Message ^ RequestReplyDatagramRouterProcessMessage(System::ServiceModel::Channels::Message ^ message);
	};

	 
	[System::ServiceModel::ServiceContract(SessionMode=System::ServiceModel::SessionMode::Allowed)]
	public interface class ISimplexDatagramRouter
	{
		  [System::ServiceModel::OperationContract(IsOneWay=true, Action=L"*")]
		  void SimplexDatagramRouterProcessMessage(System::ServiceModel::Channels::Message ^ message);
	};

}
}
}
