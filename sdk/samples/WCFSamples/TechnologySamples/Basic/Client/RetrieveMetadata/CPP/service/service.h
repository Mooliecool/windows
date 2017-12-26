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
        [System::ServiceModel::OperationContract]
        double Add(double n1, double  n2);
        [System::ServiceModel::OperationContract]
        double Divide(double n1, double  n2);
        [System::ServiceModel::OperationContract]
        double Multiply(double n1, double  n2);
        [System::ServiceModel::OperationContract]
        double Subtract(double n1, double  n2);
    };

    [System::ServiceModel::ServiceBehavior]
    public ref class CalculatorService : public ICalculator
    {
    public: 
        virtual double Add(double n1, double  n2)
        {
            return n1 + n2;
        }

        virtual double Divide(double n1, double  n2)
        {
            return n1 / n2;
        }

        virtual double Multiply(double n1, double  n2)
        {
            return n1 * n2;
        }

        virtual double Subtract(double n1, double  n2)
        {
            return n1 - n2;
        }
    };
}
}
}
