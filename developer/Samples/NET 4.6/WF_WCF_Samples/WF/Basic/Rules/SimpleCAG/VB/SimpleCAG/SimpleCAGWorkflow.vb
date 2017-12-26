'----------------------------------------------------------------
' Copyright (c) Microsoft Corporation.  All rights reserved.
'----------------------------------------------------------------

Imports System
Imports System.Collections
Imports System.Workflow.Activities

Namespace Microsoft.Samples.Rules.SimpleCAG

    Public Enum TravelNeedType
        None = 0
        Car = 1
        Airline = 2
    End Enum

    Public Class SimpleConditionedActivityGroupWorkflow
        Inherits SequentialWorkflowActivity

        Private travelNeeds As ArrayList = New ArrayList()

        Public Sub New()
            MyBase.New()
            InitializeComponent()
            travelNeeds.Add(TravelNeedType.Airline)
            travelNeeds.Add(TravelNeedType.Car)
        End Sub

        Private Sub Car_ExecuteCode(ByVal sender As System.Object, ByVal e As System.EventArgs)
            Console.WriteLine("Booking car reservation")
            travelNeeds.Remove(TravelNeedType.Car)
        End Sub

        Private Sub Airline_ExecuteCode(ByVal sender As System.Object, ByVal e As System.EventArgs)
            Console.WriteLine("Booking airline reservation")
            travelNeeds.Remove(TravelNeedType.Airline)
        End Sub

        Private Sub CarCondition(ByVal sender As System.Object, ByVal e As System.Workflow.Activities.ConditionalEventArgs)
            e.Result = travelNeeds.Contains(TravelNeedType.Car)
        End Sub

        Private Sub AirlineCondition(ByVal sender As System.Object, ByVal e As System.Workflow.Activities.ConditionalEventArgs)
            e.Result = travelNeeds.Contains(TravelNeedType.Airline)
        End Sub
    End Class
End Namespace

