Imports System
Imports System.Collections.ObjectModel

    Public Class Tasks
        Inherits ObservableCollection(Of Task)

        ' Methods
        Public Sub New()
            MyBase.Add(New Task("Grocery", "Pick up Grocery and Detergent", 2, TaskType.Home))
            MyBase.Add(New Task("Laundry", "Do my Laundry", 2, TaskType.Home))
            MyBase.Add(New Task("Email", "Email clients", 1, TaskType.Work))
            MyBase.Add(New Task("Clean", "Clean my office", 3, TaskType.Work))
            MyBase.Add(New Task("Dinner", "Get ready for family reunion", 1, TaskType.Home))
            MyBase.Add(New Task("Proposals", "Review new budget proposals", 2, TaskType.Work))
        End Sub

    End Class


