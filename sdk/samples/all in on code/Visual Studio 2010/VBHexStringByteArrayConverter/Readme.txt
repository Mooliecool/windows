=============================================================================
      Windows APPLICATION: VBHexStringByteArrayConverter Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

This sample demonstrates how to convert byte array to hex string and vice 
versa. For example, 

    "FF00EE11" <--> { FF, 00, EE, 11 }


/////////////////////////////////////////////////////////////////////////////
Demo:

Step1. Build and run the sample project in Visual Studio 2010. 

Step2. In the Hex String -> Byte Array group box, input a hex string such as 
       "FF00EE11" and click the "Hex String -> Byte Array" button.  It will 
       convert the hex string to an array of bytes and display the bytes in 
       the combobox control:

        FF
        00
        EE
        11

Step3. Click the "Copy to Clipboard" button next to the byte array combobox.  
       Then click the "Paste from Clipboard" button.  The byte array will be 
       copied and pasted to the byte array combobox as the input of the Byte 
       Array -> Hex String conversion. In the UI, you can also type more 
       bytes and "Add" them to the array.

Step4. Click the "Byte Array -> Hex String" button to convert the byte array 
       to a hex string.  The hex string will be displayed in the Hex String 
       textbox.  For example, "FF00EE11".


/////////////////////////////////////////////////////////////////////////////
Implementation:

1. The sample uses the following algorithm defined in 
   HexStringByteArrayConverter.cs to covert hex string to byte array and 
   vice versa.  Altenatively, you can also use the BitConverter.ToString 
   method to convert byte array to string of hexadecimal pairs separated by 
   hyphens, where each pair represents the corresponding element in value; 
   for example, "7F-2C-4A-00".

        Private Const hexDigits As String = "0123456789ABCDEF"

		''' <summary>
		''' Convert a byte array to hex string. Example output: "7F2C4A00".
		''' 
		''' Alternatively, you can also use the BitConverter.ToString method to 
		''' convert byte array to string of hexadecimal pairs separated by hyphens, 
		''' where each pair represents the corresponding element in value; for 
		''' example, "7F-2C-4A-00".
		''' </summary>
		''' <param name="bytes">An array of bytes</param>
		''' <returns>Hex string</returns>
		Public Shared Function BytesToHexString(ByVal bytes() As Byte) As String
			Dim sb As New StringBuilder(bytes.Length * 2)
			For Each b As Byte In bytes
				sb.AppendFormat("{0:X2}", b)
			Next b
			Return sb.ToString()
		End Function

        ''' <summary>
		''' Convert a hex string to byte array.
		''' </summary>
		''' <param name="str">hex string. For example, "FF00EE11"</param>
		''' <returns>An array of bytes</returns>
		Public Shared Function HexStringToBytes(ByVal str As String) As Byte()
			' Determine the number of bytes
			Dim bytes(str.Length >> 1 - 1) As Byte
			For i As Integer = 0 To str.Length - 1 Step 2
				Dim highDigit As Integer = hexDigits.IndexOf(Char.ToUpperInvariant(str.Chars(i)))
				Dim lowDigit As Integer = hexDigits.IndexOf(Char.ToUpperInvariant(str.Chars(i + 1)))
				If highDigit = -1 OrElse lowDigit = -1 Then
					Throw New ArgumentException("The string contains an invalid digit.", "s")
				End If
				bytes(i >> 1) = CByte((highDigit << 4) Or lowDigit)
			Next i
			Return bytes
		End Function

2. To verify if the input hex string is in the right format, the code sample 
   provides the following helper function:

        ''' <summary>
		''' Verify the format of the hex string.
		''' </summary>
		Public Shared Function VerifyHexString(ByVal str As String) As Boolean
			Dim regex_Renamed As New Regex("\A[0-9a-fA-F]+\z")
			Return regex_Renamed.IsMatch(str) AndAlso ((str.Length And 1) <> 1)
		End Function


/////////////////////////////////////////////////////////////////////////////
References:

NumberStyles Enumeration
http://msdn.microsoft.com/en-us/library/system.globalization.numberstyles.aspx

BitConverter.ToString Method (Byte[]) 
http://msdn.microsoft.com/en-us/library/3a733s97.aspx


/////////////////////////////////////////////////////////////////////////////
