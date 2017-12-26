
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;

namespace SDKSample
{
 
    public partial class Window1 : Window
    {

        // This is a counter for the number of times the TextChanged fires
        // for the tbCountingChanges TextBox.
        private int uiChanges = 0;
        
        public Window1()
        {

            // Note that a call to InitializeComponent fires the TextChanged event when tbCountingChnages
            InitializeComponent();

        } // end Constructor

        // Event handler for TextChanged Event.
        private void textChangedEventHandler(object sender, TextChangedEventArgs args)
        {

            uiChanges++;
            tbCounterText.Text = uiChanges.ToString();

        } // end setChangedFlag
    }
}