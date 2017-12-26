// service.h

#pragma once

#using <System.ServiceModel.dll>

using namespace System;
using namespace System::ServiceModel;

namespace Microsoft {
namespace ServiceModel {
namespace Samples {
    interface class ICalculator;

    [ServiceContractAttribute(Namespace=L"http://Microsoft.ServiceModel.Samples")]
    public interface class ICalculator
    {
        [OperationContract()]
		Double Add(Double n1, Double n2);
        [OperationContract()]
		Double Subtract(Double n1, Double n2);
        [OperationContract()]
		Double Multiply(Double n1, Double n2);
        [OperationContract()]
		Double Divide(Double n1, Double n2);
    };

}
}
}