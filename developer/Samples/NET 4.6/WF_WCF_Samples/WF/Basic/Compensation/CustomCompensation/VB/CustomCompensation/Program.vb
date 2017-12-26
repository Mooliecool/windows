'----------------------------------------------------------------
' Copyright (c) Microsoft Corporation.  All rights reserved.
'----------------------------------------------------------------

Imports Microsoft.VisualBasic
Imports Microsoft.VisualBasic.Activities
Imports System
Imports System.Activities
Imports System.Activities.Statements

Namespace Microsoft.Samples.Compensation.CustomCompensation

    Friend Class Program

        ' A truck rental agency processing an order
        Private Shared Function TruckRentalAgency() As Activity

            ' declaring a set of handles that can be used to refer to a particular unit of compensable work
            Dim handleCompanyA As Variable(Of CompensationToken) = New Variable(Of CompensationToken) With {.Name = "Handle to company A"}

            Dim handleCompanyC As Variable(Of CompensationToken) = New Variable(Of CompensationToken) With {.Name = "Handle to company C"}

            Dim handleServiceFee As Variable(Of CompensationToken) = New Variable(Of CompensationToken) With {.Name = "Handle for service fee"}

            Dim scopeHandle As Variable(Of CompensationToken) = New Variable(Of CompensationToken) With {.Name = "ScopeHandle"}

            Dim truckRentalRate As Variable(Of Integer) = New Variable(Of Integer)("truckRentalRate", 500)

            ' Branch waiting on customer input
            Dim customerInputSeq As New Sequence
            customerInputSeq.Activities.Add(New Delay With {.Duration = TimeSpan.FromSeconds(3)})

            ' Simulate customer deciding to cancel the order
            customerInputSeq.Activities.Add(New WriteLine With {.Text = "Received cancelation from customer."})

            Dim customBodySeq As New Sequence
            customBodySeq.Variables.Add(handleCompanyA)
            customBodySeq.Variables.Add(handleCompanyC)
            customBodySeq.Activities.Add(New WriteLine With {.Text = "Customer requested truck quotes for 4/14/2009"})
            customBodySeq.Activities.Add(New CompensableActivity With _
                                     { _
                                        .Body = New WriteLine With {.Text = "Request truck reservation from company A"}, _
                                        .CompensationHandler = New WriteLine With {.Text = "Cancel truck reservation from company A"}, _
                                        .ConfirmationHandler = New WriteLine With {.Text = "Confirm truck reservation from company A"}, _
                                        .Result = handleCompanyA _
                                     })
            customBodySeq.Activities.Add(New CompensableActivity With _
                                     { _
                                        .Body = New WriteLine With {.Text = "Request truck reservation from company B"}, _
                                        .CompensationHandler = New WriteLine With {.Text = "Cancel truck reservation from company B"}, _
                                        .ConfirmationHandler = New WriteLine With {.Text = "Confirm truck reservation from company B"} _
                                     })
            customBodySeq.Activities.Add(New CompensableActivity With _
                                     { _
                                        .Body = New WriteLine With {.Text = "Request truck reservation from company C"}, _
                                        .CompensationHandler = New WriteLine With {.Text = "Cancel truck reservation from company C"}, _
                                        .ConfirmationHandler = New WriteLine With {.Text = "Confirm truck reservation from company C"}, _
                                        .Result = handleCompanyC _
                                     })
            customBodySeq.Activities.Add(New WriteLine With {.Text = "Customer picked the truck from company B"})

            ' cancel the reservations for the trucks from the other companies
            customBodySeq.Activities.Add(New Compensate With {.Target = handleCompanyC})
            customBodySeq.Activities.Add(New Compensate With {.Target = handleCompanyA})

            ' Charge the customer on the credit card
            ' Service Fee charge
            customBodySeq.Activities.Add(New CompensableActivity With _
                                     { _
                                        .Body = New WriteLine With {.Text = "Charge agency service fee on customer credit card"}, _
                                        .CompensationHandler = New WriteLine With {.Text = "Refund agency service fee on customer credit card"}, _
                                        .ConfirmationHandler = New WriteLine With {.Text = "Confirm service fee charge notification"}, _
                                        .Result = handleServiceFee _
                                     })

            ' Truck Rental charge
            ' Notice how using Variables, one can save information that will be available 
            ' during Compensation or Confirmation
            Dim truckComp As New CompensableActivity
            truckComp.Variables.Add(truckRentalRate)

            Dim truckSeq As New Sequence
            truckSeq.Activities.Add(New Assign(Of Integer) With _
                                    { _
                                      .To = New OutArgument(Of Integer)(truckRentalRate), _
                                      .Value = New InArgument(Of Integer)(600) _
                                    })
            truckSeq.Activities.Add(New WriteLine With {.Text = New InArgument(Of String)(Function(env) ("Charge truck rental cost on customer credit card: $" & truckRentalRate.Get(env).ToString()))})

            truckComp.Body = truckSeq
            truckComp.CompensationHandler = New WriteLine With {.Text = New InArgument(Of String)(Function(env) ("Refund truck rental cost on customer credit card: $" & truckRentalRate.Get(env).ToString()))}
            truckComp.ConfirmationHandler = New WriteLine With {.Text = "Confirm truck rental charge notification"}

            customBodySeq.Activities.Add(truckComp)

            ' Used to define the custom compensation logic
            Dim customComp As New Sequence
            customComp.Activities.Add(New WriteLine With {.Text = "Customer canceled the order"})
            customComp.Activities.Add(New WriteLine With {.Text = "If customer is not a premium customer and there are less than 10 days" & _
                                                                   " to reservation date, then the service fee is not refundable"})
            customComp.Activities.Add(New WriteLine With {.Text = "This customer is not a premium customer"})
            customComp.Activities.Add(New WriteLine With {.Text = "There are less than 10 days until reservation date"})
            ' simulate a non-premium customer with less than 10 days to reservation date
            customComp.Activities.Add(New [If] With _
                                      { _
                                        .Condition = True, _
                                        .Then = New Confirm With {.Target = handleServiceFee} _
                                      })
            ' invoke default compensation (compensate in reverse order of completion) for the rest
            ' of compensable activities
            customComp.Activities.Add(New Compensate())

            ' wrap everything in a compensable activity that will be used to define custom compensation;
            ' instead of using the default compensation which compensates all completed compensable activities
            ' in reverse order of execution
            Dim customActivity As New CompensableActivity
            customActivity.Variables.Add(handleServiceFee)
            customActivity.Body = customBodySeq
            customActivity.CompensationHandler = customComp

            ' Branch processing the order
            ' Scoping compensation for the following sequence of activities
            ' If the body completes successfully child CompensableActivities will be confirmed
            Dim AutoConfirmScopeBodySeq As New Sequence
            AutoConfirmScopeBodySeq.Activities.Add(New WriteLine With {.Text = "Simulate a truck rental order"})
            AutoConfirmScopeBodySeq.Activities.Add(customActivity)
            AutoConfirmScopeBodySeq.Activities.Add(New WriteLine With {.Text = "Truck rental order completed"})
            AutoConfirmScopeBodySeq.Activities.Add(New WriteLine With {.Text = "Wait until reservation date or until cancelation, whichever comes first" & vbCrLf})
            AutoConfirmScopeBodySeq.Activities.Add(New Delay With {.Duration = TimeSpan.FromSeconds(5)})

            Dim AutoConfirmScopeCA As New CompensableActivity
            AutoConfirmScopeCA.Body = AutoConfirmScopeBodySeq
            AutoConfirmScopeCA.Result = scopeHandle

            Dim AutoConfirmScopeIf As New [If]
            AutoConfirmScopeIf.Condition = New VisualBasicValue(Of Boolean) With {.ExpressionText = "ScopeHandle Is Nothing"}
            AutoConfirmScopeIf.Else = New Confirm With {.Target = scopeHandle}

            Dim AutoConfirmScope As New TryCatch
            AutoConfirmScope.Try = AutoConfirmScopeCA
            AutoConfirmScope.Finally = AutoConfirmScopeIf
            AutoConfirmScope.Variables.Add(scopeHandle)

            Dim mainParallel As New Parallel

            ' ends when either the customer cancels the order or the reservation date is now
            mainParallel.CompletionCondition = True
            mainParallel.Branches.Add(customerInputSeq)
            mainParallel.Branches.Add(AutoConfirmScope)

            Return mainParallel
        End Function

        Shared Sub Main()
            WorkflowInvoker.Invoke(TruckRentalAgency())

            Console.WriteLine(Constants.vbLf & "Press ENTER to exit")
            Console.ReadLine()
        End Sub
    End Class
End Namespace
