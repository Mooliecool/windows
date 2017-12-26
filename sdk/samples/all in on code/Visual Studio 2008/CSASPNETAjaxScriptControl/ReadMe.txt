========================================================================
    ASP.Net APPLICATION : CSASPNETAjaxScriptControl Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The CSASPNETAjaxScriptControl sample demonstrates how to create an ASP.Net Ajax 
ScriptControl, which is a Schedule to allow the user arrange tasks in calendar.


/////////////////////////////////////////////////////////////////////////////
Prerequisite:

ASP.NET AJAX Control Toolkit
http://www.codeplex.com/AjaxControlToolkit


/////////////////////////////////////////////////////////////////////////////
Code Logic:

Microsoft ASP.Net Ajax Extensions enables you to expand the capabilities of an 
ASP.Net Web Application in order to create a rich client user experience. We 
can make use of ScriptControl or ExtenderControl to build a rich client behavior 
or web control. The difference between ExtenderControl and ScriptControl is that 
Extender is used on creating client script capabilities on an existing control, 
which is called "TargetControl" for this behavior, whereas ScriptControl is an 
absolute web control which contains rich client functionality.For example, when 
we'd like to build a ModalPopup which will pop out an existing Panel, show/hide 
functionality is the client script application, then we can build it as 
ExtenderControl. 

However, for ScriptControl, for instance, TabContainer which is the entirely 
new web control contains the client script functionality, so we can build it 
as ScriptControl.

1. To build a web server control with rich client experiences by ASP.NET page 
developers, you can use an script control.(Create ASP.Net Ajax Server Control 
project by File->New->Project->Web->ASP.Net Ajax Server Control in Visual Studio) 
An script control is a web server control that inherits the ScriptControl abstract 
class in the System.Web.UI namespace. 
Script control is used for establishing a web server control which contains rich 
client capability. 

public class Schedule : ScriptControl

2. The following two methods of the ScriptControl abstract class that you must 
implement in an script control.

        public IEnumerable<ScriptDescriptor> GetScriptDescriptors()
        {
            ScriptControlDescriptor descriptor = new ScriptControlDescriptor
("PainControls.Schedule", this.ClientID);
            descriptor.AddElementProperty("toolContainer", ToolContainer.ClientID);

            descriptor.AddElementProperty("dateTimePicker", 
DateTimePicker.ClientID);
            descriptor.AddElementProperty("calendarContainer", 
CalendarContainer.ClientID);
            descriptor.AddProperty("calendarCellContentCssClass", 
CalendarCellContentCssClass);
            if (string.IsNullOrEmpty(ServicePath))
                throw new Exception("Please set ServicePath property.");
            descriptor.AddProperty("servicePath", ServicePath);
            if (string.IsNullOrEmpty(UpdateServiceMethod))
                throw new Exception("Please set UpdateServiceMethod property.");
            descriptor.AddProperty("updateServiceMethod", UpdateServiceMethod);
            if (string.IsNullOrEmpty(DeleteServiceMethod))
                throw new Exception("Please set DeleteServiceMethod property.");
            descriptor.AddProperty("deleteServiceMethod", DeleteServiceMethod);

            

            List<string> aa = new List<string>();
            for (int i = 0; i < DropPanelClientIDCollection.Count; i++)
            {
                string a = DropPanelClientIDCollection[i].ClientID;
                aa.Add(a);
            }
         
            descriptor.AddProperty("dropPanelClientIDCollection", aa);
           
            descriptor.AddProperty("dateTimeFieldName", DateTimeFieldName);
            descriptor.AddProperty("titleFieldName", TitleFieldName);
            descriptor.AddProperty("descriptionFieldName", DescriptionFieldName);
            yield return descriptor;
        }

        // Generate the script reference
        public IEnumerable<ScriptReference> GetScriptReferences()
        {

            yield return new ScriptReference(Page.ClientScript.GetWebResourceUrl
(this.GetType(), "PainControls.Schedule.Schedule.js"));

        }

3. Embed Css reference in PreRender phase.

private void RenderCssReference()
{
string cssUrl = Page.ClientScript.GetWebResourceUrl(this.GetType(), 
"PainControls. Schedule. Schedule.css");
HtmlLink link = new HtmlLink();
link.Href = cssUrl;
link.Attributes.Add("type", "text/css");
link.Attributes.Add("rel", "stylesheet");
Page.Header.Controls.Add(link);
}

4. Set all resources(contain images, css file and js file) embedded in this 
script control as "Embedded Resource"(property "Build Action").

5. This script control can derive from IScriptControl interface and a server 
control, instead of ScriptControl if you'd like to.

The control can derive from other server controls if you want to make it inherit 
a server control than ScriptControl. 
For example: 

public class Schedule : DataBoundControl, IScriptControl,INamingContainer 

In this scenario, it should derive from IScriptControl interface and a server 
control class. Meanwhile, we have another three steps need to do:

1) Override OnPreRender method. Register the web control as the ScriptControl 
in OnPreRender phase.
ScriptManager manager = ScriptManager.GetCurrent(this.Page);
if (manager == null)
{
  throw new InvalidOperationException("A ScriptManager is required on the page.");
}
manager.RegisterScriptControl<Schedule>(this);
2) Override Render method. Register the script descriptor which has been defined. 
ScriptManager.GetCurrent(this.Page).RegisterScriptDescriptors(this);

6. The rest work is on client-side. Register client NameSpace first.

Type.registerNamespace("PainControls");

7. Build client class.

PainControls.Schedule = function(element) 
{
}
PainControls.Schedule.prototype = {
}

8. Register the class that inherits " Sys.UI.Control".

PainControls.Schedule.registerClass('PainControls. Schedule¡¯, Sys.UI.Control);

9. Call base method in constructor method

PainControls. Schedule.initializeBase(this, [element]);

10. Implementing the Initialize and Dispose Methods.
Build "initialize" and "dispose" method in prototype of the class. The 
initialize method is called when an instance of the behavior is created. 
Use this method to set default property values, to create function delegates, 
and to add delegates as event handlers. The dispose method is called when an 
instance of the behavior is no longer used on the page and is removed. Use 
this method to free any resources that are no longer required for the behavior, 
such as DOM event handlers.

initialize: function() {
PainControls. Schedule.callBaseMethod(this, 'initialize');
},
dispose: function() {
PainControls. Schedule.callBaseMethod(this, 'dispose');
}

11. Defining the Property Get and Set Methods.
Each property identified in the ScriptDescriptor object of the script control's 
GetScriptDescriptors() method must have corresponding client accessors. The 
client property accessors are defined as get_<:property> and set_<:property> 
methods of the client class prototype.

    get_titleFieldName: function() {
        return this._titleFieldName;
    },

    set_titleFieldName: function(val) {
        if (this._titleFieldName !== val) {
            this._titleFieldName = val;
            this.raisePropertyChanged('titleFieldName');
        }
    },

12. Defining the Event Handlers for the DOM Element
1) Defining the handler in constructor function:
this._element_focusHandler = null;
2) Associate the handler with the DOM Element event in initailize method:
this._element_focusHandler = Function.createDelegate(this, this._element_onfocus);
3) Add the handler in initailize method:
$addHandler(this.get_element(), 'focus', this._element_focusHandler)
4) Build callback method about this event:
_element_onfocus:function(){
}

13. Defining the Event Handlers for the behavior
Each event identified in the ScriptDescriptor object of the script control's 
GetScriptDescriptors() method must have corresponding client accessors. The 
client event accessors are defined as add_<:event> and remove_<:event> methods 
of the client class prototype. The method Raise<:event> is defined to trigger 
the event.

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

14. Use this script control Schedule in page.

1) About Schedule control.

1. The schedule is bound on DataSource control. You can select a date in left 
clanedar, it will retrieve the data from DataSource, and present the corresponding 
task in the right container.
2. Each task will be presented in a DropPanel. You can Drag & Drop it to another 
cell(day). It will be updated automatically.
3. When you mouse move on the droppanel, it will show CloseButton so that you can 
remove this task.
4. When mouse over the cell of schedule, it will expend itself by design if it has 
more task needs to show.
5. In the left calendar, it will highlight the date which has tasks.
6. Since it is an Ajax ScriptControl, you can use custom css style at will.
7. Schedule used the other two script controls ButtonList and DropPanel in this 
project.
8. You need build and bind a web service file on this control to achieve updating 
and deleting function asychronously.

2) What capability needs expend

It¡¯s a simple schedule here. The following functionality needs to expend and 
will be implemented in the next version.
1. Besides "Month" display mode, "Day" display mode is needed.
2. Recently, you can create a DetailView to insert a new task, since Schedule 
is bound on DataSource. 
   Next time, I'd like to expend the internal insert functionality.

3) How to use PainControl Schedule:

1.Register the assembly in page.

<%@ Register TagPrefix="PainControls" Assembly="PainControls" 
Namespace="PainControls" %>

2. Use it in the page
<asp:ScriptManager ID="ScriptManager1" runat="server" />
<Pain:Schedule ID="Schedule" runat="server" AutoPostBack="true" 
DataSourceID="SqlDataSource1" 
KeyField="num" 
DateTimeFieldName="date_time" 
TitleFieldName="title" 
DescriptionFieldName="description" 
ServicePath="ScheduleWebService.asmx" 
UpdateServiceMethod="UpdateWebService" 
DeleteServiceMethod="DeleteWebService" />
<asp:SqlDataSource ID="SqlDataSource1" runat="server"
ConnectionString="<%$ ConnectionStrings:DatabaseConnectionString %>"
SelectCommand="SELECT * FROM [schedule]"></asp:SqlDataSource>

3. Property
DataSourceID ---- Create a DataSource and bind it on Schedule.
KeyField ---- The field name of primary key of DataSource.
There are three mandatory field you have to create:
DateTimeFieldName ---- related field name about the datetime of the task
TitleFieldName ---- related field name about the title of the task
DescriptionFieldName ---- related field name about the description of the task
ServicePath ---- If you have used AjaxControlToolkit, you must be familar with 
this property. It means the path of the web service file.
UpdateServiceMethod ---- The web method name to execute updating function
DeleteServiceMethod ---- The web method name to execute deleting function

4. Build a web service bound to achieve updating and deleting function

The following demo snippets are the web methods of updating function and delete 
function.
In the web method of updating function:
"key" is the primary key that will be updated.
"updateFieldName" is the field name that will be updated.
"updateValue" is the related value which it is updated to about "updateFieldName".

In the web method of deleting function,
just need "key" that is the primary key that will be deleted.
[WebMethod]
[System.Web.Script.Services.ScriptMethod]
public void UpdateWebService(string key, string updateFieldName, string updateValue)
{
string constr = 
(string)ConfigurationManager.ConnectionStrings["DatabaseConnectionString"].ConnectionString;
string sql = 
"update schedule set " + updateFieldName + "='" + updateValue+"' where num="+key;
SqlConnection connection = new SqlConnection(constr);
SqlCommand sdc = new SqlCommand(sql, connection);
sdc.CommandType = CommandType.Text;
try
{
connection.Open();
sdc.ExecuteScalar();
}
catch (SqlException SQLexc)
{
throw new Exception(SQLexc.Message);
}
finally
{
connection.Close();
}
System.Threading.Thread.Sleep(2000);

}

[WebMethod]
[System.Web.Script.Services.ScriptMethod]
public void DeleteWebService(string key)
{

string constr = 
(string)ConfigurationManager.ConnectionStrings["DatabaseConnectionString"].ConnectionString;
string sql = "delete from schedule where num=" + key;
SqlConnection connection = new SqlConnection(constr);
SqlCommand sdc = new SqlCommand(sql, connection);
sdc.CommandType = CommandType.Text;
try
{
connection.Open();
sdc.ExecuteScalar();
}
catch (SqlException SQLexc)
{
throw new Exception(SQLexc.Message);
}
finally
{
connection.Close();
}
System.Threading.Thread.Sleep(2000);
}



