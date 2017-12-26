========================================================================
    CONSOLE APPLICATION : ATLCOMClient Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

ATLCOMClient shows you how to call COM server objects in ATL and implement an 
event sink using the ATL IDispEventImpl and IDispEventSimpleImpl classes for 
events in the COM server.

For the event sink, you can use IDispEventImpl when you have access to a type 
library. Use IDispEventSimpleImpl when you do not have access to the type 
library or when you want to be more efficient by not loading the type library.


/////////////////////////////////////////////////////////////////////////////
Project Relation:

ATLCOMClient -> ATLDllCOMServer
ATLCOMClient is the client application of the COM server ATLDllCOMServer.


/////////////////////////////////////////////////////////////////////////////
Creation:

Step1. Create a C++ Win32 console project in Visual Studio 2008, and add 
common header files for ATL.

#include <atlbase.h>
#include <atlstr.h>
#include <atlcom.h> // This header file requires to be added manually

Step2. Declare and initialize the current ATL module.

	// You may derive a class from CAtlModule and use it if you want to 
	// override something.
	class CATLCOMClientModule : public CAtlExeModuleT<CATLCOMClientModule>
	{ };

	// Declare and initialize the current ATL module. 
	CATLCOMClientModule _AtlModule;

Step3. Import the type library of the COM server:

	#import "ATLDllCOMServer.dll" no_namespace named_guids
	// [-or-]
	//#import "libid:9B23EFED-A0C1-46B6-A903-218206447F3E" no_namespace named_guids

Step4. Add the file ATLSimpleSinkObject.h. The file contains three different 
sink objects all of which handle the FloatPropertyChanging event which is 
fired by the source COM object "ATLDllCOMServer.SimpleObject". 

Step5. Create the ATLDllCOMServer.SimpleObject COM object using the 
#import directive and smart pointers.

	CComQIPtr<ISimpleObject> spSimpleObj;
	hr = spSimpleObj.CoCreateInstance(OLESTR(
		"ATLDllCOMServer.SimpleObject"));

Step6. Use sink object 1 (CATLSimpleSinkObject1) to set up the sink for the 
events of the source COM object.

	6.1 Construct the sink object CATLSimpleSinkObject1 defined in 
	ATLSimpleSinkObject.h
	
	6.2 Make sure the COM object corresponding to pUnk implements 
	IProvideClassInfo2 or IPersist*. Call this method to extract info about
	source type library if you specified only 2 parameters to IDispEventImpl.

	6.3 Connect the sink and source, spSimpleObj is the source COM object
	
	6.4 Invoke the source COM object
	
	6.5 Disconnect from the source COM object if connected
	
	6.6 Destroy the sink object

Step7. Use sink object 2 (CATLSimpleSinkObject2) to set up the sink for the 
events of the source COM object.

	7.1 Construct the sink object CATLSimpleSinkObject2 defined in 
	ATLSimpleSinkObject.h
	
	7.2 Connect the sink and source, m_spSrcObj is the source COM object
	
	7.3 Invoke the source COM object
	
	7.4 Disconnect from source if connected
	
	7.5 Destroy the sink object

Step8. Use sink object 3 (CATLSimpleSinkObject3) to set up the sink for the 
events of the source COM object.

	8.1 Construct the sink object CATLSimpleSinkObject3 defined in 
	ATLSimpleSinkObject.h
	
	8.2 Connect the sink and source, m_spSrcObj is the source COM object
	
	8.3 Invoke the source COM object
	
	8.4 Disconnect from source if connected
	
	8.5 Destroy the sink object

Step9. Release the COM object.


/////////////////////////////////////////////////////////////////////////////
References:

KB: AtlEvnt.exe sample shows how to creates ATL sinks by using the ATL 
IDispEventImpl and IDispEventSimpleImpl classes
http://support.microsoft.com/kb/194179


/////////////////////////////////////////////////////////////////////////////
