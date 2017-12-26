/****************************** Module Header ******************************\
* Module Name:    Default.aspx.cs
* Project:        CSASPNETFileUploadStatus
* Copyright (c) Microsoft Corporation
*
* The project illustrates how to display the upload status and progress without
* a third part component like ActiveX control, Flash or Silverlight.
* 
* This is the page which we test the Upload status for the client
* We use ICallbackEventHandler to realize the communication between
* the server side and client side without refresh the page.
* But we need to use an iframe to hold the upload controls and buttons,
* because the upload need postback to the server, we can't call the server code
* by javascript in one postback page.
* So we let the iframe do the upload postback operation.
* 
* For more details about ICallbackEventHandler,
* please read the readme file in the root directory.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/


using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Threading;
using System.Web.Script.Serialization;
using System.Text;
using CSASPNETFileUploadStatus;

public partial class _Default : System.Web.UI.Page, ICallbackEventHandler
{
    protected void Page_Load(object sender, EventArgs e)
    {
        //Register a client script for ICallbackEventHandler
        ClientScriptManager cm = Page.ClientScript;
        String cbReference = cm.GetCallbackEventReference(this, "arg",
            "ReceiveServerData", "");
        String callbackScript = "function CallServer(arg, context) {" +
            cbReference + "; }";
        if (!cm.IsClientScriptBlockRegistered(this.GetType(), "CallServer"))
        {
            cm.RegisterClientScriptBlock(this.GetType(),
                "CallServer", callbackScript, true);
        }

    }
    private string uploadModuleProgress = "";
    public string GetCallbackResult()
    {
        return uploadModuleProgress;
    }

    public void RaiseCallbackEvent(string eventArgument)
    {
        if (eventArgument == "Clear")
        {
            //operation for clear the cache
            ClearCache("fuFile");
            uploadModuleProgress = "Cleared";
        }
        if (eventArgument == "Abort")
        {
            //operation for abort uploading
            AbortUpload("fuFile");
            uploadModuleProgress = "Aborted";
        }


        try
        {
            UploadStatus status =
                HttpContext.Current.Cache["fuFile"] as UploadStatus;
            if (status == null)
            {
                return;
            }
            // We use JSON to send the data to the client,
            // because it is simple and easy to handle.
            // For more details about JavaScriptSerializer, please
            // read the readme file in the root directory.
            JavaScriptSerializer jss = new JavaScriptSerializer();

            // The StringBuilder object will hold the serialized result.
            StringBuilder sbUploadProgressResult = new StringBuilder();
            jss.Serialize(status, sbUploadProgressResult);

            uploadModuleProgress = sbUploadProgressResult.ToString();
        }
        catch (Exception err)
        {
            if (err.InnerException != null)
                uploadModuleProgress = "Error:" + err.InnerException.Message;
            else
                uploadModuleProgress = "Error:" + err.Message;
        }
    }

    private void AbortUpload(string cacheID)
    {
        UploadStatus status = HttpContext.Current.Cache[cacheID] as UploadStatus;
        if (status == null)
        {
            return;
        }
        else
        {
            status.Abort();
        }

    }

    private void ClearCache(string cacheID)
    {
        HttpContext.Current.Cache.Remove(cacheID);
    }


}