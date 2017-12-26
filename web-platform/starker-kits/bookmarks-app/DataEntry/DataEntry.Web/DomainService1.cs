
namespace DataEntry.Web
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.ComponentModel.DataAnnotations;
    using System.Data;
    using System.Linq;
    using System.ServiceModel.DomainServices.EntityFramework;
    using System.ServiceModel.DomainServices.Hosting;
    using System.ServiceModel.DomainServices.Server;


    // Implements application logic using the FavoritesEntities context.
    // TODO: Add your application logic to these methods or in additional methods.
    // TODO: Wire up authentication (Windows/ASP.NET Forms) and uncomment the following to disable anonymous access
    // Also consider adding roles to restrict access as appropriate.
    // [RequiresAuthentication]
    [EnableClientAccess()]
    public class DomainService1 : LinqToEntitiesDomainService<FavoritesEntities>
    {

        // TODO:
        // Consider constraining the results of your query method.  If you need additional input you can
        // add parameters to this method or create additional query methods with different names.
        // To support paging you will need to add ordering to the 'Sites' query.
        public IQueryable<Site> GetSites()
        {
            return this.ObjectContext.Sites;
        }

        public void InsertSite(Site site)
        {
            if ((site.EntityState != EntityState.Detached))
            {
                this.ObjectContext.ObjectStateManager.ChangeObjectState(site, EntityState.Added);
            }
            else
            {
                this.ObjectContext.Sites.AddObject(site);
            }
        }

        public void UpdateSite(Site currentSite)
        {
            this.ObjectContext.Sites.AttachAsModified(currentSite, this.ChangeSet.GetOriginal(currentSite));
        }

        public void DeleteSite(Site site)
        {
            if ((site.EntityState == EntityState.Detached))
            {
                this.ObjectContext.Sites.Attach(site);
            }
            this.ObjectContext.Sites.DeleteObject(site);
        }
    }
}


