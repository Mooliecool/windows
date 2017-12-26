================================================================================
			Microsoft Dynamics NAV APPLICATION: CSDynamicNAVWebServices                        
===============================================================================


/////////////////////////////////////////////////////////////////////////////
Summary:

This sample contains code for using Web Services with Microsoft Dynamics NAV. 
It contains sample code for the following scenarios:
•	Invoking a NAV Codeunit
•	Creating an instance of a NAV Page
•	Read a full list of records
•	Read a filtere list of records
•	Read a single record
•	Insert a new record
•	Insert multiple new records
•	Modify a record
•	Modify multiple records
•	Delete a record
This sample is intended for casual NAV web service developers. The intention is to 
have sample code in one place to copy and paste. When you can’t remember the exact
syntax for initializing a web service, how to update a record or set a filter etc,
having a bit of code to copy and paste is sometimes all it needs to get started.


////////////////////////////////////////////////////////////////////////////////
Demo:

The solution included in this sample shows the functions that are available by 
publishing a NAV page or codeunit as a web service. The solution is developed 
with c# in Visual Studio 2010.
To run the project, you must first publish a page or a codeunit from NAV from 
Form 810 “Web Services”. The solution applies default web service connections in
 the InitializeComponent() – function:

InitializeComponent();
CodeunitURL.Text = "http://lohndorf02:7047/DynamicsNAV/WS/ ... etc            
PageURL.Text = "http://lohndorf02:7047/DynamicsNAV/WS/... etc

You must change this to match the web services that you have published on your own system. 
When you run the solution you will have two buttons – one for the codeunit web
service sample, and one for the page web service sample. These two samples run 
independently of each other, but before activating a sample, check that the URL for
that sample points to a valid NAV web service.
The codeunit web service sample only just initializes connection to a codeunit that
has been published as a web service, it doesn’t actually run anything since available
functions here will depend on the codeunit that was published.
To run the Page web service sample, you must also specify which type of sample to run
(e.g. Full List, Filtered List, etc.). Important: For this to work you must publish a 
page of type Card, for example Page 21 “Customer Card”. Only card pages have functions
for updating a record. These functions are not published with a list page. 
So if you publish a list page, then when trying to run any of the update functions 
would result in an error like “Method "CreateMultiple” is invalid!”.


////////////////////////////////////////////////////////////////////////////////
Implementation:

Below is a short description of what each of the functions in this sample does:

Initialize
Just sets up a connection to a web service

Full List
Reads all records in the customer table

Filtered List (simple)
Filters customers on Name containing “S”.

Filtered List (Advanced)
This also returns a filtered list. It uses a function called AddFilter to do so. 
This function can be found near the end of the solution. While this way of filtering 
requires a bit more code to begin with, the advantage is that after having this in place, 
additional filters can be added in just one line, where with the “Filtered List (Simple)”
-example uses four lines of code for applying each filter.

Read
Reads Customer No. 10000

Create
Inserts a record in the Customer table. Note that just like any other ways of inserting 
a record in a NAV database, the primary key value must be assigned. This example sets
Customer.”No.” = ‘ABC’. Removing this line (blanking out Customer.”No.”) would make it 
use the business logic (OnInsert trigger) instead to assign a number from the number series.

CreateMultiple
An example of inserting two records with one call. The two new records are set in an array 
first, and then together they are sent for insert. This example specifically sets Customer.”No.”
= null, in which case the OnInsert trigger takes care of assigning a number.

Update
Updates customer with No. ABC (assuming that this customer exists).

UpdateMultiple
This example first makes a filtered list, and then updates all records in this list. 

Delete
Deletes customer ABC


////////////////////////////////////////////////////////////////////////////////
References:
Further information and samples for coding with NAV and web services:

Microsoft Dynamics NAV Web Services
http://msdn.microsoft.com/en-us/library/dd355398.aspx

Freddy’s blog
http://blogs.msdn.com/b/freddyk/

NAV Team Blog
http://blogs.msdn.com/b/nav/archive/tags/web+services/

