================================================================================
	   Windows APPLICATION: CSWebBrowserAutomation Overview                        
================================================================================

////////////////////////////////////////////////////////////////////////////////
Summary:

The sample demonstrates how to create a WebBrowser which supplies following features
1. Manipulate the html elements and login a website automatically.
2. Block specified sites.


////////////////////////////////////////////////////////////////////////////////
Demo:

Step1. Open the project in VS2010, replace the stored UserName and Password in 
      StoredSites\www.codeplex.com.xml with your username and password for 
	  http://www.codeplex.com first.

Step2. Build this project in VS2010. 

Step3. Run CSWebBrowserAutomation.exe. The button "Auto Complete" is disabled by default.

Step4. Type https://www.codeplex.com/site/login?RedirectUrl=https%3a%2f%2fwww.codeplex.com%2fsite%2fusers%2fupdate
	   in the Url textbox and press Enter.

	   This url is the login page of www.codeplex.com. The RedirectUrl means that the
	   page will be redirected to the url if you login the site successfully.

Step5. After the web page is loaded completed, the button "Auto Complete" is enabled. Click
	   the button and the web page will be be redirected to 
	   https://www.codeplex.com/site/users/update.

Step6. After the new web page is loaded, click the button "Auto Complete" again, and the 
	   "New email address" field in the web page will be filled.

Step7. Type http://www.contoso.com  in the urltext box and press Enter. You will view a 
       page that show you a message "Sorry, this site is blocked."


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Design a class XMLSerialization that can serialize an object to an XML file or 
   deserialize an XML file to an object.

2. Design classes HtmlCheckBox, HtmlPassword, HtmlSubmit and HtmlText that represent the
   checkbox, password text box, submit button and normal text box. All the classes inherit
   HtmlInputElement that represents an HtmlElement with the tag "input". The class 
   HtmlInputElementFactory is used to get an HtmlInputElement from an HtmlElement in the 
   web page. 

3. Design a class StoredSite that represents a site whose html elements are stored. A site
   is stored as an XML file under StoredSites folder, and can be deserialized.

   This class also supplies a method FillWebPage to complete the web page automatically.
   If a submit button could be found, then the button will also be clicked automatically.

4. Design a class BlockSites which contains that blocked sites list. The file 
   \Resource\BlockList.xml can be deserialized to a BlockSites instance.

5. Design a class WebBrowserEx that inherits class System.Windows.Forms.WebBrowser. 

   Override the OnNavigating method to check whether the url is included in the block list.
   If so, navigate the build-in Block.htm.

   Override the OnDocumentCompleted method to check whether the loaded page could be completed
   automatically. If the site and url are stored, then the method AutoComplete can be used.


/////////////////////////////////////////////////////////////////////////////

References:
http://msdn.microsoft.com/en-us/library/system.xml.serialization.xmlserializer.aspx
http://msdn.microsoft.com/en-us/library/system.windows.forms.webbrowser.aspx


/////////////////////////////////////////////////////////////////////////////
