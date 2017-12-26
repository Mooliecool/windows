//------------------------------------------------------------------------------
// <copyright file="_AutoWebProxyScriptHelper.cs" company="Microsoft">
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

#pragma warning disable 618

#define AUTOPROXY_MANAGED_JSCRIPT

namespace System.Net
{
#if AUTOPROXY_MANAGED_JSCRIPT
    using System.Security.Permissions;
#endif
    using System.Net.NetworkInformation;
    using System.Text;
    using System.Text.RegularExpressions;
    using System.Globalization;
    using System.Reflection;
    using System.Runtime.InteropServices;
    using System.Net.Sockets;

    /// <summary>
    /// Provides a set of functions that can be called by the JS script.  There are based on
    /// an earlier API set that is used for these networking scripts.
    /// for a description of the API see:
    /// http://home.netscape.com/eng/mozilla/2.0/relnotes/demo/proxy-live.html 
    /// </summary>
#if !AUTOPROXY_MANAGED_JSCRIPT
    [ComVisible(true)]
    [ClassInterface(ClassInterfaceType.AutoDispatch)]
    public sealed class WebProxyScriptHelper
    {
        internal WebProxyScriptHelper() { }
#else
    internal class WebProxyScriptHelper : IReflect
    {
        private class MyMethodInfo : MethodInfo {
            string name;
            // used by JScript
            public MyMethodInfo(string name) : base() {
                GlobalLog.Print("MyMethodInfo::.ctor() name:" + name);
                this.name = name;
            }
            // used by JScript
            public override Type ReturnType {
                get { 
                    GlobalLog.Print("MyMethodInfo::ReturnType()");
                    Type type = null;
                    if (string.Compare(name, "isPlainHostName", StringComparison.Ordinal)==0) {
                        type = typeof(bool);
                    }
                    else if (string.Compare(name, "dnsDomainIs", StringComparison.Ordinal)==0) {
                        type = typeof(bool);
                    }
                    else if (string.Compare(name, "localHostOrDomainIs", StringComparison.Ordinal)==0) {
                        type = typeof(bool);
                    }
                    else if (string.Compare(name, "isResolvable", StringComparison.Ordinal)==0) {
                        type = typeof(bool);
                    }
                    else if (string.Compare(name, "dnsResolve", StringComparison.Ordinal)==0) {
                        type = typeof(string);
                    }
                    else if (string.Compare(name, "myIpAddress", StringComparison.Ordinal)==0) {
                        type = typeof(string);
                    }
                    else if (string.Compare(name, "dnsDomainLevels", StringComparison.Ordinal)==0) {
                        type = typeof(int);
                    }
                    else if (string.Compare(name, "isInNet", StringComparison.Ordinal)==0) {
                        type = typeof(bool);
                    }
                    else if (string.Compare(name, "shExpMatch", StringComparison.Ordinal)==0) {
                        type = typeof(bool);
                    }
                    else if (string.Compare(name, "weekdayRange", StringComparison.Ordinal)==0) {
                        type = typeof(bool);
                    }
                    GlobalLog.Print("MyMethodInfo::ReturnType() name:" + name + " type:" + type.FullName);
                    return type;
                }
            }
            // used by JScript
            public override ICustomAttributeProvider ReturnTypeCustomAttributes {
                get { 
                    GlobalLog.Print("MyMethodInfo::ReturnTypeCustomAttributes()");
                    return null;
                }
            }
            public override RuntimeMethodHandle MethodHandle {
                get { 
                    GlobalLog.Print("MyMethodInfo::MethodHandle()");
                    return new RuntimeMethodHandle();
                }
            }
            public override MethodAttributes Attributes {
                get { 
                    GlobalLog.Print("MyMethodInfo::Attributes()");
                    return MethodAttributes.Public;
                }
            }
            public override string Name {
                get { 
                    GlobalLog.Print("MyMethodInfo::Name()");
                    return name;
                }
            }
            // used by JScript
            public override Type DeclaringType {
                get { 
                    GlobalLog.Print("MyMethodInfo::DeclaringType()");
                    return typeof(MyMethodInfo);
                }
            }
            public override Type ReflectedType {
                get { 
                    GlobalLog.Print("MyMethodInfo::ReflectedType()");
                    return null;
                }
            }
            public override object[] GetCustomAttributes(bool inherit) {
                GlobalLog.Print("MyMethodInfo::GetCustomAttributes() inherit:" + inherit);
                return null;
            }
            public override object[] GetCustomAttributes(Type type, bool inherit) {
                GlobalLog.Print("MyMethodInfo::GetCustomAttributes() inherit:" + inherit);
                return null;
            }
            public override bool IsDefined(Type type, bool inherit) {
                GlobalLog.Print("MyMethodInfo::IsDefined() type:" + type.FullName + " inherit:" + inherit);
                return type.Equals(typeof(WebProxyScriptHelper));
            }
            // used by JScript
            public override object Invoke(object target, BindingFlags bindingAttr, Binder binder, object[] args, CultureInfo culture) {
                GlobalLog.Print("MyMethodInfo::Invoke() target:" + target);
                return typeof(WebProxyScriptHelper).GetMethod(name, (BindingFlags)unchecked(-1)).Invoke(target, (BindingFlags)unchecked(-1), binder, args, culture);
            }
            public override ParameterInfo[] GetParameters() {
                GlobalLog.Print("MyMethodInfo::GetParameters() name:" + name);
                ParameterInfo[] pars = typeof(WebProxyScriptHelper).GetMethod(name, (BindingFlags)unchecked(-1)).GetParameters();
                GlobalLog.Print("MyMethodInfo::GetParameters() returning pars.Length:" + pars.Length);
                return pars;
            }
            public override MethodImplAttributes GetMethodImplementationFlags() {
                GlobalLog.Print("MyMethodInfo::GetMethodImplementationFlags()");
                return MethodImplAttributes.IL;
            }
            public override MethodInfo GetBaseDefinition() {
                GlobalLog.Print("MyMethodInfo::GetBaseDefinition()");
                return null;
            }


            public override Module Module
            {
                get
                {
                    return GetType().Module;
                }
            }
        }
        MethodInfo IReflect.GetMethod(string name, BindingFlags bindingAttr, Binder binder, Type[] types, ParameterModifier[] modifiers) {
            GlobalLog.Print("WebProxyScriptHelper::IReflect() GetMethod(1) name:" + ValidationHelper.ToString(name) + " bindingAttr:" + bindingAttr);
            return null;
        }
        MethodInfo IReflect.GetMethod(string name, BindingFlags bindingAttr) {
            GlobalLog.Print("WebProxyScriptHelper::IReflect() GetMethod(2) name:" + ValidationHelper.ToString(name) + " bindingAttr:" + bindingAttr);
            return null;
        }
        MethodInfo[] IReflect.GetMethods(BindingFlags bindingAttr) {
            GlobalLog.Print("WebProxyScriptHelper::IReflect() GetMethods() bindingAttr:" + bindingAttr);
            return new MethodInfo[0];
        }
        FieldInfo IReflect.GetField(string name, BindingFlags bindingAttr) {
            GlobalLog.Print("WebProxyScriptHelper::IReflect() GetField() name:" + ValidationHelper.ToString(name) + " bindingAttr:" + bindingAttr);
            return null;
        }
        FieldInfo[] IReflect.GetFields(BindingFlags bindingAttr) {
            GlobalLog.Print("WebProxyScriptHelper::IReflect() GetFields() bindingAttr:" + bindingAttr);
            return new FieldInfo[0];
        }
        PropertyInfo IReflect.GetProperty(string name, BindingFlags bindingAttr) {
            GlobalLog.Print("WebProxyScriptHelper::IReflect() GetProperty(1) name:" + ValidationHelper.ToString(name) + " bindingAttr:" + bindingAttr);
            return null;
        }
        PropertyInfo IReflect.GetProperty(string name, BindingFlags bindingAttr, Binder binder, Type returnType, Type[] types, ParameterModifier[] modifiers) {
            GlobalLog.Print("WebProxyScriptHelper::IReflect() GetProperty(2) name:" + ValidationHelper.ToString(name) + " bindingAttr:" + bindingAttr);
            return null;
        }
        PropertyInfo[] IReflect.GetProperties(BindingFlags bindingAttr) {
            GlobalLog.Print("WebProxyScriptHelper::IReflect() GetProperties() bindingAttr:" + bindingAttr);
            return new PropertyInfo[0];
        }
        // used by JScript
        MemberInfo[] IReflect.GetMember(string name, BindingFlags bindingAttr) {
            GlobalLog.Print("WebProxyScriptHelper::IReflect() GetMember() name:" + ValidationHelper.ToString(name) + " bindingAttr:" + bindingAttr);
            return new MemberInfo[]{new MyMethodInfo(name)};
        }
        MemberInfo[] IReflect.GetMembers(BindingFlags bindingAttr) {
            GlobalLog.Print("WebProxyScriptHelper::IReflect() GetMembers() bindingAttr:" + bindingAttr);
            return new MemberInfo[0];
        }
        object IReflect.InvokeMember(string name, BindingFlags bindingAttr, Binder binder, object target, object[] args, ParameterModifier[] modifiers, CultureInfo culture, string[] namedParameters) {
            GlobalLog.Print("WebProxyScriptHelper::IReflect() InvokeMember() name:" + ValidationHelper.ToString(name) + " bindingAttr:" + bindingAttr);
            return null;
        }
        Type IReflect.UnderlyingSystemType {
            get {
                GlobalLog.Print("WebProxyScriptHelper::IReflect() UnderlyingSystemType_get()");
                return null;
            }
        }   
#endif

        public bool isPlainHostName(string hostName) {
            GlobalLog.Print("WebProxyScriptHelper::isPlainHostName() hostName:" + ValidationHelper.ToString(hostName));
            if (hostName==null) {
                if(Logging.On)Logging.PrintWarning(Logging.Web, SR.GetString(SR.net_log_proxy_called_with_null_parameter, "WebProxyScriptHelper.isPlainHostName()", "hostName"));
                throw new ArgumentNullException("hostName");
            }
            return hostName.IndexOf('.') == -1;
        }

        public bool dnsDomainIs(string host, string domain) {
            GlobalLog.Print("WebProxyScriptHelper::dnsDomainIs() host:" + ValidationHelper.ToString(host) + " domain:" + ValidationHelper.ToString(domain));
            if (host==null) {
                if(Logging.On)Logging.PrintWarning(Logging.Web, SR.GetString(SR.net_log_proxy_called_with_null_parameter, "WebProxyScriptHelper.dnsDomainIs()", "host"));
                throw new ArgumentNullException("host");
            }
            if (domain==null) {
                if(Logging.On)Logging.PrintWarning(Logging.Web, SR.GetString(SR.net_log_proxy_called_with_null_parameter, "WebProxyScriptHelper.dnsDomainIs()", "domain"));
                throw new ArgumentNullException("domain");
            }
            int index = host.LastIndexOf(domain);
            return index != -1 && (index+domain.Length) == host.Length;
        }

        /// <devdoc>
        /// <para>         
        /// This is a strange function, if its not a local hostname
        /// we do a straight compare against the passed in domain
        /// string.  If its not a direct match, then its false,
        /// even if the root of the domain/hostname are the same.
        /// </para>
        /// </devdoc>
        public bool localHostOrDomainIs(string host, string hostDom) {
            GlobalLog.Print("WebProxyScriptHelper::localHostOrDomainIs() host:" + ValidationHelper.ToString(host) + " hostDom:" + ValidationHelper.ToString(hostDom));
            if (host==null) {
                if(Logging.On)Logging.PrintWarning(Logging.Web, SR.GetString(SR.net_log_proxy_called_with_null_parameter, "WebProxyScriptHelper.localHostOrDomainIs()", "host"));
                throw new ArgumentNullException("host");
            }
            if (hostDom==null) {
                if(Logging.On)Logging.PrintWarning(Logging.Web, SR.GetString(SR.net_log_proxy_called_with_null_parameter, "WebProxyScriptHelper.localHostOrDomainIs()", "hostDom"));
                throw new ArgumentNullException("hostDom");
            }
            if (isPlainHostName(host)) {
                 int index = hostDom.IndexOf('.');
                 if (index > 0) {
                     hostDom = hostDom.Substring(0,index);
                 }
            }
            return string.Compare(host, hostDom, StringComparison.OrdinalIgnoreCase)==0;
        }

        public bool isResolvable(string host) {
            GlobalLog.Print("WebProxyScriptHelper::isResolvable() host:" + ValidationHelper.ToString(host));
            if (host==null) {
                if(Logging.On)Logging.PrintWarning(Logging.Web, SR.GetString(SR.net_log_proxy_called_with_null_parameter, "WebProxyScriptHelper.isResolvable()", "host"));
                throw new ArgumentNullException("host");
            }
            IPHostEntry ipHostEntry = null;
            try
            {
                ipHostEntry = Dns.InternalGetHostByName(host);
            }
            catch { }
            if (ipHostEntry == null)
            {
                return false;
            }
            for (int i = 0; i < ipHostEntry.AddressList.Length; i++)
            {
                if (ipHostEntry.AddressList[i].AddressFamily == AddressFamily.InterNetwork)
                {
                    return true;
                }
            }
            return false;
        }

        public string dnsResolve(string host) {
            GlobalLog.Print("WebProxyScriptHelper::dnsResolve() host:" + ValidationHelper.ToString(host));
            if (host==null) {
                if(Logging.On)Logging.PrintWarning(Logging.Web, SR.GetString(SR.net_log_proxy_called_with_null_parameter, "WebProxyScriptHelper.dnsResolve()", "host"));
                throw new ArgumentNullException("host");
            }
            IPHostEntry ipHostEntry = null;
            try
            {
                ipHostEntry = Dns.InternalGetHostByName(host);
            }
            catch { }
            if (ipHostEntry == null)
            {
                return string.Empty;
            }
            for (int i = 0; i < ipHostEntry.AddressList.Length; i++)
            {
                if (ipHostEntry.AddressList[i].AddressFamily == AddressFamily.InterNetwork)
                {
                    return ipHostEntry.AddressList[i].ToString();
                }
            }
            return string.Empty;
        }

        public string myIpAddress() {
            GlobalLog.Print("WebProxyScriptHelper::myIpAddress()");
            IPAddress[] ipAddresses = NclUtilities.LocalAddresses;
            for (int i = 0; i < ipAddresses.Length; i++)
            {
                if (!IPAddress.IsLoopback(ipAddresses[i]) && ipAddresses[i].AddressFamily == AddressFamily.InterNetwork)
                {
                    return ipAddresses[i].ToString();
                }
            }
            return string.Empty;
        }

        public int dnsDomainLevels(string host) {
            GlobalLog.Print("WebProxyScriptHelper::dnsDomainLevels() host:" + ValidationHelper.ToString(host));
            if (host==null) {
                if(Logging.On)Logging.PrintWarning(Logging.Web, SR.GetString(SR.net_log_proxy_called_with_null_parameter, "WebProxyScriptHelper.dnsDomainLevels()", "host"));
                throw new ArgumentNullException("host");
            }
            int index = 0;
            int domainCount = 0;
            while((index = host.IndexOf('.', index)) != -1) {
                domainCount++;
                index++;
            }
            return domainCount;
        }

        public bool isInNet(string host, string pattern, string mask) {
            GlobalLog.Print("WebProxyScriptHelper::isInNet() host:" + ValidationHelper.ToString(host) + " pattern:" + ValidationHelper.ToString(pattern) + " mask:" + ValidationHelper.ToString(mask));
            if (host==null) {
                if(Logging.On)Logging.PrintWarning(Logging.Web, SR.GetString(SR.net_log_proxy_called_with_null_parameter, "WebProxyScriptHelper.isInNet()", "host"));
                throw new ArgumentNullException("host");
            }
            if (pattern==null) {
                if(Logging.On)Logging.PrintWarning(Logging.Web, SR.GetString(SR.net_log_proxy_called_with_null_parameter, "WebProxyScriptHelper.isInNet()", "pattern"));
                throw new ArgumentNullException("pattern");
            }
            if (mask==null) {
                if(Logging.On)Logging.PrintWarning(Logging.Web, SR.GetString(SR.net_log_proxy_called_with_null_parameter, "WebProxyScriptHelper.isInNet()", "mask"));
                throw new ArgumentNullException("mask");
            }
            try {
                IPAddress hostAddress = IPAddress.Parse(host);
                IPAddress patternAddress = IPAddress.Parse(pattern);
                IPAddress maskAddress = IPAddress.Parse(mask);

                byte[] maskAddressBytes = maskAddress.GetAddressBytes();
                byte[] hostAddressBytes = hostAddress.GetAddressBytes();
                byte[] patternAddressBytes = patternAddress.GetAddressBytes();
                if (maskAddressBytes.Length!=hostAddressBytes.Length || maskAddressBytes.Length!=patternAddressBytes.Length) {
                    return false;
                }
                for (int i=0; i<maskAddressBytes.Length; i++) {
                    if ( (patternAddressBytes[i] & maskAddressBytes[i]) != (hostAddressBytes[i] & maskAddressBytes[i]) ) {
                        return false;
                    }
                }
            }
            catch {
                return false;
            }
            return true;
        }

        // See bug 87334 for details on the implementation.
        public bool shExpMatch(string host, string pattern) {
            GlobalLog.Print("WebProxyScriptHelper::shExpMatch() host:" + ValidationHelper.ToString(host) + " pattern:" + ValidationHelper.ToString(pattern));
            if (host==null) {
                if(Logging.On)Logging.PrintWarning(Logging.Web, SR.GetString(SR.net_log_proxy_called_with_null_parameter, "WebProxyScriptHelper.shExpMatch()", "host"));
                throw new ArgumentNullException("host");
            }
            if (pattern==null) {
                if(Logging.On)Logging.PrintWarning(Logging.Web, SR.GetString(SR.net_log_proxy_called_with_null_parameter, "WebProxyScriptHelper.shExpMatch()", "pattern"));
                throw new ArgumentNullException("pattern");
            }

            try
            {
                // This can throw - treat as no match.
                ShellExpression exp = new ShellExpression(pattern);
                return exp.IsMatch(host);
            }
            catch (FormatException)
            {
                return false;
            }
        }

        public bool weekdayRange(string wd1, [Optional] object wd2, [Optional] object gmt)
        {
            GlobalLog.Print("WebProxyScriptHelper::weekdayRange() wd1:" + ValidationHelper.ToString(wd1) + " wd2:" + ValidationHelper.ToString(wd2) + " gmt:" + ValidationHelper.ToString(gmt));
            if (wd1 == null)
            {
                if(Logging.On)Logging.PrintWarning(Logging.Web, SR.GetString(SR.net_log_proxy_called_with_null_parameter, "WebProxyScriptHelper.weekdayRange()", "wd1"));
                throw new ArgumentNullException("wd1");
            }
            string _gmt = null;
            string _wd2 = null;
            if (gmt != null && gmt != DBNull.Value && gmt != Missing.Value)
            {
                _gmt = gmt as string;
                if (_gmt == null)
                {
                    throw new ArgumentException(SR.GetString(SR.net_param_not_string, gmt.GetType().FullName), "gmt");
                }
            }
            if (wd2 != null && wd2 != DBNull.Value && gmt != Missing.Value)
            {
                _wd2 = wd2 as string;
                if (_wd2 == null)
                {
                    throw new ArgumentException(SR.GetString(SR.net_param_not_string, wd2.GetType().FullName), "wd2");
                }
            }
            if (_gmt != null)
            {
                if (!isGMT(_gmt))
                {
                    if(Logging.On)Logging.PrintWarning(Logging.Web, SR.GetString(SR.net_log_proxy_called_with_null_parameter, "WebProxyScriptHelper.weekdayRange()", "gmt"));
                    throw new ArgumentException(SR.GetString(SR.net_proxy_not_gmt), "gmt");
                }
                return weekdayRangeInternal(DateTime.UtcNow, dayOfWeek(wd1), dayOfWeek(_wd2));
            }
            if (_wd2 != null)
            {
                if (isGMT(_wd2))
                {
                    return weekdayRangeInternal(DateTime.UtcNow, dayOfWeek(wd1), dayOfWeek(wd1));
                }
                return weekdayRangeInternal(DateTime.Now, dayOfWeek(wd1), dayOfWeek(_wd2));
            }
            return weekdayRangeInternal(DateTime.Now, dayOfWeek(wd1), dayOfWeek(wd1));
        }

        private static bool isGMT(string gmt)
        {
            return string.Compare(gmt, "GMT", StringComparison.OrdinalIgnoreCase)==0;
        }

        private static DayOfWeek dayOfWeek(string weekDay)
        {
            if (weekDay!=null && weekDay.Length==3) {
                if (weekDay[0]=='T' || weekDay[0]=='t') {
                    if ((weekDay[1]=='U' || weekDay[1]=='u') && (weekDay[2]=='E' || weekDay[2]=='e')) {
                        return DayOfWeek.Tuesday;
                    }
                    if ((weekDay[1]=='H' || weekDay[1]=='h') && (weekDay[2]=='U' || weekDay[2]=='u')) {
                        return DayOfWeek.Thursday;
                    }
                }
                if (weekDay[0]=='S' || weekDay[0]=='s') {
                    if ((weekDay[1]=='U' || weekDay[1]=='u') && (weekDay[2]=='N' || weekDay[2]=='n')) {
                        return DayOfWeek.Sunday;
                    }
                    if ((weekDay[1]=='A' || weekDay[1]=='a') && (weekDay[2]=='T' || weekDay[2]=='t')) {
                        return DayOfWeek.Saturday;
                    }
                }
                if ((weekDay[0]=='M' || weekDay[0]=='m') && (weekDay[1]=='O' || weekDay[1]=='o') && (weekDay[2]=='N' || weekDay[2]=='n')) {
                    return DayOfWeek.Monday;
                }
                if ((weekDay[0]=='W' || weekDay[0]=='w') && (weekDay[1]=='E' || weekDay[1]=='e') && (weekDay[2]=='D' || weekDay[2]=='d')) {
                    return DayOfWeek.Wednesday;
                }
                if ((weekDay[0]=='F' || weekDay[0]=='f') && (weekDay[1]=='R' || weekDay[1]=='r') && (weekDay[2]=='I' || weekDay[2]=='i')) {
                    return DayOfWeek.Friday;
                }
            }
            return (DayOfWeek)unchecked(-1);
        }

        private static bool weekdayRangeInternal(DateTime now, DayOfWeek wd1, DayOfWeek wd2)
        {
            if (wd1<0 || wd2<0) {
                if(Logging.On)Logging.PrintWarning(Logging.Web, SR.GetString(SR.net_log_proxy_called_with_invalid_parameter, "WebProxyScriptHelper.weekdayRange()"));
                throw new ArgumentException(SR.GetString(SR.net_proxy_invalid_dayofweek), wd1 < 0 ? "wd1" : "wd2");
            }
            if (wd1<=wd2) {
                return wd1<=now.DayOfWeek && now.DayOfWeek<=wd2;
            }
            return wd2>=now.DayOfWeek || now.DayOfWeek>=wd1;
        }
    }
}
