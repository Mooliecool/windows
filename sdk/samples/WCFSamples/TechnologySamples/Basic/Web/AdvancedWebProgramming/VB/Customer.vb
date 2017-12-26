' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.Collections.Generic
Imports System.Runtime.Serialization
Imports System.Text
Namespace Microsoft.ServiceModel.Samples.AdvancedWebProgramming

	<DataContract()> _
	Public Class Customer
		<DataMember()> _
		Public Address As String
		<DataMember()> _
		Public Name As String

		<DataMember()> _
		Public Uri As Uri

		Public Sub New(ByVal name As String, ByVal address As String, ByVal uri As Uri)
			Me.Name = name
			Me.Address = address
			Me.Uri = uri
		End Sub

		Public Overloads Overrides Function ToString() As String
			Return [String].Format("{0} {1} {2}", Name, Address, Uri)
		End Function
	End Class
End Namespace