'----------------------------------------------------------------
' Copyright (c) Microsoft Corporation.  All rights reserved.
'----------------------------------------------------------------

Imports Microsoft.VisualBasic
Imports System.ComponentModel
Imports System.ServiceModel

Namespace Microsoft.Samples.ByteStreamEncoder

    Friend NotInheritable Class HostNameComparisonModeHelper

        Private Sub New()
        End Sub

        Public Shared Sub Validate(ByVal value As HostNameComparisonMode)
            If (Not IsDefined(value)) Then
                Throw (New InvalidEnumArgumentException("value", CInt(Fix(value)), GetType(HostNameComparisonMode)))
            End If
        End Sub

        Friend Shared Function IsDefined(ByVal value As HostNameComparisonMode) As Boolean
            Return value = HostNameComparisonMode.StrongWildcard OrElse value = HostNameComparisonMode.Exact OrElse value = HostNameComparisonMode.WeakWildcard
        End Function
    End Class

End Namespace
