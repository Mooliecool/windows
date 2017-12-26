Imports System
Imports System.Collections.ObjectModel

    Public Class GreekGods
        Inherits ObservableCollection(Of GreekGod)

        ' Methods
        Public Sub New()
            MyBase.Add(New GreekGod("Aphrodite"))
            MyBase.Add(New GreekGod("Apollo"))
            MyBase.Add(New GreekGod("Ares"))
            MyBase.Add(New GreekGod("Artemis"))
            MyBase.Add(New GreekGod("Athena"))
            MyBase.Add(New GreekGod("Demeter"))
            MyBase.Add(New GreekGod("Dionysus"))
            MyBase.Add(New GreekGod("Hephaestus"))
            MyBase.Add(New GreekGod("Hera"))
            MyBase.Add(New GreekGod("Hermes"))
            MyBase.Add(New GreekGod("Poseidon"))
            MyBase.Add(New GreekGod("Zeus"))
        End Sub

    End Class


