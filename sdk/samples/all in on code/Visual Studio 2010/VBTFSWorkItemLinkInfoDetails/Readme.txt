=============================================================================
           Windows APPLICATION: VBTFSWorkItemLinkInfoDetails
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The sample demonstrates how to get the link details of a WorkItemLinkInfo object.
The detailed information is like :

Source:[Source title] ==> LinkType:[Link Type] ==> Target:[Target title]


/////////////////////////////////////////////////////////////////////////////
Prerequisite:

Team Explorer 2010.

You can download it in the following link:
http://www.microsoft.com/downloads/en/details.aspx?FamilyID=fe4f9904-0480-4c9d-a264-02fedd78ab38


/////////////////////////////////////////////////////////////////////////////
Demo:

Step1. Open this project in  Visual Studio 2010. 
        
Step2. Build the solution. 

Step3. Run the application VBTFSWorkItemLinkInfoDetails.exe in command line in 
       following format:

              VBTFSWorkItemLinkInfoDetails.exe <CollectionUrl> <WorkItemID>

       If your default credential fails to connect to the TFSTeamProjectCollection, a 
       dialog will be launched to let you type the UserName/Password.


Step4. You will see following message if the specified work item has related work item
       links.

       Source:[Source title] ==> LinkType:[Link Type] ==> Target:[Target title]

/////////////////////////////////////////////////////////////////////////////
Code Logic:


A. Connect to a TFS Team Project Collection and the WorkItemStore service.
        
        
        ' If the credential failed, an UICredentialsProvider instance will be launched.
        Me.ProjectCollection = New TfsTeamProjectCollection(
            collectionUri, credential, New UICredentialsProvider())
        Me.ProjectCollection.EnsureAuthenticated()

        ' Get the WorkItemStore service.
        Me.WorkItemStore = Me.ProjectCollection.GetService(Of WorkItemStore)()
       

B. Construct the WIQL.

       Private Const _queryFormat As String =
           "select * from WorkItemLinks where [Source].[System.ID] = {0}"
       Dim queryStr As String = String.Format(_queryFormat, workitemID)
       
       Dim linkQuery As New Query(Me.WorkItemStore, queryStr)

       ' Get all WorkItemLinkInfo objects.
       Dim linkinfos() As WorkItemLinkInfo = linkQuery.RunLinkQuery()

C. Get the detailed information.

       Private _linkTypes As Dictionary(Of Integer, WorkItemLinkType)
       
       ' The dictionary to store the ID and WorkItemLinkType KeyValuePair.
       Public ReadOnly Property LinkTypes() As Dictionary(Of Integer, WorkItemLinkType)
           Get
               ' Get all WorkItemLinkType from WorkItemStore.
               If _linkTypes Is Nothing Then
                   _linkTypes = New Dictionary(Of Integer, WorkItemLinkType)()
                   For Each type In Me.WorkItemStore.WorkItemLinkTypes
                       _linkTypes.Add(type.ForwardEnd.Id, type)
                   Next type
               End If
               Return _linkTypes
           End Get
       End Property

       ''' <summary>
       ''' Get WorkItemLinkInfoDetails from the  WorkItemLinkInfo object.
       ''' </summary>
       Public Function GetDetailsFromWorkItemLinkInfo(ByVal linkInfo As WorkItemLinkInfo) _
           As WorkItemLinkInfoDetails
       
           If Me.LinkTypes.ContainsKey(linkInfo.LinkTypeId) Then
               Dim details As New WorkItemLinkInfoDetails(
                   linkInfo,
                   Me.WorkItemStore.GetWorkItem(linkInfo.SourceId),
                   Me.WorkItemStore.GetWorkItem(linkInfo.TargetId),
                   Me.LinkTypes(linkInfo.LinkTypeId))
               Return details
           Else
               Throw New ApplicationException("Cannot find WorkItemLinkType!")
           End If
       End Function
	   
	   
/////////////////////////////////////////////////////////////////////////////
References:

TfsTeamProjectCollection Constructor (Uri, ICredentials, ICredentialsProvider)
http://msdn.microsoft.com/en-us/library/ff733681.aspx

WIQL syntax for Link Query
http://blogs.msdn.com/b/team_foundation/archive/2010/07/02/wiql-syntax-for-link-query.aspx

Work Item Tracking Queries Object Model in 2010
http://blogs.msdn.com/b/team_foundation/archive/2010/06/16/work-item-tracking-queries-object-model-in-2010.aspx


/////////////////////////////////////////////////////////////////////////////