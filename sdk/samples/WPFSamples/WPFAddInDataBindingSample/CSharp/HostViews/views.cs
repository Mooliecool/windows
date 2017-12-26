using System.AddIn.Pipeline; // AddInBaseAttribute
using System.ComponentModel; // INotifyPropertyChanged, PropertyChangedEventHandler, PropertyChangedEventArgs
using System.Windows; // FrameworkElement

namespace HostViews
{
    public interface IAddInView
    {
        FrameworkElement GetAddInUI(IPersonView personView);
    }

    [AddInBase]
    // INotifyPropertyChanged allows changes made to person object on add-in 
    // side to propagate to host UI bound to instances of IPersonView
    public interface IPersonView : INotifyPropertyChanged
    {
        string Name { get; set; }
        int Age { get; set; }
    }

    // Concrete implementation of IPersonView for host]
    public class Person : IPersonView
    {
        private string name;
        private int age;

        public Person() { }
        public Person(string name, int age)
        {
            this.name = name;
            this.age = age;
        }

        public string Name
        {
            get { return this.name; }
            set
            {
                this.name = value;
                OnPropertyChanged("Name");
            }
        }
        public int Age
        {
            get { return this.age; }
            set
            {
                this.age = value;
                OnPropertyChanged("Age");
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void OnPropertyChanged(string propertyName)
        {
            // Raise PropertyChanged event if handler(s) are registered with it
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }
    }
}
