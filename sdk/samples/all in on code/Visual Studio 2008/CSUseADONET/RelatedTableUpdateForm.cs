/****************************** Module Header ******************************\
* Module Name:	RelatedTableUpdateForm.cs
* Project:		CSUseADONET
* Copyright (c) Microsoft Corporation.
* 
* The RelatedTableUpdateForm example demonstrates the Microsoft ADO.NET 
* technology to update related data tables in the databases using Visual C#. 
* It shows hot to use TableAdapterManager to update two related data tables
* and how to write codes manually to update two related data tables.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 3/27/2009 09:00 AM Lingzhi Sun Created
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using CSUseADONET;
#endregion


public partial class RelatedTableUpdateForm : Form
{
    /// <summary>
    /// RelatedTableUpdateForm constructor
    /// </summary>
    public RelatedTableUpdateForm()
    {
        InitializeComponent();
    }

    /// <summary>
    /// Fill the data tables in Form's load event 
    /// </summary>
    private void RelatedTableUpdateForm_Load(object sender, EventArgs e)
    {
        // TODO: This line of code loads data into the 
        // 'sQLServer2005DBDataSet.Course' table. You can move, or remove it,
        // as needed.
        this.courseTableAdapter.Fill(this.sQLServer2005DBDataSet.Course);
        // TODO: This line of code loads data into the 
        // 'sQLServer2005DBDataSet.Department' table. You can move, or remove
        // it, as needed.
        this.departmentTableAdapter.Fill(this.sQLServer2005DBDataSet.
            Department);

    }


    /// <summary>
    /// Update the related data tables by TableAdapterManager
    /// </summary>
    private void departmentBindingNavigatorSaveItem_Click(object sender, 
        EventArgs e)
    {
        this.Validate();
        this.departmentBindingSource.EndEdit();

        // Add this line of code in the codes automatically created
        this.courseBindingSource.EndEdit();

        // Use TableAdapterManager to update the related data tables
        this.tableAdapterManager.UpdateAll(this.sQLServer2005DBDataSet);
    }


    /// <summary>
    /// Update the related data tables by writing codes manually
    /// </summary>
    private void departmentBindingNavigatorSaveItem2_Click(object sender, 
        EventArgs e)
    {
        this.Validate();
        this.departmentBindingSource.EndEdit();

        // Add this line of code in the codes automatically created
        this.courseBindingSource.EndEdit();

        // Writing codes manually to update the related data tables

        // Get all the deleted data rows in the Course table
        SQLServer2005DBDataSet.CourseDataTable deletedCourses = 
            (SQLServer2005DBDataSet.CourseDataTable)sQLServer2005DBDataSet.
            Course.GetChanges(DataRowState.Deleted);

        // Get all the new data rows in the Course table
        SQLServer2005DBDataSet.CourseDataTable newCourses = 
            (SQLServer2005DBDataSet.CourseDataTable)sQLServer2005DBDataSet.
            Course.GetChanges(DataRowState.Added);

        // Get all the modified data rows in the Course table
        SQLServer2005DBDataSet.CourseDataTable modifiedCourses = 
            (SQLServer2005DBDataSet.CourseDataTable)sQLServer2005DBDataSet.
            Course.GetChanges(DataRowState.Modified);

        try
        {
            // Remove all deleted coures from the Course table.
            if (deletedCourses != null)
            {
                courseTableAdapter.Update(deletedCourses);
            }

            // Update the Department table.
            departmentTableAdapter.Update(sQLServer2005DBDataSet.Department);

            // Add new courses to the Course table.
            if (newCourses != null)
            {
                courseTableAdapter.Update(newCourses);
            }

            // Update all modified courses.
            if (modifiedCourses != null)
            {
                courseTableAdapter.Update(modifiedCourses);
            }

            // Accept all the changes of the Strong Typed DataSet
            sQLServer2005DBDataSet.AcceptChanges();

        }
        catch (System.Exception ex)
        {
            MessageBox.Show("Update failed for " + ex.Message);
        }
        finally
        {
            // Cleanup the data rows objects
            if (deletedCourses != null)
            {
                deletedCourses.Dispose();
            }
            if (newCourses != null)
            {
                newCourses.Dispose();
            }
            if (modifiedCourses != null)
            {
                modifiedCourses.Dispose();
            }
        }
    }


    /// <summary>
    /// Commit parent records in the DataSet before adding
    /// new child records. 
    /// (This method should be added when using 
    ///  TableAdapterManager to update the related data 
    ///  tables)
    /// </summary>
    private void courseBindingSource_AddingNew(object sender, 
        AddingNewEventArgs e)
    {
        this.departmentBindingSource.EndEdit();
    }
}
   
