Imports System
Imports System.Collections.Generic
Imports System.Linq
Imports System.Text

Namespace CSEFEntityDataModel.TableMerging
    Class TableMergingClass
        Public Shared Sub TableMergingTest()
            Query()
        End Sub

        Public Shared Sub Query()
            Using context As New EFTblMergeEntities()
                Dim person As Person = (context.Person).First()


                Console.WriteLine("{0} " & vbLf & "{1} {2} " & vbLf & "{3}", person.PersonID, person.FirstName, person.LastName, person.Address)
            End Using
        End Sub
    End Class

End Namespace