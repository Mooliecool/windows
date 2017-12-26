// This is the main DLL file.

#include "service.h"
namespace Microsoft {
namespace ServiceModel {
namespace Samples {

    public ref class CalculatorService : public Microsoft::ServiceModel::Samples::ICalculator
    {
    public: 
        CalculatorService()
        {
        }

        virtual double Add(double n1, double n2)
        {
            return n1 + n2;
        }

        virtual double Subtract(double n1, double n2)
        {
            return n1 - n2;
        }

        virtual double Multiply(double n1, double n2)
        {
            return n1 * n2;
        }

        virtual double Divide(double n1, double n2)
        {
            return n1 / n2;
        }
    };
}
}
}