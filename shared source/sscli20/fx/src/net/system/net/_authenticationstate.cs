//------------------------------------------------------------------------------
// <copyright file="_AuthenticationState.cs" company="Microsoft">
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
    using System.Collections;
    using System.IO;
    using System.Runtime.Serialization;
    using System.Security;
    using System.Security.Cryptography.X509Certificates;
    using System.Security.Permissions;
    using System.Text;
    using System.Text.RegularExpressions;
    using System.Threading;
    using System.Globalization;
    using System.Net.Security;

    /// <devdoc>
    /// <para>Used by HttpWebRequest to syncronize and orchestrate authentication<para>
    /// </devdoc>
    internal class AuthenticationState {

        // true if we already attempted pre-authentication regardless if it has been
        // 1) possible,
        // 2) succesfull or
        // 3) unsuccessfull
        private bool                     TriedPreAuth;

        internal Authorization           Authorization;

        internal IAuthenticationModule   Module;

        // used to request a special connection for NTLM
        internal string                  UniqueGroupId;

        // used to distinguish proxy auth from server auth
        private bool                     IsProxyAuth;

        // the Uri of the host we're authenticating (proxy/server)
        // used to match entries in the CredentialCache
        internal Uri                     ChallengedUri;
        private string                   ChallengedSpn;


        internal HttpResponseHeader AuthenticateHeader {
            get {
                return IsProxyAuth ? HttpResponseHeader.ProxyAuthenticate : HttpResponseHeader.WwwAuthenticate;
            }
        }
        internal string AuthorizationHeader {
            get {
                return IsProxyAuth ? HttpKnownHeaderNames.ProxyAuthorization : HttpKnownHeaderNames.Authorization;
            }
        }
        internal HttpStatusCode StatusCodeMatch {
            get {
                return IsProxyAuth ? HttpStatusCode.ProxyAuthenticationRequired : HttpStatusCode.Unauthorized;
            }
        }

        internal AuthenticationState(bool isProxyAuth) {
            IsProxyAuth = isProxyAuth;
        }

        //
        // we need to do this to handle proxies in the correct way before
        // calling into the AuthenticationManager APIs
        //
        private void PrepareState(HttpWebRequest httpWebRequest)
        {
            Uri newUri = IsProxyAuth? httpWebRequest.ServicePoint.InternalAddress: httpWebRequest.Address;

            if ((object)ChallengedUri != (object)newUri)
            {
                if ((object)ChallengedUri == null || (object)ChallengedUri.Scheme != (object)newUri.Scheme || ChallengedUri.Host != newUri.Host || ChallengedUri.Port != newUri.Port)
                {
                    //
                    // must be a new server/port/scheme for this auth state, can happen on a redirect
                    //
                    ChallengedSpn = null;
                }
                ChallengedUri = newUri;
            }
            httpWebRequest.CurrentAuthenticationState = this;
        }
        //
        //
        //
        internal string GetComputeSpn(HttpWebRequest httpWebRequest)
        {
            if (ChallengedSpn != null)
                return ChallengedSpn;

            string spnKey = httpWebRequest.ChallengedUri.GetParts(UriComponents.Scheme | UriComponents.Host | UriComponents.Port | UriComponents.Path, UriFormat.SafeUnescaped);
            string spn = AuthenticationManager.SpnDictionary.InternalGet(spnKey);
            if (spn == null)
            {
                if (!IsProxyAuth && httpWebRequest.ServicePoint.InternalProxyServicePoint)
                {
                    // Here the NT-Security folks need us to attempt a DNS lookup to figure out
                    // the FQDN. only do the lookup for short names (no IP addresses or DNS names)
                    //
                    // Initialize a backup value
                    spn = httpWebRequest.ChallengedUri.Host;

                    if (httpWebRequest.ChallengedUri.HostNameType!=UriHostNameType.IPv6 && httpWebRequest.ChallengedUri.HostNameType!=UriHostNameType.IPv4 && spn.IndexOf('.') == -1)
                    {
                        try {
                            spn = Dns.InternalGetHostByName(spn).HostName;
                            GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::GetComputeSpn() Dns returned host:" + ValidationHelper.ToString(spn));
                        }
                        catch (Exception exception) {
                            if (NclUtilities.IsFatal(exception)) throw;
                            GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::GetComputeSpn() GetHostByName(host) failed:" + ValidationHelper.ToString(exception));
                        }
                    }
                }
                else
                {

                    spn = httpWebRequest.ServicePoint.Hostname;
                }
                spn = "HTTP/" + spn;
                spnKey = httpWebRequest.ChallengedUri.GetParts(UriComponents.SchemeAndServer, UriFormat.SafeUnescaped) + "/";
                AuthenticationManager.SpnDictionary.InternalSet(spnKey, spn);
            }
            return ChallengedSpn = spn;
        }
        //
        internal void PreAuthIfNeeded(HttpWebRequest httpWebRequest, ICredentials authInfo) {
            //
            // attempt to do preauth, if needed
            //
            GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::PreAuthIfNeeded() TriedPreAuth:" + TriedPreAuth.ToString() + " authInfo:" + ValidationHelper.HashString(authInfo));
            if (!TriedPreAuth) {
                TriedPreAuth = true;
                if (authInfo!=null) {
                    PrepareState(httpWebRequest);
                    Authorization preauth = null;
                    try {
                        preauth = AuthenticationManager.PreAuthenticate(httpWebRequest, authInfo);
                        GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::PreAuthIfNeeded() preauth:" + ValidationHelper.HashString(preauth));
                        if (preauth!=null && preauth.Message!=null) {
                            GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::PreAuthIfNeeded() setting TriedPreAuth to Complete:" + preauth.Complete.ToString());
                            UniqueGroupId = preauth.ConnectionGroupId;
                            httpWebRequest.Headers.Set(AuthorizationHeader, preauth.Message);
                        }
                    }
                    catch (Exception exception) {
                        GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::PreAuthIfNeeded() PreAuthenticate() returned exception:" + exception.Message);
                        ClearSession(httpWebRequest);
                    }
                    catch {
                        GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::PreAuthIfNeeded() PreAuthenticate() returned exception: Non-CLS Compliant Exception");
                        ClearSession(httpWebRequest);
                    }
                }
            }
        }

        //
        // attempts to authenticate the request:
        // returns true only if it succesfully called into the AuthenticationManager
        // and got back a valid Authorization and succesfully set the appropriate auth headers
        //
        internal bool AttemptAuthenticate(HttpWebRequest httpWebRequest, ICredentials authInfo) {
            //
            // Check for previous authentication attempts or the presence of credentials
            //
            GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::AttemptAuthenticate() httpWebRequest#" + ValidationHelper.HashString(httpWebRequest) + " AuthorizationHeader:" + AuthorizationHeader.ToString());

            if (Authorization!=null && Authorization.Complete) {
                GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::AttemptAuthenticate() Authorization!=null Authorization.Complete:" + Authorization.Complete.ToString());
                if (IsProxyAuth) {
                    GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::AttemptAuthenticate() ProxyAuth cleaning up auth status");
                    ClearAuthReq(httpWebRequest);
                }
                return false;
            }

            if (authInfo==null) {
                GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::AttemptAuthenticate() authInfo==null Authorization#" + ValidationHelper.HashString(Authorization));
                return false;
            }

            string challenge = httpWebRequest.AuthHeader(AuthenticateHeader);

            if (challenge==null) {
                //
                // the server sent no challenge, but this might be the case
                // in which we're succeeding an authorization handshake to
                // a proxy while a handshake with the server is still in progress.
                // if the handshake with the proxy is complete and we actually have
                // a handshake with the server in progress we can send the authorization header for the server as well.
                //
                if (!IsProxyAuth && Authorization!=null && httpWebRequest.ProxyAuthenticationState.Authorization!=null) {
                    httpWebRequest.Headers.Set(AuthorizationHeader, Authorization.Message);
                }
                GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::AttemptAuthenticate() challenge==null Authorization#" + ValidationHelper.HashString(Authorization));
                return false;
            }

            //
            // if the AuthenticationManager throws on Authenticate,
            // bubble up that Exception to the user
            //
            GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::AttemptAuthenticate() challenge:" + challenge);

            PrepareState(httpWebRequest);
            try {
                Authorization = AuthenticationManager.Authenticate(challenge, httpWebRequest, authInfo);
            }
            catch (Exception exception) {
                Authorization = null;
                GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::PreAuthIfNeeded() PreAuthenticate() returned exception:" + exception.Message);
                ClearSession(httpWebRequest);
                throw;
            }
            catch {
                Authorization = null;
                GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::PreAuthIfNeeded() PreAuthenticate() returned exception: Non-CLS Compliant Exception");
                ClearSession(httpWebRequest);
                throw;
            }


            if (Authorization==null) {
                GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::AttemptAuthenticate() Authorization==null");
                return false;
            }
            if (Authorization.Message==null) {
                GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::AttemptAuthenticate() Authorization.Message==null");
                Authorization = null;
                return false;
            }

            UniqueGroupId = Authorization.ConnectionGroupId;
            GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::AttemptAuthenticate() AuthorizationHeader:" + AuthorizationHeader + " blob: " + Authorization.Message.Length + "bytes Complete:" + Authorization.Complete.ToString());

            try {
                //
                // a "bad" module could try sending bad characters in the HTTP headers.
                // catch the exception from WebHeaderCollection.CheckBadChars()
                // fail the auth process
                // and return the exception to the user as InnerException
                //
                httpWebRequest.Headers.Set(AuthorizationHeader, Authorization.Message);
            }
            catch {
                Authorization = null;
                ClearSession(httpWebRequest);
                throw;
            }

            return true;
        }

        internal void ClearAuthReq(HttpWebRequest httpWebRequest) {
            //
            // if we are authenticating and we're being redirected to
            // another authentication space then remove the current
            // authentication header
            //
            GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::ClearAuthReq() httpWebRequest#" + ValidationHelper.HashString(httpWebRequest) + " " + AuthorizationHeader.ToString() + ": " + ValidationHelper.ToString(httpWebRequest.Headers[AuthorizationHeader]));
            TriedPreAuth = false;
            Authorization = null;
            UniqueGroupId = null;
            httpWebRequest.Headers.Remove(AuthorizationHeader);
        }

        //
        // gives the IAuthenticationModule a chance to update its internal state.
        // do any necessary cleanup and update the Complete status of the associated Authorization.
        //
        internal void Update(HttpWebRequest httpWebRequest) {
            //
            GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::Update() httpWebRequest#" + ValidationHelper.HashString(httpWebRequest) + " Authorization#" + ValidationHelper.HashString(Authorization) + " ResponseStatusCode:" + httpWebRequest.ResponseStatusCode.ToString());

            if (Authorization!=null) {

                PrepareState(httpWebRequest);

                ISessionAuthenticationModule myModule = Module as ISessionAuthenticationModule;

                if (myModule!=null) {
                    //
                    // the whole point here is to complete the Security Context. Sometimes, though,
                    // a bad cgi script or a bad server, could miss sending back the final blob.
                    // in this case we won't be able to complete the handshake, but we'll have to clean up anyway.
                    //
                    string challenge = httpWebRequest.AuthHeader(AuthenticateHeader);
                    GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::Update() Complete:" + Authorization.Complete.ToString() + " Module:" + ValidationHelper.ToString(Module) + " challenge:" + ValidationHelper.ToString(challenge));

                    if (!IsProxyAuth && httpWebRequest.ResponseStatusCode==HttpStatusCode.ProxyAuthenticationRequired) {
                        //
                        // don't call Update on the module, since there's an ongoing
                        // handshake and we don't need to update any state in such a case
                        //
                        GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::Update() skipping call to " + myModule.ToString() + ".Update() since we need to reauthenticate with the proxy");
                    }
                    else {
                        bool complete = true;
                        try {
                            complete = myModule.Update(challenge, httpWebRequest);
                            GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::Update() " + myModule.ToString() + ".Update() returned complete:" + complete.ToString());
                        }
                        catch (Exception exception) {
                            GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::Update() " + myModule.ToString() + ".Update() caught exception:" + exception.Message);
                            ClearSession(httpWebRequest);


                        }
                        catch {
                            GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::Update() " + myModule.ToString() + ".Update() caught exception: Non-CLS Compliant Exception");
                            ClearSession(httpWebRequest);


                        }

                        Authorization.SetComplete(complete);
                    }

                }

                //
                // If authentication was successful, create binding between
                // the request and the authorization for future preauthentication
                //
                if (Module != null && Authorization.Complete && Module.CanPreAuthenticate && httpWebRequest.ResponseStatusCode!=StatusCodeMatch) {
                    GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::Update() handshake is Complete calling BindModule()");
                    AuthenticationManager.BindModule(ChallengedUri, Authorization, Module);
                }
            }
        }

        internal void ClearSession() {
       }

       internal void ClearSession(HttpWebRequest httpWebRequest) {
            PrepareState(httpWebRequest);
            ISessionAuthenticationModule myModule = Module as ISessionAuthenticationModule;
            Module = null;

            if (myModule!=null) {
                try {
                    myModule.ClearSession(httpWebRequest);
                }
                catch (Exception exception) {
                    if (NclUtilities.IsFatal(exception)) throw;

                    GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::ClearSession() " + myModule.ToString() + ".Update() caught exception:" + exception.Message);
                }
                catch {
                    GlobalLog.Print("AuthenticationState#" + ValidationHelper.HashString(this) + "::ClearSession() " + myModule.ToString() + ".Update() caught exception: Non-CLS Compliant Exception");
                }
            }

        }

    }
}
