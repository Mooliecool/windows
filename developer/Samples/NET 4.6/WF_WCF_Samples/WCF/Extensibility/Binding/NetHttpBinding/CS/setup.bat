@echo on
REM -------- Make sure makecert.exe is in your path --------
makecert.exe -sr LocalMachine -ss Root -n CN=NetHttpBinding-HTTPS-CA -cy authority -r
makecert.exe -sr LocalMachine -ss My -n CN=NetHttpBinding-HTTPS-Server -sky exchange -sk NetHttpBinding-HTTPS-Key -is Root -ir LocalMachine -in NetHttpBinding-HTTPS-CA 

