/****************************** Module Header ******************************\
* Module Name:  CreateFactDemo.cs
* Project:      CSTFSDataWarehouseAdapter
* Copyright (c) Microsoft Corporation.
* 
* Demostrates how to create a TFS warehouse adapter.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 9/18/2009 12:04 AM Bill Wang Created
\***************************************************************************/

using System;
using Microsoft.TeamFoundation.Adapter;
using Microsoft.TeamFoundation.Client;
using Microsoft.TeamFoundation.Server;
using Microsoft.TeamFoundation.Warehouse;

namespace CSTFSDataWarehouseAdapter
{
    /// <summary>
    /// A TFS warehouse adapter sample.It demostrate how to create facts and
    /// add fact entries. 
    /// </summary>
    public class CreateFactDemo : IWarehouseAdapter
    {
        IDataStore m_dataStore;
        ICommonStructureService m_commonStructureService;
        Random m_random = new Random();

        #region IWarehouseAdapter Members

        /// <summary>
        /// This method is called when the plug-in is loaded for the fist 
        /// time. 
        /// </summary>
        /// <param name="ds">An IDataStore instance.</param>
        public void Initialize(IDataStore ds)
        {
            m_dataStore = ds;

            var tfs = TeamFoundationServerFactory.GetServer(
                 TeamFoundationApplication.TfsNameUrl);
            m_commonStructureService = (ICommonStructureService)
                tfs.GetService(typeof(ICommonStructureService));
        }

        /// <summary>
        /// This method is invoked every time the TfsWarehouse OLAP cube is
        /// synchronized. It is intended to add data into the cube or other
        /// repository for reporting purpose.
        /// </summary>
        /// <returns>DataChangesResult.</returns>
        public DataChangesResult MakeDataChanges()
        {
            var projects=m_commonStructureService.ListProjects();
            if (projects.Length == 0)
                return DataChangesResult.NoChanges;
            
            var entry = m_dataStore.CreateFactEntry("My Fact");
            entry["My Field"] = DateTime.Today.DayOfYear;

            int projectIndex = m_random.Next(0, projects.Length - 1);
            entry["Team Project"] = projects[projectIndex].Uri;
            entry.TrackingId = Guid.NewGuid().ToString();

            try
            {
                m_dataStore.BeginTransaction();
                m_dataStore.SaveFactEntry(entry, true);
                m_dataStore.CommitTransaction();
                return DataChangesResult.ChangesComplete;
            }
            catch
            {
                m_dataStore.RollbackTransaction();
                throw;
            }
        }

        /// <summary>
        /// This method is invoked every time the TfsWarehouse OLAP cube is
        /// synchronized and before MakeDataChanges. This method adds 
        /// "My Fact" to the cube.
        /// </summary>
        /// <returns>SchemaChangesResult</returns>
        public SchemaChangesResult MakeSchemaChanges()
        {
            var config = m_dataStore.GetWarehouseConfig();
            var fact = config.GetFact("My Fact");

            if (fact == null)
            {
                /// Add a fact to TfsWarehouse OLAP cube if it doesn't exist.
                fact = CreateFact();
                m_dataStore.BeginTransaction();

                try
                {
                    config.Facts.Add(fact);
                    m_dataStore.Add(config);

                    m_dataStore.CommitTransaction();
                    return SchemaChangesResult.ChangesComplete;
                }
                catch
                {
                    m_dataStore.RollbackTransaction();
                    throw;
                }
            }
            else
            {
                return SchemaChangesResult.NoChanges;
            }
        }

        public void RequestStop()
        {
            /// Do nothing.
        }

        #endregion

        /// <summary>
        /// Return a Fact instance. 
        /// </summary>
        /// <returns>A Fact instance</returns>
        private Fact CreateFact()
        {

            var fact = new Fact
            {
                FriendlyName = "My Fact",
                Name = "My Fact"
            };

            fact.DimensionUses.Add(
                new DimensionUse
                {
                    UseName = "Team Project",
                    DimensionName = "Team Project"
                });

            fact.Fields.Add(new Field
            {
                Name = "My Field",
                Type = "int",
                AggregationFunction = "Sum"
            });

            fact.Measures.Add(new Measure
            {
                Name = "My Measure",
                Type = "int",
                AggregationFunction = "Max",
                SourceField = "My Field"
            });

            fact.PerspectiveName = "Code Churn";
            return fact;
        }
    }
}
