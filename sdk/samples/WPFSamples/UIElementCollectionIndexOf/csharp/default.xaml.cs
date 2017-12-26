namespace WCSamples {
    using System.Windows;
    using System.Windows.Controls;
    using System.Windows.Documents;

    public partial class IndexOfSample {
		private int c_counter = 0;
		void FindIndex(object sender, RoutedEventArgs e)
		{
			c_counter+=1;
			// Create a new Text element.
			TextBlock newText = new TextBlock();
			// Add this element to the UIElementCollection of the DockPanel element.
			MainDisplayPanel.Children.Add(newText);
			// Add a text node under the Text element. This text is displayed. 
			newText.Text = "New element #" + c_counter;
			DockPanel.SetDock(newText,Dock.Top);
			// Display the Index number of the new element.    
			TxtDisplay.Text = "The Index of the new element is " +  MainDisplayPanel.Children.IndexOf(newText);
		}
	}
}
