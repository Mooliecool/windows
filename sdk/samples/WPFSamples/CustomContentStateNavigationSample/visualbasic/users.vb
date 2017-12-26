Imports System.Collections.ObjectModel

Public Class Users
    Inherits ObservableCollection(Of User)

    ' Methods
    Public Sub New()
        MyBase.Add(New User("Sneezy"))
        MyBase.Add(New User("Happy"))
        MyBase.Add(New User("Dopey"))
        MyBase.Add(New User("Grumpy"))
        MyBase.Add(New User("Bashful"))
        MyBase.Add(New User("Doc"))
        MyBase.Add(New User("Sleepy"))
    End Sub

End Class

