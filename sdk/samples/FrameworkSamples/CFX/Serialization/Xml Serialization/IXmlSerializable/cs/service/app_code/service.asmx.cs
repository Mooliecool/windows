using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Web;
using System.Web.Services;
/// <summary>
/// Summary description for Service_asmx.
/// </summary>
/// 

namespace Microsoft.Samples
{
	[WebService(Namespace = "http://demos")]
	public class Service_asmx
	{
		[WebMethod]
		public Order GetOrder()
		{
			Order order = Order.Create(5);
			return order;
		}
	}
}
