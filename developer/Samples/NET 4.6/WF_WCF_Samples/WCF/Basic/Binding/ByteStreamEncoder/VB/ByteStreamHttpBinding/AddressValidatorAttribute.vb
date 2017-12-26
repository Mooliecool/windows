'----------------------------------------------------------------
' Copyright (c) Microsoft Corporation.  All rights reserved.
'----------------------------------------------------------------

Imports Microsoft.VisualBasic
Imports System
Imports System.Configuration

Namespace Microsoft.Samples.ByteStreamEncoder

    <AttributeUsage(AttributeTargets.Property)> _
    Friend NotInheritable Class AddressValidatorAttribute
        Inherits ConfigurationValidatorAttribute

        Public Overrides ReadOnly Property ValidatorInstance() As ConfigurationValidatorBase
            Get
                Return New AddressValidator()
            End Get
        End Property
    End Class
End Namespace
