================================================================================
       WINDOWS FORMS APPLICATION : CSWinFormDesigner Project Overview
       
                   CustomEditor Sample
                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

 The CustomEditor sample demonstrates how to use a custom editor for a specific 
 property at design time.  
 

/////////////////////////////////////////////////////////////////////////////
Creation:

1. Create a UserControl named UC_CustomEditor;

2. Add a reference to the System.Designer.dll;

3. Create a class named SubClass, and add some properties in it, e.g.

   public class SubClass
    {
        private string name;
        private DateTime date = DateTime.Now;

        public string Name
        {
            get { return this.name; }
            set { this.name = value; }
        }
        public DateTime Date
        {
            get { return this.date; }
            set { this.date = value; }
        }
    }

4. Create a property of SubClass type for the UC_CustomEditor class;

5. Create a new Form named "EditorForm";

6. Drag a TextBox and a DateTimePicker onto the EditorForm, the TextBox is used
   to edit the value for the SubClass.Name property, the DateTimePicker is used
   to edit the value for the SubClass.Date property.
   
7. Create a property of SubClass type;

8. Handle the EditorForm.Load event to specify initial values for the TextBox and
   DateTimePicker;

9. Drag two buttons onto the EditorForm, one for confirmation of editing, the other
   for cancelling.

10. Create a class derive from the UITypeEditor class;

11. Override the UITypeEditor.GetEditStyle() method to return 
   "UITypeEditorEditStyle.Modal", which specifis designer to use a modal 
   dialog when editing the specified object.
 
12. Override the UITypeEditor.EditValue() method to edits the value of the 
    specified object using the editor style indicated by the GetEditStyle method. 
   
13. Mark the UC_CustomEditor.Cls property with Editor attribute to make it use 
    the Editor created by the steps above:
   
    public partial class UC_CustomEditor : UserControl
    {
        ....

        private SubClass cls = new SubClass();

        [TypeConverter(typeof(ExpandableObjectConverter))]
        [Editor(typeof(MyEditor),typeof(UITypeEditor))]
        public SubClass Cls
        {
            get { return this.cls; }
            set { this.cls = value; }
        }
    }

    
14. Build the project.

15. Now the UserControl is available on the toolbox for use.



/////////////////////////////////////////////////////////////////////////////
References:

1. UITypeEditor Class
http://msdn.microsoft.com/en-us/library/system.drawing.design.uitypeeditor.aspx

2. ExpandableObjectConverter Class
http://msdn.microsoft.com/en-us/library/system.componentmodel.expandableobjectconverter.aspx

3. Windows Forms FAQs
http://windowsclient.net/blogs/faqs/archive/tags/Custom+Designers/default.aspx


/////////////////////////////////////////////////////////////////////////////



