namespace Microsoft.Samples
{
    using System;
    using System.IO;
    using System.Xml.Serialization;
    using System.Xml.Schema;
    using System.Xml;
    using System.Collections;
    using System.ComponentModel;
    using System.Web;

    [XmlSchemaProvider ("OrderSchema")]
    [XmlRoot ("order", Namespace = "http://orders/", IsNullable = true)]
    public class Order : IXmlSerializable
    {
        internal int orderNumber;

        internal string orderName;

        internal string shipTo;

        internal string shipAddress1;

        internal string shipAddress2;

        internal static string OrdersNamespace = "http://orders/";

        public static Order Create (int id)
        {
            Order o = new Order ();

            o.Fill (id);
            return o;
        }

        public void Fill (int id)
        {
            orderNumber = id;
            orderName = "Order#" + orderNumber.ToString ();
            shipTo = "Customer Name";
            shipAddress1 = "One Main Street";
            shipAddress2 = "Springfield, VA";
        }
        internal static Order ReadOrder (XmlReader r, Order o)
        {
            if (IsNull (r))
            {
                r.Skip ();
                return o;
            }


            XmlQualifiedName qname;
            string type = r.GetAttribute ("type", XmlSchema.InstanceNamespace);

            if (type == null)
                qname= null;

            qname= ToQname (r, type);

            if (qname != null)
            {
                if (qname.Namespace != OrdersNamespace || qname.Name != "Order")
                    throw new InvalidOperationException ("Unexpected xsi:type='" + qname + "'");
            }
            type = r.GetAttribute ("nil", XmlSchema.InstanceNamespace);

            if (IsNull (r))
            {
                r.Skip ();
                return o;
            }

            if (r.IsEmptyElement)
                return o;

            if (o == null)
                o = new Order ();

            r.ReadStartElement ();
            ReadOrderMembers (r, o);
            r.ReadEndElement ();
            return o;
        }

        internal static void ReadOrderMembers (XmlReader r, Order o)
        {
            r.MoveToContent ();
            o.orderNumber = XmlConvert.ToInt32 (r.ReadElementString ("OrderNumber", OrdersNamespace));
            if (r.LocalName == "OrderName" && r.NamespaceURI == OrdersNamespace)
            {
                if (r.IsEmptyElement)
                {
                    o.orderName = IsNull (r) ? null : "";
                }
                else
                {
                    o.orderName = r.ReadElementString ();
                }
            }

            if (r.LocalName == "ShipTo" && r.NamespaceURI == OrdersNamespace)
            {
                if (r.IsEmptyElement)
                {
                    o.shipTo = IsNull (r) ? null : "";
                }
                else
                {
                    o.shipTo = r.ReadElementString ();
                }
            }

            if (r.LocalName == "ShipAddress1" && r.NamespaceURI == OrdersNamespace)
            {
                if (r.IsEmptyElement)
                {
                    o.shipAddress1 = IsNull (r) ? null : "";
                }
                else
                {
                    o.shipAddress1 = r.ReadElementString ();
                }
            }

            if (r.LocalName == "ShipAddress2" && r.NamespaceURI == OrdersNamespace)
            {
                if (r.IsEmptyElement)
                {
                    o.shipAddress2 = IsNull (r) ? null : "";
                }
                else
                {
                    o.shipAddress2 = r.ReadElementString ();
                }
            }

            r.MoveToContent ();
        }

        internal static bool IsNull (XmlReader r)
        {
            string isNull = r.GetAttribute ("nil", XmlSchema.InstanceNamespace);

            if (isNull == null || !XmlConvert.ToBoolean (isNull))
                return false;

            return true;
        }

        void IXmlSerializable.WriteXml (XmlWriter writer)
        {
            Order.Write (this, true, writer);
        }

        internal static void Write (Order o, bool isNilable, XmlWriter w)
        {
            if (IsNullOrder (o, isNilable, w)) return;
            string qname = GetQualifiedName (o.GetType (), OrdersNamespace, w);

            w.WriteAttributeString ("xsi", "type", XmlSchema.InstanceNamespace, qname);
            WriteOrderMembers (o, isNilable, w);
        }
        internal static void WriteOrderMembers (Order o, bool isNilable, XmlWriter w)
        {
            string prefix = w.LookupPrefix (OrdersNamespace);

            w.WriteElementString ("OrderNumber", OrdersNamespace, XmlConvert.ToString (o.orderNumber));
            if (o.orderName != null)
                w.WriteElementString ("OrderName", OrdersNamespace, o.orderName);

            if (o.shipTo != null)
                w.WriteElementString ("ShipTo", OrdersNamespace, o.shipTo);

            if (o.shipAddress1 != null)
                w.WriteElementString ("ShipAddress1", OrdersNamespace, o.shipAddress1);

            if (o.shipAddress2 != null)
                w.WriteElementString ("ShipAddress2", OrdersNamespace, o.shipAddress2);

        }
        XmlSchema IXmlSerializable.GetSchema () { return null; }

        void IXmlSerializable.ReadXml (XmlReader r)
        {
            ReadOrder (r, this);
        }
        internal static bool IsNullOrder (Order o, bool isNilable, XmlWriter w)
        {
            if (o == null)
            {
                if (isNilable)
                    w.WriteAttributeString ("xsi", "nil", XmlSchema.InstanceNamespace, "true");

                return true;
            }

            return false;
        }

        public static XmlQualifiedName OrderSchema (XmlSchemaSet xs)
        {
            XmlSerializer schemaSerializer = new XmlSerializer (typeof(XmlSchema));
            XmlSchema s=(XmlSchema)schemaSerializer.Deserialize (new XmlTextReader ("Order.xsd"), null);
            xs.XmlResolver = new XmlUrlResolver ();
            xs.Add (s);
            return new XmlQualifiedName ("Order", OrdersNamespace);
        }
        internal static string GetQualifiedName (Type type, string ns, XmlWriter w)
        {
            string name;
            if (type == typeof(Order))
                name= "Order";
            else
                throw new Exception ("Unknown type " + type.FullName);

            if (ns == null || ns.Length == 0) return name;

            string prefix = w.LookupPrefix (ns);

            if (prefix == null)
            {
                if (ns == "http://www.w3.org/XML/1998/namespace")
                {
                    prefix = "xml";
                }
                else
                {
                    prefix = "tns";
                    w.WriteAttributeString ("xmlns", prefix, null, ns);
                }
            }
            else if (prefix.Length == 0)
            {
                return name;
            }

            return prefix + ":" + name;
        }
        internal static XmlQualifiedName ToQname (XmlReader r, string value)
        {
            int colon = value.LastIndexOf (':');

            value = XmlConvert.DecodeName (value);
            if (colon < 0)
            {
                return new XmlQualifiedName (value, r.LookupNamespace (""));
            }
            else
            {
                string ns = r.LookupNamespace (value.Substring (0, colon));

                if (ns == null)
                {
                    // Namespace prefix '{0}' is not defined.
                    throw new InvalidOperationException ("Undefined alias: " + value.Substring (0, colon));
                }

                return new XmlQualifiedName (value.Substring (colon + 1), ns);
            }
        }

    }
}