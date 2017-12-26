'----------------------------------------------------------------
' Copyright (c) Microsoft Corporation.  All rights reserved.
'----------------------------------------------------------------

Imports System
Imports System.ComponentModel
Imports System.Workflow.Activities

Namespace Microsoft.Samples.Rules.SimpleCAG
    <Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
    Partial Public Class SimpleConditionedActivityGroupWorkflow

        'NOTE: The following procedure is required by the Workflow Designer
        'It can be modified using the Workflow Designer.  
        'Do not modify it using the code editor.
        <System.Diagnostics.DebuggerNonUserCode()> _
        Private Sub InitializeComponent()
            Me.CanModifyActivities = True
            Dim codecondition1 As System.Workflow.Activities.CodeCondition = New System.Workflow.Activities.CodeCondition
            Dim codecondition2 As System.Workflow.Activities.CodeCondition = New System.Workflow.Activities.CodeCondition
            Me.BookingCag = New System.Workflow.Activities.ConditionedActivityGroup
            Me.Car = New System.Workflow.Activities.CodeActivity
            Me.Airline = New System.Workflow.Activities.CodeActivity
            '
            'BookingCag
            '
            Me.BookingCag.Activities.Add(Me.Car)
            Me.BookingCag.Activities.Add(Me.Airline)
            Me.BookingCag.Name = "BookingCag"
            Me.BookingCag.UntilCondition = Nothing
            AddHandler codecondition1.Condition, AddressOf Me.CarCondition
            '
            'Car
            '
            Me.Car.Name = "Car"
            AddHandler Me.Car.ExecuteCode, AddressOf Me.Car_ExecuteCode
            Me.Car.SetValue(System.Workflow.Activities.ConditionedActivityGroup.WhenConditionProperty, codecondition1)
            AddHandler codecondition2.Condition, AddressOf Me.AirlineCondition
            '
            'Airline
            '
            Me.Airline.Name = "Airline"
            AddHandler Me.Airline.ExecuteCode, AddressOf Me.Airline_ExecuteCode
            Me.Airline.SetValue(System.Workflow.Activities.ConditionedActivityGroup.WhenConditionProperty, codecondition2)
            '
            'SimpleConditionedActivityGroupWorkflow
            '
            Me.Activities.Add(Me.BookingCag)
            Me.Name = "SimpleConditionedActivityGroupWorkflow"
            Me.CanModifyActivities = False
        End Sub
        Private WithEvents Car As System.Workflow.Activities.CodeActivity
        Private WithEvents Airline As System.Workflow.Activities.CodeActivity
        Private WithEvents BookingCag As System.Workflow.Activities.ConditionedActivityGroup

    End Class
End Namespace

