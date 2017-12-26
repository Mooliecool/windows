/****************************** Module Header ******************************\
* Module Name:    BridgeWebService.cs
* Project:        CSASPNETAJAXConsumeExternalWebService
* Copyright (c) Microsoft Corporation
*
* The project illustrates how to consume an external Web Service from a
* different domain.
* 
* In this file, we create a local web service to consume the remote web service
* like a bridge.
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
public class BridgeWebService : System.Web.Services.WebService
{

    public BridgeWebService() { }

    [WebMethod]
    public DateTime GetServerTime()
    {
        // Get an instance of the external web service
        ExternalWebService.ExternalWebService ews =
            new ExternalWebService.ExternalWebService();
        // Return the result from the web service method.
        return ews.GetServerTime();
    }

}
