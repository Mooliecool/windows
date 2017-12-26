using System.Windows.Controls; // ValidationRule
using System.Globalization; // CultureInfo

namespace ExpenseItStandalone.Validation
{
    // Number Validation Rule
    public class NumberValidationRule : ValidationRule
    {
        bool isFixedLength;
        int length;

        /// <summary>
        /// Must the number be a specific length
        /// </summary>
        public bool IsFixedLength
        {
            get { return this.isFixedLength; }
            set { this.isFixedLength = value; }
        }

        /// <summary>
        /// The required length of the number, if IsFixedLength is true
        /// </summary>
        public int Length
        {
            get { return this.length; }
            set { this.length = value; }
        }

        public override ValidationResult Validate(object value, CultureInfo cultureInfo)
        {
            // Is the value a number?
            int number;
            if (!int.TryParse((string)value, out number))
            {
                string msg = string.Format("{0} is not a number.", value);
                return new ValidationResult(false, msg);
            }

            // Does value contain the number of digits specified by Length?
            if (this.isFixedLength && (((string)value).Length != this.length))
            {
                string msg = string.Format("Number must be {0} digits long.", this.length);
                return new ValidationResult(false, msg);
            }

            // Number is valid
            return new ValidationResult(true, null);
        }
    }
}
