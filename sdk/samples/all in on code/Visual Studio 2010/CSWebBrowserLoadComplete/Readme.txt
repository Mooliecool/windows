================================================================================
       Windows APPLICATION: CSWebBrowserLoadComplete Overview                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The sample demonstrates how to determine when a page is done loading in webBrowser
control.

In the case of a page with no frames, the DocumentComplete event is fired once after
everything is done. In case of multiple frames, the DocumentComplete event gets fired
multiple times.

So, to check if a page is done loading, you need to check if sender of the event
is the same as the WebBrowser control.

NOTE:
1. Within one frame of the frameset, if the user clicks on a link that opens a new page 
   in the frame itself and keeps the rest of the frameset intact, the LoadCompleted
   event of the WebBrowser event will not be fired, you have to check the DocumentComplete
   event of the specified frame. 

2. If you visit some pages, such as http://www.microsoft.com, you may find that the 
   LoadCompleted event is not the latest event. This is because that the page may 
   load other links by itself after the page is done loading.


////////////////////////////////////////////////////////////////////////////////
Demo:

Step1. Run CSWebBrowserLoadComplete.exe.

Step2. The default url in the Textbox is the path of "\Resource\FramesPage.htm".
       Click the button "Go". 

       The FramesPage.htm contains 3 frames. After the page is loaded, you will get
       following message in the buttom of the form.

       DocumentCompleted:4 LoadCompleted:1.

       This meaages means that the DocumentCompleted evet was fired 4 times, and the
       LoadCompleted event was fired once. 

       And you can also get the detailed activity records in the list box.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. The interface DWebBrowserEvents2 designates an event sink interface that an application
   must implement to receive event notifications from a WebBrowser control or from the 
   Windows Internet Explorer application. The event notifications include DocumentCompleted
   and BeforeNavigate2 events that will be used in this application.

       [ComImport, TypeLibType(TypeLibTypeFlags.FHidden), 
       InterfaceType(ComInterfaceType.InterfaceIsIDispatch),
       Guid("34A715A0-6587-11D0-924A-0020AFC7AC4D")]
       public interface DWebBrowserEvents2
       {
           /// <summary>
           /// Fires when a document is completely loaded and initialized.
           /// </summary>
           [DispId(259)]
           void DocumentComplete(
               [In, MarshalAs(UnmanagedType.IDispatch)] object pDisp, 
               [In] ref object URL);
       
       
           [DispId(250)]
           void BeforeNavigate2(
               [In, MarshalAs(UnmanagedType.IDispatch)] object pDisp,
               [In] ref object URL, 
               [In] ref object flags, 
               [In] ref object targetFrameName, 
               [In] ref object postData, 
               [In] ref object headers,
               [In, Out] ref bool cancel);
               
       }

2. The class DWebBrowserEvents2Helper implements the DWebBrowserEvents2 interface to check
   whether the page is done loading.
   
   If the WebBrowser control is hosting a normal html page without frame, the 
   DocumentComplete event is fired once after everything is done.
   
   If the WebBrowser control is hosting a frameset. DocumentComplete gets fired multiple
   times. The DocumentComplete event has a pDisp parameter, which is the IDispatch of the 
   frame (shdocvw) for which DocumentComplete is fired. 
   
   Then we could check if the pDisp parameter of the DocumentComplete is the same
   as the ActiveXInstance of the WebBrowser.


            private class DWebBrowserEvents2Helper : StandardOleMarshalObject, DWebBrowserEvents2
            {
                private WebBrowserEx parent;

                public DWebBrowserEvents2Helper(WebBrowserEx parent)
                {
                    this.parent = parent;
                }
            
                public void DocumentComplete(object pDisp, ref object URL)
                {
                    string url = URL as string;
               
                    if (string.IsNullOrEmpty(url)
                        || url.Equals("about:blank", StringComparison.OrdinalIgnoreCase))
                    {
                        return;
                    }
               
                    if (pDisp != null && pDisp.Equals(parent.ActiveXInstance))
                    {
                        var e = new WebBrowserDocumentCompletedEventArgs(new Uri(url)); 
               
                        parent.OnLoadCompleted(e);
                    }
                }
               
                public void BeforeNavigate2(object pDisp, ref object URL, ref object flags,
                    ref object targetFrameName, ref object postData, ref object headers,
                    ref bool cancel)
                {
                    string url = URL as string;
               
                    if (string.IsNullOrEmpty(url)
                        || url.Equals("about:blank", StringComparison.OrdinalIgnoreCase))
                    {
                        return;
                    }
               
                    if (pDisp != null && pDisp.Equals(parent.ActiveXInstance))
                    {
                        WebBrowserNavigatingEventArgs e = new WebBrowserNavigatingEventArgs(
                            new Uri(url), targetFrameName as string);
               
                        parent.OnStartNavigating(e);
                    }
                }
            }
            
3. The WebBrowserEx class inherits WebBrowser class and supplies StartNavigating and
   LoadCompleted event.

         public partial class WebBrowserEx : WebBrowser
    {
        AxHost.ConnectionPointCookie cookie;

        DWebBrowserEvents2Helper helper;

        public event EventHandler<WebBrowserNavigatingEventArgs> StartNavigating;

        public event EventHandler<WebBrowserDocumentCompletedEventArgs> LoadCompleted;

        /// <summary>
        /// Associates the underlying ActiveX control with a client that can 
        /// handle control events including NavigateError event.
        /// </summary>
        protected override void CreateSink()
        {

            base.CreateSink();

            helper = new DWebBrowserEvents2Helper(this);
            cookie = new AxHost.ConnectionPointCookie(
                this.ActiveXInstance, helper, typeof(DWebBrowserEvents2));         
        }

        /// <summary>
        /// Releases the event-handling client attached in the CreateSink method
        /// from the underlying ActiveX control
        /// </summary>
        protected override void DetachSink()
        {
            if (cookie != null)
            {
                cookie.Disconnect();
                cookie = null;
            }
            base.DetachSink();
        }

        /// <summary>
        /// Raise the LoadCompleted event.
        /// </summary>
        protected virtual void OnLoadCompleted(WebBrowserDocumentCompletedEventArgs e)
        {
            if (LoadCompleted != null)
            {
                this.LoadCompleted(this, e);
            }
        }

        /// <summary>
        /// Raise the StartNavigating event.
        /// </summary>
        protected virtual void OnStartNavigating(WebBrowserNavigatingEventArgs e)
        {
            if (StartNavigating != null)
            {
                this.StartNavigating(this, e);
            }
        }
    }
      
4. The MainFrom is the UI of this application. If the WebBroeser was navigated to an
   URL, it will display the count of how many times DocumentCompleted event were fired
   and how many times LoadCompleted event were fired, also includes the detailed 
   activity records.


/////////////////////////////////////////////////////////////////////////////
References:

How To Determine When a Page Is Done Loading in WebBrowser Control
http://support.microsoft.com/kb/q180366/

DWebBrowserEvents2 Interface
http://msdn.microsoft.com/en-us/library/aa768283(VS.85).aspx

WebBrowser.CreateSink Method 
http://msdn.microsoft.com/en-us/library/system.windows.forms.webbrowser.createsink.aspx


/////////////////////////////////////////////////////////////////////////////
