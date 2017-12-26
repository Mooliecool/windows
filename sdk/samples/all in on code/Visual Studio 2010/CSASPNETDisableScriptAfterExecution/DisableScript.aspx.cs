/****************************** Module Header ******************************\
* Module Name: DisableScript.aspx.cs
* Project:     CSASPNETDisableScriptAfterExecution
* Copyright (c) Microsoft Corporation
*
* The sample code illustrates how to register script code at code behind and to 
* be disabled after execution. Sometimes users who register scripts do not want 
* them execute again, Actually they with to achieve this either in an automatic
* manner or by imitating an action for example, by clicking a link or button. 
* This maybe due to functional purpose, user experience or security concerns. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*****************************************************************************/



using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

namespace CSASPNETDisableScriptAfterExecution
{
    public partial class RemoveScript : System.Web.UI.Page
    {
        /// <summary>
        /// Register JavaScript functions at code behind page(Page_Load event). 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void Page_Load(object sender, EventArgs e)
        {
            ClientScriptManager clientManager = Page.ClientScript;

            // JavaScript function part 1, this function is used to execute user's logical
            // code with an additional parameter, this parameter is a callback function 
            // which is make a condition that if the main function had executed.
            string mainScript = @"
                            var flag = true;
                            var mainFunc = function main(callBack) {
                            var callBackPara = callBack;
                            if (flag) {
                                // User code.
                                alert('This is user code, in this sample code, your page will move down.');
 
                                for (var i = 1; i <= 900; i++)
                                {
                                    window.moveBy(0, 1);
                                }
                                window.moveBy(0,-750);
                     
                                // Disable JS function.
                                flag = undefined;    
                            }
                            else {
                                alert(callBackPara);
                            }}";
            clientManager.RegisterClientScriptBlock(this.GetType(), "mainScript", mainScript, true);

            // JavaScript function part 2, the callback function, check if flag variable is undefined.
            string callbackScript = @" var callFunc = function callBackFunc() {
                                       if (!flag) {
                                           return 'The JavaScript function has been disabled..';
                                       }}
                                       ";
            clientManager.RegisterStartupScript(this.GetType(), "callBackScript", callbackScript, true);
        }
    }
}