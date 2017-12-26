//---------------------------------------------------------------------
//  This file is part of the Windows Workflow Foundation SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------


using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Data.Sql;
using System.Data.Common;
using System.Workflow.Runtime.Tracking;
using System.Data.SqlClient;
using System.Configuration;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;

namespace Microsoft.Samples.Workflow.TrackingProfileDesigner.TrackingStore
{
    public class TrackingProfileStore
    {
        

        public TrackingProfileStore()
        {
            
        }

        public string ConnectionString
        {
            get
            {
                //Try to read the connection string from the config file
                AppSettingsReader reader = new AppSettingsReader();
                try
                {
                    return (string)reader.GetValue("ConnectionString", typeof(string));
                }
                catch
                {
                }
                return null;
            }
        }


        /// <summary>
        /// Validate that a connection can be established
        /// </summary>
        /// <param name="error"></param>
        /// <returns></returns>
        public bool ValidateConnection(out string error)
        {
            error = string.Empty;

            using (SqlConnection connection = new SqlConnection(ConnectionString))
            {
                try
                {
                    connection.Open();
                }
                catch (Exception ex)
                {
                    error = ex.Message;
                    return false;
                }
            }
            return true;
        }

        /// <summary>
        /// Loads a workflow and profile from the SQL Tracking Database, using the LoadFromStore winform.
        /// </summary>
        /// <param name="workflowType"></param>
        /// <param name="profile"></param>
        public void LoadWorkflowAndProfile(out Type workflowType, out TrackingProfile profile)
        {
            LoadFromStore loadFromStoreForm = new LoadFromStore();
            loadFromStoreForm.WorkflowProfiles = GetWorkflowAndProfiles();
            DialogResult result = loadFromStoreForm.ShowDialog();
            if (result == DialogResult.OK)
            {
                workflowType = loadFromStoreForm.SelectedWorkflow;
                if (workflowType != null)
                {
                    profile = GetWorkflowProfile(workflowType, loadFromStoreForm.SelectedProfileVersion, true);
                }
                else
                {
                    profile = null;
                }
            }
            else
            {
                workflowType = null;
                profile = null;
                return;
            }
        }

        /// <summary>
        /// Retrieves a table containing all the workflow types and associated profile versions, from the SQL Tracking Database
        /// </summary>
        /// <returns></returns>
        private DataTable GetWorkflowAndProfiles()
        {
            string query = @"select TypeId,TypeFullName,AssemblyFullName,Version from Type, TrackingProfile
                                where Type.TypeId = TrackingProfile.WorkflowTypeId";

            using (SqlConnection connection = new SqlConnection(ConnectionString))
            {
                SqlDataAdapter adapter = new SqlDataAdapter(query, connection);
                DataTable table = new DataTable();
                adapter.Fill(table);
                return table;
            }
        }

        /// <summary>
        /// Saves a profile for the specified workflow type
        /// </summary>
        /// <param name="workflowType"></param>
        /// <param name="profile"></param>
        public void SaveProfile(Type workflowType, TrackingProfile profile)
        {
            TrackingProfile currentProfile = GetWorkflowProfile(workflowType, false);

            if (currentProfile != null && currentProfile.Version >= profile.Version)
            {
                //If there already exists a profile (with a later version)
                //for this workflow, prompt the user to rev the version.    
                UpdateProfileVersion updateVersionForm = new UpdateProfileVersion();
                updateVersionForm.CurrentTrackingProfile = currentProfile;
                updateVersionForm.NewTrackingProfile = profile;
                updateVersionForm.WorkflowType = workflowType;
                DialogResult result = updateVersionForm.ShowDialog();
                if (result != DialogResult.OK) return;
            }
            try
            {
                using (SqlConnection connection = new SqlConnection(ConnectionString))
                {
                    using (SqlCommand command = new SqlCommand())
                    {

                        command.CommandText = "[dbo].[UpdateTrackingProfile]";
                        command.CommandType = CommandType.StoredProcedure;
                        command.Connection = connection;

                        SqlParameter typeFullName = new SqlParameter("@TypeFullName", workflowType.FullName);
                        SqlParameter assemblyFullName = new SqlParameter("@AssemblyFullName", workflowType.Assembly.FullName);

                        command.Parameters.Add(typeFullName);
                        command.Parameters.Add(assemblyFullName);

                        SqlParameter version = new SqlParameter("@Version", profile.Version.ToString());
                        SqlParameter serializedProfile = new SqlParameter("@TrackingProfileXml", new TrackingProfileManager(profile).SerializeProfile());

                        command.Parameters.Add(version);
                        command.Parameters.Add(serializedProfile);

                        connection.Open();
                        command.ExecuteNonQuery();
                        MessageBox.Show("Save successful!", "Success");
                    }
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show(string.Format("Error saving profile: {0}", ex.Message), "Error");
            }
        }

        /// <summary>
        /// Retrieves a profile from the SQL Tracking Database
        /// </summary>
        /// <param name="workflowType">The workflow type to retrieve the profile for</param>
        /// <param name="createDefault">When true, creates a default tracking profile if no profile currently exists</param>
        /// <returns></returns>
        private TrackingProfile GetWorkflowProfile(Type workflowType, bool createDefault)
        {
            return GetWorkflowProfile(workflowType, null, createDefault);
        }

        /// <summary>
        /// Retrieves a profile from the SQL Tracking Database
        /// </summary>        
        /// <param name="workflowType">The workflow type to retrieve the profile for</param>
        /// /// <param name="version">The version of the workflow to retrieve.  If null, the version is ignored.</param>
        /// <param name="createDefault">When true, creates a default tracking profile if no profile currently exists</param>
        /// <returns></returns>     
        private TrackingProfile GetWorkflowProfile(Type workflowType, Version version, bool createDefault)
        {
            SqlDataReader reader = null;
            TrackingProfile profile = null;            
            try
            {
                using (SqlConnection connection = new SqlConnection(ConnectionString))
                {
                    using (SqlCommand command = new SqlCommand())
                    {
                        command.CommandType = CommandType.StoredProcedure;
                        command.CommandText = "[dbo].[GetTrackingProfile]";
                        command.Connection = connection;

                        SqlParameter typeFullName = new SqlParameter("@TypeFullName", workflowType.FullName);
                        SqlParameter assemblyFullName = new SqlParameter("@AssemblyFullName", workflowType.Assembly.FullName);
                        SqlParameter createDefaultParameter = new SqlParameter("@CreateDefault", createDefault ? 1 : 0);

                        command.Parameters.Add(typeFullName);
                        command.Parameters.Add(assemblyFullName);
                        command.Parameters.Add(createDefaultParameter);

                        //Filter by version as well, if specified
                        if (version != null)
                        {
                            SqlParameter versionParameter = new SqlParameter("@Version", version.ToString());
                            command.Parameters.Add(versionParameter);
                        }

                        connection.Open();
                        reader = command.ExecuteReader(CommandBehavior.CloseConnection);
                        if (reader.Read())
                        {
                            if (reader.IsDBNull(0))
                            {
                                return null;
                            }

                            string tmp = reader.GetString(0);
                            TrackingProfileSerializer serializer = new TrackingProfileSerializer();
                            StringReader pReader = null;

                            try
                            {
                                pReader = new StringReader(tmp);
                                profile = serializer.Deserialize(pReader);
                            }
                            finally
                            {
                                if (null != pReader)
                                    pReader.Close();
                            }
                        }
                    }
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show(string.Format("Error retrieving profile: {0}", ex.Message), "Error");
            }         
            return profile;
        }
    }
}
