using System;
using System.Windows.Data;

namespace DemoPMFeatures
{
    [ValueConversion(typeof(bool), typeof(string))]
    public class YesNoConverter :IValueConverter
    {
        #region IValueConverter Members

        public object Convert(
            object value, 
            Type targetType, 
            object parameter, 
            System.Globalization.CultureInfo culture)
        {
            if (targetType != typeof(string)) 
                return null;

            bool what = (bool) value;
            if (what == true)
                return "Yes";
            else
                return "No";
        }

        // We only support one way binding
        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }

        #endregion
    }
}
