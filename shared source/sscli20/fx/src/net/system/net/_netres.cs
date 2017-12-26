//------------------------------------------------------------------------------
// <copyright file="_NetRes.cs" company="Microsoft">
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

    using System.Diagnostics;
    using System.Globalization;

    internal class NetRes {

        /*++
            Constructor

            This is the constructor, marked private because this class shouldn't
            be instantiated.

        --*/
        private NetRes() {
        }

        /*++

            GetWebStatusString - Get a WebExceptionStatus-specific resource string


            This method takes an input string and a WebExceptionStatus. We use the input
            string as a key to find a status message and the webStatus to produce
            a status-specific message, then we combine the two.

            Input:

                Res             - Id for resource string.
                Status          - The WebExceptionStatus to be formatted.

            Returns:

                string for localized message.

        --*/
        public static string GetWebStatusString(string Res, WebExceptionStatus Status) {
            string Msg;
            string StatusMsg;

            StatusMsg = SR.GetString(WebExceptionMapping.GetWebStatusString(Status));

            // Get the base status.

            Msg = SR.GetString(Res);

            // Format the status specific message into the base status and return
            // that

            return String.Format(CultureInfo.CurrentCulture, Msg, StatusMsg);
        }
        
        public static string GetWebStatusString(WebExceptionStatus Status) {
            return SR.GetString(WebExceptionMapping.GetWebStatusString(Status));
        }


        /*++

            GetWebStatusCodeString - Get a StatusCode-specific resource string

            This method is used to map a HTTP status code to a specific user
            readable error code.

            Input:

                statusCode      - Id for resource string.
                Status          - The WebExceptionStatus to be formatted.

            Returns:

                string for localized message.

        --*/
        public static string GetWebStatusCodeString(HttpStatusCode statusCode, string statusDescription) {
            string webStatusCode = "(" + ((int)statusCode).ToString(NumberFormatInfo.InvariantInfo) + ")";
            string statusMessage = null;
            //
            // Now combine the label with the base enum key and look up the status msg.
            //
            try {
                //
                // convert the HttpStatusCode to its label and look it up.
                //
                statusMessage = SR.GetString("net_httpstatuscode_" + statusCode.ToString(), null);
            }
            catch {
            }

            if (statusMessage!=null && statusMessage.Length>0) {
                webStatusCode += " " + statusMessage;
            }
            else {
                //
                // Otherwise try to map the base status.
                //
                if (statusDescription!=null && statusDescription.Length>0) {
                    webStatusCode += " " + statusDescription;
                }
            }

            return webStatusCode;
        }

        public static string GetWebStatusCodeString(FtpStatusCode statusCode, string statusDescription) {
            string webStatusCode = "(" + ((int)statusCode).ToString(NumberFormatInfo.InvariantInfo) + ")";
            string statusMessage = null;
            //
            // Now combine the label with the base enum key and look up the status msg.
            //
            try {
                //
                // convert the HttpStatusCode to its label and look it up.
                //
                statusMessage = SR.GetString("net_ftpstatuscode_" + statusCode.ToString(), null);
            }
            catch {
            }

            if (statusMessage!=null && statusMessage.Length>0) {
                webStatusCode += " " + statusMessage;
            }
            else {
                //
                // Otherwise try to map the base status.
                //
                if (statusDescription!=null && statusDescription.Length>0) {
                    webStatusCode += " " + statusDescription;
                }
            }

            return webStatusCode;
        }
    }
}

