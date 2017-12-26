=============================================================================
                  CSASPNETEmbedLanguageInUrl Overview
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

Step 1: Open the CSASPNETEmbedLanguageInUrl.sln.

Step 2: Expand the CSASPNETEmbedLanguageInUrl web application and press 
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

Step 1. Create a C# "ASP.NET Empty Web Application" in Visual Studio 2010 or
        Visual Web Developer 2010. Name it as "CSASPNETEmbedLanguageInUrl".

Step 2. Add one folder, "XmlFolder". In order to display different language
        content in this web page, we need a database or an xml file to store
		our data, in this code-sample we need add a Language.xml file.

Step 3. Add three web forms in application's root directory, "ShowMe.aspx" page
        for display to customers, "InvalidPage.aspx" for handle error http 
		request, "Start.aspx" for start url routing.

Step 4. Add three class files, "BasePage.cs" class for check the request url 
        language part and name part, and set the page's Culture and UICultrue
        properties. "UrlRoutingHandler.cs" class for check the file name and 
		throw them to the InvalidPage.aspx page if it not existence. "XmlLoad.cs"
		class for load the xml data to display them in ShowMe.aspx page.
		[Note]
		If you want to create more web pages include multiple languages, please
        inherits the BasePage.cs class for set page's Cultrue and UICulture.

Step 5. Register url routes in Global.asax file.
		[code]
             protected void Application_Start(object sender, EventArgs e)
             {
                 RegisterRoutes(RouteTable.Routes);
             }

             public static void RegisterRoutes(RouteCollection routes)
             {
                 routes.Add("Page", new Route("{language}/{pageName}", new UrlRoutingHandlers()));
             }
		[/code]
		After register url routes, we need create a UrlRoutingHandlers to check
		the request url. The UrlRoutingHanders code like this:
		[code]
              /// <summary>
              /// Create this RoutingHandler to check the HttpRequest and
              /// return correct url path.
              /// </summary>
              /// <param name="context"></param>
              /// <returns></returns>
              public IHttpHandler GetHttpHandler(RequestContext context)
              {
                  string language = context.RouteData.Values["language"].ToString().ToLower();
                  string pageName = context.RouteData.Values["pageName"].ToString();
                  if (pageName == "ShowMe.aspx")
                  {
                      return BuildManager.CreateInstanceFromVirtualPath("~/ShowMe.aspx", typeof(Page)) as Page;
                  }
                  else
                  {
                      return BuildManager.CreateInstanceFromVirtualPath("~/InvalidPage.aspx", typeof(Page)) as Page;
                  }
              }
		[/code] 

Step 6. Create two resource files for support multiple language web page, 
        named "Resource.resx", "Resource.zh-cn.resx".

Step 7. Add some code in BasePage.cs, setting page.Culture and page.UICulture.
        [code]
		     /// <summary>
             /// The BasePage class used to set Page.Culture and Page.UICulture.
             /// </summary>
             protected override void InitializeCulture()
             {
                 try
                 {
                     string language = RouteData.Values["language"].ToString().ToLower();
                     string pageName = RouteData.Values["pageName"].ToString();
                     Session["info"] = language + "," + pageName;
                     Page.Culture = language;
                     Page.UICulture = language;
                 }
                 catch (Exception)
                 {
				      Session["info"] = "error,error"; 
				 }
             }
		[/code]

Step 8. In ShowMe.aspx page we need add some xml data and resource file data.
        [code]
		     lbTitleContent.Text = strTitle;

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
http://msdn.microsoft.com/en-us/library/ccec7sz1.aspx


/////////////////////////////////////////////////////////////////////////////