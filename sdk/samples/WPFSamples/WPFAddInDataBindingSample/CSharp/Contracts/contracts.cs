using System.AddIn.Contract; // IContract, INativeHandleContract
using System.AddIn.Pipeline; // AddInContractAttribute
using System.ComponentModel; // PropertyChangedEventHandler

namespace Contracts
{
    [AddInContract]
    public interface IAddInContract : IContract
    {
        INativeHandleContract GetAddInUI(IPersonContract person);
    }

    public interface IPersonContract : IContract
    {
        // Name "property"
        string GetName();
        void SetName(string value);

        // Age "property"
        int GetAge();
        void SetAge(int value);

        IPropertyChangedNotifier PropertyChangedNotifier { get; set; }
    }

    public interface IPropertyChangedNotifier: IContract
    {
        event PropertyChangedEventHandler PropertyChanged;
        void PropertyChangeNotification(string propertyName);
    }
}