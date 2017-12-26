// service.h

#pragma once

#using <System.ServiceModel.dll>

using namespace System;

namespace Microsoft {
namespace ServiceModel {
namespace Samples {
    interface class ICalculatorDuplex;
    interface class ICalculatorDuplexCallback;

    [System::ServiceModel::ServiceContractAttribute(Namespace=L"http://Microsoft.ServiceModel.Samples", CallbackContract=ICalculatorDuplexCallback::typeid, SessionMode=System::ServiceModel::SessionMode::Required)]
    public interface class ICalculatorDuplex
    {
        [System::ServiceModel::OperationContract(IsOneWay=true)]
        void AddTo(double n);
        [System::ServiceModel::OperationContract(IsOneWay=true)]
        void Clear();
        [System::ServiceModel::OperationContract(IsOneWay=true)]
        void DivideBy(double n);
        [System::ServiceModel::OperationContract(IsOneWay=true)]
        void MultiplyBy(double n);
        [System::ServiceModel::OperationContract(IsOneWay=true)]
        void SubtractFrom(double n);
    };

    public interface class ICalculatorDuplexCallback
    {
        [System::ServiceModel::OperationContract(IsOneWay=true)]
        void Equals(double result);
        [System::ServiceModel::OperationContract(IsOneWay=true)]
        void Equation(System::String ^ eqn);
    };

    [System::ServiceModel::ServiceBehavior(InstanceContextMode=System::ServiceModel::InstanceContextMode::PerSession)]
    public ref class CalculatorService : public Microsoft::ServiceModel::Samples::ICalculatorDuplex
    {
    private: 
        Microsoft::ServiceModel::Samples::ICalculatorDuplexCallback ^ callback;
        System::String ^ equation;
        double result;

    public: 
        CalculatorService()
        {
            this->equation = "" + this->result;
            this->callback = System::ServiceModel::OperationContext::Current->GetCallbackChannel<Microsoft::ServiceModel::Samples::ICalculatorDuplexCallback ^>();
        }

        virtual void AddTo(double n)
        {
            this->result += n;
            this->equation = System::String::Concat(this->equation, L" + ", n);
            this->callback->Equals(this->result);
        }

        virtual void Clear()
        {
            this->callback->Equation(System::String::Concat(this->equation, L" = ", this->result));
            this->equation = "" + this->result;
        }

        virtual void DivideBy(double n)
        {
            this->result /= n;
            this->equation = System::String::Concat(this->equation, L" / ", n);
            this->callback->Equals(this->result);
        }

        virtual void MultiplyBy(double n)
        {
            this->result *= n;
            this->equation = System::String::Concat(this->equation, L" * ", n);
            this->callback->Equals(this->result);
        }

        virtual void SubtractFrom(double n)
        {
            this->result -= n;
            this->equation = System::String::Concat(this->equation, L" - ", n);
            this->callback->Equals(this->result);
        }
    };
}
}
}