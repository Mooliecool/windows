//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Text;

namespace NumberUtilities 
{
    static class NumberConverter 
    {
        // <summary>
        // Convert a number to set of English words, e.g.
        // "-2452345.203909" =>
        // "negative two million, four hundred fifty-two thousand, three hundred forty-five point two zero three nine zero nine"
        // </summary>
        static string[] _onesAndTeens = { "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen", "seventeen", "eighteen", "nineteen" };
        static string[] _tens = { "", "", "twenty", "thirty", "forty", "fifty", "sixty", "seventy", "eighty", "ninety" };
        static string[] _thousands = { "", "thousand", "million", "billon", "trillion", "quadrillion" };

        public static string ConvertNumberToWords(double number) 
        {
            // special case zero
            if (number == 0) 
            { 
                return "zero"; 
            }

            // otherwise, build the English string
            StringBuilder english = new StringBuilder();

            // handle the whole part first
            long wholePart = (long) Math.Truncate(number);
            bool negative = wholePart < 0;
            if (negative) 
            { 
                wholePart = 0 - wholePart; 
            }

            int thousand = 0;
            while (wholePart != 0) 
            {
                long threeDigits = wholePart % 1000;
                StringBuilder threeDigitsEnglish = new StringBuilder();

                if (threeDigits > 99) 
                {
                    // one hundred, two hundred, etc.
                    threeDigitsEnglish.AppendFormat("{0} hundred", _onesAndTeens[threeDigits / 100]);
                    threeDigits %= 100;
                    if (threeDigits != 0) 
                    { 
                        threeDigitsEnglish.Append(' '); 
                    }
                }

                if (threeDigits >= _onesAndTeens.Length) 
                {
                    // twenty, thirty, etc.
                    threeDigitsEnglish.Append(_tens[threeDigits / 10]);
                    threeDigits %= 10;

                    // twenty-, thirty-, etc.
                    if (threeDigits != 0) 
                    { 
                        threeDigitsEnglish.Append('-'); 
                    }
                }

                if (threeDigits > 0) 
                {
                    // one, two, ..., eighteen, nineteen, etc.
                    threeDigitsEnglish.Append(_onesAndTeens[threeDigits]);
                }

                // append thousands
                if (threeDigitsEnglish.Length != 0) 
                {
                    // * thousand, * million, etc.
                    if (threeDigitsEnglish.Length != 0) 
                    { 
                        threeDigitsEnglish.Append(' '); 
                    }
                    threeDigitsEnglish.Append(_thousands[thousand]);
                }

                if (english.Length != 0) 
                { 
                    threeDigitsEnglish.Append(", "); 
                }
                english.Insert(0, threeDigitsEnglish);

                ++thousand;
                wholePart /= 1000;
            }

            // handle the decimal part second
            StringBuilder decimalEnglish = new StringBuilder();
            string numberString = number.ToString("N99");
            int dot = numberString.IndexOf('.');
            bool includeZeros = false;
            for (int i = numberString.Length - 1; i != dot; --i) 
            {
                char digit = numberString[i];
                if ((digit == '0') && !includeZeros) 
                { 
                    continue; 
                }
                includeZeros = true;
                if (decimalEnglish.Length != 0) 
                { 
                    decimalEnglish.Insert(0, ' '); 
                }
                decimalEnglish.Insert(0, _onesAndTeens[digit - '0']);
            }

            if (decimalEnglish.Length != 0) 
            {
                english.AppendFormat("point {0}", decimalEnglish);
            }

            // handle the negative part last
            if (negative) 
            { 
                english.Insert(0, "negative "); 
            }

            return english.ToString();
        }
    }
}
