'----------------------------------------------------------------
' Copyright (c) Microsoft Corporation.  All rights reserved.
'----------------------------------------------------------------

Imports Microsoft.VisualBasic
Imports System
Imports System.Configuration

Namespace Microsoft.Samples.ByteStreamEncoder

    <AttributeUsage(AttributeTargets.Property)> _
    Friend NotInheritable Class ServiceModelEnumValidatorAttribute
        Inherits ConfigurationValidatorAttribute

        Private enumType As Type

        Public Sub New(ByVal enumHelperType As Type)
            Me.EnumHelperType = enumHelperType
        End Sub

        Public Property EnumHelperType() As Type
            Get
                Return Me.enumType
            End Get
            Set(ByVal value As Type)
                Me.enumType = value
            End Set
        End Property

        Public Overrides ReadOnly Property ValidatorInstance() As ConfigurationValidatorBase
            Get
                Return New ServiceModelEnumValidator(enumType)
            End Get
        End Property
    End Class

End Namespace
