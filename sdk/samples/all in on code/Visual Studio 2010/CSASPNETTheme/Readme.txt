========================================================================
    ASP.Net APPLICATION : CSASPNETTheme Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The CSASPNETTheme sample demonstrates how to create an ASP.Net with themes, 
which can be applied during the page inital progress.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

A theme is a collection of property settings that allow you to define the look 
of pages and controls, and then apply the look consistently across pages 
in a Web application, across an entire Web application, or across all Web 
applications on a server.

Themes are made up of a set of elements: skins, cascading style sheets (CSS), 
images, and other resources. At a minimum, a theme will contain skins. 
Themes are defined in special directories in your Web site or on your Web server.

You can define themes for a single Web application, or as global themes 
that can be used by all applications on a Web server. After a theme is defined, 
it can be placed on individual pages using the Theme or StyleSheetTheme attribute 
of the @ Page directive, or it can be applied to all pages in an application 
by setting the <pages> element in the application configuration file. 
If the <pages> element is defined in the Machine.config file, 
the theme will apply to all pages in Web applications on the server.

This sample contain 2 themes, which can be changed during page initial period.
There are 2 buttons on the default page. Once they are clicked, a redirect to 
default page happens which including the theme name. While the page is initailizating,
the theme parament will be cought, and new theme will be applied.

Step 1: Create a C# ASP.NET Web Application Project 
	in Visual Studio 2010 or Visual Web Developer 2010.
	Name it as CSASPNETTheme.

Step 2: Open Default.aspx, add following controls:

	<asp:Image ID="Image1" SkinId="MainTheme" runat="server" 
	    ImageUrl="~/images/Blank.jpg" />
    <asp:Button ID="Button1" SkinId="BlueTheme" 
        runat="server" Text="Blue" ForeColor="Blue" BackColor="Azure"/>
    <asp:Button ID="Button2" SkinId="PinkTheme"
        runat="server" Text="Pink" ForeColor="Pink" BackColor="Red"/>

Step 3: Right click on project in Solution Explorer, go to Add then Add ASP.Net 
	Folder, then App_Themes. Right click on App_Theme in Solution Explorer, 
	go to Add then Add ASP.Net Folder, then Theme to add a theme. 
	Add two themes and rename them as BlueTheme and PinkTheme.
	Add a skin file and a CSS file in each theme.

Step 4: Open skin file in the BlueTheme, copy and paste following codes:

	<asp:Image runat="server" SkinId="MainTheme" ImageUrl="~/images/Blue.jpg"/>
	<asp:Button SkinId="BlueTheme" runat="server" Visible="false"/>
	<asp:Button SkinId="PinkTheme" runat="server" Visible="true"/>

	NOTE: A skin file contains property settings for individual controls.

Step 5: Open CSS file in the BlueTheme, copy and paste following codes in body:

	background:blue

	NOTE: Cascading Style Sheets is a simple mechanism for adding style to Web documents.

Step 6: Add PinkTheme defines similar to Step 3 & 4.

Step 7: Double click on the Blue button on default.aspx copy and paste following
    codes to Button1_Click(object sender, EventArgs e):

		Response.Redirect("~/default.aspx?theme=Blue");

Step 8: Add Pink button actions similar to Step 7.

Step 9: Open Defult.aspx.cs copy and paste following codes in the partial class:

	protected void Page_PreInit(object sender, EventArgs e)
    {
        switch (Request.QueryString["theme"])
        {
            case "Blue":
               Page.Theme = "BlueTheme";
            break;
            case "Pink":
               Page.Theme = "PinkTheme";
            break;
       }
    } 
	       	
/////////////////////////////////////////////////////////////////////////////
References:

ASP.NET Themes and Skins
http://msdn.microsoft.com/en-us/library/ykzx33wh.aspx?PHPSESSID=8415f84585668e69ce791db4abfd0c45

How to: Define ASP.NET Page Themes
http://msdn.microsoft.com/en-us/library/ms247256.aspx?PHPSESSID=8415f84585668e69ce791db4abfd0c45

Cascading Style Sheets
http://www.w3.org/Style/CSS/
/////////////////////////////////////////////////////////////////////////////