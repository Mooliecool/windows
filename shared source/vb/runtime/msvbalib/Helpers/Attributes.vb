'*****************************************************************************/
'*  Attributes.vb
'*
'*  Copyright (c) Microsoft Corporation.  All rights reserved.
'*  Information Contained Herein Is Proprietary and Confidential.
'*
'* Purpose:
'*  Defines custom attributes used by the compiler
'*
'*****************************************************************************/

Imports System

Namespace Microsoft.VisualBasic.CompilerServices


    ' -------------------------------------------------------------------
    ' StandardModuleAttribute is used by the compiler to mark all Module
    ' declarations.  This is needed so we can promote the module's
    ' contents into the default namespace.
    '
    ' WARNING: Do not rename this attribute or move it out of this 
    ' module.  Otherwise there are compiler changes that will
    ' need to be made!
    ' -------------------------------------------------------------------
    <System.AttributeUsage(System.AttributeTargets.Class, Inherited:=False, AllowMultiple:=False), _
     System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Never)> _
    Public NotInheritable Class StandardModuleAttribute
        Inherits System.Attribute

        Public Sub New()
            MyBase.New()
        End Sub
    End Class

    ' -------------------------------------------------------------------
    ' OptionTextAttribute is used by the compiler to mark all Classes/Modules
    ' as to whether we Option Compare Text is defined or not
    '
    ' WARNING: Do not rename this attribute or move it out of this 
    ' module.  Otherwise there are compiler changes that will
    ' need to be made!
    ' -------------------------------------------------------------------
    <System.AttributeUsage(System.AttributeTargets.Class, Inherited:=False, AllowMultiple:=False), _
        System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Never)> _
    Public NotInheritable Class OptionTextAttribute
        Inherits System.Attribute

        Public Sub New()
            MyBase.New()
        End Sub
    End Class


    ' -------------------------------------------------------------------
    ' OptionCompareAttribute is used by the compiler to determine 
    ' when the Option Compare setting should be passed as the default 
    ' value for the attributed argument.
    '
    ' WARNING: Do not rename this attribute or move it out of this 
    ' module.  Otherwise there are compiler changes that will
    ' need to be made!
    ' -------------------------------------------------------------------
    <System.AttributeUsage(System.AttributeTargets.Parameter, Inherited:=False, AllowMultiple:=False), _
        System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Never)> _
    Public NotInheritable Class OptionCompareAttribute

        Inherits System.Attribute

        Public Sub New()
            MyBase.New()
        End Sub
    End Class

    '''**************************************************************************
    ''' ;DesignerGeneratedAttribute
    ''' <summary>
    ''' When applied to a class, the compiler will generate an implicit call to
    ''' to a private InitializeComponent method from the default synthetic
    ''' constructor. The compiler will also verify that this method is called
    ''' from user defined constructors and report a warning or error it it is not.
    ''' The IDE will honor this attribute when generating code on behalf of the
    ''' user. 
    ''' </summary>
    ''' <remarks>
    ''' WARNING: Do not rename this attribute or move it out of this module.  Otherwise there
    ''' are compiler changes that will need to be made
    ''' </remarks>
    <AttributeUsage(AttributeTargets.Class, AllowMultiple:=False, Inherited:=False)> _
    <System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Never)> _
    Public NotInheritable Class DesignerGeneratedAttribute
        Inherits System.Attribute

    End Class

End Namespace
