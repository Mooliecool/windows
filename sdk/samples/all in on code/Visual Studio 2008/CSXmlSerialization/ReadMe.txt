========================================================================
    CONSOLE APPLICATION : CSXmlSerialization Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This sample shows how to serialize an in-memory object to a local xml file 
and how to deserialize the xml file back to an in-memory object using C#. 
The designed MySerializableType includes int, string, generic, as well as 
customized type field and property.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. We define a MySerializableType whose instance will be serialized to xml 
file. The MySerializableType includes int, string, bool, generic List and 
a customized type field/property. Mark the type as [Serializable()].

2. We define a AnotherType which is used for MySerializableType's inner 
customized type. Mark the type as [Serializable()].

3. In the main method, the codes firstly create and initialize an object of
MySerializableType.

4. Then it creates a XmlSerializer and StreamWriter to serialize the instance
to local driver as XML file. The generated xml file in step4 looks like,

  <?xml version="1.0" encoding="utf-8" ?> 
- <MySerializableType xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
     xmlns:xsd="http://www.w3.org/2001/XMLSchema">
  <StringValue>Test String</StringValue> 
  <BoolValue>true</BoolValue> 
  <IntValue>1</IntValue> 
- <AnotherTypeValue>
  <StringValue>Inner Test String</StringValue> 
  <IntValue>2</IntValue> 
  </AnotherTypeValue>
- <ListValue>
  <string>List Item 1</string> 
  <string>List Item 2</string> 
  <string>List Item 3</string> 
  </ListValue>
  </MySerializableType>

5. Then it creates a StreamReader to read and deserialize the xml file back to
object instance


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: XMLSerializer
http://msdn.microsoft.com/en-us/library/system.xml.serialization.xmlserializer.aspx


/////////////////////////////////////////////////////////////////////////////
