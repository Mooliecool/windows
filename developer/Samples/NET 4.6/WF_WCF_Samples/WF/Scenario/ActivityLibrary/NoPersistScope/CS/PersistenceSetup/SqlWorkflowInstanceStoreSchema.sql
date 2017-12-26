use NoPersistScopeSample
go

set ansi_nulls on
set quoted_identifier on
set nocount on
go

if not exists( select 1 from [dbo].[sysusers] where name=N'System.Activities.DurableInstancing.InstanceStoreUsers' and issqlrole=1 )
	create role [System.Activities.DurableInstancing.InstanceStoreUsers]
go

if not exists (select * from sys.schemas where name = N'System.Activities.DurableInstancing')
	exec ('create schema [System.Activities.DurableInstancing]')
go

if exists (select name from sys.views v where v.name = 'InstanceStateProperties' and v.schema_id = schema_id('System.Activities.DurableInstancing'))
	drop view [System.Activities.DurableInstancing].[InstanceStateProperties]
go 

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[InstancesTable]') and type in (N'U'))
	drop table [System.Activities.DurableInstancing].[InstancesTable]
go

create table [System.Activities.DurableInstancing].[InstancesTable]
(
	[Id] uniqueidentifier not null,
	[SurrogateInstanceId] bigint identity not null,
	[SurrogateLockOwnerId] bigint null,
	[PrimitiveDataProperties] varbinary(max) default null,
	[ComplexDataProperties] varbinary(max) default null,
	[WriteOnlyPrimitiveDataProperties] varbinary(max) default null,
	[WriteOnlyComplexDataProperties] varbinary(max) default null,
	[MetadataProperties] varbinary(max) default null,
	[SerializationMethod] tinyint default 0,
	[Version] bigint not null,
	[PendingTimer] datetime null,
	[CreationTime] datetime not null,
	[LastUpdated] datetime default null,
	[ServiceDeploymentId] bigint not null,
	[SuspensionReason] nvarchar(450) default null,
	[BlockingBookmarks] nvarchar(450) default null,
	[LastMachineRunOn] nvarchar(450) default null,
	[ExecutionStatus] nvarchar(450) default null,
	[IsInitialized] bit default 0,
	[IsSuspended] bit default 0,
	[IsReadyToRun] bit default 0,
	[IsCompleted] bit default 0
)
go

create unique clustered index CIX_InstancesTable
	on [System.Activities.DurableInstancing].[InstancesTable] ([SurrogateInstanceId])
	with (allow_page_locks = off)
go

create unique nonclustered index NCIX_InstancesTable_Id
	on [System.Activities.DurableInstancing].[InstancesTable] ([Id])
	with (allow_page_locks = off)
go

create nonclustered index NCIX_InstancesTable_SurrogateLockOwnerId
	on [System.Activities.DurableInstancing].[InstancesTable] ([SurrogateLockOwnerId])
	with (allow_page_locks = off)
go

create nonclustered index NCIX_InstancesTable_IsReadyToRun
	on [System.Activities.DurableInstancing].[InstancesTable] ([IsReadyToRun])
	with (allow_page_locks = off)
go

create nonclustered index NCIX_InstancesTable_PendingTimer
	on [System.Activities.DurableInstancing].[InstancesTable] ([PendingTimer])
	with (allow_page_locks = off)
go

create nonclustered index NCIX_InstancesTable_LastUpdated
	on [System.Activities.DurableInstancing].[InstancesTable] ([LastUpdated])
	with (allow_page_locks = off)
go

create nonclustered index NCIX_InstancesTable_CreationTime
	on [System.Activities.DurableInstancing].[InstancesTable] ([CreationTime])
	with (allow_page_locks = off)
go

create nonclustered index NCIX_InstancesTable_SuspensionReason
	on [System.Activities.DurableInstancing].[InstancesTable] ([SuspensionReason])
	with (allow_page_locks = off)
go

create nonclustered index NCIX_InstancesTable_ServiceDeploymentId
	on [System.Activities.DurableInstancing].[InstancesTable] ([ServiceDeploymentId])
	with (allow_page_locks = off)
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[KeysTable]') and type in (N'U'))
	drop table [System.Activities.DurableInstancing].[KeysTable]
go

create table [System.Activities.DurableInstancing].[KeysTable]
(
	[Id] uniqueidentifier not null,
	[SurrogateKeyId] bigint identity not null,
	[SurrogateInstanceId] bigint null,
	[IsAssociated] bit
) 
go

create unique clustered index CIX_KeysTable
	on [System.Activities.DurableInstancing].[KeysTable] ([Id])	
	with (ignore_dup_key = on, allow_page_locks = off)
go

create nonclustered index NCIX_KeysTable_SurrogateInstanceId
	on [System.Activities.DurableInstancing].[KeysTable] ([SurrogateInstanceId])
	with (allow_page_locks = off)
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[LockOwnersTable]') and type in (N'U'))
	drop table [System.Activities.DurableInstancing].[LockOwnersTable]
go

create table [System.Activities.DurableInstancing].[LockOwnersTable]
(
	[Id] uniqueidentifier not null,
	[SurrogateLockOwnerId] bigint identity not null,
	[LockExpiration] datetime not null,
	[ServiceDeploymentId] bigint not null,
	[MachineName] nvarchar(128) not null,
	[EnqueueCommand] bit not null,
	[DeletesInstanceOnCompletion] bit not null,
	[PrimitiveLockOwnerData] varbinary(max) default null,
	[ComplexLockOwnerData] varbinary(max) default null,
	[WriteOnlyPrimitiveLockOwnerData] varbinary(max) default null,
	[WriteOnlyComplexLockOwnerData] varbinary(max) default null,
	[SerializationMethod] tinyint default 0
)
go

create unique clustered index CIX_LockOwnersTable
	on [System.Activities.DurableInstancing].[LockOwnersTable] ([SurrogateLockOwnerId])
	with (allow_page_locks = off)
go

create unique nonclustered index NCIX_LockOwnersTable_Id
	on [System.Activities.DurableInstancing].[LockOwnersTable] ([Id])
	with (ignore_dup_key = on, allow_page_locks = off)

create nonclustered index NCIX_LockOwnersTable_LockExpiration
	on [System.Activities.DurableInstancing].[LockOwnersTable] ([LockExpiration])
	with (allow_page_locks = off)
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[KeyPropertiesTable]') and type in (N'U'))
	drop table [System.Activities.DurableInstancing].[KeyPropertiesTable]
go

create table [System.Activities.DurableInstancing].[KeyPropertiesTable]
(
	[SurrogateKeyId] bigint not null,
	[PropertyNameId] bigint not null,
	[IsWriteOnly] bit default 0 not null,
	[PropertyValue] sql_variant null,
	[PropertyBinaryValue] varbinary(max) null
)
go

create unique clustered index CIX_KeyPropertiesTable
	on [System.Activities.DurableInstancing].[KeyPropertiesTable] ([SurrogateKeyId], [PropertyNameId])
	with (ignore_dup_key = on, allow_page_locks = off)
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[PropertyNamesTable]') and type in (N'U'))
	drop table [System.Activities.DurableInstancing].[PropertyNamesTable]
go

create table [System.Activities.DurableInstancing].[PropertyNamesTable]
(
    [Id] bigint identity not null,
	[PropertyNameId] bigint not null,
	[PropertyNamespaceId] bigint not null
)
go

create unique clustered index CIX_PropertyNamesTable
	on [System.Activities.DurableInstancing].[PropertyNamesTable] ([PropertyNameId], [PropertyNamespaceId])
	with (ignore_dup_key = on, allow_page_locks = off)
go
	
create unique nonclustered index NCIX_PropertyNamesTable_Id
	on [System.Activities.DurableInstancing].[PropertyNamesTable] ([Id])
	with (allow_page_locks = off)
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[StringsTable]') and TYPE in (N'U'))
	drop table [System.Activities.DurableInstancing].[StringsTable]
go

create table [System.Activities.DurableInstancing].[StringsTable]
(
	[Id] bigint identity,
	[String] nvarchar(450) not null
)
go

create unique clustered index CIX_StringsTable
	on [System.Activities.DurableInstancing].[StringsTable] ([String])
	with (ignore_dup_key = on, allow_page_locks = off)
go

create unique nonclustered index NCIX_StringsTable_Id
	on [System.Activities.DurableInstancing].[StringsTable] ([Id])
	with (allow_page_locks = off)
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[ServiceDeploymentTable]') and type in (N'U'))
	drop table [System.Activities.DurableInstancing].[ServiceDeploymentTable]
go

create table [System.Activities.DurableInstancing].[ServiceDeploymentTable]
(
	[Id] bigint identity not null,
	[ServiceDeploymentHash] varbinary(32) not null,
	[SiteName] nvarchar(max) null,
	[RelativeServicePath] nvarchar(max) null,
	[RelativeApplicationPath] nvarchar(max) null,
	[ServiceName] nvarchar(max) null,
	[ServiceNamespace] nvarchar(max) null,
	[ControlEndpointAddress] nvarchar(max) null
)
go

create unique clustered index CIX_ServiceDeploymentTable
	on [System.Activities.DurableInstancing].[ServiceDeploymentTable] ([Id])
	with (allow_page_locks = off)
go

create unique nonclustered index NCIX_ServiceDeploymentTable_ServiceDeploymentHash
	on [System.Activities.DurableInstancing].[ServiceDeploymentTable] ([ServiceDeploymentHash])
	with (ignore_dup_key = on, allow_page_locks = off)
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[InstancePromotedPropertiesTable]') and type in (N'U'))
	drop table [System.Activities.DurableInstancing].[InstancePromotedPropertiesTable]
go

create table [System.Activities.DurableInstancing].[InstancePromotedPropertiesTable]
(
	  [SurrogateInstanceId] bigint not null,
      [PromotionName] nvarchar(400) not null,
      [Value1] sql_variant null,
      [Value2] sql_variant null,
      [Value3] sql_variant null,
      [Value4] sql_variant null,
      [Value5] sql_variant null,
      [Value6] sql_variant null,
      [Value7] sql_variant null,
      [Value8] sql_variant null,
      [Value9] sql_variant null,
      [Value10] sql_variant null,
      [Value11] sql_variant null,
      [Value12] sql_variant null,
      [Value13] sql_variant null,
      [Value14] sql_variant null,
      [Value15] sql_variant null,
      [Value16] sql_variant null,
      [Value17] sql_variant null,
      [Value18] sql_variant null,
      [Value19] sql_variant null,
      [Value20] sql_variant null,
      [Value21] sql_variant null,
      [Value22] sql_variant null,
      [Value23] sql_variant null,
      [Value24] sql_variant null,
      [Value25] sql_variant null,
      [Value26] sql_variant null,
      [Value27] sql_variant null,
      [Value28] sql_variant null,
      [Value29] sql_variant null,
      [Value30] sql_variant null,
      [Value31] sql_variant null,
      [Value32] sql_variant null,
      [Value33] varbinary(max) null,
      [Value34] varbinary(max) null,
      [Value35] varbinary(max) null,
      [Value36] varbinary(max) null,
      [Value37] varbinary(max) null,
      [Value38] varbinary(max) null,
      [Value39] varbinary(max) null,
      [Value40] varbinary(max) null,
      [Value41] varbinary(max) null,
      [Value42] varbinary(max) null,
      [Value43] varbinary(max) null,
      [Value44] varbinary(max) null,
      [Value45] varbinary(max) null,
      [Value46] varbinary(max) null,
      [Value47] varbinary(max) null,
      [Value48] varbinary(max) null,
      [Value49] varbinary(max) null,
      [Value50] varbinary(max) null,
      [Value51] varbinary(max) null,
      [Value52] varbinary(max) null,
      [Value53] varbinary(max) null,
      [Value54] varbinary(max) null,
      [Value55] varbinary(max) null,
      [Value56] varbinary(max) null,
      [Value57] varbinary(max) null,
      [Value58] varbinary(max) null,
      [Value59] varbinary(max) null,
      [Value60] varbinary(max) null,
      [Value61] varbinary(max) null,
      [Value62] varbinary(max) null,
      [Value63] varbinary(max) null,
      [Value64] varbinary(max) null
)
go

create unique clustered index CIX_InstancePromotedPropertiesTable
	on [System.Activities.DurableInstancing].[InstancePromotedPropertiesTable] ([SurrogateInstanceId], [PromotionName])
	with (allow_page_locks = off)
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[SqlWorkflowStoreVersionTable]') and type in (N'U'))
	drop table [System.Activities.DurableInstancing].[SqlWorkflowStoreVersionTable]
go

create table [System.Activities.DurableInstancing].[SqlWorkflowStoreVersionTable]
(
	[Major] bigint,
	[Minor] bigint,
	[Build] bigint,
	[Revision] bigint,
	[LastUpdated] datetime
)
go

insert into [System.Activities.DurableInstancing].[SqlWorkflowStoreVersionTable]
values (4, 0, 0, 0, getutcdate())

--Workflow Managament Service Artifacts

if not exists (select principal_id from sys.database_principals where name = 'System.Activities.DurableInstancing.ControlUsers' and type = 'R')
	create role [System.Activities.DurableInstancing.ControlUsers]
go

if not exists (select principal_id from sys.database_principals where name = 'System.Activities.DurableInstancing.WorkflowManagementServiceUsers' and type = 'R')
	create role [System.Activities.DurableInstancing.WorkflowManagementServiceUsers]
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[CommandQueueTable]') and type in (N'U'))
	drop table [System.Activities.DurableInstancing].[CommandQueueTable]
go

create table [System.Activities.DurableInstancing].[CommandQueueTable](
	[Id] [bigint] identity(1,1) not null,
	[SurrogateInstanceId] [bigint] not null,
	[Type] [tinyint] not null,
	[IsSystemCommand] [bit] not null default(1), 	
	[OriginalExecutionTime] [datetime] not null,
	[ExecutionTime] [datetime] not null,
	[ExecutionAttemptsCount] [tinyint] not null default (0),
	[LockOwner] [uniqueidentifier] null default (null),
	[LockExpiryTime] [datetime] null default (null) 
)
go

create clustered index [CIX_ExecutionTime] on [System.Activities.DurableInstancing].[CommandQueueTable] 
(
	[ExecutionTime]
)
go

create nonclustered index [IX_SurrogateInstanceId] on [System.Activities.DurableInstancing].[CommandQueueTable] 
(
	[SurrogateInstanceId]
)
go

create unique nonclustered index [IX_CommandId] on [System.Activities.DurableInstancing].[CommandQueueTable] 
(
	[Id]
)
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[CommandQueueExecutionLogTable]') and type in (N'U'))
	drop table [System.Activities.DurableInstancing].[CommandQueueExecutionLogTable]
go

create table [System.Activities.DurableInstancing].[CommandQueueExecutionLogTable]
(
	[CommandId] [bigint] not null,
	[SurrogateInstanceId] [bigint] not null,
	[Type] [tinyint] not null,	
	[IsSystemCommand] [bit] not null, 	
	[OriginalExecutionTime] [datetime] not null,
	[IsExecutionSuccessful] [bit] not null,	
	[ExecutionAttemptTime] [datetime] not null,
	[ExecutionAttemptsCount] [int] not null, 
	[MachineName] nvarchar(446) not null,
	[ExceptionCode] nvarchar(100) null,
	[Exception] nvarchar(max) null,
)
go

create unique clustered index [CIX_SurrogateInstanceId] on [System.Activities.DurableInstancing].[CommandQueueExecutionLogTable] 
(
	[SurrogateInstanceId], [Type], [IsSystemCommand], [ExceptionCode]
) with (ignore_dup_key = on)
go

create nonclustered index [NCIX_CommandId] on [System.Activities.DurableInstancing].[CommandQueueExecutionLogTable] 
(
	[CommandId]
)
go

create nonclustered index [NCIX_ExecutionAttemptTimeMachineName] on [System.Activities.DurableInstancing].[CommandQueueExecutionLogTable] 
(
	[ExecutionAttemptTime], [MachineName]
)
go

grant select on [System.Activities.DurableInstancing].[CommandQueueExecutionLogTable] to [System.Activities.DurableInstancing.ControlUsers]
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[InstanceRecoveryJobLastRunTable]') and type in (N'U'))
	drop table [System.Activities.DurableInstancing].[InstanceRecoveryJobLastRunTable]
go

create table [System.Activities.DurableInstancing].[InstanceRecoveryJobLastRunTable](
	[LastRunTime] [datetime] not null	
)
go

insert into [System.Activities.DurableInstancing].[InstanceRecoveryJobLastRunTable] values ('1753-01-01')
go
