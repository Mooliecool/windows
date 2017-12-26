using System;
using System.Reflection;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace Reflection
{
	/// <summary>
	/// Summary description for ReflectionForm.
	/// </summary>
	public class ReflectionForm : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Button GetAssemblyInfo;
		private System.Windows.Forms.ComboBox listAssemblies;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.ListBox listTypes;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;


		private Type[] PersonTypes;
		private Type[] EmployeeTypes;
		private Type[] CustomerTypes;

		public ReflectionForm()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.GetAssemblyInfo = new System.Windows.Forms.Button();
			this.listAssemblies = new System.Windows.Forms.ComboBox();
			this.label1 = new System.Windows.Forms.Label();
			this.listTypes = new System.Windows.Forms.ListBox();
			this.SuspendLayout();
			// 
			// GetAssemblyInfo
			// 
			this.GetAssemblyInfo.Location = new System.Drawing.Point(280, 40);
			this.GetAssemblyInfo.Name = "GetAssemblyInfo";
			this.GetAssemblyInfo.TabIndex = 0;
			this.GetAssemblyInfo.Text = "Get Info";
			this.GetAssemblyInfo.Click += new System.EventHandler(this.btnGetInfo_Click);
			// 
			// listAssemblies
			// 
			this.listAssemblies.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.listAssemblies.Items.AddRange(new object[] {
															   "Person",
															   "Employee",
															   "Customer"});
			this.listAssemblies.Location = new System.Drawing.Point(144, 40);
			this.listAssemblies.Name = "listAssemblies";
			this.listAssemblies.Size = new System.Drawing.Size(121, 21);
			this.listAssemblies.TabIndex = 1;
			this.listAssemblies.SelectedIndexChanged += new System.EventHandler(this.cboAssemblies_SelectedIndexChanged);
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(16, 40);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(112, 16);
			this.label1.TabIndex = 2;
			this.label1.Text = "Select An Assembly: ";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// listTypes
			// 
			this.listTypes.Location = new System.Drawing.Point(8, 88);
			this.listTypes.Name = "listTypes";
			this.listTypes.Size = new System.Drawing.Size(376, 173);
			this.listTypes.TabIndex = 3;
			// 
			// ReflectionForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(400, 273);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.listTypes,
																		  this.label1,
																		  this.listAssemblies,
																		  this.GetAssemblyInfo});
			this.Name = "ReflectionForm";
			this.Text = "Reflection Sample";
			this.Load += new System.EventHandler(this.ReflectionForm_Load);
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new ReflectionForm());
		}

		private void btnGetInfo_Click(object sender, System.EventArgs e)
		{
			//get the name of the assembly selected by the user.
			string SelectedAssembly = this.listAssemblies.SelectedItem.ToString();
			
			//call the LoadTypeInfo with the 
			//correct assembly array.
			if(SelectedAssembly == "Person")
			{
				listTypes.Items.Clear();
				LoadTypeInfo(PersonTypes);
			}
			else if (SelectedAssembly == "Employee")
			{
				listTypes.Items.Clear();
				LoadTypeInfo(EmployeeTypes);
			}
			else if (SelectedAssembly == "Customer")
			{
				listTypes.Items.Clear();
				LoadTypeInfo(CustomerTypes);
			}
			else
			{
				listTypes.Items.Clear();
			}

		}


		private void LoadTypeInfo(Type[] AssemblyType)
		{
			//loop through the Assembly Type and write the 
			//attributes out to the list box. 
			foreach(Type baseType in AssemblyType)
			{
				//get the full name of the assembly.
				listTypes.Items.Add("Assembly Type: "+baseType.FullName+"("+baseType.UnderlyingSystemType+")");
				//get the base type of the assembly.
				listTypes.Items.Add("Base Type: "+baseType.BaseType);
				//is the assembly serializable.
				listTypes.Items.Add("Serializable: " + baseType.IsSerializable);
				//is the assembly defined as abstract.
				listTypes.Items.Add("Abstract: " + baseType.IsAbstract);
				//is the assembly a class library.
				listTypes.Items.Add("Class: " + baseType.IsClass);
				//is the assembly public.
				listTypes.Items.Add("Public: " + baseType.IsPublic);
				//is the assembly sealed.
				listTypes.Items.Add("Sealed: " + baseType.IsSealed);

				//get the attributes defined - returns only .NET base attributes.
				//for customattributes use the "GetCustomAttributes()" method.
				System.Reflection.TypeAttributes baseTypeAttributes = baseType.Attributes;

				listTypes.Items.Add("");
				listTypes.Items.Add("Attributes:");
				listTypes.Items.Add(baseTypeAttributes.ToString());

				//get all of the properties defined on the assembly.
				PropertyInfo[] propInfo = baseType.GetProperties();
				//get all of the methods defined on the assembly.
				MethodInfo[] methodInfo = baseType.GetMethods();
				
				listTypes.Items.Add("");				
				listTypes.Items.Add("Properties:");
				
				//loop through the property info array.
				foreach(PropertyInfo prop in propInfo)
				{
					//get the property name.
					listTypes.Items.Add("Name: " +prop.Name);
					//get the member type "Property".
					listTypes.Items.Add("Member Type: " +prop.MemberType);
					//get the property type.
					listTypes.Items.Add("Property Type: " +prop.PropertyType);
					//does the property have a get method defined.
					listTypes.Items.Add("Read: " +prop.CanRead);
					//does the property have a set method defined.
					listTypes.Items.Add("Write: " +prop.CanWrite);
					listTypes.Items.Add("");
				}

				listTypes.Items.Add("");
				listTypes.Items.Add("Methods:");

				//loop through the method info array.
				foreach(MethodInfo method in methodInfo)
				{
					//get the method name.
					listTypes.Items.Add("Name: " +method.Name);
					//get the membertype.
					listTypes.Items.Add("Member Type: " +method.MemberType);
					//get the return type of the method.
					listTypes.Items.Add("Return Type: " +method.ReturnType);
					//is the method defined as public.
					listTypes.Items.Add("Public: " +method.IsPublic);
					//is the method defined as private.
					listTypes.Items.Add("Private: " +method.IsPrivate);
					//is the method defined as abstract.
					listTypes.Items.Add("Abstract: " +method.IsAbstract);
					//is the method defined as virtual.
					listTypes.Items.Add("Virtual: " +method.IsVirtual);
					//is the method defined as static.
					listTypes.Items.Add("Static: " +method.IsStatic);
					listTypes.Items.Add("");
				}
			
			}

		}
		
		
		private void ReflectionForm_Load(object sender, System.EventArgs e)
		{
			//return the Assembly types
			Assembly PersonAssembly = Assembly.LoadFrom("..\\..\\Person.dll");
			PersonTypes = PersonAssembly.GetTypes();

			Assembly EmployeeAssembly = Assembly.LoadFrom("..\\..\\Employee.dll");
			EmployeeTypes = EmployeeAssembly.GetTypes();

			Assembly CustomerAssembly = Assembly.LoadFrom("..\\..\\Customer.dll");
			CustomerTypes = CustomerAssembly.GetTypes();

			listAssemblies.SelectedIndex=0;
			
		}

		private void cboAssemblies_SelectedIndexChanged(object sender, System.EventArgs e)
		{
		
		}
	}
}
