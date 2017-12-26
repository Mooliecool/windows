@echo on
REM -------- Make sure certmgr.exe is in your path --------
certmgr.exe -del -r LocalMachine -s Root -c -n NetHttpBinding-HTTPS-CA
certmgr.exe -del -r LocalMachine -s My -c -n NetHttpBinding-HTTPS-Server
