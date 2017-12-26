==============================================================================
 ASP.NET APPLICATION : CSASPNETBreadcrumbWithQueryString Project Overview
==============================================================================

//////////////////////////////////////////////////////////////////////////////
Summary:

By default, SiteMapPath control is very static. It only shows the nodes whose 
location can be matched in the site map. Sometimes we want to change SiteMapPath 
control's titles and paths according to Query String values. And sometimes we 
want to create the SiteMapPath dynamically. This code sample shows how to 
achieve these goals by handling SiteMap.SiteMapResolve event.

//////////////////////////////////////////////////////////////////////////////
Demo the Sample:

1. Open page Default.aspx, click a link in the categories list to navigate 
   to page Category.aspx, then click a link to navigate to page Item.aspx.
   you can see that the breadcrumb is showing the dynamic nodes according 
   to Query String values.

2. Open page DynamicBreadcrumb.aspx to see the dynamically created breadcrumb.

//////////////////////////////////////////////////////////////////////////////
Code Logical:

The point of this sample project is that we handle SiteMap.SiteMapResolve 
event to dynamically create/change current SiteMapNode.

	SiteMap.SiteMapResolve += new SiteMapResolveEventHandler(SiteMap_SiteMapResolve);

	SiteMapNode SiteMap_SiteMapResolve(object sender, SiteMapResolveEventArgs e)
    {
        // Only need one execution in one request.
        SiteMap.SiteMapResolve -= new SiteMapResolveEventHandler(SiteMap_SiteMapResolve);

        if (SiteMap.CurrentNode != null)
        {
            // SiteMap.CurrentNode is readonly, so we need to clone one to operate.
            SiteMapNode currentNode = SiteMap.CurrentNode.Clone(true);

            currentNode.Title = Request.QueryString["name"];

            // Use the changed one in the breadcrumb.
            return currentNode;
        }
        return null;
    }

//////////////////////////////////////////////////////////////////////////////
References:

SiteMapPath Web Server Control Overview
http://msdn.microsoft.com/en-us/library/x20z8c51.aspx

SiteMap Class
http://msdn.microsoft.com/en-us/library/system.web.sitemap.aspx

SiteMap.SiteMapResolve Event
http://msdn.microsoft.com/en-us/library/system.web.sitemap.sitemapresolve.aspx