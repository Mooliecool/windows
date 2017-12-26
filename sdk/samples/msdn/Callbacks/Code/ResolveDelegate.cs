using System;
using System.Net;

namespace CallbackSample
{
	/// <summary>
	/// The ResolveDelegate is used to wrap calls to methods having this signature.
	/// </summary>
  internal delegate IPHostEntry ResolveDelegate(string host);
}
