'****************************** Module Header ******************************'
' Module Name:  MainWindow.xaml.vb
' Project:      VBWPFDataBinding
' Copyright (c) Microsoft Corporation.
' 
' This example demonstrates how to use DataBinding in WPF
' 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Imports System.Collections.ObjectModel

''' <summary> 
''' Interaction logic for MainWindow.xaml 
''' </summary> 
Partial Public Class MainWindow
    Inherits Window
    Private p As New Person()
    Private pers As New Persons()
    Public Sub New()
        InitializeComponent()
        p.Name = "Bruce"
        p.Job = "Nothing"
        p.Interest = "PC Game"
        p.Salary = 20
        p.Age = 15
        SetBindings()
    End Sub
    Private Sub SetBindings()
        'set Binding for Name TextBox 
        Dim b As New Binding()
        b.Mode = BindingMode.TwoWay
        b.UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged
        b.Source = p
        b.Path = New PropertyPath("Name")
        BindingOperations.SetBinding(Me.tbPersonName, TextBox.TextProperty, b)

        'set Binding for Job TextBox 

        Dim b1 As New Binding()
        b1.Mode = BindingMode.OneWayToSource
        b1.UpdateSourceTrigger = UpdateSourceTrigger.LostFocus
        b1.Source = p
        b1.Path = New PropertyPath("Job")
        BindingOperations.SetBinding(Me.tbPersonJob, TextBox.TextProperty, b1)

        'set Binding for Age TextBox 

        Dim b2 As New Binding()
        b2.UpdateSourceTrigger = UpdateSourceTrigger.LostFocus
        b2.Source = p
        b2.Mode = BindingMode.TwoWay
        b2.ValidationRules.Add(New AgeValidationRule())
        b2.Path = New PropertyPath("Age")
        BindingOperations.SetBinding(Me.tbPersonAge, TextBox.TextProperty, b2)

        'set Binding for Salary TextBox 
        Dim b3 As New Binding()
        b3.Source = p
        b3.Mode = BindingMode.TwoWay
        b3.UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged
        b3.Path = New PropertyPath("Salary")

        BindingOperations.SetBinding(Me.tbPersonSalary, TextBox.TextProperty, b3)

        'Set Binding for Interest TextBox 

        Dim b4 As New Binding()
        b4.Source = p
        b4.Mode = BindingMode.OneWay
        b4.Path = New PropertyPath("Interest")
        BindingOperations.SetBinding(Me.tbPersonInterest, TextBox.TextProperty, b4)

        'set Binding for Name Label 
        Dim b5 As New Binding()
        b5.Source = p
        b5.Path = New PropertyPath("Name")
        BindingOperations.SetBinding(Me.lblPersonName, Label.ContentProperty, b5)

        'set Binding for Job Label 
        Dim b6 As New Binding()
        b6.Source = p
        b6.Path = New PropertyPath("Job")
        BindingOperations.SetBinding(Me.lblPersonJob, Label.ContentProperty, b6)

        'set Binding for Age Label; 

        Dim b7 As New Binding()
        b7.Source = p
        b7.Path = New PropertyPath("Age")
        BindingOperations.SetBinding(Me.lblPersonAge, Label.ContentProperty, b7)

        'set Binding for Salary Label 
        Dim b8 As New Binding()
        b8.Source = p
        b8.Converter = New SalaryFormmatingConverter()
        b8.Path = New PropertyPath("Salary")
        BindingOperations.SetBinding(Me.lblPersonSalary, Label.ContentProperty, b8)

        'set Binding for Label Interest 
        Dim b9 As New Binding()
        b9.Source = p
        b9.Path = New PropertyPath("Interest")
        BindingOperations.SetBinding(Me.lblPersonInterest, Label.ContentProperty, b9)

        'set Binding for ListBox 
        lb.ItemsSource = pers
    End Sub
End Class