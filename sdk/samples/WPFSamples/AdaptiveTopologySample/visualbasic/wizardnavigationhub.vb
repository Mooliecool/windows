Imports System
Imports System.Collections.Generic
Imports System.Windows
Imports System.Windows.Navigation

Namespace AdaptiveTopologySample
    Public Class WizardNavigationHub
        Inherits PageFunction(Of WizardContext)

        Public Sub New()
            Me.navigationOrder = New List(Of PageFunction(Of WizardResult))
            Me.WizardData = New WizardData
            WizardNavigationHub.WizardNavigationHub = Me
        End Sub

        Public Function CanFinish(ByVal currentPageFunction As PageFunction(Of WizardResult)) As Boolean
            Dim index As Integer = Me.navigationOrder.IndexOf(DirectCast(currentPageFunction, PageFunction(Of WizardResult)))
            Return (index = (Me.navigationOrder.Count - 1))
        End Function

        Public Function CanGoBack(ByVal currentPageFunction As PageFunction(Of WizardResult)) As Boolean
            Dim index As Integer = Me.navigationOrder.IndexOf(DirectCast(currentPageFunction, PageFunction(Of WizardResult)))
            Return (index > 0)
        End Function

        Public Function CanGoNext(ByVal currentPageFunction As PageFunction(Of WizardResult)) As Boolean
            Dim index As Integer = Me.navigationOrder.IndexOf(DirectCast(currentPageFunction, PageFunction(Of WizardResult)))
            Return (index < (Me.navigationOrder.Count - 1))
        End Function

        Private Sub dataEntryPage_Return(ByVal sender As Object, ByVal e As ReturnEventArgs(Of WizardContext))

            Dim WizardContext As WizardContext = e.Result

            ' Cancel Wizard if data entry page was canceled
            If (WizardContext.Result = WizardResult.Canceled) Then
                Me.OnReturn(New ReturnEventArgs(Of WizardContext)(New WizardContext(WizardResult.Canceled, Nothing)))
                Return
            End If

            ' Organize navigation direction
            If (DirectCast(WizardContext.Data, WizardNavigationDirection) = WizardNavigationDirection.Forwards) Then
                Me.navigationOrder.Add(DirectCast(New WizardPage1(Me.WizardData), PageFunction(Of WizardResult)))
                Me.navigationOrder.Add(DirectCast(New WizardPage2(Me.WizardData), PageFunction(Of WizardResult)))
                Me.navigationOrder.Add(DirectCast(New WizardPage3(Me.WizardData), PageFunction(Of WizardResult)))
            Else
                Me.navigationOrder.Add(DirectCast(New WizardPage3(Me.WizardData), PageFunction(Of WizardResult)))
                Me.navigationOrder.Add(DirectCast(New WizardPage2(Me.WizardData), PageFunction(Of WizardResult)))
                Me.navigationOrder.Add(DirectCast(New WizardPage1(Me.WizardData), PageFunction(Of WizardResult)))
            End If

            ' Navigate to first page
            AddHandler Me.navigationOrder.Item(0).Return, New ReturnEventHandler(Of WizardResult)(AddressOf Me.NavigationHub_Return)
            DirectCast(Application.Current.MainWindow, NavigationWindow).Navigate(Me.navigationOrder.Item(0))

        End Sub

        Public Function GetNextWizardPage(ByVal currentPageFunction As PageFunction(Of WizardResult)) As PageFunction(Of WizardResult)
            Dim index As Integer = Me.navigationOrder.IndexOf(DirectCast(currentPageFunction, PageFunction(Of WizardResult)))
            Return Me.navigationOrder.Item((index + 1))
        End Function

        Private Sub NavigationHub_Return(ByVal sender As Object, ByVal e As ReturnEventArgs(Of WizardResult))
            ' If returning, Wizard was completed (finished or canceled),
            ' so continue returning to calling page
            Me.OnReturn(New ReturnEventArgs(Of WizardContext)(New WizardContext(e.Result, Me.WizardData)))
        End Sub

        Protected Overrides Sub Start()
            ' Keep this page function instance in navigation history until completion
            JournalEntry.SetKeepAlive(Me, True)
            MyBase.RemoveFromJournal = True

            ' Navigate to data entry page to determine navigation sequence
            Dim dataEntryPage As New DataEntryPage
            AddHandler dataEntryPage.Return, New ReturnEventHandler(Of WizardContext)(AddressOf Me.dataEntryPage_Return)
            MyBase.NavigationService.Navigate(dataEntryPage)

        End Sub

        Public Shared ReadOnly Property Current() As WizardNavigationHub
            Get
                Return WizardNavigationHub.WizardNavigationHub
            End Get
        End Property

        Private navigationOrder As List(Of PageFunction(Of WizardResult))
        Private WizardData As WizardData
        Private Shared WizardNavigationHub As WizardNavigationHub

    End Class
End Namespace


