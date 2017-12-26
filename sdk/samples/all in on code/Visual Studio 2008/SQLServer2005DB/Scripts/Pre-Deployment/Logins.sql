/****************************** Module Header ******************************\
* Module Name:  Logins.sql
* Project:      SQLServer2005DB
* Copyright (c) Microsoft Corporation.
* 
* Create the LOGIN user: HelloWorld with the password 111111.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 2/14/2009 9:28 PM Jialiang Ge Created
\***************************************************************************/


/* If [HelloWorld] does not exist */
IF NOT EXISTS (SELECT * FROM master.dbo.syslogins WHERE loginname = N'HelloWorld') 
BEGIN
	/* Create the LOGIN [HelloWorld] with Password = 111111 */
	CREATE LOGIN [HelloWorld] WITH PASSWORD = '111111', 
	DEFAULT_LANGUAGE=[us_english], CHECK_EXPIRATION=OFF, CHECK_POLICY=OFF
	/* Grant the dbcreator role to the LOGIN */
	EXEC sys.sp_addsrvrolemember @loginame = N'HelloWorld', @rolename = N'dbcreator'
END
ELSE/* If [HelloWorld] already exists */
BEGIN
	/* Alter the LOGIN [HelloWorld] with Password = 111111 */
	ALTER LOGIN [HelloWorld] WITH PASSWORD = '111111', 
	DEFAULT_LANGUAGE=[us_english], CHECK_EXPIRATION=OFF, CHECK_POLICY=OFF
	/* Grant the dbcreator role to the LOGIN */
	EXEC sys.sp_addsrvrolemember @loginame = N'HelloWorld', @rolename = N'dbcreator'
END

GO
