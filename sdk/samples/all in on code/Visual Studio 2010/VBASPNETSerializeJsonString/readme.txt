========================================================================
  ASP.NET APPLICATION :  VBASPNETSerializeJsonString Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

  This project illustrates how to serialize Json string. we use jQuery at client 
side and manipulate XML data at server side.

  It demonstrates how to use the serializable json data through an autocomplete 
example. 


/////////////////////////////////////////////////////////////////////////////
Demo the Sample. 

Open the VBASPNETSerializeJsonString.sln directly, expand the web application 
node and press F5 to test the application.

Step 1.  View default.aspx in browser

Step 2.  By default, we could see  a search input textbox at the top of the page, 
	 you can enter a character, for example "m", you will see an autocomplete 
	 list under that input, move mouse to select one book name, then you'll 
	 find this book's related information was display in the result area.


/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step 1.  Create a VB ASP.NET Empty Web Application in Visual Studio 2010.

Step 2.  Add a VB class file which named 'Book' in Visual Studio 2010, declare
	 the class members: id,lable,value,author,genre,price,publish_date,description. 
	 this class is used to store the book's information

Step 3.  Add a VB ashx file which named 'AutoComplete' in Visual Studio 2010. 
	 write code in method 'ProcessRequest', the logic as below:

	 1, load a flat XML dataset and filter dataset records
	 2, assign corresponding fields to the class Book' members
	 3, initializes a new instance of the 'Collection<Book>' class, add
	    new elements into 'Collection<Book>'
	 4, serialize the object 'Collection<Book>'     


Step 4.  Create a new directory, "Scripts". Right-click the directory and click
         Add -> New Item -> JScript File. We need to reference jquery javascript 
	 library files to support AutoComplete effect.
	 files in this sample: jquery.min.js,jquery-ui.min.js


Step 5.  Create a new directory, "Styles". Right-click the directory and click
         Add -> New Item -> Style Sheet File. reference jquery UI style files called 
	 jquery-ui.css. To make the sample looks better, there refers one other UI 
	 markups called site.css.
		 

Step 6.  Open the Default.aspx,(If there is no Default.aspx, create one.)
         In the Head block, add javascript and style references like below.
	 [CODE]
    	 <link rel="stylesheet" href="Styles/jquery-ui.css" type="text/css" media="all" />
    	 <link rel="stylesheet" href="Styles/site.css" type="text/css" />
    	 <script type="text/javascript" src="Scripts/jquery.min.js"></script>
    	 <script type="text/javascript" src="Scripts/jquery-ui.min.js"></script>
	 [/CODE]

	 write the autocomplete javascript as below.
	 [CODE]
	 <script type="text/javascript">
         $(function () {
            $('#<%= tbBookName.ClientID %>').autocomplete({
                source: "AutoComplete.ashx",
                select: function (event, ui) {
                    
                    $(".author").text(ui.item.Author);
                    $(".genre").text(ui.item.Genre);
                    $(".price").text(ui.item.Price);
                    $(".publish_date").text(ui.item.Publish_date);
                    $(".description").text(ui.item.Description);
                }
            });
         });
    	 </script>
	 [CODE]		
		 
	 For more details, please refer to the Default.aspx in this sample.

Step 7.  Everything is ready, test the application and hope you can succeed. 


/////////////////////////////////////////////////////////////////////////////
References:

http://msdn.microsoft.com/en-us/library/system.web.script.serialization.javascriptserializer.aspx