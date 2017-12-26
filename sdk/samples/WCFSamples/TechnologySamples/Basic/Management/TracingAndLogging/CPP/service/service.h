// service.h

#pragma once

#using <System.ServiceModel.dll>

using namespace System;

namespace Microsoft {
namespace ServiceModel {
namespace Samples {
    interface class ICalculator;

    [System::ServiceModel::ServiceContractAttribute(Namespace=L"http://Microsoft.ServiceModel.Samples")]
    public interface class ICalculator
    {
        [System::ServiceModel::OperationContract()]
		System::Double Add(System::Double n1, System::Double n2);

		[System::ServiceModel::OperationContract()]
		System::Double Subtract(System::Double n1, System::Double n2);

		[System::ServiceModel::OperationContract()]
		System::Double Multiply(System::Double n1, System::Double n2);
        
		[System::ServiceModel::OperationContract()]
		System::Double Divide(System::Double n1, System::Double n2);
    };

}
}
}