Imports System
Imports System.IO
Imports System.Xml.Serialization
Imports System.Xml.Schema
Imports System.Xml
Imports System.Collections
Imports System.ComponentModel
Imports System.Web

Namespace Microsoft.Samples
    <XmlSchemaProvider("OrderSchema"), XmlRoot("order", [Namespace]:="http://orders/", IsNullable:=True)> _
   Public Class Order
        Implements IXmlSerializable

        Friend orderNumber As Integer

        Friend orderName As String

        Friend shipTo As String

        Friend shipAddress1 As String

        Friend shipAddress2 As String

        Friend Shared OrdersNamespace As String = "http://orders/"


        Public Shared Function Create(ByVal id As Integer) As Order
            Dim o As New Order()

            o.Fill(id)
            Return o
        End Function 'Create


        Public Sub Fill(ByVal id As Integer)
            orderNumber = id
            orderName = "Order#" + orderNumber.ToString()
            shipTo = "Customer Name"
            shipAddress1 = "One Main Street"
            shipAddress2 = "Springfield, VA"
        End Sub 'Fill

        Friend Shared Function ReadOrder(ByVal r As XmlReader, ByVal o As Order) As Order
            If IsNull(r) Then
                r.Skip()
                Return o
            End If


            Dim qname As XmlQualifiedName
            Dim type As String = r.GetAttribute("type", XmlSchema.InstanceNamespace)

            If type Is Nothing Then
                qname = Nothing
            End If
            qname = ToQname(r, type)

            If Not (qname Is Nothing) Then
                If qname.Namespace <> OrdersNamespace Or qname.Name <> "Order" Then
                    Throw New InvalidOperationException("Unexpected xsi:type='" & qname.Name & "'")
                End If
            End If
            type = r.GetAttribute("nil", XmlSchema.InstanceNamespace)

            If IsNull(r) Then
                r.Skip()
                Return o
            End If

            If r.IsEmptyElement Then
                Return o
            End If
            If o Is Nothing Then
                o = New Order()
            End If
            r.ReadStartElement()
            ReadOrderMembers(r, o)
            r.ReadEndElement()
            Return o
        End Function 'ReadOrder


        Friend Shared Sub ReadOrderMembers(ByVal r As XmlReader, ByVal o As Order)
            r.MoveToContent()
            o.orderNumber = XmlConvert.ToInt32(r.ReadElementString("OrderNumber", OrdersNamespace))
            If r.LocalName = "OrderName" And r.NamespaceURI = OrdersNamespace Then
                If r.IsEmptyElement Then
                    If IsNull(r) Then
                        o.orderName = Nothing
                    Else
                        o.orderName = ""
                    End If
                Else
                    o.orderName = r.ReadElementString()
                End If
            End If


            If r.LocalName = "ShipTo" And r.NamespaceURI = OrdersNamespace Then
                If r.IsEmptyElement Then
                    If IsNull(r) Then
                        o.shipTo = Nothing
                    Else
                        o.shipTo = ""
                    End If
                Else
                    o.shipTo = r.ReadElementString()
                End If
            End If

            If r.LocalName = "ShipAddress1" And r.NamespaceURI = OrdersNamespace Then
                If r.IsEmptyElement Then
                    If IsNull(r) Then
                        o.shipAddress1 = Nothing
                    Else
                        o.shipAddress1 = ""
                    End If
                Else
                    o.shipAddress1 = r.ReadElementString()
                End If
            End If

            If r.LocalName = "ShipAddress2" And r.NamespaceURI = OrdersNamespace Then
                If r.IsEmptyElement Then
                    If IsNull(r) Then
                        o.shipAddress2 = Nothing
                    Else
                        o.shipAddress2 = ""
                    End If
                Else
                    o.shipAddress2 = r.ReadElementString()
                End If
            End If

            r.MoveToContent()
        End Sub 'ReadOrderMembers


        Friend Shared Function IsNull(ByVal r As XmlReader) As Boolean
            Dim null As String = r.GetAttribute("nil", XmlSchema.InstanceNamespace)

            If null Is Nothing Or Not XmlConvert.ToBoolean(IsNull) Then
                Return False
            End If
            Return True
        End Function 'IsNull


        Sub WriteXml(ByVal writer As XmlWriter) Implements IXmlSerializable.WriteXml
            Order.Write(Me, True, writer)
        End Sub 'IXmlSerializable.WriteXml


        Friend Shared Sub Write(ByVal o As Order, ByVal isNilable As Boolean, ByVal w As XmlWriter)
            If IsNullOrder(o, isNilable, w) Then
                Return
            End If
            Dim qname As String = GetQualifiedName(o.GetType(), OrdersNamespace, w)

            w.WriteAttributeString("xsi", "type", XmlSchema.InstanceNamespace, qname)
            WriteOrderMembers(o, isNilable, w)
        End Sub 'Write

        Friend Shared Sub WriteOrderMembers(ByVal o As Order, ByVal isNilable As Boolean, ByVal w As XmlWriter)
            Dim prefix As String = w.LookupPrefix(OrdersNamespace)

            w.WriteElementString("OrderNumber", OrdersNamespace, XmlConvert.ToString(o.orderNumber))
            If Not (o.orderName Is Nothing) Then
                w.WriteElementString("OrderName", OrdersNamespace, o.orderName)
            End If
            If Not (o.shipTo Is Nothing) Then
                w.WriteElementString("ShipTo", OrdersNamespace, o.shipTo)
            End If
            If Not (o.shipAddress1 Is Nothing) Then
                w.WriteElementString("ShipAddress1", OrdersNamespace, o.shipAddress1)
            End If
            If Not (o.shipAddress2 Is Nothing) Then
                w.WriteElementString("ShipAddress2", OrdersNamespace, o.shipAddress2)
            End If
        End Sub 'WriteOrderMembers

        Function GetSchema() As XmlSchema Implements IXmlSerializable.GetSchema
            Return Nothing
        End Function 'IXmlSerializable.GetSchema

        Sub ReadXml(ByVal r As XmlReader) Implements IXmlSerializable.ReadXml
            ReadOrder(r, Me)
        End Sub 'IXmlSerializable.ReadXml

        Friend Shared Function IsNullOrder(ByVal o As Order, ByVal isNilable As Boolean, ByVal w As XmlWriter) As Boolean
            If o Is Nothing Then
                If isNilable Then
                    w.WriteAttributeString("xsi", "nil", XmlSchema.InstanceNamespace, "true")
                End If
                Return True
            End If

            Return False
        End Function 'IsNullOrder


        Public Shared Function OrderSchema(ByVal xs As XmlSchemaSet) As XmlQualifiedName
            Dim schemaSerializer As New XmlSerializer(GetType(XmlSchema))
            Dim s As XmlSchema = CType(schemaSerializer.Deserialize(New XmlTextReader(HttpContext.Current.Server.MapPath("Order.xsd"))), XmlSchema)
            xs.XmlResolver = New XmlUrlResolver()
            xs.Add(s)
            Return New XmlQualifiedName("Order", OrdersNamespace)
        End Function 'OrderSchema

        Friend Shared Function GetQualifiedName(ByVal type As Type, ByVal ns As String, ByVal w As XmlWriter) As String
            Dim name As String
            If type Is GetType(Order) Then
                name = "Order"
            Else
                Throw New Exception("Unknown type " + type.FullName)
            End If
            If ns Is Nothing Or ns.Length = 0 Then
                Return name
            End If
            Dim prefix As String = w.LookupPrefix(ns)

            If prefix Is Nothing Then
                If ns = "http://www.w3.org/XML/1998/namespace" Then
                    prefix = "xml"
                Else
                    prefix = "tns"
                    w.WriteAttributeString("xmlns", prefix, Nothing, ns)
                End If
            Else
                If prefix.Length = 0 Then
                    Return name
                End If
            End If
            Return prefix + ":" + name
        End Function 'GetQualifiedName

        Friend Shared Function ToQname(ByVal r As XmlReader, ByVal value As String) As XmlQualifiedName
            Dim colon As Integer = value.LastIndexOf(":"c)

            value = XmlConvert.DecodeName(value)
            If colon < 0 Then
                Return New XmlQualifiedName(value, r.LookupNamespace(""))
            Else
                Dim ns As String = r.LookupNamespace(value.Substring(0, colon))

                If ns Is Nothing Then
                    ' Namespace prefix '{0}' is not defined.
                    Throw New InvalidOperationException("Undefined alias: " + value.Substring(0, colon))
                End If

                Return New XmlQualifiedName(value.Substring((colon + 1)), ns)
            End If
        End Function 'ToQname


    End Class 'Order 
End Namespace 'Microsoft.Samples