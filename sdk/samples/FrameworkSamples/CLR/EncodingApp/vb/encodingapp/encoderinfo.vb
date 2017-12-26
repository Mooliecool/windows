
'-----------------------------------------------------------------------
'  This file is part of the Microsoft .NET SDK Code Samples.
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
'-----------------------------------------------------------------------
Imports System
Imports System.Globalization
Imports System.Text


'/ <summary>
'/ Class to encapsulate encoding options of Encoding type, Fallback mechanism and Normalization scheme
'/ </summary>

Public Class EncoderInfo
    Public Shared NormalizationNone As String = "None"
    'Different fallback schemes
    Public Shared FallbackSchemes As String() =  {"Replacement", "Exception", "Best Fit"}
    
    'Code page of encoding selected in options
    Private encodingCodePage As Integer = 0

	'Encoding that we're using
	Private targetEncoding As Encoding = Encoding.Default
    
    'EncoderFallback selected in options
    Private encFallback As EncoderFallback = EncoderFallback.ReplacementFallback
    
    'Normalization scheme selected in options
    Private normalizationForm As String
    
    
    'Constructor initializing members to default values
    Public Sub New() 
        'Initialize encoding to UTF8
        SetEncodingCodePage(Encoding.UTF8.CodePage)
        
        'Initialize fallback scheme to replacement
        SetEncoderFallback(FallbackSchemes(0))
        
        'Initialize normalization scheme to none
        SetNormalizationForm(NormalizationNone)
    
    End Sub 'New
    
    'Property to get the encoding scheme
    
    Public ReadOnly Property CurrentEncoding() As Encoding
        Get
            Return Me.targetEncoding.Clone()
        End Get
    End Property
    
    
    'Method to set the code page of the selected encoding
    Public Sub SetEncodingCodePage(ByVal encodingCodePage As Integer) 
        Me.encodingCodePage = encodingCodePage
        Me.targetEncoding = Encoding.GetEncoding(encodingCodePage, Me.encFallback, DecoderFallback.ReplacementFallback)
        If TypeOf Me.encFallback Is EncoderBestFitFallback Then
        	Dim bestFit As EncoderBestFitFallback = Me.encFallback
        	bestFit.ParentEncoding = Me.targetEncoding
        End If
    End Sub 'SetEncodingCodePage
    
    
    'Method to set the fallback scheme of the destination encoding
    Public Sub SetEncoderFallback(ByVal strFallback As String) 
        ' Replacement
        If strFallback = FallbackSchemes(0) Then
            Me.encFallback = New EncoderReplacementFallback("?")
        Else ' Exception
            If strFallback = FallbackSchemes(1) Then
                Me.encFallback = New EncoderExceptionFallback()
            Else
                Me.encFallback = New EncoderBestFitFallback(Me.targetEncoding)
            End If
        End If
        Me.targetEncoding.EncoderFallback = Me.encFallback
    
    End Sub 'SetEncoderFallback
    
    
    'Method to set the normalization scheme
    Public Sub SetNormalizationForm(ByVal strNormalization As String) 
        normalizationForm = strNormalization
    
    End Sub 'SetNormalizationForm
    
    
    'Method to normalize a given string in the normalization form specified
    Public Function GetNormalizedString(ByVal inputString As String) As String 
        If normalizationForm <> NormalizationNone Then
            Dim n As NormalizationForm = CType([Enum].Parse(GetType(NormalizationForm), normalizationForm), NormalizationForm)
            'Normalize string
            inputString = inputString.Normalize(n)
        End If
        
        Return inputString
    
    End Function 'GetNormalizedString
End Class 'EncoderInfo
