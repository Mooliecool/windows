'****************************** Module Header ******************************\
' Module Name:    Message.vb
' Project:        VBASPNETReverseAJAX
' Copyright (c) Microsoft Corporation
'
' Message class contains all necessary fields in a message package.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'*****************************************************************************/

''' <summary>
''' This is a entity class that represents a message item.
''' </summary>
Public Class Message
    ''' <summary>
    ''' The name who will receive this message.
    ''' </summary>
    Public Property RecipientName() As String

    ''' <summary>
    ''' The message content.
    ''' </summary>
    Public Property MessageContent() As String
End Class