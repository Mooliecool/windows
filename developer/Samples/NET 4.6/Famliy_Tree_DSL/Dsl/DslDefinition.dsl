<?xml version="1.0" encoding="utf-8"?>
<Dsl xmlns:dm0="http://schemas.microsoft.com/VisualStudio/2008/DslTools/Core" dslVersion="1.0.0.0" Id="e7fd0ef0-9a64-4ef7-ad2b-29a44eb2ba55" Description="Description for Fabrikam.Dsl.TestValidation.TestValidation" Name="TestValidation" DisplayName="TestValidation" Namespace="Fabrikam.Dsl.TestValidation" ProductName="TestValidation" CompanyName="Microsoft" PackageGuid="2be27ab8-f24f-4792-8c75-f039c204a3ea" PackageNamespace="Fabrikam.Dsl.TestValidation" xmlns="http://schemas.microsoft.com/VisualStudio/2005/DslTools/DslDefinitionModel">
  <Classes>
    <DomainClass Id="9fec4439-064a-47e7-8f2c-de17dab57210" Description="Description for Fabrikam.Dsl.TestValidation.NamedElement" Name="NamedElement" DisplayName="Named Element" InheritanceModifier="Abstract" Namespace="Fabrikam.Dsl.TestValidation">
      <Properties>
        <DomainProperty Id="4a557c1e-0a89-49b7-b4bd-fa095f6267d7" Description="Description for Fabrikam.Dsl.TestValidation.NamedElement.Name" Name="Name" DisplayName="Name" DefaultValue="" IsElementName="true">
          <Type>
            <ExternalTypeMoniker Name="/System/String" />
          </Type>
        </DomainProperty>
      </Properties>
    </DomainClass>
    <DomainClass Id="5b4f38c2-1f23-41e8-80ca-6b5650fd255d" Description="" Name="Family" DisplayName="Family" Namespace="Fabrikam.Dsl.TestValidation">
      <BaseClass>
        <DomainClassMoniker Name="NamedElement" />
      </BaseClass>
      <ElementMergeDirectives>
        <ElementMergeDirective>
          <Index>
            <DomainClassMoniker Name="Person" />
          </Index>
          <LinkCreationPaths>
            <DomainPath>FamilyHasPeople.People</DomainPath>
          </LinkCreationPaths>
        </ElementMergeDirective>
      </ElementMergeDirectives>
    </DomainClass>
    <DomainClass Id="1d6398bd-f42c-4582-9b03-d49324df79ae" Description="" Name="Person" DisplayName="Person" Namespace="Fabrikam.Dsl.TestValidation">
      <BaseClass>
        <DomainClassMoniker Name="NamedElement" />
      </BaseClass>
      <Properties>
        <DomainProperty Id="a188132b-49e4-409e-a4de-88a240132357" Description="" Name="Birth" DisplayName="Birth" DefaultValue="0">
          <Type>
            <ExternalTypeMoniker Name="/System/Int32" />
          </Type>
        </DomainProperty>
        <DomainProperty Id="89c0e4f3-f364-4222-acec-4141775c5657" Description="" Name="Death" DisplayName="Death" DefaultValue="0">
          <Type>
            <ExternalTypeMoniker Name="/System/Int32" />
          </Type>
        </DomainProperty>
      </Properties>
    </DomainClass>
  </Classes>
  <Relationships>
    <DomainRelationship Id="bf46514c-56b0-4e38-9ae5-ed7cb351470e" Description="Description for Fabrikam.Dsl.TestValidation.FamilyHasPeople" Name="FamilyHasPeople" DisplayName="Family Has People" Namespace="Fabrikam.Dsl.TestValidation" IsEmbedding="true">
      <Source>
        <DomainRole Id="90931e45-48bb-44bd-aad0-bfe0a68a6684" Description="Description for Fabrikam.DSL.TestValidation.FamilyHasPeople.People" Name="Family" DisplayName="Family" PropertyName="People" PropertyDisplayName="People">
          <RolePlayer>
            <DomainClassMoniker Name="Family" />
          </RolePlayer>
        </DomainRole>
      </Source>
      <Target>
        <DomainRole Id="bdbf3bea-eb88-404e-bb9d-1b3419316280" Description="Description for Fabrikam.DSL.TestValidation.FamilyHasPeople.Family" Name="People" DisplayName="People" PropertyName="Family" Multiplicity="One" PropagatesDelete="true" PropertyDisplayName="Family">
          <RolePlayer>
            <DomainClassMoniker Name="Person" />
          </RolePlayer>
        </DomainRole>
      </Target>
    </DomainRelationship>
    <DomainRelationship Id="bb9c7302-c7d5-4dce-96ae-27367306f5fa" Description="Description for Fabrikam.Dsl.TestValidation.ParentRelation" Name="ParentRelation" DisplayName="Parent Relation" Namespace="Fabrikam.Dsl.TestValidation">
      <Properties>
        <DomainProperty Id="af942eb2-89b5-4c25-b364-4bfdda7faed1" Description="" Name="ExampleProperty" DisplayName="Example Property" DefaultValue="">
          <Type>
            <ExternalTypeMoniker Name="/System/String" />
          </Type>
        </DomainProperty>
      </Properties>
      <Source>
        <DomainRole Id="1f342b5a-0be5-48c8-84b0-6ba0f3287ed9" Description="Description for Fabrikam.DSL.TestValidation.ParentRelation.Parent" Name="Child" DisplayName="Child" PropertyName="Parent" PropertyDisplayName="Parent">
          <RolePlayer>
            <DomainClassMoniker Name="Person" />
          </RolePlayer>
        </DomainRole>
      </Source>
      <Target>
        <DomainRole Id="e3aefbf8-c20d-4c0c-aa09-ad8890d976ae" Description="Description for Fabrikam.DSL.TestValidation.ParentRelation.Child" Name="Parent" DisplayName="Parent" PropertyName="Child" PropertyDisplayName="Child">
          <RolePlayer>
            <DomainClassMoniker Name="Person" />
          </RolePlayer>
        </DomainRole>
      </Target>
    </DomainRelationship>
  </Relationships>
  <Types>
    <ExternalType Name="Int32" Namespace="System" />
    <ExternalType Name="String" Namespace="System" />
  </Types>
  <Shapes>
    <GeometryShape Id="daa110ef-b7be-46fb-ac8d-55eae1cbf8db" Description="Description for Fabrikam.Dsl.TestValidation.PersonShape" Name="PersonShape" DisplayName="Person Shape" Namespace="Fabrikam.Dsl.TestValidation" FixedTooltipText="Person Shape" FillColor="LightBlue" InitialWidth="2" InitialHeight="0.75" Geometry="Rectangle">
      <ShapeHasDecorators Position="Center" HorizontalOffset="0" VerticalOffset="0">
        <TextDecorator Name="Name" DisplayName="Name" DefaultText="PersonShapeNameDecorator" />
      </ShapeHasDecorators>
      <ShapeHasDecorators Position="InnerBottomLeft" HorizontalOffset="0" VerticalOffset="0">
        <TextDecorator Name="Birth" DisplayName="Birth" DefaultText="PersonShapeBirthDecorator" />
      </ShapeHasDecorators>
      <ShapeHasDecorators Position="InnerBottomCenter" HorizontalOffset="0" VerticalOffset="0">
        <TextDecorator Name="Hyphen" DisplayName="Hyphen" DefaultText="-" />
      </ShapeHasDecorators>
      <ShapeHasDecorators Position="InnerBottomRight" HorizontalOffset="0" VerticalOffset="0">
        <TextDecorator Name="Death" DisplayName="Death" DefaultText="PersonShapeDeathDecorator" />
      </ShapeHasDecorators>
    </GeometryShape>
  </Shapes>
  <Connectors>
    <Connector Id="6025a52f-fa8c-4bd1-a2ab-c9ce3a0b5152" Description="Description for Fabrikam.Dsl.TestValidation.ParentConnector" Name="ParentConnector" DisplayName="Parent Connector" Namespace="Fabrikam.Dsl.TestValidation" FixedTooltipText="Parent Connector" DashStyle="Dash" TargetEndStyle="EmptyArrow">
      <ConnectorHasDecorators Position="TargetBottom" OffsetFromShape="0" OffsetFromLine="0">
        <TextDecorator Name="Label" DisplayName="Label" DefaultText="DefaultLabelText" />
      </ConnectorHasDecorators>
    </Connector>
  </Connectors>
  <XmlSerializationBehavior Name="TestValidationSerializationBehavior" Namespace="Fabrikam.Dsl.TestValidation">
    <ClassData>
      <XmlClassData TypeName="NamedElement" MonikerAttributeName="Name" MonikerElementName="namedElementMoniker" ElementName="namedElement" MonikerTypeName="NamedElementMoniker">
        <DomainClassMoniker Name="NamedElement" />
        <ElementData>
          <XmlPropertyData XmlName="Name" IsMonikerKey="true">
            <DomainPropertyMoniker Name="NamedElement/Name" />
          </XmlPropertyData>
        </ElementData>
      </XmlClassData>
      <XmlClassData TypeName="FamilyHasPeople" MonikerAttributeName="" MonikerElementName="familyHasPeopleMoniker" ElementName="familyHasPeople" MonikerTypeName="FamilyHasPeopleMoniker">
        <DomainRelationshipMoniker Name="FamilyHasPeople" />
      </XmlClassData>
      <XmlClassData TypeName="ParentRelation" MonikerAttributeName="" MonikerElementName="parentRelationMoniker" ElementName="parentRelation" MonikerTypeName="ParentRelationMoniker">
        <DomainRelationshipMoniker Name="ParentRelation" />
        <ElementData>
          <XmlPropertyData XmlName="ExampleProperty">
            <DomainPropertyMoniker Name="ParentRelation/ExampleProperty" />
          </XmlPropertyData>
        </ElementData>
      </XmlClassData>
      <XmlClassData TypeName="Family" MonikerAttributeName="" MonikerElementName="familyMoniker" ElementName="family" MonikerTypeName="FamilyMoniker">
        <DomainClassMoniker Name="Family" />
        <ElementData>
          <XmlRelationshipData RoleElementName="People">
            <DomainRelationshipMoniker Name="FamilyHasPeople" />
          </XmlRelationshipData>
        </ElementData>
      </XmlClassData>
      <XmlClassData TypeName="Person" MonikerAttributeName="" MonikerElementName="personMoniker" ElementName="person" MonikerTypeName="PersonMoniker">
        <DomainClassMoniker Name="Person" />
        <ElementData>
          <XmlPropertyData XmlName="Birth">
            <DomainPropertyMoniker Name="Person/Birth" />
          </XmlPropertyData>
          <XmlPropertyData XmlName="Death">
            <DomainPropertyMoniker Name="Person/Death" />
          </XmlPropertyData>
          <XmlRelationshipData UseFullForm="true" RoleElementName="Parent">
            <DomainRelationshipMoniker Name="ParentRelation" />
          </XmlRelationshipData>
        </ElementData>
      </XmlClassData>
      <XmlClassData TypeName="PersonShape" MonikerAttributeName="" MonikerElementName="personShapeMoniker" ElementName="personShape" MonikerTypeName="PersonShapeMoniker">
        <GeometryShapeMoniker Name="PersonShape" />
      </XmlClassData>
      <XmlClassData TypeName="ParentConnector" MonikerAttributeName="" MonikerElementName="parentConnectorMoniker" ElementName="parentConnector" MonikerTypeName="ParentConnectorMoniker">
        <ConnectorMoniker Name="ParentConnector" />
      </XmlClassData>
      <XmlClassData TypeName="TestValidationDiagram" MonikerAttributeName="" MonikerElementName="testValidationDiagramMoniker" ElementName="testValidationDiagram" MonikerTypeName="TestValidationDiagramMoniker">
        <DiagramMoniker Name="TestValidationDiagram" />
      </XmlClassData>
    </ClassData>
  </XmlSerializationBehavior>
  <ExplorerBehavior Name="TestValidationExplorer" />
  <ConnectionBuilders>
    <ConnectionBuilder Name="ConnectParentRelation">
      <LinkConnectDirective>
        <DomainRelationshipMoniker Name="ParentRelation" />
        <SourceDirectives>
          <RolePlayerConnectDirective>
            <AcceptingClass>
              <DomainClassMoniker Name="Person" />
            </AcceptingClass>
          </RolePlayerConnectDirective>
        </SourceDirectives>
        <TargetDirectives>
          <RolePlayerConnectDirective>
            <AcceptingClass>
              <DomainClassMoniker Name="Person" />
            </AcceptingClass>
          </RolePlayerConnectDirective>
        </TargetDirectives>
      </LinkConnectDirective>
    </ConnectionBuilder>
  </ConnectionBuilders>
  <Diagram Id="365d29f4-a94d-49d6-bcfe-1f099aff9b4d" Description="Description for Fabrikam.Dsl.TestValidation.TestValidationDiagram" Name="TestValidationDiagram" DisplayName="Test Validation Diagram" Namespace="Fabrikam.Dsl.TestValidation">
    <Class>
      <DomainClassMoniker Name="Family" />
    </Class>
    <ShapeMaps>
      <ShapeMap>
        <DomainClassMoniker Name="Person" />
        <ParentElementPath>
          <DomainPath>FamilyHasPeople.Family/!Family</DomainPath>
        </ParentElementPath>
        <DecoratorMap>
          <TextDecoratorMoniker Name="PersonShape/Name" />
          <PropertyDisplayed>
            <PropertyPath>
              <DomainPropertyMoniker Name="NamedElement/Name" />
            </PropertyPath>
          </PropertyDisplayed>
        </DecoratorMap>
        <DecoratorMap>
          <TextDecoratorMoniker Name="PersonShape/Birth" />
          <PropertyDisplayed>
            <PropertyPath>
              <DomainPropertyMoniker Name="Person/Birth" />
            </PropertyPath>
          </PropertyDisplayed>
        </DecoratorMap>
        <DecoratorMap>
          <TextDecoratorMoniker Name="PersonShape/Death" />
          <PropertyDisplayed>
            <PropertyPath>
              <DomainPropertyMoniker Name="Person/Death" />
            </PropertyPath>
          </PropertyDisplayed>
        </DecoratorMap>
        <GeometryShapeMoniker Name="PersonShape" />
      </ShapeMap>
    </ShapeMaps>
    <ConnectorMaps>
      <ConnectorMap>
        <ConnectorMoniker Name="ParentConnector" />
        <DomainRelationshipMoniker Name="ParentRelation" />
        <DecoratorMap>
          <TextDecoratorMoniker Name="ParentConnector/Label" />
          <PropertyDisplayed>
            <PropertyPath>
              <DomainPropertyMoniker Name="ParentRelation/ExampleProperty" />
            </PropertyPath>
          </PropertyDisplayed>
        </DecoratorMap>
      </ConnectorMap>
    </ConnectorMaps>
  </Diagram>
  <Designer FileExtension="ftree" EditorGuid="ded7bf55-2ed6-4b7e-883b-2a5098e3691b">
    <RootClass>
      <DomainClassMoniker Name="Family" />
    </RootClass>
    <XmlSerializationDefinition CustomPostLoad="false">
      <XmlSerializationBehaviorMoniker Name="TestValidationSerializationBehavior" />
    </XmlSerializationDefinition>
    <ToolboxTab TabText="TestValidation">
      <ElementTool Name="Person" ToolboxIcon="resources\personshapetoolbitmap.bmp" Caption="Person" Tooltip="Drag onto diagram to create a Person" HelpKeyword="ConnectPersonF1Keyword">
        <DomainClassMoniker Name="Person" />
      </ElementTool>
      <ConnectionTool Name="ParentRelation" ToolboxIcon="resources\parentconnectortoolbitmap.bmp" Caption="ParentRelation" Tooltip="Click, click then drag from child to parent" HelpKeyword="ConnectParentRelationF1Keyword">
        <ConnectionBuilderMoniker Name="TestValidation/ConnectParentRelation" />
      </ConnectionTool>
    </ToolboxTab>
    <Validation UsesMenu="true" UsesOpen="true" UsesSave="true" UsesLoad="false" />
    <DiagramMoniker Name="TestValidationDiagram" />
  </Designer>
  <Explorer ExplorerGuid="d1cc0445-53c8-41a7-a943-ea743684186a" Title="TestValidation">
    <ExplorerBehaviorMoniker Name="TestValidation/TestValidationExplorer" />
  </Explorer>
</Dsl>