'******************************* Module Header *******************************'
' Module Name:  MainModule.vb
' Project:      VBAutomateOutlook
' Copyright (c) Microsoft Corporation.
' 
' The VBAutomateOutlook example demonstrates the use of Visual Basic.NET 
' code to automate Microsoft Outlook to enumerate contacts and send a mail.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*****************************************************************************'

#Region "Imports directives"

Imports System.Reflection
Imports System.Runtime.InteropServices
Imports Outlook = Microsoft.Office.Interop.Outlook

#End Region


Module MainModule

    <STAThread()> _
    Sub Main()

        AutomateOutlook()


        ' Clean up the unmanaged Outlook COM resources by forcing a garbage 
        ' collection as soon as the calling function is off the stack (at 
        ' which point these objects are no longer rooted).

        GC.Collect()
        GC.WaitForPendingFinalizers()
        ' GC needs to be called twice in order to get the Finalizers called - 
        ' the first time in, it simply makes a list of what is to be finalized, 
        ' the second time in, it actually is finalizing. Only then will the 
        ' object do its automatic ReleaseComObject.
        GC.Collect()
        GC.WaitForPendingFinalizers()

    End Sub


    Sub AutomateOutlook()

        Dim missing As Object = Type.Missing

        Dim oOutlook As Outlook.Application = Nothing
        Dim oNS As Outlook.NameSpace = Nothing
        Dim oCtFolder As Outlook.MAPIFolder = Nothing
        Dim oCts As Outlook.Items = Nothing
        Dim oMail As Outlook.MailItem = Nothing


        Try
            ' Start Microsoft Outlook and log on with your profile.

            ' Create an Outlook application.
            oOutlook = New Outlook.Application()
            Console.WriteLine("Outlook.Application is started")

            Console.WriteLine("User logs on ...")

            ' Get the namespace
            oNS = oOutlook.GetNamespace("MAPI")

            ' Log on by using a dialog box to choose the profile.
            oNS.Logon(missing, missing, True, True)

            ' Alternative logon method that uses a specific profile.
            ' If you use this logon method, change the profile name to an 
            ' appropriate value. The second parameter of Logon is the password 
            ' (if any) associated with the profile. This parameter exists only 
            ' for backwards compatibility and for security reasons, and it is 
            ' not recommended for use.
            'oNS.Logon("YourValidProfile", missing, False, True)

            Console.WriteLine("Press ENTER to continue when Outlook is ready.")
            Console.ReadLine()

            ' Enumerate the contact items.

            Console.WriteLine("Enumerate the contact items")

            oCtFolder = oNS.GetDefaultFolder( _
            Outlook.OlDefaultFolders.olFolderContacts)
            oCts = oCtFolder.Items

            ' Enumerate the contact items. Be careful with foreach loops. 
            ' See: http://tiny.cc/uXw8S.
            For i As Integer = 1 To oCts.Count
                Dim oItem As Object = oCts(i)

                If (TypeOf oItem Is Outlook.ContactItem) Then
                    Dim oCt As Outlook.ContactItem = oItem
                    Console.WriteLine(oCt.Email1Address)
                    ' Do not need to Marshal.ReleaseComObject oCt because 
                    ' Dim oCt As Outlook.ContactItem = oItem is a simple .NET 
                    ' type casting, instead of a COM QueryInterface.
                ElseIf (TypeOf oItem Is Outlook.DistListItem) Then
                    Dim oDl As Outlook.DistListItem = oItem
                    Console.WriteLine(oDl.DLName)
                    ' Do not need to Marshal.ReleaseComObject oDl because 
                    ' Dim oDl As Outlook.DistListItem = oItem is a simple .NET 
                    ' type casting, instead of a COM QueryInterface.
                End If

                ' Release the COM object of the Outlook item.
                Marshal.FinalReleaseComObject(oItem)
                oItem = Nothing
            Next

            ' Create and send a new mail item.

            Console.WriteLine("Create and send a new mail item")

            oMail = oOutlook.CreateItem(Outlook.OlItemType.olMailItem)

            ' Set the properties of the email.
            oMail.Subject = "Feedback of All-In-One Code Framework"
            oMail.To = "codefxf@microsoft.com"
            oMail.HTMLBody = "<b>Feedback:</b><br />"

            ' Displays a new Inspector object for the item and allows users to 
            ' click on the Send button to send the mail manually.
            ' Modal = true makes the Inspector window modal
            oMail.Display(True)
            ' [-or-]
            ' Automatically send the mail without a new Inspector window.
            'oMail.Send()

            ' User logs off and quits Outlook.

            Console.WriteLine("Log off and quit the Outlook application")
            oNS.Logoff()
            oOutlook.Quit()

        Catch ex As Exception
            Console.WriteLine("AutomateOutlook throws the error: {0}", ex.Message)
        Finally

            ' Manually clean up the explicit unmanaged Outlook COM resources by  
            ' calling Marshal.FinalReleaseComObject on all accessor objects. 
            ' See http://support.microsoft.com/kb/317109.

            If Not oMail Is Nothing Then
                Marshal.FinalReleaseComObject(oMail)
                oMail = Nothing
            End If
            If Not oCts Is Nothing Then
                Marshal.FinalReleaseComObject(oCts)
                oCts = Nothing
            End If
            If Not oCtFolder Is Nothing Then
                Marshal.FinalReleaseComObject(oCtFolder)
                oCtFolder = Nothing
            End If
            If Not oNS Is Nothing Then
                Marshal.FinalReleaseComObject(oNS)
                oNS = Nothing
            End If
            If Not oOutlook Is Nothing Then
                Marshal.FinalReleaseComObject(oOutlook)
                oOutlook = Nothing
            End If

        End Try

    End Sub

End Module
