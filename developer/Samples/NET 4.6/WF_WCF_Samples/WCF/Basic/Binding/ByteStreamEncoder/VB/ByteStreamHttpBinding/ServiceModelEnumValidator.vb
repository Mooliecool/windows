'----------------------------------------------------------------
' Copyright (c) Microsoft Corporation.  All rights reserved.
'----------------------------------------------------------------

Imports Microsoft.VisualBasic
Imports System
Imports System.ComponentModel
Imports System.Configuration
Imports System.Reflection

Namespace Microsoft.Samples.ByteStreamEncoder

    Friend Class ServiceModelEnumValidator
        Inherits ConfigurationValidatorBase

        Private enumHelperType As Type
        Private isDefined As MethodInfo

        Public Sub New(ByVal enumHelperType As Type)
            Me.enumHelperType = enumHelperType
            Me.isDefined = Me.enumHelperType.GetMethod("IsDefined", BindingFlags.Static Or BindingFlags.NonPublic Or BindingFlags.Public)
        End Sub

        Public Overrides Function CanValidate(ByVal type As Type) As Boolean
            Return (Me.isDefined IsNot Nothing)
        End Function

        Public Overrides Sub Validate(ByVal value As Object)
            Dim retVal As Boolean = CBool(Me.isDefined.Invoke(Nothing, New Object() {value}))

            If (Not retVal) Then
                Dim isDefinedParameters() As ParameterInfo = Me.isDefined.GetParameters()
                Throw (New InvalidEnumArgumentException("value", CInt(Fix(value)), isDefinedParameters(0).ParameterType))
            End If
        End Sub
    End Class
End Namespace
