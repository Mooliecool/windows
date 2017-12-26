=============================================================================
       Sivlerlight APPLICATION: CSSL4FragmentSearch Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The CSSL4FragmentSearch is a simple sample demonstrating the use of fragment 
navigation within Silverlight to perform a search.  The advantage this 
provides is that the search can then be saved by URL, and linked directly, 
with consistent behavior, allowing users to send links to direct results 
within Silverlight.


/////////////////////////////////////////////////////////////////////////////
Demo:

To run the sample, simply launch the project.  A web page will load, with the 
Silverlight fragment search component.  Type a search, and a Bing search 
filtered on Microsoft.com will be performed with the search terms.  Note that 
the URL will now include the search terms as a fragment, which can 
subsequently be bookmarked or copied and pasted into a new browser tab, after 
which time the same search will again be performed (as long as the URL 
remains valid - in the debug mode, as long as the project continues to run on 
the same port).


/////////////////////////////////////////////////////////////////////////////
Implementation:

The sample uses one simple Silverlight component, and all the relevant code 
is in MainPage.xaml, and its codebehind, MainPage.xaml.cs.  The code simply 
implements a fragment navigation event, which performs a web search with the 
Bing web service API, applying the fragment text as the search terms.

    <sdk:Frame x:Name="ContentFrame" FragmentNavigation="Frame_FragmentNavigation">

    private void Frame_FragmentNavigation(object sender, System.Windows.Navigation.FragmentNavigationEventArgs e)
    {
        results.Clear();

        Bing.SearchRequest sr = new Bing.SearchRequest();
        sr.Query = e.Fragment + " (site:microsoft.com)";
        sr.AppId = "1009092976966EFB6DD6B0F0B98FE5E617990903";
        sr.Sources = new SourceType[] { SourceType.Web };
        sr.Web = new Bing.WebRequest();
        Bing.BingPortTypeClient bing = new BingPortTypeClient();
        bing.SearchCompleted += new EventHandler<SearchCompletedEventArgs>(bing_SearchCompleted);
        bing.SearchAsync(sr);
    }


/////////////////////////////////////////////////////////////////////////////
References:

For more information about Silverlight fragment navigation, see 
http://msdn.microsoft.com/en-us/library/system.windows.controls.page.onfragmentnavigation%28VS.95%29.aspx.


/////////////////////////////////////////////////////////////////////////////