'*************************** Module Header ******************************'
' Module Name:  PropertyNamingRule.vb
' Project:	    VBCustomCodeAnalysisRule
' Copyright (c) Microsoft Corporation.
' 
' The class PropertyNamingRule inherits the class Microsoft.FxCop.Sdk.BaseIntrospectionRule
' and override the method 
'     public ProblemCollection Check(Member member).
' 
' This rule is use to check whether a property name starts with a uppercase character. 
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

Friend NotInheritable Class PropertyNamingRule
    Inherits BaseIntrospectionRule
    ''' <summary>
    ''' Define the rule name, resource file and resource assembly.
    ''' </summary>
    Public Sub New()
        MyBase.New("PropertyNamingRule", "VBCustomCodeAnalysisRule.Rules",
                   GetType(PropertyNamingRule).Assembly)

    End Sub


    ''' <summary>
    ''' Check the name of the member if it is a method.
    ''' The name of a property should start with a uppercase character.
    ''' </summary>
    Public Overrides Function Check(ByVal memb As Member) _
        As ProblemCollection
        If TypeOf memb Is PropertyNode Then
            Dim p As PropertyNode = TryCast(memb, PropertyNode)

            If p.Name.Name(0) < "A"c _
                OrElse p.Name.Name(0) > "Z"c Then
                Me.Problems.Add(New Problem(Me.GetNamedResolution(
                                            "UppercaseProperty",
                                            p.Name.Name,
                                            p.DeclaringType.FullName)))
            End If
        End If

        Return Me.Problems
    End Function


End Class
