using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Input;
using System.Windows.Media.Effects;

namespace SDKSample
{

    public partial class BlurInteractiveExample : Page
    {

        // Add Bevel effect.
        void ChangeSelection(object sender, RoutedEventArgs args)
        {
            ComboBox cb = (ComboBox)sender;
            ComboBoxItem cbi = (ComboBoxItem)cb.SelectedValue;

            string s = cbi.Content.ToString();
            if(s == "Box")
            {
                InteractiveEffect.KernelType = KernelType.Box;
            }
            else if(s == "Gaussian")
            {
                InteractiveEffect.KernelType = KernelType.Gaussian;
            }
        }

    }
}