' This script creates the COM+ app and sets it to "Leave running when idle"
' to allow the service model endpoints to run indefinitely

Set catalog = CreateObject("COMAdmin.COMAdminCatalog.1")
Set applications = catalog.GetCollection("Applications")

applications.Populate

Set application = applications.Add()

application.Value("Name") = "ServiceModelHostedSample"
application.Value("ID") = "{4CDCDB2C-0B19-4534-95CD-FBBFF4D67DD9}"
application.Value("RunForever") = True
 
applications.SaveChanges
 
Set application = Nothing
Set applications = Nothing
Set catalog = Nothing
