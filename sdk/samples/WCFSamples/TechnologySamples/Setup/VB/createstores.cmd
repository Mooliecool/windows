echo Creating NetFx35Samples_ServiceWorkflowStore database...
Osql -S %COMPUTERNAME%\SQLExpress -E  -n -i "ServiceWorkflowStore.sql" 

echo Creating persistence tables...
Osql -S %COMPUTERNAME%\SQLExpress -E  -n -d NetFx35Samples_ServiceWorkflowStore -i "%windir%\Microsoft.Net\Framework\v3.0\Windows Workflow Foundation\SQL\EN\SqlPersistenceService_Schema.sql" 

echo creating persistence stored procedures...
Osql -S %COMPUTERNAME%\SQLExpress -E  -n -d NetFx35Samples_ServiceWorkflowStore -i "%windir%\Microsoft.Net\Framework\v3.0\Windows Workflow Foundation\SQL\EN\SqlPersistenceService_Logic.sql"

echo Creating NetFx35Samples_ClientWorkflowStore database...
Osql -S %COMPUTERNAME%\SQLExpress -E  -n -i "ClientWorkflowStore.sql" 

echo Creating persistence tables...
Osql -S %COMPUTERNAME%\SQLExpress -E  -n -d NetFx35Samples_ClientWorkflowStore -i "%windir%\Microsoft.Net\Framework\v3.0\Windows Workflow Foundation\SQL\EN\SqlPersistenceService_Schema.sql" 

echo creating persistence stored procedures...
Osql -S %COMPUTERNAME%\SQLExpress -E  -n -d NetFx35Samples_ClientWorkflowStore -i "%windir%\Microsoft.Net\Framework\v3.0\Windows Workflow Foundation\SQL\EN\SqlPersistenceService_Logic.sql"

echo Creating NetFx35Samples_DurableServiceStore database...
Osql -S %COMPUTERNAME%\SQLExpress -E  -n -i "DurableServiceStore.sql" 

echo Creating persistence tables...
for /F %%i in ('dir /s /b "%windir%\Microsoft.NET\Framework\SqlPersistenceProviderSchema.sql"') do set SqlPersistenceProviderSchemaPath=%%i
Osql -S %COMPUTERNAME%\SQLExpress -E  -n -d NetFx35Samples_DurableServiceStore -i "%SqlPersistenceProviderSchemaPath%" 
 
echo creating persistence stored procedures...
for /F %%i in ('dir /s /b "%windir%\Microsoft.NET\Framework\SqlPersistenceProviderLogic.sql"') do set SqlPersistenceProviderLogicPath=%%i
Osql -S %COMPUTERNAME%\SQLExpress -E  -n -d NetFx35Samples_DurableServiceStore -i "%SqlPersistenceProviderLogicPath%"

Pause
