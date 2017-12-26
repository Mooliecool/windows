namespace LogonScreen
{
    using System;
    using System.Collections;
    using System.IO;
    using System.Windows;
    using System.Windows.Controls;
    using System.Windows.Media;
    using System.Windows.Navigation;
    using System.Windows.Markup;
    using System.Windows.Input;
    using System.Windows.Media.Animation; 

    public partial class Default
    {
        private static bool resourceLoaded = false;
        ResourceDictionary _defaultTheme ;
        ResourceDictionary _lunaTheme;
        ResourceDictionary _toonsTheme ;
        ResourceDictionary _xBoxTheme;

        private void Init (object sender, System.EventArgs args)
        {           
            _defaultTheme = new Resources_Default();
            _lunaTheme = new Resources_Luna();
            _toonsTheme = new Resources_Toons();
            _xBoxTheme = new Resources_XBox();

            Application.Current.Resources = _defaultTheme;
        }

        private void OnMouseEnter(object sender, MouseEventArgs args)
        {
            resourceLoaded = true;
        }
           
        void ChangeUser (object sender, SelectionChangedEventArgs e)
        {
            if (!resourceLoaded)
                return;

            switch (UserTilesListBox.SelectedIndex)
            {
                case 0:
                    Application.Current.Resources = _defaultTheme;
                    break;

                case 1:
                    Application.Current.Resources = _lunaTheme;
                    break;

               case 2:
                   Application.Current.Resources = _xBoxTheme;
                   break;

                case 3:
                    Application.Current.Resources = _toonsTheme;
                    break; 
            }
        }
    }
}