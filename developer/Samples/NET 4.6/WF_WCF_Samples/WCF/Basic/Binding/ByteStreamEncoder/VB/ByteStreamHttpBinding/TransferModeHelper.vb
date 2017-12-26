'----------------------------------------------------------------
' Copyright (c) Microsoft Corporation.  All rights reserved.
'----------------------------------------------------------------

Imports Microsoft.VisualBasic
Imports System.ComponentModel
Imports System.ServiceModel

Namespace Microsoft.Samples.ByteStreamEncoder

    Friend NotInheritable Class TransferModeHelper

        Private Sub New()
        End Sub

        Public Shared Function IsDefined(ByVal v As TransferMode) As Boolean
            Return ((v = TransferMode.Buffered) OrElse (v = TransferMode.Streamed) OrElse (v = TransferMode.StreamedRequest) OrElse (v = TransferMode.StreamedResponse))
        End Function

        Public Shared Function IsRequestStreamed(ByVal v As TransferMode) As Boolean
            Return ((v = TransferMode.StreamedRequest) OrElse (v = TransferMode.Streamed))
        End Function

        Public Shared Function IsResponseStreamed(ByVal v As TransferMode) As Boolean
            Return ((v = TransferMode.StreamedResponse) OrElse (v = TransferMode.Streamed))
        End Function

        Public Shared Sub Validate(ByVal value As TransferMode)
            If (Not IsDefined(value)) Then
                Throw (New InvalidEnumArgumentException("value", CInt(Fix(value)), GetType(TransferMode)))
            End If
        End Sub
    End Class
End Namespace
