using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Data;
using System.Windows.Media;
using System.Windows.Media.Converters;
using System.Windows.Media.Imaging; 

namespace SDKSample
{
    public partial class Window1 : Window
    {

        public Window1()
        {
            InitializeComponent();
        }

        private void UpDownFunctions(object sender, RoutedEventArgs e) {
	        
            string currentID = ((Button)sender).Name;

            switch (currentID)
            {
			    case "btPageDown":
                    tbNavigate.PageDown();
				    break;
			    case "btPageUp":
                    tbNavigate.PageUp();
				    break;
			    case "btLineDown":
                    tbNavigate.LineDown();
				    break;
			    case "btLineUp":
                    tbNavigate.LineUp();
				    break;
			    case "btScrollToEnd":
                    tbNavigate.ScrollToEnd();
				    break;
                case "btScrollToHome":
                    tbNavigate.ScrollToHome();
				    break;
			    default:
				    break;
            } // switch (currentID)

        } // UpDownFunctions()

		private void RLFunctions(object sender, RoutedEventArgs e)
		{
			string currentID = ((Button)sender).Name;

            switch (currentID)
    		{
				case "btPageRight":
                    tbNavigate.PageRight();
					break;
				case "btPageLeft":
                    tbNavigate.PageLeft();
					break;
				case "btLineRight":
                    tbNavigate.LineRight();
					break;
				case "btLineLeft":
                    tbNavigate.LineLeft();
					break;
				default:
					break;
            } // switch (currentID)

        } // RLFunctions()

    }
}