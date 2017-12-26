========================================================================
    VISUAL STUDIO EXTENSIBILITY : CSVSService Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This sample demonstrates how to provide, register, consume service in 
VSPackage.

1) Provide services in VSPackage as a Service Provider, this sample providing 
   a global service and a local service.
   We adding callback methods to the service container to create the services 
   at first, and then implementing the callback methods and the services 
   classes.

2) Register the services we provided.
   Only the global services need to be registered, we using 
   ProvideServiceAttribute attribute to register them, and using 
   DefaultRegistryRootAttribute and PackageRegistrationAttribute attributes to 
   specify the path in the registry.

3) Consume the services we provided in another VSPackage.
   In sited VSPackage, we using GetService method provided by Package class to
   get the services.  In non-sited scenario, such as in a tool window, we could 
   use GetGlobalService method to get the services.


//////////////////////////////////////////////////////////////////////////////
Prerequisites:

VS 2008 SDK must be installed on the machine. You can download it from:
http://www.microsoft.com/downloads/details.aspx?FamilyID=30402623-93ca-479a-867c-04dc45164f5b&displaylang=en

Otherwise the project may not be opened by Visual Studio.

If you run this project on a x64 OS, please also config the Debug tab of the project
Setting. Set the "Start external program" to 
C:\Program Files(x86)\Microsoft Visual Studio 9.0\Common7\IDE\devenv.exe

NOTE: The Package Load Failure Dialog occurs because there is no PLK(Package Load Key)
      Specified in this package. To obtain a PLK, please to go to WebSite:
      http://msdn.microsoft.com/en-us/vsx/cc655795.aspx
      More info
      http://msdn.microsoft.com/en-us/library/bb165395.aspx


/////////////////////////////////////////////////////////////////////////////
Creation:

Step1. Create a Visual Studio Integration Package project from the New 
Project dialog, with Menu Command checkbox checked, so we have a .vsct file 
which responsible to the menu commands.

Step2. Open the default package class file(we make it as Service Provider), add 
[ProvideService] attribute at the head of Package class's definition to provide
our global service.

Step3. In the Service Provider package class's constructor, add service callback
to the service container:
IServiceContainer serviceContainer = this as IServiceContainer;
ServiceCreatorCallback callback = new ServiceCreatorCallback(CreateService);
serviceContainer.AddService(typeof(SCSGlobalService), callback, true);
serviceContainer.AddService(typeof(SCSLocalService), callback);

Step4. Implement the service callback:
private object CreateService(IServiceContainer container, Type serviceType)
{
    if (typeof(SCSGlobalService) == serviceType)
    {
        return new GlobalService(this);
    }

    else if (typeof(SCSLocalService) == serviceType)
    {
        return new LocalService(this);
    }

    else
    {
        return null;
    }
}

Step4. Create a new class file to implement the global service class.

Step5. Create a new class file to implement the local service class.

Step6. Create a new package class file to implement the Service Consumer, apply
[ProvideMenuResource] attribute to it to expose the menus created by .vsct file.

Step7. Add the menu items in Initialize method of package class, implement the
handlers of the menu items.

Step8. Modify the .vsct file to specify the Menu Command.
1) Create two menu groups for the submenu and buttons:
<Groups>
    <Group guid="guidCSVSServiceCmdSet" id="ServiceMenuGroup" priority="0x0600">
        <Parent guid="guidSHLMainMenu" id="IDM_VS_MENU_TOOLS"/>
    </Group>

    <Group guid="guidCSVSServiceCmdSet" id="ServiceButtonGroup" priority="0x0600">
        <Parent guid="guidCSVSServiceCmdSet" id="ServiceMenu"/>
    </Group>
</Groups>

2) Create the submenu:
<Menus>
    <Menu guid="guidCSVSServiceCmdSet" id="ServiceMenu" priority="0x700" type="Menu">
        <Parent guid="guidCSVSServiceCmdSet" id="ServiceMenuGroup" />
        <Strings>
            <ButtonText>Service Sample</ButtonText>
            <CommandName>Service Sample</CommandName>
        </Strings>
    </Menu>
</Menus>

3) Create the buttons:
<Buttons>
    <Button guid="guidCSVSServiceCmdSet" id="cmdidCallLocalService" priority="0x0100" type="Button">
        <Parent guid="guidCSVSServiceCmdSet" id="ServiceButtonGroup" />
        <Icon guid="guidImages" id="bmpPic1" />
        <Strings>
            <CommandName>cmdidCallLocalService</CommandName>
            <ButtonText>CallLocalService</ButtonText>
        </Strings>
    </Button>

    <Button guid="guidCSVSServiceCmdSet" id="cmdidCallGlobalService" priority="0x0100" type="Button">
        <Parent guid="guidCSVSServiceCmdSet" id="ServiceButtonGroup" />
        <Icon guid="guidImages" id="bmpPic2" />
        <Strings>
            <CommandName>cmdidCallGlobalService</CommandName>
            <ButtonText>CallGlobalService</ButtonText>
        </Strings>
    </Button>
</Buttons>


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Services
http://msdn.microsoft.com/en-us/library/bb166389.aspx


/////////////////////////////////////////////////////////////////////////////
