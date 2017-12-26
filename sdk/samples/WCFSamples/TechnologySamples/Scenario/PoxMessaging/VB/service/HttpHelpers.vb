' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.Collections.Generic
Imports System.Text

Namespace Microsoft.ServiceModel.Samples

    Class HttpHelpers

        Public Shared Function ParseQueryString(ByVal queryString As String) As Dictionary(Of String, String)

            Dim results As New Dictionary(Of String, String)()

            queryString = queryString.TrimStart("?"c)
            Dim keyValPairs As String() = queryString.Split("&"c)
            For Each keyValPair As String In keyValPairs

                Dim keyAndVal As String() = keyValPair.Split("="c)
                results.Add(keyAndVal(0), keyAndVal(1))

            Next

            Return results

        End Function

    End Class

End Namespace
