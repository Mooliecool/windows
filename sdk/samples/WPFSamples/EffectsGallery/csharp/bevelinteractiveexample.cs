using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Input;
using System.Windows.Media.Effects;

namespace SDKSample
{

    public partial class BevelInteractiveExample : Page
    {

        void ChangeSelection(object sender, RoutedEventArgs args)
        {
            ComboBox cb = (ComboBox)sender;
            ComboBoxItem cbi = (ComboBoxItem)cb.SelectedValue;

            string s = cbi.Content.ToString();
            switch (s)
            {
                case "BulgedUp":
                    InteractiveEffect.EdgeProfile = EdgeProfile.BulgedUp;
                    break;
                case "CurvedIn":
                    InteractiveEffect.EdgeProfile = EdgeProfile.CurvedIn;
                    break;
                case "CurvedOut":
                    InteractiveEffect.EdgeProfile = EdgeProfile.CurvedOut;
                    break;
                case "Linear":
                    InteractiveEffect.EdgeProfile = EdgeProfile.Linear;
                    break;
            }
        }

    }
}