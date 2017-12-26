================================================================================
       WINDOWS FORMS APPLICATION : CSWinFormDesigner Project Overview
       
                   PropertyDisplayOrder Sample
                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The PropertyDisplayOrder sample demonstrates how to change the default display 
order on Properties windows for properties, and how to expand a nested property 
on the Properties window. 
 

/////////////////////////////////////////////////////////////////////////////
Creation:

1. Create a UserControl;

2. Add a reference to the System.Designer.dll;

3. Create a class named SubClass, and add some properties in it;

4. Create a property of SubClass type for the UserControl class;

5. Create a class derive from the ExpandableObjectConverter class ( The 
   ExpandableObjectConverter class provides a type converter to convert 
   expandable objects to and from various other representations.);

6. Override the ExpandableObjectConverter.GetPropertiesSupported() method to 
   return "true", which makes sure the corresponding object supports properties;
 
7. Override the ExpandableObjectConverter.GetProperties() method to specify 
   the display order for the properties;
   
5. Mark the UserControl.Cls property with TypeConverter attribute to make it use 
   the TypeConverter created by the steps above:
   
   public partial class UC_PropertyDisplayOrder : UserControl
    {
        ....

        private SubClass cls = new SubClass();

        [TypeConverter(typeof(PropertyOrderConverter))]
        public SubClass Cls
        {
            get { return this.cls; }
            set { this.cls = value; }
        }
    }

    
6. Build the project.

7. Now the UserControl is available on the toolbox for use.


/////////////////////////////////////////////////////////////////////////////
References:

1. ExpandableObjectConverter Class
http://msdn.microsoft.com/en-us/library/system.componentmodel.expandableobjectconverter.aspx

2. TypeConverter.GetPropertiesSupported Method 
http://msdn.microsoft.com/en-us/library/7afsdzes.aspx

3. ExpandableObjectConverter.GetProperties Method 
http://msdn.microsoft.com/en-us/library/9k32yxw5.aspx

4. Windows Forms FAQs
http://windowsclient.net/blogs/faqs/archive/tags/Custom+Designers/default.aspx


/////////////////////////////////////////////////////////////////////////////



