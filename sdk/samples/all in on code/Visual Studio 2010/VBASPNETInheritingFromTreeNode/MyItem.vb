'****************************** Module Header ******************************\
' Module Name:    MyItem.vb
' Project:        VBASPNETInheritingFromTreeNode
' Copyright (c) Microsoft Corporation
'
' This class defines the custom object which cab be used to assigned to 
' custom tree node.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'*****************************************************************************/

''' <summary>
''' We can store custom objects in the tree node.
''' All objects which are stored in View State need to be serializable.
''' </summary>
''' <remarks></remarks>
<Serializable()> _
Public Class MyItem
    Public Property Title As String
End Class