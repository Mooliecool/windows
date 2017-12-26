'*************************** Module Header ******************************'
' Module Name:  FieldNamingRule.vb
' Project:	    VBCustomCodeAnalysisRule
' Copyright (c) Microsoft Corporation.
' 
' The class FieldNamingRule inherits the class Microsoft.FxCop.Sdk.BaseIntrospectionRule
' and override the method 
'     public ProblemCollection Check(Member member).
' 
' This rule is use to check whether a field name starts with a lowercase character. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'**************************************************************************'

Imports Microsoft.FxCop.Sdk

Friend NotInheritable Class FieldNamingRule
    Inherits BaseIntrospectionRule
    ''' <summary>
    ''' Define the rule name, resource file and resource assembly.
    ''' </summary>
    Public Sub New()
        MyBase.New("FieldNamingRule", "VBCustomCodeAnalysisRule.Rules",
                   GetType(FieldNamingRule).Assembly)

    End Sub

    ''' <summary>
    ''' Check the name of the member if it is a field.
    ''' If the field is not an event or an static menber, its name should 
    ''' start with a lowercase character.
    ''' </summary>
    Public Overrides Function Check(ByVal memb As Member) _
        As ProblemCollection
        If TypeOf memb Is Field Then
            Dim fld As Field = TryCast(memb, Field)

            If Not (TypeOf fld.Type Is DelegateNode) _
                AndAlso (Not fld.IsStatic) Then

                If fld.Name.Name(0) < "a"c _
                    OrElse fld.Name.Name(0) > "z"c Then

                    Me.Problems.Add(New Problem(
                                    Me.GetNamedResolution("LowercaseField",
                                                          fld.Name.Name,
                                                          fld.DeclaringType.FullName)))
                End If
            End If

        End If

        Return Me.Problems
    End Function

End Class

