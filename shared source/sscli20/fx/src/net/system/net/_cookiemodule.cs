//------------------------------------------------------------------------------
// <copyright file="_CookieModule.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>
//------------------------------------------------------------------------------

namespace System.Net {

    internal static class CookieModule {

    // fields

    // constructors

    // properties

    // methods

        internal static void OnSendingHeaders(HttpWebRequest httpWebRequest) {
            GlobalLog.Print("CookieModule::OnSendingHeaders()");
            try {
                if (httpWebRequest.CookieContainer == null) {
                    return;
                }
                //
                // remove all current cookies. This could be a redirect
                //
                httpWebRequest.Headers.RemoveInternal(HttpKnownHeaderNames.Cookie);
                //
                // add in the new headers from the cookie container for this request
                //
                string optCookie2;
                string cookieString = httpWebRequest.CookieContainer.GetCookieHeader(
                                                                            httpWebRequest.Address,
                                                                            out optCookie2);

                if (cookieString.Length > 0) {
                    GlobalLog.Print("CookieModule::OnSendingHeaders() setting Cookie header to:[" + cookieString + "]");
                    httpWebRequest.Headers[HttpKnownHeaderNames.Cookie] = cookieString;

                }
            }
            catch {
            }

        }

        internal static void OnReceivedHeaders(HttpWebRequest httpWebRequest) {
            GlobalLog.Print("CookieModule.OnReceivedHeaders()");
            //
            // if the app doesn't want us to handle cookies then there's nothing
            // to do. Note that we're leaving open the possibility that these
            // settings could be changed between the request being made and the
            // response received
            //
            try {
                if (httpWebRequest.CookieContainer == null) {
                    return;
                }

                //
                // add any received cookies for this response to the container
                //
                HttpWebResponse response = httpWebRequest._HttpResponse as HttpWebResponse;
                if (response == null) {
                    return;
                }

                CookieCollection cookies = null;
                try {
                    string cookieString = response.Headers.SetCookie;
                    GlobalLog.Print("CookieModule::OnSendingHeaders() received Set-Cookie:[" + cookieString + "]");
                    if ((cookieString != null) && (cookieString.Length > 0)) {
                        cookies = httpWebRequest.CookieContainer.CookieCutter(
                                                            response.ResponseUri,
                                                            HttpKnownHeaderNames.SetCookie,
                                                            cookieString,
                                                            false);
                    }
                }
                catch {
                }

                try {
                    string cookieString = response.Headers.SetCookie2;
                    GlobalLog.Print("CookieModule::OnSendingHeaders() received Set-Cookie2:[" + cookieString + "]");
                    if ((cookieString != null) && (cookieString.Length > 0)) {
                        CookieCollection cookies2 = httpWebRequest.CookieContainer.CookieCutter(
                                                                    response.ResponseUri,
                                                                    HttpKnownHeaderNames.SetCookie2,
                                                                    cookieString,
                                                                    false);
                        if (cookies != null && cookies.Count != 0) {
                            cookies.Add(cookies2);
                        }
                        else {
                            cookies = cookies2;
                        }
                    }
                }
                catch {
                }
                if (cookies != null) {
                    response.Cookies = cookies;
                }
            }
            catch {
            }

        }
    }
}


