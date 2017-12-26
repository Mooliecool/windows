echo Deleting NetFx35Samples_ServiceWorkflowStore database...
Osql -S %COMPUTERNAME%\SQLExpress -E  -n -i "DeleteServiceWorkflowStore.sql" 

echo Deleting NetFx35Samples_ClientWorkflowStore database...
Osql -S %COMPUTERNAME%\SQLExpress -E  -n -i "DeleteClientWorkflowStore.sql" 

echo Deleting NetFx35Samples_DurableServiceStore database...
Osql -S %COMPUTERNAME%\SQLExpress -E  -n -i "DeleteDurableServiceStore.sql" 

Pause