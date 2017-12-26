================================================================================
       Windows APPLICATION: VBWebBrowserLoadComplete Overview                        
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

Step1. Run VBWebBrowserLoadComplete.exe.

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

       <ComImport(),
       TypeLibType(TypeLibTypeFlags.FHidden),
       InterfaceType(ComInterfaceType.InterfaceIsIDispatch),
       Guid("34A715A0-6587-11D0-924A-0020AFC7AC4D")>
       Public Interface DWebBrowserEvents2
          ''' <summary>
          ''' Fires when a document is completely loaded and initialized.
          ''' </summary>
          <DispId(259)>
          Sub DocumentComplete(<[In](), MarshalAs(UnmanagedType.IDispatch)> ByVal pDisp As Object,
                               <[In]()> ByRef URL As Object)
      
          <DispId(250)>
          Sub BeforeNavigate2(<[In](), MarshalAs(UnmanagedType.IDispatch)> ByVal pDisp As Object,
                              <[In]()> ByRef URL As Object,
                              <[In]()> ByRef flags As Object,
                              <[In]()> ByRef targetFrameName As Object,
                              <[In]()> ByRef postData As Object,
                              <[In]()> ByRef headers As Object,
                              <[In](), Out()> ByRef cancel As Boolean)
       End Interface

2. The class DWebBrowserEvents2Helper implements the DWebBrowserEvents2 interface to check
   whether the page is done loading.
   
   If the WebBrowser control is hosting a normal html page without frame, the 
   DocumentComplete event is fired once after everything is done.
   
   If the WebBrowser control is hosting a frameset. DocumentComplete gets fired multiple
   times. The DocumentComplete event has a pDisp parameter, which is the IDispatch of the 
   frame (shdocvw) for which DocumentComplete is fired. 
   
   Then we could check if the pDisp parameter of the DocumentComplete is the same
   as the ActiveXInstance of the WebBrowser.


       Private Class DWebBrowserEvents2Helper
        Inherits StandardOleMarshalObject
        Implements DWebBrowserEvents2

        Private parent As WebBrowserEx

        Public Sub New(ByVal parent As WebBrowserEx)
            Me.parent = parent
        End Sub

        ''' <summary>
        ''' Fires when a document is completely loaded and initialized.
        ''' If the frame is the top-level frame / window element, then the page is
        ''' done loading.
        ''' 
        ''' Then reset the glpDisp to null after the WebBrowser is done loading.
        ''' </summary>
        Public Sub DocumentComplete(ByVal pDisp As Object, ByRef URL As Object) _
             Implements DWebBrowserEvents2.DocumentComplete

            Dim _url As String = TryCast(URL, String)

            If String.IsNullOrEmpty(_url) OrElse _
                _url.Equals("about:blank", StringComparison.OrdinalIgnoreCase) Then
                Return
            End If

            If pDisp IsNot Nothing AndAlso pDisp.Equals(parent.ActiveXInstance) Then
                Dim e = New WebBrowserDocumentCompletedEventArgs(New Uri(_url))

                parent.OnLoadCompleted(e)
            End If
        End Sub

        ''' <summary>
        ''' Fires before navigation occurs in the given object 
        ''' (on either a window element or a frameset element).
        ''' 
        ''' </summary>
        Public Sub BeforeNavigate2(ByVal pDisp As Object,
                                   ByRef URL As Object,
                                   ByRef flags As Object,
                                   ByRef targetFrameName As Object,
                                   ByRef postData As Object,
                                   ByRef headers As Object,
                                   ByRef cancel As Boolean) _
                               Implements DWebBrowserEvents2.BeforeNavigate2

            Dim _url As String = TryCast(URL, String)

            If String.IsNullOrEmpty(_url) OrElse _
                _url.Equals("about:blank", StringComparison.OrdinalIgnoreCase) Then
                Return
            End If

            If pDisp IsNot Nothing AndAlso pDisp.Equals(parent.ActiveXInstance) Then
                Dim e As New WebBrowserNavigatingEventArgs(
                    New Uri(_url), TryCast(targetFrameName, String))

                parent.OnStartNavigating(e)
            End If
        End Sub


    End Class
            
3. The WebBrowserEx class inherits WebBrowser class and supplies StartNavigating and
   LoadCompleted event.

         <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust"),
         PermissionSetAttribute(SecurityAction.InheritanceDemand, Name:="FullTrust")>
         Partial Public Class WebBrowserEx
             Inherits WebBrowser
             Private cookie As AxHost.ConnectionPointCookie
         
             Private helper As DWebBrowserEvents2Helper
         
             Public Event LoadCompleted As EventHandler(Of WebBrowserDocumentCompletedEventArgs)
         
             Public Event StartNavigating As EventHandler(Of WebBrowserNavigatingEventArgs)
         
             ''' <summary>
             ''' Associates the underlying ActiveX control with a client that can 
             ''' handle control events including NavigateError event.
             ''' </summary>
             Protected Overrides Sub CreateSink()
                 MyBase.CreateSink()
         
                 helper = New DWebBrowserEvents2Helper(Me)
                 cookie = New AxHost.ConnectionPointCookie(
                     Me.ActiveXInstance, helper, GetType(DWebBrowserEvents2))
             End Sub
         
             ''' <summary>
             ''' Releases the event-handling client attached in the CreateSink method
             ''' from the underlying ActiveX control
             ''' </summary>
             Protected Overrides Sub DetachSink()
                 If cookie IsNot Nothing Then
                     cookie.Disconnect()
                     cookie = Nothing
                 End If
                 MyBase.DetachSink()
             End Sub
         
             ''' <summary>
             ''' Raise the LoadCompleted event.
             ''' </summary>
             Protected Overridable Sub OnLoadCompleted(ByVal e As WebBrowserDocumentCompletedEventArgs)
         
                 RaiseEvent LoadCompleted(Me, e)
             End Sub
         
             ''' <summary>
             ''' Raise the StartNavigating event.
             ''' </summary>
             Protected Overridable Sub OnStartNavigating(ByVal e As WebBrowserNavigatingEventArgs)
                 RaiseEvent StartNavigating(Me, e)
             End Sub
         End Class
      
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
