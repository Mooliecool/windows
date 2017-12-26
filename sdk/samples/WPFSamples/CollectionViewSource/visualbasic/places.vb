Imports System
Imports System.Collections.ObjectModel

    Public Class Places
        Inherits ObservableCollection(Of Place)

        ' Methods
        Public Sub New()
            MyBase.Add(New Place("Seattle", "WA"))
            MyBase.Add(New Place("Redmond", "WA"))
            MyBase.Add(New Place("Bellevue", "WA"))
            MyBase.Add(New Place("Kirkland", "WA"))
            MyBase.Add(New Place("Portland", "OR"))
            MyBase.Add(New Place("San Francisco", "CA"))
            MyBase.Add(New Place("Los Angeles", "CA"))
            MyBase.Add(New Place("San Diego", "CA"))
            MyBase.Add(New Place("San Jose", "CA"))
            MyBase.Add(New Place("Santa Ana", "CA"))
            MyBase.Add(New Place("Bellingham", "WA"))
        End Sub

    End Class


