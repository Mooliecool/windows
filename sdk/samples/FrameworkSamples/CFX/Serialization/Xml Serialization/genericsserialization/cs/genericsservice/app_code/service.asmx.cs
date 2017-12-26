using System.Web.Services;
using System.Web.Services.Protocols;
using System.Collections.Generic;
using System.Xml.Serialization;

[WebServiceBinding(ConformsTo=WsiProfiles.BasicProfile1_1,EmitConformanceClaims = true)]
public class Service_asmx {

    [WebMethod]
    public MyList<string> HelloWorld() {
		MyList<string> results = new MyList<string>();
		results.Add("Hello World");
		return results;
	}

	[XmlType("MyGenericListOf{T}")]
	public class MyList<T>: List<T>
	{
	}

}
