/****************************** Module Header ******************************\
* Module Name: Default.aspx.cs
* Project:     CSASPNETStripHtmlCode
* Copyright (c) Microsoft Corporation
*
* This page retrieve the entire html code from SourcePage.aspx.
* User can strip or parse many parts of html code, such as pure
* text, images, links, script code, etc.
* The code-sample can be used in many web applications. For example,
* search engines, the search engines need check the short message
* of web-pages, like titles, pure text, images and so on.
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
using System.Net;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;

namespace CSASPNETStripHtmlCode
{
    public partial class Default : System.Web.UI.Page
    {
        string strUrl = String.Empty;
        string strWholeHtml = string.Empty;
        const string MsgPageRetrieveFailed = "Sorry, the web page is not run successful";
        bool flgPageRetrieved = true;
        protected void Page_Load(object sender, EventArgs e)
        {
            strUrl = this.Page.Request.Url.ToString().Replace("Default","SourcePage");         
            tbResult.Text = string.Empty;
        }

        protected void btnRetrieveAll_Click(object sender, EventArgs e)
        {
            strWholeHtml = this.GetWholeHtmlCode(strUrl);
            if (flgPageRetrieved)
            {
                tbResult.Text = strWholeHtml;
            }
            else
            {
                tbResult.Text = MsgPageRetrieveFailed;
            }
        }

        /// <summary>
        /// Retrieve the entire html code from SourcePage.aspx with WebRequest and
        /// WebRespond. We transfer the format of html code to uft-8.
        /// </summary>
        /// <param name="url"></param>
        /// <returns></returns>
        public string GetWholeHtmlCode(string url)
        {
            string strHtml = string.Empty;
            StreamReader strReader = null;
            HttpWebResponse wrpContent = null;       
            try
            {
                HttpWebRequest wrqContent = (HttpWebRequest)WebRequest.Create(strUrl);
                wrqContent.Timeout = 300000;
                wrpContent = (HttpWebResponse)wrqContent.GetResponse();
                if (wrpContent.StatusCode != HttpStatusCode.OK)
                {
                    flgPageRetrieved = false;
                    strHtml = "Sorry, the web page is not run successful";
                }
                if (wrpContent != null)
                {
                    strReader = new StreamReader(wrpContent.GetResponseStream(), Encoding.GetEncoding("utf-8"));
                    strHtml = strReader.ReadToEnd();
                }
            }
            catch (Exception e)
            {
                flgPageRetrieved = false;
                strHtml = e.Message;
            }
            finally 
            {
                if (strReader != null)
                    strReader.Close();
                if (wrpContent != null)
                    wrpContent.Close();
            }
            return strHtml;
        }

        /// <summary>
        /// Retrieve the pure text from html code, this pure text include 
        /// only the Body tags of html.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void btnRetrievePureText_Click(object sender, EventArgs e)
        {
            strWholeHtml = this.GetWholeHtmlCode(strUrl);
            if (flgPageRetrieved)
            {
                string strRegexScript = @"(?m)<body[^>]*>(\w|\W)*?</body[^>]*>";
                string strRegex = @"<[^>]*>";
                string strMatchScript = string.Empty;
                Match matchText = Regex.Match(strWholeHtml, strRegexScript, RegexOptions.IgnoreCase);
                strMatchScript = matchText.Groups[0].Value;
                string strPureText = Regex.Replace(strMatchScript, strRegex, string.Empty, RegexOptions.IgnoreCase);
                tbResult.Text = strPureText;
            }
            else
            {
                tbResult.Text = MsgPageRetrieveFailed;
            }
        }

        /// <summary>
        /// Retrieve the script code from html code.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void btnRetrieveSriptCode_Click(object sender, EventArgs e)
        {
            strWholeHtml = this.GetWholeHtmlCode(strUrl);
            if (flgPageRetrieved)
            {
                string strRegexScript = @"(?m)<script[^>]*>(\w|\W)*?</script[^>]*>";
                string strRegex = @"<[^>]*>";
                string strMatchScript = string.Empty;
                MatchCollection matchList = Regex.Matches(strWholeHtml, strRegexScript, RegexOptions.IgnoreCase);
                StringBuilder strbScriptList = new StringBuilder();
                foreach (Match matchSingleScript in matchList)
                {
                    string strSingleScriptText = Regex.Replace(matchSingleScript.Value, strRegex, string.Empty, RegexOptions.IgnoreCase);
                    strbScriptList.Append(strSingleScriptText + "\r\n");
                }
                tbResult.Text = strbScriptList.ToString();
            }
            else
            {
                tbResult.Text = MsgPageRetrieveFailed;
            }
        }

        /// <summary>
        /// Retrieve the image information from html code
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void btnRetrieveImage_Click(object sender, EventArgs e)
        {
            strWholeHtml = this.GetWholeHtmlCode(strUrl);
            if (flgPageRetrieved)
            {
                string strRegexImg = @"(?is)<img.*?>";
                MatchCollection matchList = Regex.Matches(strWholeHtml, strRegexImg, RegexOptions.IgnoreCase);
                StringBuilder strbImageList = new StringBuilder();

                foreach (Match matchSingleImage in matchList)
                {
                    strbImageList.Append(matchSingleImage.Value + "\r\n");
                }
                tbResult.Text = strbImageList.ToString();
            }
            else
            {
                tbResult.Text = MsgPageRetrieveFailed;
            }
        }

        /// <summary>
        /// Retrieve the links from html code
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void btnRetrievelink_Click(object sender, EventArgs e)
        {
            strWholeHtml = this.GetWholeHtmlCode(strUrl);
            if (flgPageRetrieved)
            {

                string strRegexLink = @"(?is)<a .*?>";
                MatchCollection matchList = Regex.Matches(strWholeHtml, strRegexLink, RegexOptions.IgnoreCase);
                StringBuilder strbLinkList = new StringBuilder();

                foreach (Match matchSingleLink in matchList)
                {
                    strbLinkList.Append(matchSingleLink.Value + "\r\n");
                }
                tbResult.Text = strbLinkList.ToString();
            }
            else
            {
                tbResult.Text = MsgPageRetrieveFailed;
            }
        }

    }
}