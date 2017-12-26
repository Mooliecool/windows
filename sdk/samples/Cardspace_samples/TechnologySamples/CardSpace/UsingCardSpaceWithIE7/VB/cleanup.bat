@echo off
setlocal 
cd %~dp0

REM REMOVE CERTIFICATES
cscript scripts\remove-certificates.vbs

REM REMOVE WEBSITES
cscript scripts\remove-website.vbs

REM REMOVE HOSTS
cscript scripts\remove-hosts.vbs

