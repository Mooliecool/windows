' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.Collections.Generic
Imports System.Text
Namespace Microsoft.ServiceModel.Samples.BasicWebProgramming

	Public Class Service
		Implements IService
        Public Function EchoWithGet(ByVal s As String) As String Implements IService.EchoWithGet
            Return "You said " + s
        End Function

        Public Function EchoWithPost(ByVal s As String) As String Implements IService.EchoWithPost

            Return "You said " + s
        End Function
	End Class
End Namespace