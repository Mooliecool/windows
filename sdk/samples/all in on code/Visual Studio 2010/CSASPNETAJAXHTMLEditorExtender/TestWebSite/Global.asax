<%@ Application Language="C#" %>
<script RunAt="server">
    /****************************** Module Header ******************************\
* Module Name:    Global.asax
* Project:        CSASPNETHTMLEditorExtender
* Copyright (c) Microsoft Corporation
* 
* We don't have to create this file, it is only used to catch the exception 
* if we don't rebuild the whole solution when we first use it.
* 
\*****************************************************************************/
    void Application_Error(object sender, EventArgs e)
    {
        Exception err = Context.Server.GetLastError();
        if (err.Message.IndexOf("AjaxControlToolkit") > -1)
        {
            Context.Server.ClearError();
            Context.Response.Write(
                "<html><head><title>CSASPNETHTMLEditorExtender</title></head>" +
                "<body><p style='text-align:center'>Please rebuild " +
                "the whole solution before you test it!</p></body></html>");
        }
    }
</script>
