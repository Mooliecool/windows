========================================================================
               CSASPNETControlPermissionForFiles Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The project illustrates how to control the permission for project files and 
folders on server, and protect them from being downloaded. Here we give a 
solution that when the web application receive a URL request, we will make a 
judgment that if the request file's extension name is not .jpg file then 
redirect to NoPermissionPage page. Also, user can not access the image file 
via copy URL. 


/////////////////////////////////////////////////////////////////////////////
Demo:

Please follow these demonstration steps below.

Step 1: Open the CSASPNETControlPermissionForFiles.sln.

Step 2: Expand the CSASPNETControlPermissionForFiles web application and press 
        Ctrl + F5 to show the Default.aspx.

Step 3: You will a ListView control on the page, you can visit the specifically
        image by click these links.

Step 4: If you click the txt file or copy the URL of image file for visiting.
        The Browser will redirect to NoPermissionPage.aspx page. 

Step 5: Validation finished.


/////////////////////////////////////////////////////////////////////////////
Implementation:

Step 1. Create a C# "ASP.NET Empty Web Application" in Visual Studio 2010 or
        Visual Web Developer 2010. Name it as "CSASPNETControlPermissionForFiles".

Step 2. Add two web forms in the root directory, name them as "Default.aspx", 
        "NoPermissionPage.aspx".

Step 3. Add two folders, and named them as "Files", and "XmlFile", put image files
        and text file in "Files" folder, Xml file in "XmlFile" folder.
		The XML file will like this:
		[code]
		<?xml version="1.0" encoding="utf-8" ?>
        <Root>
          <File>
              <Name>My Picture MSDN</Name>
              <ID>1</ID>
              <FilePath>Files/Image1.jpg</FilePath>
          </File>
          <File>
              <Name>My Picture Developers</Name>
              <ID>2</ID>
              <FilePath>Files/Image2.jpg</FilePath>
          </File>
          <File>
              <Name>My Picture ASP.NET</Name>
              <ID>3</ID>
              <FilePath>Files/Image3.jpg</FilePath>
          </File>
          <File>
              <Name>My Picture Microsoft</Name>
              <ID>4</ID>
              <FilePath>Files/Image4.jpg</FilePath>
          </File>
          <File>
              <Name>My Picture Bing</Name>
              <ID>5</ID>
              <FilePath>Files/Image5.jpg</FilePath>
          </File>
          <File>
              <Name>My Text File</Name>
              <ID>6</ID>
              <FilePath>Files/Text.txt</FilePath>
          </File>
        </Root>
		[/code]

Step 4. The NoPermissionPage.aspx page is used to show the error message to users.
        So you needn't add any code in this page, just add message in aspx page, 

Step 5. The Default.aspx page is used to show the data of XML file. we need add
        a ListView control as data control, The code as shown below:
		[code]
		/// <summary>
        /// Binding ListView control with XML files.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void Page_Load(object sender, EventArgs e)
        {
            XmlDocument xmlDocument = new XmlDocument();
            xmlDocument.Load(AppDomain.CurrentDomain.BaseDirectory + "XmlFile/PermissionFilesXml.xml");
            XmlNodeList nodeList = xmlDocument.SelectNodes("Root/File");
            DataTable tabNodes = new DataTable();
            tabNodes.Columns.Add("ID", Type.GetType("System.Int32"));
            tabNodes.Columns.Add("Name", Type.GetType("System.String"));
            tabNodes.Columns.Add("Path", Type.GetType("System.String"));
            foreach (XmlNode node in nodeList)
            {
                DataRow drTab = tabNodes.NewRow();
                drTab["ID"] = node["ID"].InnerText;
                drTab["Name"] = node["Name"].InnerText;
                drTab["Path"] = node["FilePath"].InnerText;
                tabNodes.Rows.Add(drTab);
            }
            ListView1.DataSource = tabNodes;
            ListView1.DataBind();
        }
		[/code]
		
Step 6  Add a Global.asax file and re-write Application_BeginRequest method.
		[code]
        /// <summary>
        /// The Application_BeginRequest method is used to make a judgment whether the request file 
        /// is jpg file, and throw illegal request to NoPermissionPage.aspx page.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void Application_BeginRequest(object sender, EventArgs e)
        {
            string path = HttpContext.Current.Request.Path; 
            string[] pathElements = path.Split('.');
            string extenseName = pathElements[pathElements.Length - 1];
            if (!extenseName.Equals("aspx",StringComparison.OrdinalIgnoreCase))
            {
                if (!extenseName.Equals("jpg",StringComparison.OrdinalIgnoreCase) || !IsUrl())
                {
                    HttpContext.Current.Response.Redirect("~/NoPermissionPage.aspx");
                }
            }
        }

        /// <summary>
        /// The method is used to check whether the page is opened by typing the URL in browser  
        /// </summary>
        /// <returns></returns>
        protected bool IsUrl()
        {
            string httpReferer = System.Web.HttpContext.Current.Request.ServerVariables["HTTP_REFERER"];
            string serverName = System.Web.HttpContext.Current.Request.ServerVariables["SERVER_NAME"];
            if ((httpReferer != null) && (httpReferer.IndexOf(serverName) == 7))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
	    [/code] 

Step 7. Build the application and you can debug it.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Global.Application_BeginRequest Method
http://msdn.microsoft.com/en-us/library/ee255086(BTS.10).aspx

MSDN: HttpRequest.ServerVariables Property 
http://msdn.microsoft.com/en-us/library/system.web.httprequest.servervariables.aspx

MSDN: XmlDocument Class
http://msdn.microsoft.com/en-us/library/system.xml.xmldocument.aspx


/////////////////////////////////////////////////////////////////////////////