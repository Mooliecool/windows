=============================================================================
                Console APPLICATION: VBTFSEventListener
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The sample demonstrates how to create a TFS event listener using WCF Console 
Application.

This WCF service is used to subscribe a TFS Check-in Event. If a user checked 
in a changeset which meets the filters of the subscription, TFS will call the 
Notify method of this WCF service with the parameters. The Notify method will 
display a message if the Changeset has policy failures. 


/////////////////////////////////////////////////////////////////////////////
Prerequisite:

This sample have to run on TFS2010 Application Tier. 

To set the check-in policy and check-in a changeset, you need a machine that 
Team Explorer2010 and TFS2010 PowerTools are installed on it. The machine could
be the same as TFS2010 application tier. 


////////////////////////////////////////////////////////////////////////////////
Demo:


Build and run this WCF application.

Step1. Open this project in  Visual Studio 2010. 
		
Step2. Build the solution. 

Step3. Open VBTFSEventListener.exe.config in the output folder.
	   Set the baseAddress of the service, like 
		  http://localhost:8732/VBTFSEventListener/EventService/

Step4. Run VBTFSEventListener.exe.



Subscribe Check-in Event.

Step1. On TFS2010 application tier, open command line and navigate to   
Step2. Run following command in command line.
       BisSubscribe.exe /eventtype CheckinEvent /address http://localhost:8732/VBTFSEventListener/EventService/ 
       /collection http://Server:8080/tfs/collectionName /deliveryType Soap

You can also set the alert using Alert Explorer of TFS2010 PowerTools.
		  


Set Check-in policy

Step1. Open VS and connect to the specified Project Collection and Team Project 
	   using Team Explorer.

Step2. Right click the Team Project node in Team Explorer, and in the context menu,
	   expand Team Project Settings, select Source Control.

Step3. In the Source Control Settings dialog, choose Check-in Policy tab and click
	   "Add..." button. 

Step4. In the Add Check-in Policy dialog, choose "Changest Comments Policy" and click
	   OK. You will see a new check-in policy in the Source Control Settings dialog, 
	   click OK to close this dialog.

	   

Check in files to fire the event.

Step1. Open VS and connect to the specified Project Collection and Team Project 
	   using Team Explorer.

Step2. Double click the "Source Control" node under the Team Project in Team Explorer.
	   VS will open Source Control Explorer.

	   You have to create a workspace and map a work folder.

Step3. Check out a file of the Team Project in Source Control Explorer, open the file 
	   edit it and save it. Then right click the file in Source Control Explorer, select 
	   "Check In Pending Changes". 
	   
Step4. In "Policy Warning" tab of the "Check In" dialog, you will find a message that 
	   "Please Provide some comments about your check-in". Ignore it and continue to 
	   click the "Check In" button to check in the file.
	   
Step5. VS will alert a message that "Checkin cannot proceed because the policy requirement
	   have not been satisfied". Check "Override policy failure and continue checkin", and type 
	   "Test checkin event" in the Reason textbox. Click OK, and then VS will check-in the
	   files. 	     

Step6. Wait for one or two minutes, and then you will see following message in the 
       VBTFSEventListener.exe.	

	   ChangeSetChangeset 10 Check In Policy Policy Failed.
	   Committer : <Domain>\<user>
       Override Comment : Test checkin event
       Check in policy failures:
			Changeset Comments Policy : Please provide some comments about your check-in


/////////////////////////////////////////////////////////////////////////////
Code Logic:

A. Define the WCF Service Contract which is used to listen TFS event.

    <ServiceContract(Namespace:="http://schemas.microsoft.com/TeamFoundation/2005/06/Services/Notification/03")>
    Public Interface IEventService
      <OperationContract(Action:="http://schemas.microsoft.com/TeamFoundation/2005/06/Services/Notification/03/Notify")>
      <XmlSerializerFormat(Style:=OperationFormatStyle.Document)>
      Sub Notify(ByVal eventXml As String, ByVal tfsIdentityXml As String)
    End Interface

B. Create CheckinEventService class that implement the Service Contract.

   The eventXml parameter of the Notify method is serialized from a CheckinEvent object, 
   we can use XmlSerializer to deserialize it back to a CheckinEvent object.
   
   The CheckinEvent contains a PolicyFailures field from which we can determine whether 
   the Changeset has policy failures. 

C. Create a console application to host the WCF Service. 
   In the main method of this application, use the ServiceHost class to configure and 
   expose a service for use

  
        Using host As New ServiceHost(GetType(CheckinEventService))
            host.Open()
            Console.WriteLine(host.BaseAddresses.First())

            Console.WriteLine("Press <Enter> to exit...")
            Console.ReadLine()
        End Using


	In the configuration file, make sure this WCF service use wsHttpBinding and security mode
	is None.

	<system.serviceModel>
    <bindings>
      <wsHttpBinding>
        <binding name="EventService">
          <security mode="None"></security>
        </binding>
      </wsHttpBinding>
    </bindings>
    <behaviors>
      <serviceBehaviors>
        <behavior name="EventServiceBehavior">
          <serviceMetadata httpGetEnabled="true"/>
          <serviceDebug includeExceptionDetailInFaults="true"/>
        </behavior>
      </serviceBehaviors>
    </behaviors>
    <services>
      <service behaviorConfiguration="EventServiceBehavior" name="VBTFSEventListener.CheckinEventService">
        <endpoint address="" binding="wsHttpBinding" bindingConfiguration="EventService" contract="VBTFSEventListener.IEventService"/>
        <endpoint address="mex" binding="mexHttpBinding" contract="IMetadataExchange"/>
        <host>
          <baseAddresses>
            <add baseAddress="http://localhost:8732/VBTFSEventListener/EventService/"/>
          </baseAddresses>
        </host>
      </service>
    </services>
  </system.serviceModel>


/////////////////////////////////////////////////////////////////////////////
References:

Team Foundation Server Event Service
http://msdn.microsoft.com/en-us/magazine/cc507647.aspx

CheckinEvent Class
http://msdn.microsoft.com/en-us/library/microsoft.teamfoundation.versioncontrol.common.checkinevent.aspx

ServiceHost Class
http://msdn.microsoft.com/en-us/library/system.servicemodel.servicehost.aspx


/////////////////////////////////////////////////////////////////////////////