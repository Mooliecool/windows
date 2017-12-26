using System;
using System.Text.RegularExpressions;
using System.Windows.Controls; // ValidationRule
using System.Globalization; // CultureInfo

namespace ExpenseItStandalone.Validation
{
    // Email Validation Rule
    public class EmailValidationRule : ValidationRule
    {
        public override ValidationResult Validate(object value, CultureInfo cultureInfo)
        {
            // Is a valid email address?
            string pattern = @"\w+([-+.]\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*([,;]\s*\w+([-+.]\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*)*";
            if( !Regex.IsMatch((string)value, pattern) )
            {
                string msg = string.Format("{0} is not a valid email address.", value);
                return new ValidationResult(false, msg);
            }

            // Email address is valid
            return new ValidationResult(true, null);
        }
    }
}
