using System;
using System.Net;

namespace CallbackSample
{
	/// <summary>
	/// The Resolver class supplies some synchronous methods to do DNS resolution
	/// with (this wraps the Dns.Resolve method defined by the framework, but any
	/// long-running operation will suffice).  Some of these methods accept a
	/// callback interface or delegate as a parameter to inform the caller when
	/// the operation is complete.
	/// </summary>
	internal sealed class Resolver
	{
    /// <summary>
    /// Resolve an IP address or host name, and return the result synchronously
    /// through the interface provided.
    /// </summary>
    /// <param name="host">The IP address or host name to resolve</param>
    /// <param name="callbackInterface">The interface defining the callback method</param>
    internal void Resolve(string host, IResolveCallback callbackInterface)
    {
      // This performs a long-running operation (DNS resolution) and
      // returns the result through the interface accepted as a parameter.
      callbackInterface.HostResolved(Dns.Resolve(host));
    }

    /// <summary>
    /// Resolve an IP address or host name, and return the result synchronously
    /// through the delegate passed in by the caller.
    /// </summary>
    /// <param name="host">The IP address or host name to resolve</param>
    /// <param name="callbackDelegate">The delegate wrapping the callback method</param>
    internal void Resolve(string host, ResolveCallbackDelegate callbackDelegate)
    {
      // This performs the same task as above, but instead of calling back
      // through an interface, a delegate is used to inform the caller of the
      // results.
      callbackDelegate(Dns.Resolve(host));
    }

    /// <summary>
    /// Resolve an IP address or host name, and return the result synchronously.
    /// </summary>
    /// <param name="host">The IP address or host name to resolve</param>
    /// <returns>An IPHostEntry object containing information about the host</returns>
    internal IPHostEntry Resolve(string host)
    {
      // This simply forwards the request to a known long-running method
      // (DNS resolution).
      return Dns.Resolve(host);
    }
	}
}
