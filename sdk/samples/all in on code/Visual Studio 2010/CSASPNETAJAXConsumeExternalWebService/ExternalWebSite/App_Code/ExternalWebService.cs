/****************************** Module Header ******************************\
* Module Name:    ExternalWebService.cs
* Project:        CSASPNETAJAXConsumeExternalWebService
* Copyright (c) Microsoft Corporation
*
* The project illustrates how to consume an external Web Service from a
* different domain.
* 
* In this file, we impersonate a remote web service in a different
* domain. Make sure this web service is online when we test this sample.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/
using System;
using System.Web.Services;


[WebService(Namespace = "http://tempuri.org/")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
[System.Web.Script.Services.ScriptService]
public class ExternalWebService : System.Web.Services.WebService {

    public ExternalWebService () {}

    [WebMethod]
    public DateTime GetServerTime() {
        return DateTime.Now;
    }
    
}
