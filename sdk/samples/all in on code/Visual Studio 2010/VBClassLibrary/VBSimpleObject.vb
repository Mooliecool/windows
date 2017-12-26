'****************************** Module Header ******************************'
' Module Name:  VBSimpleObject.vb
' Project:      VBClassLibrary
' Copyright (c) Microsoft Corporation.
' 
' The code sample demonstrates a VB.NET class library that we can use in 
' other applications. The class library exposes a simple class named 
' VBSimpleObject. The process of creating the class library is very straight-
' forward.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Public Class VBSimpleObject

    ''' <summary>
    ''' Constructor
    ''' </summary>
    ''' <remarks></remarks>
    Sub New()
    End Sub


    Private fField As Single = 0.0F

    ''' <summary>
    ''' This is a public Property. It allows you to get and set the value of 
    ''' a float field.
    ''' </summary>
    Public Property FloatProperty() As Single
        Get
            Return Me.fField
        End Get
        Set(ByVal value As Single)
            Dim cancel As Boolean = False
            ' Raise the event FloatPropertyChanging
            RaiseEvent FloatPropertyChanging(value, cancel)
            If Not cancel Then
                Me.fField = value
            End If
        End Set
    End Property


    ''' <summary>
    ''' Returns a String that represents the current Object. Here, we return 
    ''' the string form of the float field fField.
    ''' </summary>
    ''' <returns>the string form of the float field fField.</returns>
    Public Overrides Function ToString() As String
        Return Me.fField.ToString("F2")
    End Function


    ''' <summary>
    ''' This is a public static method. It returns the number of characters 
    ''' in a string.
    ''' </summary>
    ''' <param name="str">a string</param>
    ''' <returns>the number of characters in the string</returns>
    Public Shared Function GetStringLength(ByVal str As String) As Integer
        Return str.Length
    End Function


    ''' <summary>
    ''' This is an event. The event is fired when the float property is set.
    ''' </summary>
    ''' <param name="NewValue">the new value of the property</param>
    ''' <param name="Cancel">
    ''' Output whether the change should be cancelled or not.
    ''' </param>
    Public Event FloatPropertyChanging(ByVal NewValue As Single, _
                                       ByRef Cancel As Boolean)

End Class
