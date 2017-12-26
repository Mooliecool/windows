========================================================================
    ASP.Net APPLICATION : CSASPNETAjaxExtender Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The CSASPNETAjaxExtender sample demonstrates how to create an ASP.Net Ajax 
ExtenderControl, which is a TimePicker to allow the user draging the 
minute/hour pointer to select a time of a day on a clock.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

Microsoft ASP.Net Ajax Extensions enables you to expand the capabilities of an 
ASP.Net Web Application in order to create a rich client user experience. 
To encapsulate the client behavior for use by ASP.NET page developers, you can 
use an extender control. 
An extender control is a Web server control that inherits the ExtenderControl 
abstract class in the System.Web.UI namespace. 

1. Creating an Extender Control.
   We can create ExtenderControl by using the template "Asp.Net Ajax Server Control Extender". 
   It will present to you a class file, a resource file(Actually, we don't need it in this sample.) 
   and a js file as default. We can create the extender in class file and create behavior in js file.
   
2. Extender control is used for client script functionality extension of an 
   existing web control. It can be applied to specific Web server control types. 
   You identify the types of Web server controls to which an extender control 
   can be applied by using the TargetControlTypeAttribute attribute.

   [TargetControlType(typeof(TextBox))]
   public class TimePicker: ExtenderControl


3. The following two methods of the ExtenderControl abstract class that you must 
   implement in an extender control.

   protected override IEnumerable<ScriptDescriptor> 
GetScriptDescriptors(Control targetControl)
   {
      ScriptControlDescriptor descriptor = new ScriptControlDescriptor
("CSASPNETAjaxExtender.TimePicker", targetControl.ClientID);

      descriptor.AddElementProperty("errorSpan", this.NamingContainer.FindControl
(ErrorPresentControlID).ClientID);
			
      descriptor.AddProperty("timeType", TimeType);

      descriptor.AddEvent("showing", OnClientShowing);

      yield return descriptor;
   }


   protected override IEnumerable<ScriptReference> GetScriptReferences()
   {
      yield return new ScriptReference(Page.ClientScript.GetWebResourceUrl
(this.GetType(), "CSASPNETAjaxExtender.TimePicker.TimePicker.js"));
   }

4. Embed Css reference in PreRender phase if you have a css style file to decorate the
   extender control.

   private void RenderCssReference()
   {
      string cssUrl = Page.ClientScript.GetWebResourceUrl
(this.GetType(), "CSASPNETAjaxExtender.TimePicker.TimePicker.css");

      HtmlLink link = new HtmlLink();
      link.Href = cssUrl;
      link.Attributes.Add("type", "text/css");
      link.Attributes.Add("rel", "stylesheet");
      Page.Header.Controls.Add(link);
   }
   
5. Set all resources(contain images, css file and js file) embedded in this extender
   control as "Embedded Resource"(property "Build Action").

6. The control can derive from other server controls if you want to make it inherit a
   server control than ExtenderControl. In this scenario, it should derive from
   IExtenderControl interface and a server control class. Meanwhile, we have another
   three steps need to do:
   1) Define TargetControl property
   2) Override OnPreRender method. Register the web control as the ExtenerControl in OnPreRender phase.
   
            ScriptManager manager = ScriptManager.GetCurrent(this.Page);
            if (manager == null)
            {
                throw new InvalidOperationException("A ScriptManager is required on the page.");
            }
            manager.RegisterExtenderControl<TimePicker>(this);
   3) Override Render method. Register the script descriptor which has been defined.
   
            ScriptManager.GetCurrent(this.Page).RegisterScriptDescriptors(this);

7. The rest work is on client-side. Register client NameSpace first.
   
   Type.registerNamespace("CSASPNETAjaxExtender");

8. Build client class. 

   CSASPNETAjaxExtender.TimePicker = function(element) {
    
   }

   CSASPNETAjaxExtender.TimePicker.prototype = {

   }

9. Register the class that inherits "Sys.UI.Behavior".
   
   CSASPNETAjaxExtender.TimePicker.registerClass('CSASPNETAjaxExtender.TimePicker', Sys.UI.Behavior);

10.Call base method in constructor method
   
   CSASPNETAjaxExtender.TimePicker.initializeBase(this, [element]);

11. Implementing the Initialize and Dispose Methods.

   Build "initialize" and "dispose" method in prototype of the class.The initialize 
   method is called when an instance of the behavior is created. Use this method to 
   set default property values, to create function delegates, and to add delegates 
   as event handlers. The dispose method is called when an instance of the behavior 
   is no longer used on the page and is removed. Use this method to free any resources 
   that are no longer required for the behavior, such as DOM event handlers.


   initialize: function() {
       CSASPNETAjaxExtender.TimePicker.callBaseMethod(this, 'initialize');       

   },

   dispose: function() {        
       CSASPNETAjaxExtender.TimePicker.callBaseMethod(this, 'dispose');
   }

12. Defining the Property Get and Set Methods.

   Each property identified in the ScriptDescriptor object of the extender control's 
   GetScriptDescriptors(Control) method must have corresponding client accessors. 
   The client property accessors are defined as get_<property name> and set_<property name> 
   methods of the client class prototype.


   get_timeType: function() {
       return this._timeType;
   },

   set_timeType: function(val) {
       if (this._timeType !== val) {
           this._timeType = val;
           this.raisePropertyChanged('timeType');
       }
   },

13. Defining the Event Handlers for the DOM Element
   1) Defining the handler in constructor function:
        this._element_focusHandler = null;
   2) Associate the handler with the DOM Element event in initailize method:
		this._element_focusHandler = Function.createDelegate(this, this._element_onfocus);
   3) Add the handler in initailize method:
		$addHandler(this.get_element(), 'focus', this._element_focusHandler) 
   4) Build callback method about this event:
		_element_onfocus:function(){

		}

14. Defining the Event Handlers for the behavior

    Each event identified in the ScriptDescriptor object of the extender control's 
    GetScriptDescriptors(Control) method must have corresponding client accessors. 
    The client event accessors are defined as add_<event name> and remove_<event name> 
    methods of the client class prototype. 
    The method Raise<event name> is defined to trigger the event.  

    add_showing: function(handler) {
        this.get_events().addHandler("showing", handler);
    },
    remove_showing: function(handler) {

        this.get_events().removeHandler("showing", handler);
    },
    raiseShowing: function(eventArgs) {

        var handler = this.get_events().getHandler('showing');
        if (handler) {
            handler(this, eventArgs);
        }
    },
    
15. Use this extender control TimePicker in page.
    The usage of the extender control is the same to the custom control.    
	1) Register the assembly in page.
	   <%@ Register TagPrefix="CSASPNETAjaxExtender" Assembly="CSASPNETAjaxExtender" 
	   Namespace="CSASPNETAjaxExtender" %>
	2) Add a ScriptManager control in page, and create TimePicker control to bind on a TextBox.
	
	   <asp:TextBox ID="TextBox1" Text="" runat="server"></asp:TextBox>
	   <CSASPNETAjaxExtender:TimePicker runat="server" ID="t1" TargetControlID="TextBox1" TimeType="H24" />


/////////////////////////////////////////////////////////////////////////////
References:

Creating an Extender Control to Associate a Client Behavior with a Web Server Control
http://www.asp.net/AJAX/Documentation/Live/tutorials/ExtenderControlTutorial1.aspx

/////////////////////////////////////////////////////////////////////////////