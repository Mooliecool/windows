=============================================================================
                  VBASPNETEmbedLanguageInUrl Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The project illustrates how to embed the language code in URL such 
as http://domain/en-us/ShowMe.aspx. The page will display different
content according to the language code, the sample use url-routing 
and resource files to localize the content of web page.


/////////////////////////////////////////////////////////////////////////////
Demo the Sample. 

Please follow these demonstration steps below.

Step 1: Open the VBASPNETEmbedLanguageInUrl.sln.

Step 2: Expand the VBASPNETEmbedLanguageInUrl web application and press 
        Ctrl + F5 to start the web application. 

Step 3: We will see a normal English web page, we try to modify the value of
        address bar of browsers, for example, update the "en-us" to "zh-cn"

Step 4: If you update the url correctly, you will see a Chinese language 
        version of this web page. 

Step 5: Good, if you input a language we did not have in this web application,
        you will see a default language web page with English.

Step 6: Validation finished.


/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step 1. Create a VB "ASP.NET Empty Web Application" in Visual Studio 2010 or
        Visual Web Developer 2010. Name it as "VBASPNETEmbedLanguageInUrl".

Step 2. Add one folder, "XmlFolder". In order to display different language
        content in this web page, we need a database or an xml file to store
		our data, in this code-sample we need add a Language.xml file.

Step 3. Add three web forms in application's root directory, "ShowMe.aspx" page
        for display to customers, "InvalidPage.aspx" for handle error http 
		request, "Start.aspx" for start url routing.

Step 4. Add three class files, "BasePage.vb" class for check the request url 
        language part and name part, and set the page's Culture and UICultrue
        properties. "UrlRoutingHandler.vb" class for check the file name and 
		throw them to the InvalidPage.aspx page if it not existence. "XmlLoad.vb"
		class for load the xml data to display them in ShowMe.aspx page.
		[Note]
		If you want to create more web pages include multiple languages, please
        inherits the BasePage.vb class for set page's Cultrue and UICulture.

Step 5. Register url routes in Global.asax file.
        [code]
             Sub Application_Start(ByVal sender As Object, ByVal e As EventArgs)
                 RegisterRoutes(RouteTable.Routes)
             End Sub
             ''' <summary>
             ''' Url routing
             ''' </summary>
             ''' <param name="routes"></param>
             Public Shared Sub RegisterRoutes(ByVal routes As RouteCollection)
                 routes.Add("Page", New Route("{language}/{pageName}", New UrlRoutingHandlers()))
             End Sub
		[/code]
		After register url routes, we need create a UrlRoutingHandlers to check
		the request url. The UrlRoutingHanders code like this:
		[code]
             ''' <summary>
             ''' Create this RoutingHandler to check the HttpRequest and
             ''' return correct url path.
             ''' </summary>
             ''' <param name="context"></param>
             ''' <returns></returns>
             Public Function GetHttpHandler1(ByVal context As System.Web.Routing.RequestContext) 
			     As System.Web.IHttpHandler Implements System.Web.Routing.IRouteHandler.GetHttpHandler
                 Dim language As String = context.RouteData.Values("language").ToString().ToLower()
                 Dim pageName As String = context.RouteData.Values("pageName").ToString()
                 If pageName = "ShowMe.aspx" Then
                     Return TryCast(BuildManager.CreateInstanceFromVirtualPath("~/ShowMe.aspx", GetType(Page)), Page)
                 Else
                    Return BuildManager.CreateInstanceFromVirtualPath("~/InvalidPage.aspx", GetType(Page))
                 End If
             End Function
		[/code] 

Step 6. Create two resource files for support multiple language web page, 
        named "Resource.resx", "Resource.zh-cn.resx".

Step 7. Add some code in BasePage.vb, setting page.Culture and page.UICulture.
        [code]
		     ''' <summary>
             ''' The BasePage class used to set Page.Culture and Page.UICulture.
             ''' </summary>
             Protected Overrides Sub InitializeCulture()
                 Try
                     Dim language As String = RouteData.Values("language").ToString().ToLower()
                     Dim pageName As String = RouteData.Values("pageName").ToString()
                     Session("info") = language & "," & pageName
                     Page.Culture = language
                     Page.UICulture = language
                 Catch generatedExceptionName As Exception
				     Session("info") = "error,error"
                 End Try

             End Sub
		[/code]

Step 8. In ShowMe.aspx page we need add some xml data and resource file data.
        [code]
		     lbTitleContent.Text = strTitle

			 <asp:Literal ID="litTitle" runat="server" Text='<%$ Resources:Resource,Title %>'></asp:Literal>
		[/code] 

Step 9. Build the application and you can debug it.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Url Routing
http://msdn.microsoft.com/en-us/magazine/dd347546.aspx

MSDN: UrlRoutingHandler Class	
http://msdn.microsoft.com/en-us/library/system.web.routing.urlroutinghandler.aspx

MSDN: Resourse File
http://msdn.microsoft.com/en-us/library/ccec7sz1(VS.80).aspx


/////////////////////////////////////////////////////////////////////////////