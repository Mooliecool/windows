using System;
using System.ComponentModel;
using System.ComponentModel.Composition.Hosting;
using System.ComponentModel.Composition;
using System.Collections.Generic;
using System.Windows;

namespace CSSL4MEF
{
    /// <summary>
    /// Dynamic load xap class. Code from 
    /// http://mef.codeplex.com/wikipage?title=DeploymentCatalog&referringTitle=Guide
    /// </summary>
    [Export(typeof(IDeploymentCatalogService))]
    public class DeploymentCatalogService : IDeploymentCatalogService
    {

        private static AggregateCatalog _aggregateCatalog;     
        Dictionary<string, DeploymentCatalog> _catalogs;
       
        public DeploymentCatalogService()
        {
            _catalogs = new Dictionary<string, DeploymentCatalog>();
        }


        public static void Initialize()
        {
            _aggregateCatalog = new AggregateCatalog();
            _aggregateCatalog.Catalogs.Add(new DeploymentCatalog());
            var container = new CompositionContainer(_aggregateCatalog);
            CompositionHost.Initialize(_aggregateCatalog);
        }

        public void AddXap(string uri, Action<AsyncCompletedEventArgs> completedAction = null)
        {
            DeploymentCatalog catalog;
            if (!_catalogs.TryGetValue(uri, out catalog))
            {
                catalog = new DeploymentCatalog(uri);
                if (completedAction != null)
                {
                    catalog.DownloadCompleted +=
                        (s, e) => completedAction(e);
                }
                else
                {
                    catalog.DownloadCompleted +=
                        (catalog_DownloadCompleted);
                }

                catalog.DownloadAsync();
                _catalogs[uri] = catalog;
                _aggregateCatalog.Catalogs.Add(catalog);
            }
        }

        void catalog_DownloadCompleted(object sender, System.ComponentModel.AsyncCompletedEventArgs e)
        {
            if (e.Error != null)
                MessageBox.Show("Error occured while loading xap file.\n" + e.Error.Message);
        }

        public void RemoveXap(string uri)
        {
            DeploymentCatalog catalog;
            if (_catalogs.TryGetValue(uri, out catalog))
            {
                _aggregateCatalog.Catalogs.Remove(catalog);
            }
        }
    }


    public interface IDeploymentCatalogService
    {
        void AddXap(string uri, Action<AsyncCompletedEventArgs> completedAction = null);
        void RemoveXap(string uri);
    }
}
