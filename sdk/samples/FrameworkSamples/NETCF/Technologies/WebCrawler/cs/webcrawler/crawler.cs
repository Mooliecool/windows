//---------------------------------------------------------------------
// File: Crawler.cs
//
// This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
// Copyright (C) Microsoft Corporation.  All rights reserved.
// 
// This source code is intended only as a supplement to Microsoft
// Development Tools and/or on-line documentation.  See these other
// materials for detailed information regarding Microsoft code samples.
// 
// THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//---------------------------------------------------------------------

using System;
using System.Collections;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Net;
using System.Text.RegularExpressions;
using System.Threading;
using System.Windows.Forms;

namespace Microsoft.Samples.NetCF
{
    /// <summary>
    /// The worker class
    /// </summary>
    public class Crawler
    {
        /// <summary>
        /// Have we been asked to stop?
        /// </summary>
        private bool stopNow = true;

        /// <summary>
        /// The link we will use to start our crawl
        /// </summary>
        private string startingPage = "";

        /// <summary>
        /// Disable proxy for web requests
        /// </summary>
        private bool noProxy = false;

        /// <summary>
        /// Event(s)
        /// </summary>
        public delegate void CurrentPageEventHandler(object sender,
                                                    CurrentPageEventArgs e);
        public event CurrentPageEventHandler CurrentPageEvent;
        public event EventHandler PageFoundEvent;
        public event EventHandler CrawlFinishedEvent;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="startingPage">
        /// The link we will use to start our crawl
        /// </param>
        /// <param name="noProxy">
        /// Disable crawling through an HTTP proxy server
        /// </param>
        public Crawler(string startingPage,
                    bool noProxy)
        {
            this.startingPage = startingPage;

            this.noProxy = noProxy;
        }

        /// <summary>
        /// Start the crawl
        /// </summary>
        public void Start()
        {
            lock (this)
            {
                this.stopNow = false;
            }
            new Thread(new ThreadStart(Crawl)).Start();
        }

        /// <summary>
        /// Stop the crawl
        /// </summary>
        public void Stop()
        {
            lock (this)
            {
                this.stopNow = true;
            }
        }

        /// <summary>
        /// Does the link point to HTML data?
        /// </summary>
        /// <param name="pageAddress">
        /// The link we are checking
        /// </param>
        /// <param name="status">
        /// The status code returned by the connection attempt
        /// </param>
        /// <returns>
        /// True if the link points to an HTML page, false otherwise
        /// </returns>
        private bool PageIsHtml(string pageAddress,
                                out HttpStatusCode status)
        {
            HttpWebResponse resp = null;
            bool isHtml = false;
            const string TypeHTML = "text/html";

            status = (HttpStatusCode)(-2);  // not html

            try
            {
                // create the web request
                HttpWebRequest req = (HttpWebRequest)WebRequest.Create(pageAddress);

                // disable the proxy?
                if (this.noProxy)
                {
                    req.Proxy = new WebProxy();
                }

                // get headers only
                req.Method = "HEAD";

                // make the connection
                resp = (HttpWebResponse)req.GetResponse();

                // read the headers
                WebHeaderCollection headers = resp.Headers;

                // check the content type
                string contentType = headers["Content-type"];
                if (contentType != null)
                {
                    contentType = contentType.ToLower(
                                            CultureInfo.InvariantCulture);
                    if (contentType.StartsWith(TypeHTML))
                    {
                        isHtml = true;
                    }
                }

                // get the status code (should be 200)
                status = resp.StatusCode;
            }
            catch (WebException e)
            {
                string str = string.Format(CultureInfo.CurrentCulture, 
                                        "Caught WebException: {0}",
                                        e.Status.ToString()); ;

                resp = (HttpWebResponse)e.Response;
                if (null != resp)
                {
                    // get the failure code from the response
                    status = resp.StatusCode;
                    str = string.Format(CultureInfo.CurrentCulture, 
                                        "{0} ({1})", 
                                        str, status);
                }
                else
                {
                    // generic connection error
                    status = (HttpStatusCode)(-1);
                }

                // update the ui so we can know what went wrong
                if (CurrentPageEvent != null)
                {
                    CurrentPageEvent(this, 
                                    new CurrentPageEventArgs(str));
                }
            }
            catch (NotSupportedException)
            {
                // the link is a reference that 
                //  WebRequest.Create does not support
                status = (HttpStatusCode)(-1);
            }
            finally
            {
                // close the response
                if (null != resp)
                {
                    resp.Close();
                }
            }

            // done
            return isHtml;
        }

        /// <summary>
        /// Read the specified web page's data
        /// </summary>
        /// <param name="pageUri">
        /// Uri object pointing to the desired page
        /// </param>
        /// <param name="pageData">
        /// The data downloaded from the provided Uri
        /// </param>
        /// <returns>
        /// The status code returned by the connection attempt
        /// </returns>
        private HttpStatusCode GetPageData(ref Uri pageUri,
                                        out string pageData)
        {
            HttpStatusCode status = (HttpStatusCode)0;
            HttpWebResponse resp = null;

            // initialize the out param (in case of error)
            pageData = "";

            try
            {
                // create the web request
                HttpWebRequest req = (HttpWebRequest)WebRequest.Create(pageUri);

                // disable the proxy?
                if (this.noProxy)
                {
                    req.Proxy = new WebProxy();
                }

                // make the connection
                resp = (HttpWebResponse)req.GetResponse();
                pageUri = resp.ResponseUri;

                // get the page data
                StreamReader sr = new StreamReader(resp.GetResponseStream());
                pageData = sr.ReadToEnd();
                sr.Close();

                // get the status code (should be 200)
                status = resp.StatusCode;
            }
            catch (WebException e)
            {
                string str = string.Format(CultureInfo.CurrentCulture, 
                                        "Caught WebException: {0}",
                                        e.Status.ToString()); ;

                resp = (HttpWebResponse)e.Response;
                if (null != resp)
                {
                    // get the failure code from the response
                    status = resp.StatusCode;
                    str = string.Format(CultureInfo.CurrentCulture, 
                                        "{0} ({1})", 
                                        str, status);
                }
                else
                {
                    // generic connection error
                    status = (HttpStatusCode)(-1);
                }

                // update the ui so we can know what went wrong
                if (CurrentPageEvent != null)
                {
                    CurrentPageEvent(this, 
                                    new CurrentPageEventArgs(str));
                }
            }
            finally
            {
                // close the response
                if (null != resp)
                {
                    resp.Close();
                }
            }

            // done
            return status;
        }

        /// <summary>
        /// Discover the links on the specified web page
        /// </summary>
        /// <param name="pageUri">
        /// Uri object pointing to the desired page
        /// </param>
        /// <param name="pageBody">
        /// The data downloaded from the specified Uri
        /// </param>
        /// <param name="tag">
        /// The HTML tag to match
        /// </param>
        /// <param name="attribute">
        /// The HTML attribute to match
        /// </param>
        /// <param name="links">
        /// Hashtable to be used for storing discovered links
        /// </param>
        private void GetPageLinks(Uri pageUri,
                                string pageBody,
                                string tag,
                                string attribute,
                                Hashtable links)
        {
            // construct the match strings
            string tagPattern = string.Format(CultureInfo.InvariantCulture, 
                                            "< *{0} +[^<>]*>", 
                                            tag);
            string attributePattern = string.Format(CultureInfo.InvariantCulture, 
                                                    "{0} *= *([^ >])*", 
                                                    attribute);

            // get rid of line breaks and tabs (replace them with spaces)
            pageBody = Regex.Replace(pageBody,
                                    "(\\n|\\r|\\t)",
                                    " ");

            // match the html tag
            MatchCollection tagMatches = Regex.Matches(pageBody,
                                                    tagPattern,
                                                    RegexOptions.IgnoreCase);
            foreach (Match m in tagMatches)
            {
                if (m.Groups[0].Success)
                {
                    // now match the attribute within the matched tag
                    string tagData = m.Groups[0].Captures[0].ToString();
                    Match attributeMatch = Regex.Match(tagData,
                                                    attributePattern,
                                                    RegexOptions.IgnoreCase);
                    if (attributeMatch.Groups[0].Success)
                    {
                        // get rid of the quotes around the link reference
                        string link = attributeMatch.Groups[0].Captures[0].ToString();
                        link = Regex.Replace(link, "[ \"']", "");

                        // trim off the attribute
                        //  leave only the link reference
                        link = link.Substring(attribute.Length + 1);

                        // add the link to our local hashtable
                        try
                        {
                            // reconstruct relative paths
                            link = new Uri(pageUri, link).AbsoluteUri;

                            // add the link
                            links.Add(link, new LinkInfo(link, 
                                    (HttpStatusCode)0));

                            // ring the event handler(s) 
                            //  letting them know we found a new link
                            if (null != PageFoundEvent)
                            {
                                PageFoundEvent(this, EventArgs.Empty);
                            }
                        }
                        catch (IndexOutOfRangeException)
                        { /* skip the malformed uri */ }
                        catch (UriFormatException)
                        { /* skip the malformed uri */ }
                        catch (ArgumentException)
                        { /* duplicate link, skipping */ }
                    }
                }
            }

        }

        /// <summary>
        /// Worker thread method
        /// </summary>
        private void Crawl()
        {
            Hashtable links = null;

            try
            {
                // store for all the links we encounter
                //  this will keep us from walking the same link more
                //  than once
                links = new Hashtable(StringComparer.InvariantCultureIgnoreCase);

                // make sure the starting page has a scheme
                if (-1 == this.startingPage.IndexOf("://"))
                {
                    // add http to the page link
                    this.startingPage = string.Format(CultureInfo.InvariantCulture, 
                                                    "http://{0}", 
                                                    this.startingPage);
                }

                // add the starting page to the links table
                //  mark the link as not having been tried
                links.Add(this.startingPage, new LinkInfo(this.startingPage,
                                                        (HttpStatusCode)0));
                while (!this.stopNow)
                {
                    // temporary storage
                    Hashtable found = 
                        new Hashtable(StringComparer.InvariantCultureIgnoreCase);

                    foreach (string page in links.Keys)
                    {
                        // check to see if we have been asked to stop
                        if (this.stopNow)
                        {
                            continue;
                        }

                        // ring our event handler(s) to tell them what page
                        //  is being crawled
                        if (CurrentPageEvent != null)
                        {
                            CurrentPageEvent(this, 
                                            new CurrentPageEventArgs(page));
                        }

                        LinkInfo li = (LinkInfo)links[page];
                        // check that the page contains html text
                        try
                        {
                            // get the currently stored HttpStatusCode
                            //  for the target page
                            HttpStatusCode currentStatus = li.StatusCode;

                            // check to see if we have tried to connect
                            //  and if the page is HTML
                            if (((HttpStatusCode)0 == currentStatus) &&
                                PageIsHtml(page, out currentStatus))
                            {
                                // read the page
                                Uri pageUri = new Uri(page);
                                string pageData = "";
                                currentStatus = GetPageData(ref pageUri,
                                                        out pageData);
                                
                                // if we successfully retrieved the page data
                                if (HttpStatusCode.OK == currentStatus)
                                {
                                    // <a href=
                                    GetPageLinks(pageUri,
                                                pageData,
                                                "a",
                                                "href",
                                                found);
                                    // <frame src=
                                    GetPageLinks(pageUri,
                                                pageData,
                                                "frame",
                                                "src",
                                                found);
                                    // <area href=
                                    GetPageLinks(pageUri,
                                                pageData,
                                                "area",
                                                "href",
                                                found);
                                    // <link href=
                                    GetPageLinks(pageUri,
                                                pageData,
                                                "link",
                                                "href",
                                                found);
                                }
                            }
                            else
                            { /* the link is not a web page */ }

                            // update the HttpStatusCode for the
                            //  target page
                            li.StatusCode = currentStatus;
                        }
                        catch (UriFormatException)
                        {
                            // the link is invalid.
                            //  ensure that it is not crawled in the future
                            li.StatusCode = (HttpStatusCode)(-1);

                            // ring our event handler(s) to allow the 
                            //  interface to display the crawl status
                            //  message
                            if (CurrentPageEvent != null)
                            {
                                String message = String.Format(
                                    CultureInfo.CurrentCulture, 
                                    "Unable to crawl {0} (UriFormatException)",
                                    page);
                                CurrentPageEvent(this, 
                                                new CurrentPageEventArgs(message));
                            }
                        }
                    }

                    // did we discover any new links?
                    if (0 == found.Count)
                    {
                        // nope, we're done
                        lock (this)
                        {
                            this.stopNow = true;
                        }
                        continue;
                    }

                    // add found links to the master list
                    foreach (string page in found.Keys)
                    {
                        if (!links.ContainsKey(page))
                        {
                            links.Add(page, (LinkInfo)found[page]);
                        }
                    }
                }

            }
            catch (OutOfMemoryException)
            {
                // we're out of memory, we need to stop

                // release the memory we have consumed
                links = null;

                lock (this)
                {
                    this.stopNow = true;
                }

                // ring our event handler(s) to allow the interface to 
                //  display the out of memory status
                if (null != CurrentPageEvent)
                {
                    CurrentPageEvent(this, 
                                    new CurrentPageEventArgs(
                                        "Crawl halted: out of memory"));
                }

            }
            catch (Exception e)
            {
                // something bad happened, we need to stop
                lock (this)
                {
                    this.stopNow = true;
                }

                // ring our event handler(s) to allow the interface to 
                //  display the exception message
                if (null != CurrentPageEvent)
                {
                    string message = string.Format(CultureInfo.CurrentCulture, 
                                                "Crawl halted: {0} - {1}",
                                                e.ToString(),
                                                e.Message);
                    CurrentPageEvent(this, 
                                    new CurrentPageEventArgs(message));
                }
            }

            // ring our event handler(s) to let them know we are done
            if (null != CrawlFinishedEvent)
            {
                CrawlFinishedEvent(this, EventArgs.Empty);
            }
        }
    }

    /// <summary>
    /// Class providing the current page address to the CurrentPageEvent handler
    /// </summary>
    public class CurrentPageEventArgs : EventArgs
    {
        
        private string pageAddressValue;
        /// <summary>
        /// The address of the current page(ex: http://www.microsoft.com)
        /// </summary>
        public string PageAddress
        {
            get
            {
                return this.pageAddressValue;
            }
        }

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="page">
        /// The address of the current page
        /// </param>
        public CurrentPageEventArgs(string page)
        {
            this.pageAddressValue = page;
        }
    }
}
