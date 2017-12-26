=============================================================================
         SILVERLIGHT APPLICATION : CSSL4MEF Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

Managed Extensibility Framework (MEF) is a framework which could assist 
developers to design extensible application. It's supported by Silverlight 4. 
This sample uses MEF to create a simple text formater. By using the 
predefined contract, users can create components to enhance the formater's 
functionality. The components can be loaded at runtime.


/////////////////////////////////////////////////////////////////////////////
Demo:

To test this sample:
1. Open CSSL4MEF solution and build the solution.
2. Right click CSSL4MEFTestPage.aspx file, select "View in Browser".
3. In opened page, you may find a Silverlight application.
	a. In right area of Silverlight, there are some controls which cloud change
	the state of text shown on left side. For each controls in right area, it's
	composed with application by MEF.
	b. By Clicking the "Click to load color config control" button, MEF would load
	extension component and recompose the config panel dynamicly.


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

Silverlight 4 Tools for Visual Studio 2010
http://www.silverlight.net/getstarted/

Silverilght 4 runtime
http://www.silverlight.net/getstarted/


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. How to use MEF to build an extensible Silverlight Application?
	It's a long story, I recommend CodeBetter's blog MEF series
	http://codebetter.com/blogs/glenn.block/archive/2009/11/30/building-the-hello-mef-dashboard-in-silverlight-4-part-i.aspx

2. What's the solution structure?
	CSSL4MEF project utilized MEF to implement the ConfigPanel control, which cloud
	download plugin xap and extend functionality at runtime.

	CSSL4MEF.Web project is CSSL4MEF silverlight application's host web application.

	ConfigControl.Contract project defines the contract as the ConfigPanel's extension
	interface.

	ConfigControl.Extension project implement configpanel's extension interface, create
	a components "ColorPicker".

3. How does MEF work in this project?
	When running the application, you may see there are mainly two regions on
	view. The left side shows a short text, and the right side has certain
	controls which could change text style. Actually, these controls are binded to a
	predefined DataModel. Those styled text has no magical, just bind UI property
	to datamodel and utilize INotifiyPropertyChanged to update UI in realtime.
	For right side, it is a Silverlight control "ConfigPanel". It could 
	bind to datamodel by "ConfigData" property, and generate config controls
	automaticly.
	
	Because the datamodel's property can be any type and has various requirement,
	To make the ConfigPanel be able to generate appropriate controls for any
	datamodels, we need to let the ConfigPanel be extensible. In this scenario,
	MEF could help to meet this desgin task.

	We defined an interface called "IConfigControl", which should be able to 
	return an edit control binded to the given property field. The ConfigPanel
	utilze MEF to hold a list of IConfigControl, by calling 
	IConfigControl.MatchTest method, configPanel would find most suitable
	configControl for each property and add	control to config panel.

	Suppose we have a datamodel, which has Color type property, to extend
	the ConfigPanel to support Color type, we would create a new silverlight
	project, implementing IConfigControl and mark with Export attribute to
	make it discoverable. Then, by using "DeploymentCatalogService", we could
	dynamicly load the extension config control, once the calagory changed,
	configPanel get notificaton and recompose UI for datamodel.
    

/////////////////////////////////////////////////////////////////////////////
References:

MEF community site
http://mef.codeplex.com/


/////////////////////////////////////////////////////////////////////////////