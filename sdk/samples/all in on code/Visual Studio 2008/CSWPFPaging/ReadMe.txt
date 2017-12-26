================================================================================
       WPF APPLICATION : CSWPFPaging Project Overview
       
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

        void view_Filter(object sender, FilterEventArgs e)
        {
            int index = customers.IndexOf((Customer)e.Item);

            if (index >= itemPerPage * currentPageIndex && index < 
                itemPerPage * (currentPageIndex + 1))
            {
                e.Accepted = true;
            }
            else
            {
                e.Accepted = false;
            }
        }
        
7. Binding the CollectionViewSource object to the ListView.



/////////////////////////////////////////////////////////////////////////////
References:

   

/////////////////////////////////////////////////////////////////////////////
