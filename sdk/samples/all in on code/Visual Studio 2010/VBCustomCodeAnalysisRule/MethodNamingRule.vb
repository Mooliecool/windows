'*************************** Module Header ******************************'
' Module Name:  MethodNamingRule.vb
' Project:	    VBCustomCodeAnalysisRule
' Copyright (c) Microsoft Corporation.
' 
' The class MethodNamingRule inherits the class Microsoft.FxCop.Sdk.BaseIntrospectionRule
' and override the method 
'     public ProblemCollection Check(Member member).
' 
' This rule is use to check whether a method name starts with a uppercase character. 
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

Friend NotInheritable Class MethodNamingRule
    Inherits BaseIntrospectionRule
    ''' <summary>
    ''' Define the rule name, resource file and resource assembly.
    ''' </summary>
    Public Sub New()
        MyBase.New("MethodNamingRule", "VBCustomCodeAnalysisRule.Rules",
                   GetType(MethodNamingRule).Assembly)

    End Sub

    ''' <summary>
    ''' Check the name of the member if it is a method.
    ''' If the method is not a constructor or an accessor, its name should 
    ''' start with a uppercase character.
    ''' </summary>
    Public Overrides Function Check(ByVal memb As Member) _
        As ProblemCollection
        If TypeOf memb Is Method _
            AndAlso Not (TypeOf memb Is InstanceInitializer) _
            AndAlso Not (TypeOf memb Is StaticInitializer) Then

            Dim mthd As Method = TryCast(memb, Method)

            If (Not mthd.IsAccessor) _
                AndAlso (mthd.Name.Name(0) < "A"c _
                         OrElse mthd.Name.Name(0) > "Z"c) Then
                Me.Problems.Add(New Problem(Me.GetNamedResolution(
                                            "UppercaseMethod",
                                            mthd.Name.Name,
                                            mthd.DeclaringType.FullName)))
            End If
        End If
        Return Me.Problems
    End Function


End Class
