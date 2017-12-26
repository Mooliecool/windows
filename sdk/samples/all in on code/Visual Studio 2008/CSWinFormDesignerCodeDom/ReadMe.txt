================================================================================
       WINDOWS FORMS APPLICATION : CSWinFormDesignerCodeDom Project Overview
                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This sample demonstrates how to add your own customized generate code for 
control.

In this example, if we don't customize the code, 
system will generate following code for MyList property
this.myComponent1.MyList = 
((System.Collections.Generic.List<string>)(resources.GetObject("myComponent1.MyList")));

But now we want the code with following format
this.myComponent1.MyList.Add("string5");
this.myComponent1.MyList.Add("string4");
this.myComponent1.MyList.Add("string3");
this.myComponent1.MyList.Add("string2");
this.myComponent1.MyList.Add("string1");


/////////////////////////////////////////////////////////////////////////////
Creation:

1. Create a class named "MyCodeGenerator" inherits from CodeDomSerializer.

2. Override the Deserialize and Serialize method

3. Get base class's CodeDomSerializer from the IDesignerSerializationManager

4. Create CodeAssignStatement to generate
   myComponent1.MyList = new System.Collections.Generic.List<string>();

5. Create CodeCommentStatement to add comment to our generated code

6. Create CodeMethodInvokeExpression to generate
   this.myComponent1.MyList.Add("string1");

7. Use MyCodeGenerator for MyComponent class
   [DesignerSerializer(typeof(MyCodeGenerator), typeof(CodeDomSerializer))]

/////////////////////////////////////////////////////////////////////////////
References:

1. CodeDomSerializer Class
http://msdn.microsoft.com/en-us/library/system.componentmodel.design.serialization.codedomserializer.aspx

2. Windows Forms FAQs
http://windowsclient.net/blogs/faqs/archive/tags/Custom+Designers/default.aspx


/////////////////////////////////////////////////////////////////////////////



