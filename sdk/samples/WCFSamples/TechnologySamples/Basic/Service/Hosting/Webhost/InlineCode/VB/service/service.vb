' THIS IS A DYUMMY FILE FOR THE BCZ AND IS NOT NEEDED, NOT USED.
' THE SERVICE CODE IS ENTIRELY CONTAINED IN SERVICE.SVC.

' Copyright (c) Microsoft Corporation.  All Rights Reserved.
Imports System.ServiceModel

Namespace Microsoft.ServiceModel.Samples


    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Interface ICalculatorSession
    End Interface

    Public Class CalculatorService
        Implements ICalculatorSession

    End Class

End Namespace
