' Copyright (c) Microsoft Corporation. All rights reserved.

 '----------------------------------------------------------------
 ' Copyright (c) Microsoft Corporation. All rights reserved.
 '----------------------------------------------------------------

 Imports System
 Imports System.Collections.Generic
 Imports System.Text
 Imports System.Diagnostics
 Namespace NumberUtilities
    
     NotInheritable Class NumberConverter
         Private Sub New()
         End Sub
         ' <summary>
         ' Convert a number to set of English words, e.g.
         ' "-2452345.203909" =>
         ' "negative two million, four hundred fifty-two thousand, three hundred forty-five point two zero three nine zero nine"
         ' </summary>
        Shared _onesAndTeens As String() = {"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen", "seventeen", "eighteen", "nineteen"}
        Shared _tens As String() = {"", "", "twenty", "thirty", "forty", "fifty", "sixty", "seventy", "eighty", "ninety"}
        Shared _thousands As String() = {"", "thousand", "million", "billon", "trillion", "quadrillion"}
        
         Public Shared Function ConvertNumberToWords(ByVal number As Double) As String
             ' special case zero
             If number = 0 Then
                 Return "zero"
             End If
            
             ' otherwise, build the English string
             Dim english As New StringBuilder()
            
             ' handle the whole part first
             Dim wholePart As Long = CLng(Math.Truncate(number))
             Dim negative As Boolean = wholePart < 0
             If negative Then
                 wholePart = 0 - wholePart
             End If
            
             Dim thousand As Integer = 0
             While wholePart <> 0
                 Dim threeDigits As Long = wholePart Mod 1000
                 Dim threeDigitsEnglish As New StringBuilder()
                
                 If threeDigits > 99 Then
                     ' one hundred, two hundred, etc.
                     threeDigitsEnglish.AppendFormat("{0} hundred", _onesAndTeens(threeDigits / 100))
                     threeDigits = threeDigits Mod 100
                     If threeDigits <> 0 Then
                         threeDigitsEnglish.Append(" "C)
                     End If
                 End If
                
                 If threeDigits >= _onesAndTeens.Length Then
                     ' twenty, thirty, etc.
                     threeDigitsEnglish.Append(_tens(threeDigits / 10))
                     threeDigits = threeDigits Mod 10
                    
                     ' twenty-, thirty-, etc.
                     If threeDigits <> 0 Then
                         threeDigitsEnglish.Append("-"C)
                     End If
                 End If
                
                 If threeDigits > 0 Then
                     ' one, two, ..., eighteen, nineteen, etc.
                     threeDigitsEnglish.Append(_onesAndTeens(threeDigits))
                 End If
                
                 ' append thousands
                 If threeDigitsEnglish.Length <> 0 Then
                     ' * thousand, * million, etc.
                     If threeDigitsEnglish.Length <> 0 Then
                         threeDigitsEnglish.Append(" "C)
                     End If
                     threeDigitsEnglish.Append(_thousands(thousand))
                 End If
                
                 If english.Length <> 0 Then
                     threeDigitsEnglish.Append(", ")
                 End If
                 english.Insert(0, threeDigitsEnglish)
                
                 thousand += 1
                 wholePart /= 1000
             End While
            
             ' handle the decimal part second
             Dim decimalEnglish As New StringBuilder()
             Dim numberString As String = number.ToString("N99")
             Dim dot As Integer = numberString.IndexOf("."C)
             Dim includeZeros As Boolean = False
             Dim i As Integer = numberString.Length - 1
             While i <> dot
                 Dim digit As Char = numberString(i)
                If (digit = "0"c) AndAlso Not includeZeros Then
                    i -= 1
                    Continue While
                End If
                 includeZeros = True
                 If decimalEnglish.Length <> 0 Then
                     decimalEnglish.Insert(0, " "C)
                 End If
                decimalEnglish.Insert(0, _onesAndTeens(Convert.ToInt16(digit) - Convert.ToInt16("0"c)))
                 i -= 1
             End While
            
             If decimalEnglish.Length <> 0 Then
                 english.AppendFormat("point {0}", decimalEnglish)
             End If
            
             ' handle the negative part last
             If negative Then
                 english.Insert(0, "negative ")
             End If
            
             Return english.ToString()
         End Function
     End Class
 End Namespace
