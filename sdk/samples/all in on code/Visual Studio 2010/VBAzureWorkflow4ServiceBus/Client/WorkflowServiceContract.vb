Imports System.ServiceModel

<ServiceContract()> _
Public Interface IProcessDataWorkflowService
	' Methods
	<OperationContract()> _
	Function ProcessData(ByVal request As ProcessDataRequest) As ProcessDataResponse
End Interface

Public Interface IProcessDataWorkflowServiceChannel
	Inherits IProcessDataWorkflowService, IClientChannel
End Interface

' The message contract.
<MessageContract(IsWrapped:=False)> _
Public Class ProcessDataRequest
	Public Sub New()
	End Sub

	Public Sub New(ByVal int As Integer?)
		Me.int = int
	End Sub

	<MessageBodyMember(Namespace:="http://schemas.microsoft.com/2003/10/Serialization/", Order:=0)> _
	Public int As Integer?
End Class

<MessageContract(IsWrapped:=False)> _
Public Class ProcessDataResponse
	Public Sub New()
	End Sub

	Public Sub New(ByVal [string] As String)
		Me.string = [string]
	End Sub

	<MessageBodyMember(Namespace:="http://schemas.microsoft.com/2003/10/Serialization/", Order:=0)> _
 Public [string] As String
End Class








