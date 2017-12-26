using System.AddIn.Contract; // INativeHandleContract
using System.AddIn.Pipeline; // AddInAdapterAttribute, ContractBase, FrameworkElementAdapters
using System.ComponentModel; // PropertyChangedEventHandler, PropertyChangedEventArgs
using System.Windows; //FrameworkElement

using AddInViews; // IAddInView, IPersonView
using Contracts; // IAddInContract, IPersonContract

using System;

namespace AddInSideAdapters
{
    [AddInAdapter]
    public class AddIn_ViewToContractAddInSideAdapter : ContractBase, IAddInContract
    {
        IAddInView addInView;

        public AddIn_ViewToContractAddInSideAdapter(IAddInView addInView)
        {
            this.addInView = addInView;
        }

        public INativeHandleContract GetAddInUI(IPersonContract personContract)
        {
            Person_ContractToViewAddInSideAdapter personView = new Person_ContractToViewAddInSideAdapter(personContract);
            FrameworkElement fe = this.addInView.GetAddInUI(personView);
            INativeHandleContract addInUIHwnd = FrameworkElementAdapters.ViewToContractAdapter(fe);
            return addInUIHwnd;
        }
    }

    public class PropertyChangedNotifier : ContractBase, IPropertyChangedNotifier
    {
        public event PropertyChangedEventHandler PropertyChanged;
        public void PropertyChangeNotification(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(null, new PropertyChangedEventArgs(propertyName));
            }
        }
    }

    [HostAdapter]
    public class Person_ContractToViewAddInSideAdapter : ContractBase, IPersonView
    {
        IPersonContract personContract;
        ContractHandle contractHandle;
        PropertyChangedNotifier notifier;

        public Person_ContractToViewAddInSideAdapter(IPersonContract personContract)
        {
            this.personContract = personContract;
            this.contractHandle = new ContractHandle(personContract);

            this.notifier = new PropertyChangedNotifier();
            this.notifier.PropertyChanged += notifier_PropertyChanged;
            this.personContract.PropertyChangedNotifier = this.notifier;
        }

        void notifier_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, e);
            }
        }

        public string Name
        {
            get { return this.personContract.GetName(); }
            set { this.personContract.SetName(value); }
        }
        public int Age
        {
            get { return this.personContract.GetAge(); }
            set { this.personContract.SetAge(value); }
        }

        public event PropertyChangedEventHandler PropertyChanged;
    }
}
