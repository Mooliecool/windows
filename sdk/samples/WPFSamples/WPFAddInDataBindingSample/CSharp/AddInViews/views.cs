using System.AddIn.Pipeline; // AddInBaseAttribute
using System.ComponentModel; // INotifyPropertyChanged
using System.Windows; // FrameworkElement

namespace AddInViews
{
    [AddInBase]
    public interface IAddInView
    {
        FrameworkElement GetAddInUI(IPersonView person);
    }

    public interface IPersonView : INotifyPropertyChanged
    {
        string Name { get; set; }
        int Age { get; set; }        
    }        
}
