//This is a list of commonly used namespaces for a window.
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Navigation;


namespace Grid_Convert_Value_Csharp
{
	public partial class Window1 : Window
	{
		private void ChangeMargin(object sender, SelectionChangedEventArgs args)
		{
			ListBoxItem li = ((sender as ListBox).SelectedItem as ListBoxItem);
            ThicknessConverter myThicknessConverter = new ThicknessConverter();
            Thickness th1 = (Thickness)myThicknessConverter.ConvertFromString(li.Content.ToString());
            text1.Margin = th1;
            String st1 = (String)myThicknessConverter.ConvertToString(text1.Margin);
            gridVal.Text = "The Margin property is set to " + st1 +".";
		}

	}
}