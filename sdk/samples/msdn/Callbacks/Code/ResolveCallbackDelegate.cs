using System;
using System.Net;

namespace CallbackSample
{
	/// <summary>
	/// The ResolveCallbackDelegate is used to wrap calls having this signature.
	/// </summary>
	public delegate void ResolveCallbackDelegate(IPHostEntry hostInformation);
}
