Imports Microsoft.WindowsAzure
Imports Microsoft.WindowsAzure.ServiceRuntime

' Note: For instructions on enabling IIS6 or IIS7 classic mode, 
' visit http://go.microsoft.com/?LinkId=9394802

Public Class MvcApplication
    Inherits System.Web.HttpApplication

    Shared Sub RegisterRoutes(ByVal routes As RouteCollection)
        routes.IgnoreRoute("{resource}.axd/{*pathInfo}")

        ' MapRoute takes the following parameters, in order:
        ' (1) Route name
        ' (2) URL with parameters
        ' (3) Parameter defaults
        routes.MapRoute( _
            "Default", _
            "{controller}/{action}/{id}", _
            New With {.controller = "Home", .action = "Index", .id = UrlParameter.Optional} _
        )

    End Sub

    Sub Application_Start()
        CloudStorageAccount.SetConfigurationSettingPublisher(Function(configName, configSetter)
                                                                 configSetter(RoleEnvironment.GetConfigurationSettingValue(configName))
                                                                 Return Nothing
                                                             End Function)

        AreaRegistration.RegisterAllAreas()

        RegisterRoutes(RouteTable.Routes)
    End Sub
End Class
