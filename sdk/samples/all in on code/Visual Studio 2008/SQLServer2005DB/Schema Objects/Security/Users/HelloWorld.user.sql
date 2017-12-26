/****************************** Module Header ******************************\
* Module Name:  HelloWorld.user.sql
* Project:      SQLServer2005DB
* Copyright (c) Microsoft Corporation.
* 
* Add the user [HelloWorld] as the DB owner of SQLServer2005DB.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 2/14/2009 9:28 PM Jialiang Ge Created
\***************************************************************************/


CREATE USER [HelloWorld] FOR LOGIN [HelloWorld] WITH DEFAULT_SCHEMA=[dbo]


