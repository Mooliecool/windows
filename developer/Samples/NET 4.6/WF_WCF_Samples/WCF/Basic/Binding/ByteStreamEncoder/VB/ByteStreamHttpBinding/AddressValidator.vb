'----------------------------------------------------------------
' Copyright (c) Microsoft Corporation.  All rights reserved.
'----------------------------------------------------------------

Imports Microsoft.VisualBasic
Imports System
Imports System.Configuration

Namespace Microsoft.Samples.ByteStreamEncoder

    Friend Class AddressValidator
        Inherits ConfigurationValidatorBase

        Public Sub New()
        End Sub

        Public Overrides Function CanValidate(ByVal type As Type) As Boolean
            Return type Is GetType(System.Net.IPAddress)
        End Function

        Public Overrides Sub Validate(ByVal value As Object)
            If Not (CType(value, Uri)).IsWellFormedOriginalString() Then
                Throw New ArgumentException("The provided Uri is not well formed")
            End If
        End Sub
    End Class
End Namespace
