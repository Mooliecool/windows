=============================================================================
             Windows APPLICATION: CSTFSWorkItemLinkInfoDetails
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The sample demonstrates how to get the link details of a WorkItemLinkInfo 
object.  The detailed information is like :

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

Step3. Run the application CSManipulateImagesInWordDocument.exe in command line in 
       following format:

              CSTFSWorkItemLinkInfoDetails.exe <CollectionUrl> <WorkItemID>

       If your default credential fails to connect to the TFSTeamProjectCollection, a 
       dialog will be launched to let you type the UserName/Password.


Step4. You will see following message if the specified work item has related work item
       links.

       Source:[Source title] ==> LinkType:[Link Type] ==> Target:[Target title]

	   
/////////////////////////////////////////////////////////////////////////////
Code Logic:


A. Connect to a TFS Team Project Collection and the WorkItemStore service.
        
        
       // If the credential failed, an UICredentialsProvider instance will be launched.
       this.ProjectCollection =
           new TfsTeamProjectCollection(collectionUri, credential, new UICredentialsProvider());
       this.ProjectCollection.EnsureAuthenticated();
       
       // Get the WorkItemStore service.
       this.WorkItemStore = this.ProjectCollection.GetService<WorkItemStore>();
       

B. Construct the WIQL.

       const string queryFormat =
           "select * from WorkItemLinks where [Source].[System.ID] = {0}";
       string queryStr = string.Format(queryFormat, workitemID);
       
       Query linkQuery = new Query(this.WorkItemStore, queryStr);
       
       // Get all WorkItemLinkInfo objects.
       WorkItemLinkInfo[] linkinfos = linkQuery.RunLinkQuery();

C. Get the detailed information.

        public Dictionary<int, WorkItemLinkType> LinkTypes
        {
            get
            {
                // Get all WorkItemLinkType from WorkItemStore.
                if (linkTypes == null)
                {
                    linkTypes = new Dictionary<int, WorkItemLinkType>();
                    foreach (var type in this.WorkItemStore.WorkItemLinkTypes)
                    {
                        linkTypes.Add(type.ForwardEnd.Id, type);
                    }
                }
                return linkTypes;
            }
        }
   
        /// <summary>
        /// Get WorkItemLinkInfoDetails from the  WorkItemLinkInfo object.
        /// </summary>
        public WorkItemLinkInfoDetails GetDetailsFromWorkItemLinkInfo(WorkItemLinkInfo linkInfo)
        {
            if (linkInfo == null)
            {
                throw new ArgumentNullException("linkInfo");
            }

            if (this.LinkTypes.ContainsKey(linkInfo.LinkTypeId))
            {
                WorkItemLinkInfoDetails details = new WorkItemLinkInfoDetails(
                   linkInfo,
                   this.WorkItemStore.GetWorkItem(linkInfo.SourceId),
                   this.WorkItemStore.GetWorkItem(linkInfo.TargetId),
                   this.LinkTypes[linkInfo.LinkTypeId]);
                return details;
            }
            else
            {
                throw new ApplicationException("Cannot find WorkItemLinkType!");
            }
        }

		
/////////////////////////////////////////////////////////////////////////////
References:

TfsTeamProjectCollection Constructor (Uri, ICredentials, ICredentialsProvider)
http://msdn.microsoft.com/en-us/library/ff733681.aspx

WIQL syntax for Link Query
http://blogs.msdn.com/b/team_foundation/archive/2010/07/02/wiql-syntax-for-link-query.aspx

Work Item Tracking Queries Object Model in 2010
http://blogs.msdn.com/b/team_foundation/archive/2010/06/16/work-item-tracking-queries-object-model-in-2010.aspx


/////////////////////////////////////////////////////////////////////////////