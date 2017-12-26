/****************************** Module Header ******************************\
* Module Name:	FederationCallbackHandler.aspx.cs
* Project:		AzureBingMaps
* Copyright (c) Microsoft Corporation.
* 
* The callback handler. Configure both ACS and Messenger Connect
* to redirect to this page after the user signs in.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Linq;
using System.Net;
using System.ServiceModel.Syndication;
using System.Threading;
using System.Web;
using System.Xml;
using Microsoft.IdentityModel.Claims;

namespace AzureBingMaps.WebRole
{
    public partial class FederationCallbackHandler : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            // Obtain return page from session, and redirect to the page later.
            string returnPage = "HtmlClient.aspx";
            if (Session["ReturnPage"] != null)
            {
                returnPage = (string)Session["ReturnPage"];
            }

            // Parse wl_internalState cookie,
            // and extract information for Windows Live Messenger Connect Profile API.
            // wl_internalState could be null if the user hasn't tried to login using Live ID.
            if (Response.Cookies["wl_internalState"] != null)
            {
                string accessToken = this.ExtractWindowsLiveInternalState("wl_accessToken");
                string cid = this.ExtractWindowsLiveInternalState("wl_cid");
                string uri = "http://apis.live.net/V4.1/cid-" + cid + "/Profiles";

                // wl_internalState could be invalid if LiveID login failed.
                if (!string.IsNullOrEmpty(accessToken) && !string.IsNullOrEmpty(cid))
                {
                    // Make a request to profile API.
                    HttpWebRequest request = (HttpWebRequest)HttpWebRequest.Create(uri);
                    request.Headers["Authorization"] = accessToken;
                    HttpWebResponse response = (HttpWebResponse)request.GetResponse();
                    if (response.StatusCode == HttpStatusCode.OK)
                    {
                        // Use WCF Syndication API to parse the response.
                        XmlReader xmlReader = XmlReader.Create(response.GetResponseStream());
                        SyndicationFeed feed = SyndicationFeed.Load(xmlReader);
                        var entry = feed.Items.FirstOrDefault();
                        if (entry != null)
                        {
                            var content = entry.Content as XmlSyndicationContent;
                            if (content != null)
                            {
                                // WindowsLiveProfile is a class
                                // corresponding to the profile API's response.
                                var profile = content.ReadContent<WindowsLiveProfile>();
                                var liveID = profile.Emails.Where(m =>
                                    string.Equals(m.Type, "WindowsLiveID")).FirstOrDefault();

                                // If profile API succeeds,
                                // we'll be able to obtain the user's LiveID.
                                // The LiveID will be the user's identity.
                                // We store user identity in session.
                                if (liveID != null)
                                {
                                    Session["User"] = liveID.Address;
                                }
                            }
                        }
                        xmlReader.Close();
                    }
                }
            }

            // The following code deals with ACS via WIF.
            var principal = Thread.CurrentPrincipal as IClaimsPrincipal;
            if (principal != null && principal.Identities.Count > 0)
            {
                var identity = principal.Identities[0];

                // Query for email claim.
                var query = from c in identity.Claims where c.ClaimType == ClaimTypes.Email select c;
                var emailClaim = query.FirstOrDefault();
                if (emailClaim != null)
                {
                    // Store user identity in session.
                    Session["User"] = emailClaim.Value;
                }
            }
            // Redirect user to the return page.
            Response.Redirect(returnPage);
        }

        /// <summary>
        /// Extract information for Windows Live Messenger Connect Profile API
        /// from wl_internalState cookie.
        /// The cookie contains a bunch of information
        /// such as cid and access token.
        /// </summary>
        /// <param name="key">Which data to extract.</param>
        /// <returns>The value of the data.</returns>
        private string ExtractWindowsLiveInternalState(string key)
        {
            string result = Request.Cookies["wl_internalState"].Value;
            try
            {
                result = HttpUtility.UrlDecode(result);
                result = result.Substring(result.IndexOf(key));
                result = result.Substring(key.Length + 3,
                    result.IndexOf(',') - key.Length - 4);
            }
            // wl_internalState could be invalid if LiveID login failed.
            // In this case, we return null.
            catch
            {
                result = null;
            }
            return result;
        }
    }
}