//------------------------------------------------------------------------------
// <copyright file="stringformat.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>                                                                
//------------------------------------------------------------------------------

using System;
using System.IO;
using System.Globalization;

public enum Color
{
  Red = 25,
  Blue,
  Green
}//enum Color

/// <summary>
/// Class with Main entry point.
/// </summary>
public class stringFormat
{

  /// <summary>
  /// Main entry point.
  /// </summary>
  /// <param name="args"></param>
  public static void Main(string [] args)
  {
    // DateTime built in formats

    Console.WriteLine("DateTime build-in formats using current date and time.");
    Console.WriteLine("");
    Console.WriteLine ("{0}\t{1}" ,"Code", "Format");
    Console.WriteLine ("{0}\t{1}" ,"----", "------");

    DateTime d = DateTime.Now;

    PrintFormat(d, "d");
    PrintFormat(d, "D");
    PrintFormat(d, "f");
    PrintFormat(d, "F");
    PrintFormat(d, "g");
    PrintFormat(d, "G");
    PrintFormat(d, "m");
    PrintFormat(d, "r");
    PrintFormat(d, "s");
    PrintFormat(d, "t");
    PrintFormat(d, "T");
    PrintFormat(d, "u");
    PrintFormat(d, "U");
    PrintFormat(d, "y");

    TryUserFormat(d);

    //DateTime custom format patterns
    
/*
d	        The day of the month. Single-digit days will not have a leading zero.
dd	      The day of the month. Single-digit days will have a leading zero.
ddd	      The abbreviated name of the day of the week, as defined in AbbreviatedDayNames.
dddd	    The full name of the day of the week, as defined in DayNames.
M	        The numeric month. Single-digit months will not have a leading zero.
MM	      The numeric month. Single-digit months will have a leading zero.
MMM	      The abbreviated name of the month, as defined in AbbreviatedMonthNames.
MMMM	    The full name of the month, as defined in MonthNames.
y	        The year without the century. If the year without the century is less than 10, the year is displayed with no leading zero.
yy	      The year without the century. If the year without the century is less than 10, the year is displayed with a leading zero.
yyyy	    The year in four digits, including the century.
gg	      The period or era. This pattern is ignored if the date to be formatted does not have an associated period or era string.
h	        The hour in a 12-hour clock. Single-digit hours will not have a leading zero.
hh	      The hour in a 12-hour clock. Single-digit hours will have a leading zero.
H	        The hour in a 24-hour clock. Single-digit hours will not have a leading zero.
HH	      The hour in a 24-hour clock. Single-digit hours will have a leading zero.
m	        The minute. Single-digit minutes will not have a leading zero.
mm	      The minute. Single-digit minutes will have a leading zero.
s	        The second. Single-digit seconds will not have a leading zero.
ss	      The second. Single-digit seconds will have a leading zero.
f	        The fraction of a second in single-digit precision. The remaining digits are truncated.
ff	      The fraction of a second in double-digit precision. The remaining digits are truncated.
fff	      The fraction of a second in three-digit precision. The remaining digits are truncated.
ffff	    The fraction of a second in four-digit precision. The remaining digits are truncated.
fffff	    The fraction of a second in five-digit precision. The remaining digits are truncated.
ffffff	  The fraction of a second in six-digit precision. The remaining digits are truncated.
fffffff	  The fraction of a second in seven-digit precision. The remaining digits are truncated.
t	        The first character in the AM/PM designator defined in AMDesignator or PMDesignator.
tt	      The AM/PM designator defined in AMDesignator or PMDesignator.
z	        The time zone offset ("+" or "-" followed by the hour only). Single-digit hours will not have a leading zero. For example, Pacific Standard Time is "-8".
zz	      The time zone offset ("+" or "-" followed by the hour only). Single-digit hours will have a leading zero. For example, Pacific Standard Time is "-08".
zzz	      The full time zone offset ("+" or "-" followed by the hour and minutes). Single-digit hours and minutes will have leading zeros. For example, Pacific Standard Time is "-08:00".
:	        The default time separator defined in TimeSeparator.
/	        The default date separator defined in DateSeparator.
% c       Where c is a format pattern if used alone. The "%" character can be omitted if the format pattern is combined with literal characters or other format patterns.
\ c	      Where c is any character. Displays the character literally. To display the backslash character, use "\\".
*/

    Console.WriteLine("DateTime Formatting:  user-defined custom format patterns");
    Console.WriteLine("{0}\t{1}" ,"Code", "Format");
    Console.WriteLine("{0}\t{1}" ,"----", "------");
    PrintFormat(d, "ddd");
    PrintFormat(d, "MMMM dd, yyyy");
    PrintFormat(d, "gg");
    PrintFormat(d, "hh");
    PrintFormat(d, "HH");
    PrintFormat(d, "f");
    PrintFormat(d, "ffff");

    TryUserFormat(d);

    Console.WriteLine("-----------------------------------------------------------------------------");
    Console.WriteLine("Using Modified DateTimeFormatInfo provider\n");
    Console.WriteLine("AbbreviatedDayNames = new string [] {Sund, Mond, Tues, Weds, Thur, Frid, Satu}");
    DateTimeFormatInfo dtfi = new DateTimeFormatInfo();
    dtfi.AbbreviatedDayNames = 
               new string [] {"Sund", "Mond", "Tues", "Weds", "Thur", "Frid", "Satu"};
    PrintFormat(d, "ddd", dtfi);
    Console.WriteLine("------------------------------------------------------------------------------\n");

    /*
    Alignment
    To specify the alignment for a formatted string in the
    composite formatting scheme, you can place a comma after the
    parameter specifier and before the colon used to separate
    the parameter specifier and the format specifier. A number
    indicating the field width should follow the comma. A
    negative number indicates that this parameter should be
    right-aligned within that field, and a positive number
    indicates it should be left-aligned. For right-aligned
    fields, the alignment is calculated so that the last field
    in the new alignment is the last character in the string
    being aligned. For left-aligned fields, the alignment is
    calculated so that the first field in the new alignment is
    the first character of the string being aligned. Alignment
    is always achieved using white spaces. The text is never
    shortened to meet the specified width; instead, the width is
    expanded to allow the full text to be displayed. Therefore,
    in order for alignment to be meaningful, the number you pass
    should be greater than the length of the original string you
    are aligning.
    The following code examples demonstrate the use of alignment
    in formatting. The arguments that are formatted are placed
    between '|' characters to highlight the resulting alignment.
    */   
    // Formatting a table
    Console.WriteLine("Composite formatting in a table");
    string [] names = {"Mary-Beth", "Aunt Alma", "Sue", "My Really Long Name", "Matt"};
    for (int k = 0; k < 5; k++)
    {
      Console.WriteLine ("| {0,-4}{1,-20} |", k, names[k]);
    }
    double [] nums = {123.456, 888.999, -12.33333, 333.45678, -99.77777};
    Console.WriteLine();
    for (int k = 0; k < 5; k++)
    {
      Console.WriteLine ("| {0,-4:d}{1,-20:e9} |", k, nums[k]);
    }
    Console.WriteLine();


    // Enum formatting
    Console.WriteLine("Enum Formatting");
    Console.WriteLine 
      (
      "Name: {0}, Enum Value: {1}, Enum Value Hex: {2}", 
      Color.Green.ToString("G"), 
      Color.Green.ToString("D"), 
      Color.Green.ToString("X")
      );
    Console.WriteLine();

    //Number standard formats
    double num = System.Math.PI * -3.0;

    Console.WriteLine("Numeric Formatting:  Predefined floating point formats");
    Console.WriteLine("Double data is System.Math.PI * -3.0: {0}", num);
    Console.WriteLine("{0}\t{1}" ,"Code", "Format");
    Console.WriteLine("{0}\t{1}" ,"----", "------");
    PrintFormat(num, "c");
    PrintFormat(num, "c4");
    PrintFormat(num, "e");
    PrintFormat(num, "e4");
    PrintFormat(num, "e10");
    PrintFormat(num, "f");
    PrintFormat(num, "f0");
    PrintFormat(num, "f6");
    PrintFormat(num, "g");
    PrintFormat(num, "g2");
    PrintFormat(num, "g7");
    PrintFormat(num, "n");
    PrintFormat(num, "p");
    PrintFormat(num, "p8");
    PrintFormat(num, "r");

    TryUserFormat(num);
    
    int i = 77;
    Console.WriteLine("Numeric Formatting:  Predefined integer point formats");
    Console.WriteLine("Integer data is {0}", i);
    Console.WriteLine("{0}\t{1}" ,"Code", "Format");
    Console.WriteLine("{0}\t{1}" ,"----", "------");
    PrintFormat(i, "d");
    PrintFormat(i, "d8");
    PrintFormat(i, "g");
    PrintFormat(i, "g8");
    PrintFormat(i, "x");
    PrintFormat(i, "x8");

    TryUserFormat(i);

    decimal dec = new System.Decimal(-122345678.123456789012345678901234567890);
    Console.WriteLine("Numeric Formatting:  Predefined decimal formats");
    Console.WriteLine("Decimal data is {0}", dec);
    Console.WriteLine("{0}\t{1}" ,"Code", "Format");
    Console.WriteLine("{0}\t{1}" ,"----", "------");
    PrintFormat(dec, "c");
    PrintFormat(dec, "f");
    PrintFormat(dec, "g");
    PrintFormat(dec, "n");

    TryUserFormat(dec);

    // Numeric user-defined format patterns
/*
0 Zero placeholder
If the value being formatted has a digit in the position
where the '0' appears in the format string, then that digit
is copied to the output string. The position of the leftmost
'0' before the decimal point and the rightmost '0' after the
decimal point determines the range of digits that are always
present in the output string.

# Digit placeholder
If the value being formatted has a digit in the position
where the '#' appears in the format string, then that digit
is copied to the output string. Otherwise, nothing is stored
in that position in the output string. Note that this
specifier never displays the '0' character if it is not a
significant digit, even if '0' is the only digit in the
string. It will display the '0' character if it is a
significant digit in the number being displayed.

. Decimal point
The first '.' character in the format string determines the
location of the decimal separator in the formatted value;
any additional '.' characters are ignored. The actual
character used as the decimal separator is determined by the
NumberDecimalSeparator property of the NumberFormatInfo
object that controls formatting.

, Thousand separator and number scaling
The ',' character serves two purposes. First, if the format
string contains a ',' character between two digit
placeholders (0 or #) and to the left of the decimal point
if one is present, then the output will have thousand
separators inserted between each group of three digits to
the left of the decimal separator. The actual character used
as the decimal separator in the output string is determined
by the NumberGroupSeparator property of the current
NumberFormatInfo object that controls formatting.
Second, if the format string contains one or more ','
characters immediately to the left of the decimal point,
then the number will be divided by the number of ','
characters multiplied by 1000 before it is formatted. For
example, the format string '0,,' will represent 100 million
as simply 100. Use of the ',' character to indicate scaling
does not include thousand separators in the formatted
number. Thus, to scale a number by 1 million and insert
thousand separators you would use the format string
'#,##0,,'.

% Percentage placeholder
The presence of a '%' character in a format string causes a
number to be multiplied by 100 before it is formatted. The
appropriate symbol is inserted in the number itself at the
location where the '%' appears in the format string. The
percent character used is dependent on the current
NumberFormatInfo class.

E0, E+0, E-0, e0, e+0, e-0 Scientific notation
If any of the strings 'E', 'E+', 'E-', 'e', 'e+', or 'e-'
are present in the format string and are followed
immediately by at least one '0' character, then the number
is formatted using scientific notation with an 'E' or 'e'
inserted between the number and the exponent. The number of
'0' characters following the scientific notation indicator
determines the minimum number of digits to output for the
exponent. The 'E+' and 'e+' formats indicate that a sign
character (plus or minus) should always precede the
exponent. The 'E', 'E-', 'e', or 'e-' formats indicate that
a sign character should only precede negative exponents.

\ Escape character
In C# the backslash character causes the next character in the 
format string to be interpreted as an escape sequence. 
It is used with traditional formatting sequences like "\n" (new line).
Use the string "\\" to display "\".

'ABC' , "ABC"
 Literal string Characters enclosed in single or double
quotes are copied to the output string literally, and do not
affect formatting.

; Section separator
The ';' character is used to separate sections for positive,
negative, and zero numbers in the format string.
*/

    //Demonstrate some numeric user-defined format patterns

    i = 1234356789;
    num = 8976543.21;
    int hexnum = 1102;

    Console.WriteLine("Numeric Formatting:  user-defined custom format patterns");
    Console.WriteLine("Integer data is {0}", i);
    Console.WriteLine ("{0}\t{1}" ,"Code", "Format");
    Console.WriteLine ("{0}\t{1}" ,"----", "------");
    PrintFormat(i, "#");
    PrintFormat(i, "###");
    PrintFormat(i, "#.00");
    PrintFormat(i, "(###) ### - ####");
    PrintFormat(i, "D4");
    PrintFormat(i, "#0.##%");
    PrintFormat(num, "%#");
    PrintFormat(num, "#,,");
    PrintFormat(num, "0.###E+000");
    PrintFormat(hexnum, "x");
    PrintFormat(hexnum, "x8");

    TryUserFormat(i);

    Console.WriteLine("-----------------------------------------------------------------------------");
    Console.WriteLine("Using Modified NumberFormatInfo\n");
    NumberFormatInfo nfi = new NumberFormatInfo();
    long n = 7654321445511;
    Console.WriteLine("\nPercentDecimalDigits = 5");
    Console.WriteLine("PercentGroupSizes = new int [] {1, 2, 3, 0}\n");
    nfi.PercentDecimalDigits = 5;
    nfi.PercentGroupSizes = new int [] {1, 2, 3, 0};
    PrintFormat(n, "p", nfi);
    Console.WriteLine("-----------------------------------------------------------------------------\n");


    // User defined types can specify their own formatting that works in exactly the same way.
    // See the definition of MyType below

    Console.WriteLine("Formatting Custom Types Example");
    Console.WriteLine("This uses the formats defined in type MyType and data 43");
    Console.WriteLine("{0}\t{1}" ,"Code", "Format");
    Console.WriteLine("{0}\t{1}" ,"----", "------");

    MyType t = new MyType (43);

    PrintFormat (t, "b");
    PrintFormat (t, "o");
    PrintFormat (t, "d");
    PrintFormat (t, "h");

    TryUserFormat(t);

    // It is also possible to add new formatting codes to existing types (such as Int32 in this example).
    // See the definition for NBaseFormatter below
    int j = 100;
    Console.WriteLine("Example of adding additional formatting codes to existing types.");
    Console.WriteLine("Uses NBaseFormatter with data value of 100.\n");

    NBaseFormatter nbf = new NBaseFormatter();

    PrintFormat(j, "B8", nbf);
    PrintFormat(j, "B16", nbf);
    
    Console.WriteLine(string.Format(nbf, "{0} in the non-custom format 'c' is {1:c}", j, j));
    Console.WriteLine(string.Format(nbf, "{0} with no formatting is {1}", j, j));

  } //Main()

  /// <summary>
  /// Overload that uses null for the IFormatProvider.
  /// </summary>
  /// <param name="inputData"></param>
  /// <param name="formatString"></param>
  public static void PrintFormat(IFormattable inputData, string formatString)
  {
    PrintFormat(inputData, formatString, null);
  }//PrintFormat()


  /// <summary>
  /// Outputs the data with the specified format.
  /// </summary>
  /// <param name="inputData"></param>
  /// Data item
  /// <param name="formatString"></param>
  /// Format string
  /// <param name="outFormat"></param>
  /// Special output format for custom output spacing.
  /// <param name="provider"></param>
  ///  Custom format provider
  public static void PrintFormat(IFormattable inputData, 
                                 string formatString,
                                 IFormatProvider provider)
  {
    try
    {
      if (provider == null)
      {
        Console.WriteLine("{0}\t{1}", 
                          formatString, 
                          inputData.ToString(formatString, provider));
      }//if
      else
      {
        string formstr = "{0} in the custom " + formatString + " format is {1:" + formatString + "}";
        Console.WriteLine(string.Format(provider, formstr, inputData, inputData));
      }//else
    }//try
    catch (Exception e)
    {
      Console.WriteLine("Exception in PrintFormat(): {0}", e.Message);
      Console.WriteLine("Data was {0}, Format string was: {1}, Type was {2}", 
                         inputData, 
                         formatString,
                         inputData.GetType().Name);
    }//catch
  }//PrintFormat(IFormattable inputData, string formatString, IFormatProvider provider)

  /// <summary>
  /// Gets a format string from the user and calls PrintFormat() with it.
  /// </summary>
  /// <param name="inputData"></param>
  public static void TryUserFormat(IFormattable inputData)
  {
    while (true)
    {
      Console.Write("\nType a test format or <enter> to continue: ");
      string UserFormat = Console.ReadLine();
      if (UserFormat == null || UserFormat == string.Empty)
      {
        Console.WriteLine();
        return;
      }//if
      else
      {
        PrintFormat(inputData, UserFormat);
      }//else
    }//while (true)
  }//TryUserFormat()
}//class stringformat

//===========================================================================================
// The classes below demonstrate:
// - Creating a type that supports custom formatting via the IFormattable interface.
// - Creating a custom format provider to extend the formatting codes supported by Int32.
//   This implements the IFormatProvider and ICustomFormatter interfaces.
//===========================================================================================

/// <summary>
/// Custom type demonstrating IFormattable interface.
/// This is a custom type that implements IFormattable and respects the "b" format for binary
/// as well as all the formatting codes for Int32.
/// 
/// The IFormattable interface requires that an overloaded ToString() member be implemented.
/// 
/// Console.WriteLine, String.Format, and so forth will call the ToString() method to get the 
/// string form of an instance of this type.
/// 
/// </summary>
public class MyType : IFormattable
{
  private int m_value; // to store the inputValue
  public MyType (int inputValue)
  {
    m_value = inputValue;
  }//constructor

  /// <summary>
  /// ToString() is the formatting method called by String.Format.  
  /// 
  /// This is the implementation of the ToString() overload required by the IFormattable interface which
  /// takes a format and a IFormatProvider.
  /// 
  /// This *not* overriding the default implementation of ToString inherited from object since that
  /// base method takes no parameters.
  /// 
  /// In this method we look for the "b" format which we respect or fall through
  /// to the default Int32 format for anything we don't know about. 
  /// 
  /// </summary>
  /// <param name="format"></param>
  /// <param name="provider"></param>
  /// <returns></returns>
  public string ToString(string format, IFormatProvider provider)
  {
    switch(format)
    {
      case "b":
        return Convert.ToString(m_value, 2);
      case "o":
        return Convert.ToString(m_value, 8);
      case "d":
        return Convert.ToString(m_value, 10);
      case "h":
        return Convert.ToString(m_value, 16);
    }//switch
     //Otherwise return the standard format
    return m_value.ToString (format, provider);
  }//ToString()
}//class MyType


/// <summary>
/// This class provides a new formatting code: Bn where n is any number between 2 and 64. 
/// This Formatting code allows numbers to be printed out in any base.
/// To get access to the formatting code, a user needs to pass NBaseFormatter provider
///  to string.Format()
/// </summary>
public class NBaseFormatter : IFormatProvider, ICustomFormatter
{
  /// <summary>
  /// String.Format calls this method to get an instance of a ICustomFormatter to handle the formatting.
  /// In this case the same instance (this) is returned, but it would be possible return an instance
  /// of a different type.
  /// 
  /// This method implements IFormatProvider
  /// </summary>
  /// <param name="format"></param>
  /// <returns></returns>
  public object GetFormat(Type formatType)
  {
    if (formatType == typeof(ICustomFormatter)) 
      return this;
    else 
      return null;
  }//GetFormat()

  /// <summary>
  /// Once string.Format gets the ICustomFormatter, it calls this format method on each argument.
  /// 
  /// This implements the ICustomFormatter interface.
  /// </summary>
  /// <param name="format"></param>
  /// <param name="arg"></param>
  /// <param name="provider"></param>
  /// <returns></returns>
  public string Format (string format, object arg, IFormatProvider provider)
  {
    if (format != null && format.StartsWith("B"))
    {
      // This is the custom code so get the base out of the format string and 
      // use it to form the output string.
      format = format.Trim(new char [] {'B'});
      int b = Convert.ToInt32(format);
      return Convert.ToString((int)arg, b);
    }//if
    else  // fall through to standard string formatting
    {
      if (arg is IFormattable)
        return ((IFormattable)arg).ToString(format, provider);
      else
        return arg.ToString(); 
    }//else
  }//Format()
}//class NBaseFormatter

