 '---------------------------------------------------------------------
'  This file is part of the Microsoft .NET Framework SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'---------------------------------------------------------------------
Imports System
Imports System.Text


'/ <summary>
'/ Best fit fallback defining custom encoding
'/ </summary>

Public Class EncoderBestFitFallback
    Inherits EncoderFallback

    Private useEncoding As Encoding
    
    ' Constructor
    Public Sub New(ByVal targetEncoding As Encoding) 
        ParentEncoding = targetEncoding
    End Sub 'New

    ' Change parent encoding
    Public Property ParentEncoding() As Encoding
        Set
            useEncoding = value.Clone()
            useEncoding.EncoderFallback = New EncoderReplacementFallback("")
            useEncoding.DecoderFallback = New DecoderReplacementFallback(String.Empty)
        End Set
    
        Get
            Return useEncoding
        End Get
    End Property    

    ' Get maximum character count 
    
    Public Overrides ReadOnly Property MaxCharCount() As Integer 
        Get
            Return 18
        End Get
    End Property
    
    Public Overrides Function CreateFallbackBuffer() As EncoderFallbackBuffer 
        Return New EncoderBestFitFallbackBuffer(Me)
    
    End Function 'CreateFallbackBuffer
End Class 'EncoderBestFitFallback

'/ <summary>
'/ Class to encapsulate the buffer for best fit fallback
'/ </summary>

NotInheritable Public Class EncoderBestFitFallbackBuffer
    Inherits EncoderFallbackBuffer
    Private strFallback As String = String.Empty
    Private charIndex As Integer = 1
    Private parentEncoding As Encoding
    
    
    ' Constructor
    Public Sub New(ByVal fallback As EncoderBestFitFallback) 
        parentEncoding = fallback.ParentEncoding
    End Sub 'New
    
    
    ' Fallback methods
    Public Overloads Overrides Function Fallback(ByVal charUnknown As Char, ByVal index As Integer) As Boolean
        ' Since both fallback methods require normalizing a string, make a string out of our char
        Dim strUnknown As New String(charUnknown, 1)

        Return Fallback(strUnknown)

    End Function 'Fallback
    
    
    Public Overloads Overrides Function Fallback(ByVal charUnknownHigh As Char, ByVal charUnknownLow As Char, ByVal index As Integer) As Boolean
        ' Since both fallback methods require normalizing a string, make a string out of our chars
        Dim strUnknown As New String(New Char() {charUnknownHigh, charUnknownLow})

        Return Fallback(strUnknown)

    End Function 'Fallback
    
    
    Overloads Private Function Fallback(ByVal strUnknown As String) As Boolean 
        ' If they were falling back already they need to fail
        If charIndex <= strFallback.Length Then
            ' Throw it, using our complete character
            charIndex = 1
            strFallback = String.Empty
            Throw New ArgumentException("Unexpected recursive fallback", "chars")
        End If
        
        ' They need to fallback our character
        Dim strNormal As String = String.Empty
        Try
            ' Normalize our character
            strNormal = strUnknown.Normalize(NormalizationForm.FormKD)
            
            ' If it didn't change we'll still have to be ?
            If strNormal = strUnknown Then
                strNormal = "?"
            End If
        ' Illegal Unicode can throw a normalization exception
        Catch 
            ' Allow the ? fallback we already assigned to happen
        End Try

        ' Try encoding the normalized string
        strFallback = parentEncoding.GetString(parentEncoding.GetBytes(strNormal))

        ' Any data from this fallback?
        If strFallback = String.Empty Or strFallback(0) <> strNormal(0) Then
            strFallback = "?"
            strNormal = "?"
        End If
            
        charIndex = 0
        
        Return True
    
    End Function 'Fallback


    ' Default version is overridden in EncoderReplacementFallback.cs
    Public Overrides Function GetNextChar() As Char 
        ' If we're above string length then return nothing
        If charIndex >= strFallback.Length Then
            ' We use charIndex = length + 1 as a flag to tell us we're long past done.
            If charIndex = strFallback.Length Then
                charIndex += 1
            End If
            Return Convert.ToChar(vbNullChar)
        End If
        
        ' Return the character at our charIndex (& increment it)
        charIndex = charIndex + 1
        Return strFallback(charIndex-1)
    
    End Function 'GetNextChar
    
    
    ' Caller needs to back up for some reason
    Public Overrides Function MovePrevious() As Boolean
        ' Back up only if we're within the string or just did the last char
        If charIndex <= strFallback.Length Then
            charIndex -= 1
        End If
        ' Return true if we were able to move previous
        Return charIndex >= 0 OrElse charIndex < strFallback.Length

    End Function 'MovePrevious
    
    
    ' How many characters left to output?
    
    Public Overrides ReadOnly Property Remaining() As Integer
        Get
            ' Any characters remaining?  If so, how many?
            If charIndex < strFallback.Length Then
                Return strFallback.Length - charIndex
            End If
            Return 0
        End Get
    End Property
    
    
    ' Clear the buffer
    Public Overrides Sub Reset()
        strFallback = String.Empty
        charIndex = 1

    End Sub 'Reset
End Class 'EncoderBestFitFallbackBuffer 
