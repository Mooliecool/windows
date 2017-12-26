================================================================================
       WPF APPLICATION : VBWPFPaging Project Overview
       
                       WPF Paging Sample
                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The sample demonstrates how to page data in WPF.
   

/////////////////////////////////////////////////////////////////////////////
Code Logic:


1. Create a Customer class with properties of ID, Name, Age, Country, etc.

2. Define a ListView with columns binding to each properties of the Customer object;

3. Drag 4 buttons on to the MainWindow, which are for displaying first, previous,
   next, last page.
 
4. Construct an ObservableCollection collection of Customer objects. 

5. Create a CollectionViewSource object and set source to the customer list.

6. Handle the CollectionViewSource.Filter event to show data only in the current page.

        Private Sub view_Filter(ByVal sender As Object, ByVal e As FilterEventArgs)
            Dim index As Integer = customers.IndexOf(DirectCast(e.Item, Customer))

            If index >= itemPerPage * currentPageIndex AndAlso index < itemPerPage * 
						(currentPageIndex + 1) Then
                e.Accepted = True
            Else
                e.Accepted = False
            End If
        End Sub
        
7. Binding the CollectionViewSource object to the ListView.



/////////////////////////////////////////////////////////////////////////////
References:

   

/////////////////////////////////////////////////////////////////////////////
Demo

Step1. Build the sample project in Visual Studio 2008.

Step2. Start Without Debugging, and the mian window of the project will show.

Step3. It shows a listview and four buttons in the window, and there are 20 
items in the list ID from 0 to 19.

Step4. Click the Next button, it shows the next 20 items if exist.

Step5. Click the Previous button, it shows the previous 20 items if exist.

Step6. Click the Last button, it shows the items of the last page.

Step7. Click the First button, it shows the items of the first page.
/////////////////////////////////////////////////////////////////////////////