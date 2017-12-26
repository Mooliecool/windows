echo Creating Tracking database...
Osql -S %COMPUTERNAME%\SQLExpress -E  -n -i "CreateTrackingDatabase.sql" 

echo Creating tracking service tables...
Osql -S %COMPUTERNAME%\SQLExpress -E  -n -d Tracking -i "%windir%\Microsoft.Net\Framework\v3.0\Windows Workflow Foundation\SQL\EN\Tracking_Schema.sql" 

echo creating tracking service stored procedures...
Osql -S %COMPUTERNAME%\SQLExpress -E  -n -d Tracking -i "%windir%\Microsoft.Net\Framework\v3.0\Windows Workflow Foundation\SQL\EN\Tracking_Logic.sql"

::Pause
