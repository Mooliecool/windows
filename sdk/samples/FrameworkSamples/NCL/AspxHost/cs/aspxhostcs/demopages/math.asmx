<%@ WebService class="MathService" language="C#" %>

using System.Web.Services;

[WebService(Namespace="http://example.org/math")]
public class MathService
{
    [WebMethod]
    public double Add(double x, double y)
    {
        return x+y;
    }
}