using System;
using System.Net;

namespace CallbackSample
{
	/// <summary>
	/// The IResolveCallback interface defines a callback method to use when the called
	/// object completes its work.
	/// </summary>
  public interface IResolveCallback
  {
    /// <summary>
    /// This method will be called when the callback option is UseInterface
    /// and the host has been resolved.
    /// </summary>
    /// <param name="hostInformation">The host information returned to the caller.</param>
    void HostResolved(IPHostEntry hostInformation);
  }
}
