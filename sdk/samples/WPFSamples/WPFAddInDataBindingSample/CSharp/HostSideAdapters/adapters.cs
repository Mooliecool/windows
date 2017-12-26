using System.AddIn.Contract; // INativeHandleContract
using System.AddIn.Pipeline; // HostAdapterAttribute, AddInAdapterAttribute, ContractHandle, FrameworkElementAdapters, ContractBase
using System.ComponentModel; // PropertyChangedEventHandler, PropertyChangedEventArgs
using System.Windows; // FrameworkElement

using Contracts; // IAddInContract, IPersonContract
using HostViews; // IAddInView, IPersonView

namespace HostSideAdapters
{
    [HostAdapter]
    public class AddIn_ContractToViewHostSideAdapter : IAddInView
    {
        IAddInContract addInContract;
        ContractHandle contractHandle;

        public AddIn_ContractToViewHostSideAdapter(IAddInContract addInContract)
        {
            this.addInContract = addInContract;
            this.contractHandle = new ContractHandle(addInContract);
        }

        public FrameworkElement GetAddInUI(IPersonView personView)
        {
            Person_ViewToContractHostSideAdapter personContract = new Person_ViewToContractHostSideAdapter(personView);

            INativeHandleContract addInUIHwnd = this.addInContract.GetAddInUI(personContract);
            FrameworkElement addInUI = FrameworkElementAdapters.ContractToViewAdapter(addInUIHwnd);
            return addInUI;
        }
    }

    [AddInAdapter]
    public class Person_ViewToContractHostSideAdapter : ContractBase, IPersonContract
    {
        IPersonView personView;

        public Person_ViewToContractHostSideAdapter(IPersonView personView)
        {
            this.personView = personView;

            // Detect when the host UI that is bound to the person view instance
            // is edited ie INotifyPropertyChanged.PropertyChanged is raised, so 
            // we can let the add-in's person view be updated as well
            this.personView.PropertyChanged += personView_PropertyChanged;
        }

        public string GetName() 
        { 
            return this.personView.Name; 
        }

        public void SetName(string value)
        {
            this.personView.Name = value;
        }

        public int GetAge() 
        { 
            return this.personView.Age; 
        }

        public void SetAge(int value)
        {
            this.personView.Age = value;
        }

        // Handles when the Person data object is changed by the UI that is bound to it,
        // so we need to communicate the changes to the add-in side
        //public event PropertyChanged PropertyChanged;
        private void personView_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (this.propertyChangedNotifier != null)
            {
                this.propertyChangedNotifier.PropertyChangeNotification(e.PropertyName);
            }
        }

        IPropertyChangedNotifier propertyChangedNotifier;
        public IPropertyChangedNotifier PropertyChangedNotifier
        {
            get { return this.propertyChangedNotifier; }
            set { this.propertyChangedNotifier = value; }
        }
    }
}
