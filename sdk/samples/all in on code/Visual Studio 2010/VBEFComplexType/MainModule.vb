'****************************** Module Header ******************************\
' Module Name:    MainModule.vb
' Project:        VBEFComplexType
' Copyright (c) Microsoft Corporation.
'
' The VBEFComplexType example illustrates how to work with the Complex Type
' which is new in Entity Framework 4.0.  It shows how to add Complex Type 
' properties to entities, how to map Complex Type properties to table columns,
' and how to map a Function Import to a Complex Type.
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Module MainModule

    Sub Main()
        InsertEntityWithComplexTypeProperties()
        QueryEntityWithComplexTypeProperties()
        GetComplexTypeByFunctionImport()
    End Sub


    ''' <summary>
    ''' Insert Person entity with Complex Types PersonAddress and 
    ''' PersonDate
    ''' </summary>
    Private Sub InsertEntityWithComplexTypeProperties()
        Using context As New ComplexTypeEntities()
            ' Create a new PersonAddress Complex Type object.
            Dim peronAddress As New PersonAddress With
                {
                    .Address = "Shanghai, China",
                    .Postcode = "200000"
                }

            ' Create a new PersonDate Complex Type object.
            Dim personDate As New PersonDate With
                {
                    .EnrollmentDate = DateTime.Now
                }

            ' Create a new Person entity with the two Complex Types.
            Dim person As New Person With
                {
                    .PersonID = 50,
                    .FirstName = "Lingzhi",
                    .LastName = "Sun",
                    .PersonDate = personDate,
                    .PersonAddress = peronAddress
                }

            Try
                context.AddToPeople(person)
                context.SaveChanges()

            Catch ex As Exception
                Console.WriteLine(ex.Message)
            End Try
        End Using
    End Sub


    ''' <summary>
    ''' Query Person entity with Complex Types PersonAddress and 
    ''' PersonDate
    ''' </summary>
    Private Sub QueryEntityWithComplexTypeProperties()
        Using context As New ComplexTypeEntities()
            ' Get the Person entity whose PersonID is 40.
            ' Note: The Single method is new in EF 4.0.
            Dim person = context.People.Single(Function(p) p.PersonID = 50)

            ' Display the Person information and Complex Type properties.
            Console.WriteLine(
                    "Person:{0} {1}" + vbNewLine +
                    "EnrollmentDate:{2}" + vbNewLine +
                    "Address:{3}" + vbNewLine +
                    "Postcode:{4}",
                    person.FirstName, person.LastName,
                    person.PersonDate.EnrollmentDate,
                    person.PersonAddress.Address,
                    person.PersonAddress.Postcode)
        End Using
    End Sub


    ''' <summary>
    ''' Retrieve Complex Type value by Function Import
    ''' </summary>
    Private Sub GetComplexTypeByFunctionImport()
        Using context As New ComplexTypeEntities()
            ' Retrieve the PersonName Complex Type object by the
            ' Function Import GetPersonNameByPersonID.
            Dim personNames = context.GetPersonNameByPersonID(50)

            ' Display the PersonName Complex Type object.
            ' Note: The ToString() method of the PersonName class has 
            ' been overrided.  
            Console.WriteLine("Person name whose ID is 50: {0}",
                    personNames.Single())
        End Using
    End Sub

End Module

' Complex Type PersonName partial class
Partial Public Class PersonName
    ' Override the ToString() method
    Public Overrides Function ToString() As String
        Return Me.FirstName + " " + Me.LastName
    End Function
End Class
