use NoPersistScopeSample
go

set ansi_nulls on
go

set quoted_identifier on
go

--Workflow Management Service artifacts
if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[EnqueueWorkflowInstanceControlCommand]') and type in (N'P', N'PC'))
	drop procedure [System.Activities.DurableInstancing].[EnqueueWorkflowInstanceControlCommand]
go

create procedure [System.Activities.DurableInstancing].[EnqueueWorkflowInstanceControlCommand]
	@instanceId uniqueIdentifier,
	@commandType tinyint,	
	@commandId bigint output
	--return value - 0 Success, 
    --				 1 - Instance does not exist, 
	--				 2 - Instance already has a pending command in the queue
	--               3 - Command not recognized
AS
begin
    
	set nocount on;
	set transaction isolation level read committed;
	set xact_abort on;
	set deadlock_priority low;
	
	begin tran
	declare @result as int
	set @result = 0	
	
	declare @now datetime
	set @now  = getutcdate()
	
	if ((@commandType < 1) or (@commandType > 6))
	begin
		set @result = 3		
		goto ExitProc
	end
	
	declare @surrogateInstanceId as bigint	
	select @surrogateInstanceId = [SurrogateInstanceId]
	from [InstancesTable] with (holdlock)
	where Id = @instanceId 
	
	if (@@ROWCOUNT = 0)
	begin
		set @result = 1		
		goto ExitProc
	end
	

	select top 1 1 
	from [CommandQueueTable] with (holdlock) 
	where [SurrogateInstanceId] = @surrogateInstanceId
		and [IsSystemCommand] = 0 --Non system command
	
	if (@@rowcount = 1)
	begin
		set @result = 2		
		goto ExitProc
	end

		
	declare @CommandIdTempTable table
	(
		Id bigint
	)	
	
	insert into [CommandQueueTable]
	    ([SurrogateInstanceId], [Type], [IsSystemCommand], [OriginalExecutionTime], [ExecutionTime])
		output inserted.[Id] into @CommandIdTempTable
		values( @surrogateInstanceId, @commandType, 0, @now, @now)
		
	select @commandId = [Id]
	from @CommandIdTempTable
	
ExitProc:
	commit tran
	return @result
end
go

grant execute on [System.Activities.DurableInstancing].[EnqueueWorkflowInstanceControlCommand] to [System.Activities.DurableInstancing.ControlUsers]
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[AdjustWorkflowInstanceRunCommand]') and type in (N'P', N'PC'))
	drop procedure [System.Activities.DurableInstancing].[AdjustWorkflowInstanceRunCommand]
go

create procedure [System.Activities.DurableInstancing].[AdjustWorkflowInstanceRunCommand]
	@surrogateInstanceId bigint,
	@commandExecutionTime datetime
AS
begin    
	set nocount on;
	set transaction isolation level read committed;
	set xact_abort on;	

	delete from [CommandQueueTable]
	where [SurrogateInstanceId] = @surrogateInstanceId
		and [Type] = 1
		and [IsSystemCommand] = 1
	
	if (@commandExecutionTime is not null)
	begin
		insert into [CommandQueueTable]
			([SurrogateInstanceId], [Type], [OriginalExecutionTime], [ExecutionTime])
			values( @surrogateInstanceId, 1, @commandExecutionTime, @commandExecutionTime)
	end
end
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[DeleteWorkflowInstanceCommands]') and type in (N'P', N'PC'))
	drop procedure [System.Activities.DurableInstancing].[DeleteWorkflowInstanceCommands]
go

create procedure [System.Activities.DurableInstancing].[DeleteWorkflowInstanceCommands]
	@surrogateInstanceId bigint
AS
begin    
	set nocount on;
	set transaction isolation level read committed;
	set xact_abort on;
	
	delete from [CommandQueueTable]	
	where [SurrogateInstanceId] = @surrogateInstanceId	
end
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[CancelWorkflowInstanceCommand]') and type in (N'P', N'PC'))
	drop procedure [System.Activities.DurableInstancing].[CancelWorkflowInstanceCommand]
go

create procedure [System.Activities.DurableInstancing].[CancelWorkflowInstanceCommand]
	@commandId bigint
	--return 0 success, 1 - command not found or being executed
AS
begin    
	set nocount on;
	set transaction isolation level read committed;
	set xact_abort on;
	
	delete from [CommandQueueTable]	
	where [Id] = @commandId
		and (([LockOwner] is null) or ([LockExpiryTime] <= getutcdate()))
	
	if (@@rowcount = 0)
	begin
		return 1
	end
end
go

grant execute on [System.Activities.DurableInstancing].[CancelWorkflowInstanceCommand] to [System.Activities.DurableInstancing.ControlUsers]
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[DequeueWorkflowInstanceControlCommands]') and type in (N'P', N'PC'))
	drop procedure [System.Activities.DurableInstancing].[DequeueWorkflowInstanceControlCommands]
go

create procedure [System.Activities.DurableInstancing].[DequeueWorkflowInstanceControlCommands]
    @timeRange as int,
	@lockOwner as uniqueidentifier,
	@commandExectionTimeout as int,
	@batchSize as int
AS
begin

	set nocount on;
	set transaction isolation level repeatable read;
	set xact_abort on;
	set deadlock_priority low;
	
	declare @now as datetime
	declare @expiryData as datetime
	set @now = getutcdate()
	set @expiryData = dateadd(second, @timeRange, @now)
	
	update [System.Activities.DurableInstancing].[CommandQueueTable] with (readpast)
	set [LockOwner] = @lockOwner,		
		[LockExpiryTime] = 
		(
			case 
				when ([ExecutionTime] < @now) then dateadd(second, @commandExectionTimeout, @now)
				else dateadd(second, @commandExectionTimeout, [ExecutionTime])
			end
		)
	output inserted.[Id] [Id], readyCommands.[InstanceId] [InstanceId], inserted.[Type] [Type], inserted.[ExecutionTime] [ExecutionTime], inserted.[ExecutionAttemptsCount] [ExecutionAttemptsCount], readyCommands.[ControlEndpointAddress] [ControlEndpointAddress]
	from [System.Activities.DurableInstancing].[CommandQueueTable] commandQueue with (readpast)
		 inner join 
		 ( 		select top (@batchSize) commandQueue.[Id] [Id], instances.[Id] [InstanceId], serviceDeployments.[ControlEndpointAddress] [ControlEndpointAddress]
				from [System.Activities.DurableInstancing].[CommandQueueTable] commandQueue with (readpast)
					inner join 
				[System.Activities.DurableInstancing].[InstancesTable] instances with (readpast)
					on commandQueue.[SurrogateInstanceId] = instances.[SurrogateInstanceId]
				inner join 
				[System.Activities.DurableInstancing].[ServiceDeploymentTable] serviceDeployments with (readpast)
					on instances.[ServiceDeploymentId] =  serviceDeployments.[Id]
				left outer join 
				[System.Activities.DurableInstancing].[LockOwnersTable] lockOwners with (readpast)
					on 	instances.[SurrogateLockOwnerId] = lockOwners.[SurrogateLockOwnerId]
				where ([ExecutionTime] <= @expiryData)
						and (([LockOwner] is null) or ([LockExpiryTime] <= @now))
						and ((instances.[SurrogateLockOwnerId] is null) or (lockOwners.LockExpiration <= @now) or (lockOwners.[MachineName] = host_name()))
				order by [ExecutionTime]
		 ) readyCommands
		 on commandQueue.[Id] = readyCommands.[Id]
end
go

grant execute on [System.Activities.DurableInstancing].[DequeueWorkflowInstanceControlCommands] to [System.Activities.DurableInstancing.WorkflowManagementServiceUsers]
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[UnlockCommands]') and type in (N'P', N'PC'))
	drop procedure [System.Activities.DurableInstancing].[UnlockCommands]
go

create procedure [System.Activities.DurableInstancing].[UnlockCommands]
	@commands xml
as
begin
	set nocount on;
	set transaction isolation level read committed;
	set xact_abort on;
	set deadlock_priority low;
	
	declare @CommandsListTempTable table
	(
		Id bigint,
		Success bit,
		Exception nvarchar(max)
	)		
	
	insert into @CommandsListTempTable 
    select T.Item.value('@id', 'bigint'), T.Item.value('@success', 'bit'), T.Item.value('@exception', 'nvarchar(max)')
	from @commands.nodes('//commands/command') as T(Item)		
	
	update [CommandQueueTable]
	set [LockOwner] = null,
		[LockExpiryTime] = null
	from @CommandsListTempTable commandsToUnlock
			inner loop join 
		[CommandQueueTable] commandQueue
			on commandQueue.[Id] = commandsToUnlock.[Id]	
end

grant execute on [System.Activities.DurableInstancing].[UnlockCommands] to [System.Activities.DurableInstancing.WorkflowManagementServiceUsers]
go


if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[LogCommandsExecutionResults]') and type in (N'P', N'PC'))
	drop procedure [System.Activities.DurableInstancing].[LogCommandsExecutionResults]
go

create procedure [System.Activities.DurableInstancing].[LogCommandsExecutionResults]
	@commands xml
as
begin
	set nocount on;
	set transaction isolation level read committed;
	set xact_abort on;
	set deadlock_priority low;
	
	declare @now as datetime
	set @now = getutcdate()	
	
	DECLARE @CommandsListTempTable TABLE
	(
		Id bigint,
		Success bit,
		ExceptionCode nvarchar(100),
		Exception nvarchar(max)
	)		
    
    insert into @CommandsListTempTable 
    select T.Item.value('@id', 'bigint'), T.Item.value('@success', 'bit'), T.Item.value('@exceptionCode', 'nvarchar(100)'), T.Item.value('@exception', 'nvarchar(max)')
	from @commands.nodes('//commands/command') as T(Item)
	option (maxdop 1)	
	
	--Try to update existing log entries for commands
	update [CommandQueueExecutionLogTable]
	set [CommandId] = commandQueue.[Id],
		[OriginalExecutionTime] = commandQueue.[OriginalExecutionTime],		
		[ExecutionAttemptTime] = @now,
		[ExecutionAttemptsCount] = commandLog.[ExecutionAttemptsCount] + 1,
		[MachineName] = host_name(),
		[Exception] = commands.[Exception]
	from @CommandsListTempTable commands
			inner loop join 
		[CommandQueueTable] commandQueue
			on commandQueue.[Id] = commands.[Id]
		inner loop join [CommandQueueExecutionLogTable]	commandLog
			on commandQueue.[SurrogateInstanceId] = commandLog.[SurrogateInstanceId]
				and commandQueue.[Type] = commandLog.[Type]
				and commandQueue.[IsSystemCommand] = commandLog.[IsSystemCommand] 
				and (((commands.ExceptionCode is null) and  (commandLog.[ExceptionCode]is null)) 
					or (commands.ExceptionCode = commandLog.[ExceptionCode]))
	
	--Add new entries for commands 
	insert into [CommandQueueExecutionLogTable]
	select commandQueue.[Id], commandQueue.[SurrogateInstanceId], commandQueue.[Type], commandQueue.[IsSystemCommand], commandQueue.[OriginalExecutionTime], commands.[Success], @now, 1, host_name(), commands.[ExceptionCode], commands.[Exception]
	from @CommandsListTempTable commands inner join [CommandQueueTable] commandQueue
		on commandQueue.[Id] = commands.[Id]
	where ((commandQueue.[IsSystemCommand] = 1) and (commands.[Success] = 0))
		or (commandQueue.[IsSystemCommand] = 0)
end

go
if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[CompleteCommandsExecution]') and type in (N'P', N'PC'))
	drop procedure [System.Activities.DurableInstancing].[CompleteCommandsExecution]
go

create procedure [System.Activities.DurableInstancing].[CompleteCommandsExecution]
	@lockOwner uniqueidentifier,
	@commandsToDelete xml,
	@commandsToRetry xml,
	@commandsToUnlock xml,
	@retryCount as int,
	@pollingInterval as int
AS
begin
	set nocount on;
	set transaction isolation level read committed;
	set xact_abort on;
	set deadlock_priority low;
	
	declare @now as datetime
	set @now = getutcdate()	

    DECLARE @CommandsListTempTable TABLE
	(
		Id bigint
	)		
    
    insert into @CommandsListTempTable 
    select T.Item.value('@id', 'bigint')
	from @commandsToDelete.nodes('//commands/command') as T(Item)
	option (maxdop 1)	
    
    exec [System.Activities.DurableInstancing].[LogCommandsExecutionResults] @commandsToDelete
	
	delete from [CommandQueueTable]
	from @CommandsListTempTable commandsToDelete inner loop join 
		[System.Activities.DurableInstancing].[CommandQueueTable] commandQueue 	
		on commandQueue.[Id] = commandsToDelete.[Id]
	
	delete from @CommandsListTempTable
	
	insert into @CommandsListTempTable 
    select T.Item.value('@id', 'bigint')
	from @commandsToRetry.nodes('//commands/command') as T(Item)		
	option (maxdop 1)
	
	exec [System.Activities.DurableInstancing].[LogCommandsExecutionResults] @commandsToRetry
		
	update [CommandQueueTable]
	set [ExecutionAttemptsCount] = [ExecutionAttemptsCount] + 1,
		[LockOwner] = null,
		[LockExpiryTime] = null,
		[ExecutionTime] = dateadd(second, @pollingInterval  + 1, @now)
	from @CommandsListTempTable commandsToRetry
			inner loop join 
		[CommandQueueTable] commandQueue
			on commandQueue.[Id] = commandsToRetry.[Id]	
	where [ExecutionAttemptsCount] < @retryCount	
	
	delete from [CommandQueueTable]
	from @CommandsListTempTable  commandsToRetry
	inner loop join [CommandQueueTable] commandQueue 	
		on commandQueue.[Id] = commandsToRetry.[Id]
	where [ExecutionAttemptsCount] = @retryCount

    exec [System.Activities.DurableInstancing].[UnlockCommands] @commandsToUnlock
end
go

grant execute on [System.Activities.DurableInstancing].[CompleteCommandsExecution] to [System.Activities.DurableInstancing.WorkflowManagementServiceUsers]
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[InstanceRecoveryJob]') and type in (N'P', N'PC'))
	drop procedure [System.Activities.DurableInstancing].[InstanceRecoveryJob]
go

create procedure [System.Activities.DurableInstancing].[InstanceRecoveryJob]
	@runInterval as int,
	@commandExecutionLogPurgeTime as int
as
begin
	set nocount on;
	set transaction isolation level read committed;
	set xact_abort on;
	set deadlock_priority low;
    
    --This stored procedure does all the necessary cleaning in the sql instance store
    --Current clean up tasks :
	--      1       - Detect abandoned instances & place a run command in the command queue
	--      2       - Detect unloaded (ready to run) instances & place a run command in the command queue	
	--	    3		- Detect expired lock owners & delete them	
	--      4       - Detect instances with timers & place a timed run command in the command queue
	--      5       - Clean command execution log
	
	declare @now as datetime
	set @now = getutcdate()	
	
	update [InstanceRecoveryJobLastRunTable]
	set [LastRunTime] = @now
	where dateadd(second, @runInterval, [LastRunTime])  <= @now
	
	if (@@ROWCOUNT = 0)
	begin
		return
	end

	--Performing Task 1
	insert into [CommandQueueTable] ([SurrogateInstanceId], [Type], [OriginalExecutionTime], [ExecutionTime])
		select [SurrogateInstanceId], 1, @now, @now
		from [LockOwnersTable] lockOwners inner loop join
			 [InstancesTable] instances
				on instances.[SurrogateLockOwnerId] = lockOwners.[SurrogateLockOwnerId]
			where 
				lockOwners.LockExpiration <= @now
				and not exists
				(
					select TOP 1 1
					from [CommandQueueTable] commandQueue
					where commandQueue.[SurrogateInstanceId] = instances.[SurrogateInstanceId] 
						and commandQueue.[Type] = 1 --Run command
						and commandQueue.[OriginalExecutionTime] <= @now  
				)
			
	--Performing Task 2
	insert into [CommandQueueTable] ([SurrogateInstanceId], [Type], [OriginalExecutionTime], [ExecutionTime])
		select instances.[SurrogateInstanceId], 1, @now, @now
		from   [InstancesTable] instances
		where
				instances.[IsReadyToRun] = 1
				and not exists
				(
					select TOP 1 1
					from [CommandQueueTable] commandQueue
					where commandQueue.[SurrogateInstanceId] = instances.[SurrogateInstanceId] 
						and commandQueue.[Type] = 1 --Run command
						and commandQueue.[OriginalExecutionTime] <= @now  
				)
		
	--Perform Task 3
	delete from [LockOwnersTable]
	from [LockOwnersTable] lockOwners
	where [LockExpiration] <= @now
	and not exists
	(
		select top 1 1
		from [InstancesTable] instances
		where instances.[SurrogateLockOwnerId] = lockOwners.[SurrogateLockOwnerId]
	)
	
	--Task 4
	insert into [CommandQueueTable] ([SurrogateInstanceId], [Type], [OriginalExecutionTime], [ExecutionTime])
		select instances.[SurrogateInstanceId], 1, instances.[PendingTimer], instances.[PendingTimer]
		from   [InstancesTable] instances
		where
				instances.[PendingTimer] <= dateadd(second, @runInterval, @now)
				and not exists
				(
					select TOP 1 1
					from [CommandQueueTable] commandQueue
					where commandQueue.[SurrogateInstanceId] = instances.[SurrogateInstanceId] 
						and commandQueue.[Type] = 1 --Run command
						and commandQueue.[OriginalExecutionTime] <= [PendingTimer] 
				)
		
	--Task 5
	delete from [CommandQueueExecutionLogTable]
	where [ExecutionAttemptTime] < dateadd(second, - @commandExecutionLogPurgeTime, @now)
end
go

grant execute on [System.Activities.DurableInstancing].[InstanceRecoveryJob] to [System.Activities.DurableInstancing.WorkflowManagementServiceUsers]
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[ParsePropertyValue]') and type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
	drop function [System.Activities.DurableInstancing].[ParsePropertyValue]
go

create function [System.Activities.DurableInstancing].[ParsePropertyValue] (@propertyType tinyint, @propertyValue nvarchar(4000))
returns sql_variant
as
begin
	return case @propertyType 
					when 0 then cast (cast (@propertyValue as bigint) as sql_variant)
					when 1 then cast (@propertyValue as sql_variant)
					when 2 then cast (cast (@propertyValue as uniqueidentifier) as sql_variant)			
					when 3 then cast (cast (@propertyValue as datetime) as sql_variant)
					when 4 then cast (cast (@propertyValue as float) as sql_variant)
					when 5 then cast (cast (@propertyValue as bit) as sql_variant)
					when 6 then cast (cast (@propertyValue as int) as sql_variant)
					when 7 then cast (cast (@propertyValue as smallint) as sql_variant)
					when 8 then cast (cast (@propertyValue as tinyint) as sql_variant)
					when 9 then cast (cast (@propertyValue as float(24)) as sql_variant)
					when 10 then cast (cast (@propertyValue as decimal(38, 18)) as sql_variant)
					else null
				 end
end
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[ParseBinaryPropertyValue]') and type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
	drop function [System.Activities.DurableInstancing].[ParseBinaryPropertyValue]
go

create function [System.Activities.DurableInstancing].[ParseBinaryPropertyValue] (@startPosition int, @length int, @concatenatedBinaryBlob varbinary(max))
returns varbinary(max)
as
begin
	if (@length > 0)
		return substring(@concatenatedBinaryBlob, @startPosition + 1, @length)
	return null
end
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[ParsePropertyXmlBlob]',N'IF'))
	drop function [System.Activities.DurableInstancing].[ParsePropertyXmlBlob]
go
	
create function [System.Activities.DurableInstancing].[ParsePropertyXmlBlob] (@PropertyValues xml, @concatenatedBinaryBlob varbinary(max), @keysXmlBlob bit, @propertiesToRemove bit)
returns table
as
return (		
		select T.Item.value('@Name', 'nvarchar(450)') as [Name],
			   T.Item.value('@Namespace', 'nvarchar(450)') as [Namespace],
			   case @keysXmlBlob 
					when 1 then T.Item.value('@KeyId', 'uniqueidentifier')
					else null
			   end as [KeyId],
			   case @propertiesToRemove
					when 0 then 
						case 
							when T.Item.value('@WriteOnly', 'bit') is null
							then 0
							else 1
						end
					else null
			   end as [IsWriteOnly],
			   case @propertiesToRemove
					when 0 then [System.Activities.DurableInstancing].[ParsePropertyValue](T.Item.value('@Type', 'tinyint'), T.Item.value('@Value', 'nvarchar(4000)'))
					else null
			   end as [Value],
			   case @propertiesToRemove
					when 0 then [System.Activities.DurableInstancing].[ParseBinaryPropertyValue](T.Item.value('@StartPosition', 'int'), T.Item.value('@BinaryLength', 'int'), @concatenatedBinaryBlob)
					else null
			   end as [BinaryValue]
	   from @PropertyValues.nodes('/PropertyValues/PropertyValue') as T(Item)
)
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[InsertPropertyNames]') and type in (N'P', N'PC'))
	drop procedure [System.Activities.DurableInstancing].[InsertPropertyNames]
go
create procedure [System.Activities.DurableInstancing].[InsertPropertyNames]
	@propertyXmlBlob xml
as
begin
	declare @propertyNames table([Name] nvarchar(450), [Namespace] nvarchar(450))
	
	insert into @propertyNames
	select T.Item.value('@Name', 'nvarchar(450)'), T.Item.value('@Namespace', 'nvarchar(450)')
	from @propertyXmlBlob.nodes('/PropertyValues/PropertyValue') as T(Item)
	where T.Item.value('@Name', 'nvarchar(450)') is not null
	option (maxdop 1)

	if (exists(
			select [PropertyNames].[Name], [PropertyNames].[Namespace] 
			from @propertyNames as [PropertyNames]
			except
			select [StringsTable1].[String], [StringsTable2].[String]
			from [PropertyNamesTable]
			join [StringsTable] as [StringsTable1] on [StringsTable1].[Id] = [PropertyNamesTable].[PropertyNameId]
			join [StringsTable] as [StringsTable2] on [StringsTable2].[Id] = [PropertyNamesTable].[PropertyNamespaceId])
	)
	begin
		insert into [StringsTable] ([String])
		select [PropertyNames].[Name]
		from @propertyNames as [PropertyNames]
		
		insert into [StringsTable] ([String])
		select [PropertyNames].[Namespace]
		from @propertyNames as [PropertyNames]
		
		insert into [PropertyNamesTable] ([PropertyNameId], [PropertyNamespaceId])
		select [StringsTable1].[Id], [StringsTable2].[Id]
		from @propertyNames as [PropertyNames]
		join [StringsTable] as [StringsTable1] on [StringsTable1].[String] = [PropertyNames].[Name]
		join [StringsTable] as [StringsTable2] on [StringsTable2].[String] = [PropertyNames].[Namespace]
	end
end
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[CreateLockOwner]') and type in (N'P', N'PC'))
	drop procedure [System.Activities.DurableInstancing].[CreateLockOwner]
go

create procedure [System.Activities.DurableInstancing].[CreateLockOwner]
	@lockOwnerId uniqueidentifier,
	@lockTimeout int,
	@enqueueCommand bit,
	@deleteInstanceOnCompletion bit,
	@serviceDeploymentHash varbinary(32),
	@siteName nvarchar(450),
	@relativeServicePath nvarchar(450),
	@relativeApplicationPath nvarchar(450),
	@serviceName nvarchar(450),
	@serviceNamespace nvarchar(450),
	@controlEndpointAddress nvarchar(450),
	@primitiveLockOwnerData varbinary(max),
	@complexLockOwnerData varbinary(max),
	@writeOnlyPrimitiveLockOwnerData varbinary(max),
	@writeOnlyComplexLockOwnerData varbinary(max),
	@serializationMethod tinyint
as
begin
	set nocount on
	set transaction isolation level read committed
	begin transaction
	
	declare @lockAcquired bigint
	declare @lockExpiration datetime
	declare @machineName nvarchar(128)
	declare @now datetime
	declare @result int
	declare @surrogateLockOwnerId bigint
	declare @serviceDeploymentId bigint
	
	set @result = 0
	set @machineName = host_name()
	
	exec @lockAcquired = sp_getapplock @Resource = 'InstanceStoreLock', @LockMode = 'Shared', @LockTimeout = 10000
		
	if (@lockAcquired < 0)
	begin
		select @result as 'Result'
		set @result = 13
	end
	
	if (@result = 0)
	begin
		set @now = getutcdate()
		
		if (@lockTimeout = 0)
			set @lockExpiration = '9999-12-31T23:59:59';
		else 
			set @lockExpiration = dateadd(second, @lockTimeout, getutcdate());

		insert into [ServiceDeploymentTable] ([ServiceDeploymentHash], [SiteName], [RelativeServicePath], [RelativeApplicationPath], [ServiceName], [ServiceNamespace], [ControlEndpointAddress])
		values (@serviceDeploymentHash, @siteName, @relativeServicePath, @relativeApplicationPath, @serviceName, @serviceNamespace, @controlEndpointAddress)
		
		if (@@rowcount = 1)
			set @serviceDeploymentId = scope_identity()
		else
		begin
			select @serviceDeploymentId = [Id]
			from [ServiceDeploymentTable]
			where [ServiceDeploymentHash] = @serviceDeploymentHash
		end
		
		insert into [LockOwnersTable] ([Id], [LockExpiration], [MachineName], [ServiceDeploymentId], [EnqueueCommand], [DeletesInstanceOnCompletion], [PrimitiveLockOwnerData], [ComplexLockOwnerData], [WriteOnlyPrimitiveLockOwnerData], [WriteOnlyComplexLockOwnerData], [SerializationMethod])
		values (@lockOwnerId, @lockExpiration, @machineName, @serviceDeploymentId, @enqueueCommand, @deleteInstanceOnCompletion, @primitiveLockOwnerData, @complexLockOwnerData, @writeOnlyPrimitiveLockOwnerData, @writeOnlyComplexLockOwnerData, @serializationMethod)
		
		if (@@rowcount = 0)
		begin
			set @result = 15
			select @result as 'Result'
		end

		if (@result = 0)
			set @surrogateLockOwnerId = scope_identity()
	end
	
	if (@result != 13)
		exec sp_releaseapplock @Resource = 'InstanceStoreLock'
	
	if (@result = 0)
	begin
		commit transaction
		select 0 as 'Result', @surrogateLockOwnerId
	end
	else
		rollback transaction
end
go

grant execute on [System.Activities.DurableInstancing].[CreateLockOwner] to [System.Activities.DurableInstancing.InstanceStoreUsers] 
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[DeleteLockOwner]') and type in (N'P', N'PC'))
	drop procedure [System.Activities.DurableInstancing].[DeleteLockOwner]
go

create procedure [System.Activities.DurableInstancing].[DeleteLockOwner]
	@surrogateLockOwnerId bigint
as
begin
	set nocount on
	set transaction isolation level read committed
	set deadlock_priority low
	begin transaction
	
	declare @lockAcquired bigint
	declare @result int
	set @result = 0
	
	exec @lockAcquired = sp_getapplock @Resource = 'InstanceStoreLock', @LockMode = 'Shared', @LockTimeout = 10000
		
	if (@lockAcquired < 0)
	begin
		select @result as 'Result'
		set @result = 13
	end
	
	if (@result = 0)
	begin
		update [InstancesTable]
		set	[SurrogateLockOwnerId] = null
		where [SurrogateLockOwnerId] = @surrogateLockOwnerId
		
		delete from [LockOwnersTable]
		where [SurrogateLockOwnerId] = @surrogateLockOwnerId
	end
	
	if (@result != 13)
		exec sp_releaseapplock @Resource = 'InstanceStoreLock' 
	
	if (@result = 0)
	begin
		commit transaction
		select 0 as 'Result'
	end
	else
		rollback transaction
end
go

grant execute on [System.Activities.DurableInstancing].[DeleteLockOwner] to [System.Activities.DurableInstancing.InstanceStoreUsers] 
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[ExtendLock]') and type in (N'P', N'PC'))
	drop procedure [System.Activities.DurableInstancing].[ExtendLock]
go
create procedure [System.Activities.DurableInstancing].[ExtendLock]
	@surrogateLockOwnerId bigint,
	@lockTimeout int	
as
begin
	set nocount on
	set transaction isolation level read committed
	begin transaction	
	
	declare @now datetime
	declare @newLockExpiration datetime
	declare @result int
	
	set @now = getutcdate()
	set @result = 0
	
	if (@lockTimeout = 0)
		set @newLockExpiration = '9999-12-31T23:59:59'
	else
		set @newLockExpiration = dateadd(second, @lockTimeout, @now)
	
	update [LockOwnersTable]
	set [LockExpiration] = @newLockExpiration
	where ([SurrogateLockOwnerId] = @surrogateLockOwnerId) and
		  ([LockExpiration] > @now)
	
	if (@@rowcount = 0) 
	begin
		if exists (select * from [LockOwnersTable] where ([SurrogateLockOwnerId] = @surrogateLockOwnerId))
		begin
			exec [System.Activities.DurableInstancing].[DeleteLockOwner] @surrogateLockOwnerId
			set @result = 11
		end
		else
			set @result = 12
	end
	
	select @result as 'Result'
	commit transaction
end
go

grant execute on [System.Activities.DurableInstancing].[ExtendLock] to [System.Activities.DurableInstancing.InstanceStoreUsers] 
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[AssociateKeys]') and type in (N'P', N'PC'))
	drop procedure [System.Activities.DurableInstancing].[AssociateKeys]
go
create procedure [System.Activities.DurableInstancing].[AssociateKeys]
	@surrogateInstanceId bigint,
	@keysToAssociate xml = null,
	@concatenatedBinaryBlob varbinary(max) = null
as
begin	
	declare @badKeyId uniqueidentifier
	declare @numberOfKeys int
	declare @result int
	declare @keyIds table([KeyId] uniqueidentifier)
	declare @keyProperties table([KeyId] uniqueidentifier,
								 [Name] nvarchar(450),
								 [Namespace] nvarchar(450),
								 [Value] sql_variant,
								 [BinaryValue] varbinary(max),
								 [IsWriteOnly] bit
								 )
	
	set @result = 0
	
	if (@keysToAssociate is not null)
	begin
		insert into @keyProperties
		select [KeyId], [Name], [Namespace], [Value], [BinaryValue], [IsWriteOnly]
		from [System.Activities.DurableInstancing].[ParsePropertyXmlBlob](@keysToAssociate, @concatenatedBinaryBlob, 1, 0)
		option (maxdop 1)

		insert into @keyIds
		select distinct [KeyId]
		from @keyProperties
		
		select @numberOfKeys = count(1) from @keyIds
		
		insert into [KeysTable] ([Id], [SurrogateInstanceId], [IsAssociated])
		select [KeyIds].[KeyId], @surrogateInstanceId, 1
		from @keyIds as [KeyIds]
		
		if (@@rowcount != @numberOfKeys)
		begin
			select top 1 @badKeyId = [KeysTable].[Id] 
			from @keyIds as [KeyIds]
			join [KeysTable] on [KeyIds].[KeyId] = [KeysTable].[Id]
			where [KeysTable].[SurrogateInstanceId] != @surrogateInstanceId
			
			if (@@rowcount != 0)
			begin
				select 3 as 'Result', @badKeyId
				return 3
			end
		end
		
		exec [System.Activities.DurableInstancing].[InsertPropertyNames] @keysToAssociate
		
		insert into [KeyPropertiesTable] ([SurrogateKeyId], [PropertyNameId], [PropertyBinaryValue], [PropertyValue], [IsWriteOnly])
		select [KeysTable].[SurrogateKeyId], [PropertyNamesTable].[Id], [KeyProperties].[BinaryValue], [KeyProperties].[Value], [KeyProperties].[IsWriteOnly]
		from @keyProperties as [KeyProperties]
		join [KeysTable] with (readpast) on [KeysTable].[Id] = [KeyProperties].[KeyId]
		join [StringsTable] as [StringsTable1] on [KeyProperties].[Name] = [StringsTable1].[String]
		join [StringsTable] as [StringsTable2] on [KeyProperties].[Namespace] = [StringsTable2].[String]
		join [PropertyNamesTable] on ([StringsTable1].[Id] = [PropertyNamesTable].[PropertyNameId] and
									  [StringsTable2].[Id] = [PropertyNamesTable].[PropertyNamespaceId])
	end
end
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[CompleteKeys]') and type in (N'P', N'PC'))
	drop procedure [System.Activities.DurableInstancing].[CompleteKeys]
go
create procedure [System.Activities.DurableInstancing].[CompleteKeys]
	@surrogateInstanceId bigint,
	@keysToComplete xml = null
as
begin	
	declare @badKeyId uniqueidentifier
	declare @numberOfKeys int
	declare @result int
	declare @keyIds table([KeyId] uniqueidentifier)
	
	set @result = 0
	
	if (@keysToComplete is not null)
	begin
		insert into @keyIds
		select T.Item.value('@Id', 'uniqueidentifier')
		from @keysToComplete.nodes('//CorrelationKey') as T(Item)
		option(maxdop 1)
		
		select @numberOfKeys = count(1) from @keyIds
		
		update [KeysTable]
		set [IsAssociated] = 0
		from @keyIds as [KeyIds]
		join [KeysTable] on [KeyIds].[KeyId] = [KeysTable].[Id]
		where [SurrogateInstanceId] = @surrogateInstanceId
		
		if (@@rowcount != @numberOfKeys)
		begin
			select top 1 @badKeyId = [MissingKeys].[MissingKeyId]
			from
				(select [KeyIds].[KeyId] as [MissingKeyId] 
				 from @keyIds as [KeyIds]
				 except
				 select [Id] from [KeysTable] where [SurrogateInstanceId] = @surrogateInstanceId) as MissingKeys
		
			select 4 as 'Result', @badKeyId
			return 4
		end
	end
end
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[FreeKeys]') and type in (N'P', N'PC'))
	drop procedure [System.Activities.DurableInstancing].[FreeKeys]
go
create procedure [System.Activities.DurableInstancing].[FreeKeys]
	@surrogateInstanceId bigint,
	@keysToFree xml = null
as
begin	
	declare @badKeyId uniqueidentifier
	declare @numberOfKeys int
	declare @result int
	declare @keyIds table([KeyId] uniqueidentifier)
	
	set @result = 0
	
	if (@keysToFree is not null)
	begin
		insert into @keyIds
		select T.Item.value('@Id', 'uniqueidentifier')
		from @keysToFree.nodes('//CorrelationKey') as T(Item)
		option(maxdop 1)
		
		select @numberOfKeys = count(1) from @keyIds
		
		delete [KeyPropertiesTable]
		from @keyIds as [KeyIds]
		join [KeysTable] on [KeysTable].[Id] = [KeyIds].[KeyId]
		join [KeyPropertiesTable] on [KeyPropertiesTable].[SurrogateKeyId] = [KeysTable].[SurrogateKeyId]
		
		delete [KeysTable]
		from @keyIds as [KeyIds]
		join [KeysTable] on [KeyIds].[KeyId] = [KeysTable].[Id]
		where [SurrogateInstanceId] = @surrogateInstanceId

		if (@@rowcount != @numberOfKeys)
		begin
			select top 1 @badKeyId = [MissingKeys].[MissingKeyId] from
				(select [KeyIds].[KeyId] as [MissingKeyId]
				 from @keyIds as [KeyIds]
				 except
				 select [Id] from [KeysTable] where [SurrogateInstanceId] = @surrogateInstanceId) as MissingKeys
		
			select 4 as 'Result', @badKeyId
			return 4
		end
	end
end
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[UpdateKeyProperties]') and type in (N'P', N'PC'))
	drop procedure [System.Activities.DurableInstancing].[UpdateKeyProperties]
go
create procedure [System.Activities.DurableInstancing].[UpdateKeyProperties]
	@surrogateInstanceId bigint,
	@keyPropertiesToAdd xml,
	@keyPropertiesToRemove xml,
	@concatenatedBinaryBlob varbinary(max)
as
begin
	declare @badKeyId uniqueidentifier
	
	if (@keyPropertiesToAdd is not null)
	begin
		exec [System.Activities.DurableInstancing].[InsertPropertyNames] @keyPropertiesToAdd
		
		declare @keyProperties table([PropertyNameId] bigint,
									 [KeyId] uniqueidentifier,
									 [Value] sql_variant,
									 [BinaryValue] varbinary(max),
									 [IsWriteOnly] bit
									)
										 
		insert into @keyProperties
		select [PropertyNamesTable].[Id], [KeyId], [Value], [BinaryValue], [IsWriteOnly]
		from [System.Activities.DurableInstancing].[ParsePropertyXmlBlob](@keyPropertiesToAdd, @concatenatedBinaryBlob, 1, 0) as [KeyProperties]
		join [StringsTable] as [StringsTable1] on [KeyProperties].[Name] = [StringsTable1].[String]
		join [StringsTable] as [StringsTable2] on [KeyProperties].[Namespace] = [StringsTable2].[String]
		join [PropertyNamesTable] on ([StringsTable1].[Id] = [PropertyNamesTable].[PropertyNameId] and
									  [StringsTable2].[Id] = [PropertyNamesTable].[PropertyNamespaceId])
		option(maxdop 1)
		
		select top 1 @badKeyId = [MissingKeys].[MissingKeyId]
		from
			(select distinct [KeyId] as [MissingKeyId]
		     from @keyProperties as [KeyProperties]
			 except
			 select [Id] from [KeysTable]
			 where [SurrogateInstanceId] = @surrogateInstanceId) as MissingKeys
		
		if (@@rowcount != 0)
		begin
			select 4 as 'Result', @badKeyId
			return 4
		end
		
		insert into [KeyPropertiesTable] ([SurrogateKeyId], [PropertyNameId])
		select [KeysTable].[SurrogateKeyId], [KeyProperties].[PropertyNameId]
		from @keyProperties as [KeyProperties]
		join [KeysTable] on [KeysTable].[Id] = [KeyProperties].[KeyId]
		
		update [KeyPropertiesTable]
		set [PropertyValue] = [KeyProperties].[Value],
			[PropertyBinaryValue] = [KeyProperties].[BinaryValue],
			[IsWriteOnly] = [KeyProperties].[IsWriteOnly]
		from @keyProperties as [KeyProperties]
		join [KeysTable] on [KeysTable].[Id] = [KeyProperties].[KeyId]
		join [KeyPropertiesTable] on ([KeyPropertiesTable].[PropertyNameId] = [KeyProperties].[PropertyNameId] and
									  [KeyPropertiesTable].[SurrogateKeyId] = [KeysTable].[SurrogateKeyId])
	end
	
	if (@keyPropertiesToRemove is not null)
	begin
		declare @deleteKeyProperties table([PropertyNameId] bigint,
									       [KeyId] uniqueidentifier
									       )
		
		insert into @deleteKeyProperties
		select [PropertyNamesTable].[Id], [KeyId]
		from [System.Activities.DurableInstancing].[ParsePropertyXmlBlob](@keyPropertiesToRemove, @concatenatedBinaryBlob, 1, 1) as [KeyProperties]
		join [StringsTable] as [StringsTable1] on [KeyProperties].[Name] = [StringsTable1].[String]
		join [StringsTable] as [StringsTable2] on [KeyProperties].[Namespace] = [StringsTable2].[String]
		join [PropertyNamesTable] on ([StringsTable1].[Id] = [PropertyNamesTable].[PropertyNameId] and
									  [StringsTable2].[Id] = [PropertyNamesTable].[PropertyNamespaceId])
		option(maxdop 1)
		
		select top 1 @badKeyId = [MissingKeys].[MissingKeyId]
		from
			(select distinct [KeyId] as [MissingKeyId]
		     from @deleteKeyProperties as [KeyProperties]
			 except
			 select [Id] from [KeysTable]
			 where [SurrogateInstanceId] = @surrogateInstanceId) as MissingKeys
		
		if (@@rowcount != 0)
		begin
			select 4 as 'Result', @badKeyId
			return 4
		end
		
		delete [KeyPropertiesTable]
		from @deleteKeyProperties as [KeyProperties]
		join [KeysTable] on [KeysTable].[Id] = [KeyProperties].[KeyId]
		join [KeyPropertiesTable] on ([KeyPropertiesTable].[PropertyNameId] = [KeyProperties].[PropertyNameId] and
									  [KeyPropertiesTable].[SurrogateKeyId] = [KeysTable].[SurrogateKeyId])
	end
end
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[CreateInstance]') and type in (N'P', N'PC'))
	drop procedure [System.Activities.DurableInstancing].[CreateInstance]
go
create procedure [System.Activities.DurableInstancing].[CreateInstance]
	@instanceId uniqueidentifier,
	@surrogateLockOwnerId bigint,
	@result int output
as
begin
	declare @surrogateInstanceId bigint
	set @surrogateInstanceId = 0
	set @result = 0
	
	begin try			
		insert into [InstancesTable] ([Id], [SurrogateLockOwnerId], [CreationTime], [ServiceDeploymentId], [Version])
		select @instanceId, @surrogateLockOwnerId, getutcdate(), [ServiceDeploymentId], 1
		from [LockOwnersTable]
		where ([SurrogateLockOwnerId] = @surrogateLockOwnerId)
		
		set @surrogateInstanceId = scope_identity()
	end try
	begin catch
		if (error_number() != 2601)
		begin
			set @result = 99
			select @result as 'Result'
		end
	end catch
	
	return @surrogateInstanceId
end
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[LockInstance]') and type in (N'P', N'PC'))
	drop procedure [System.Activities.DurableInstancing].[LockInstance]
go
create procedure [System.Activities.DurableInstancing].[LockInstance]
	@instanceId uniqueidentifier,
	@surrogateLockOwnerId bigint,
	@expectedInstanceVersion bigint,
	@isNotExpectedToBeLocked bit,
	@isLoadCommand bit,
	@result int output
as
begin
	declare @isCompleted bit
	declare @surrogateInstanceId bigint
	declare @currentLockOwnerId bigint
	declare @currentVersion bigint

TryLockInstance:
	set @currentLockOwnerId = 0
	set @surrogateInstanceId = 0
	set @result = 0
	
	update [InstancesTable]
	set [SurrogateLockOwnerId] = @surrogateLockOwnerId,
		[Version] = case when ([InstancesTable].[SurrogateLockOwnerId] is null or 
							   [InstancesTable].[SurrogateLockOwnerId] != @surrogateLockOwnerId)
						 then [Version] + 1
						 else [Version]
					end,
		@surrogateInstanceId = [SurrogateInstanceId]
	from [InstancesTable]
	left outer join [LockOwnersTable] on [InstancesTable].[SurrogateLockOwnerId] = [LockOwnersTable].[SurrogateLockOwnerId]
	where ([InstancesTable].[Id] = @instanceId) and
		  ([InstancesTable].[IsCompleted] = 0) and
		  (
		   (@isNotExpectedToBeLocked = 1 and
		    (
		     ([InstancesTable].[SurrogateLockOwnerId] is null) or
		     ([LockOwnersTable].[SurrogateLockOwnerId] is null) or
		     ([LockOwnersTable].[LockExpiration] < getutcdate())
		    )
		   ) or 
		   (
		    ([LockOwnersTable].[SurrogateLockOwnerId] = @surrogateLockOwnerId) and
		    ([LockOwnersTable].[LockExpiration] > getutcdate()) and
			(@isLoadCommand = 0) and
		    (
		     (@isNotExpectedToBeLocked = 1) or
		     ([InstancesTable].[Version] = @expectedInstanceVersion)
		    )
		   )
		  )
	
	if (@@rowcount = 0)
	begin
		if not exists (select * from [LockOwnersTable] where ([SurrogateLockOwnerId] = @surrogateLockOwnerId) and ([LockExpiration] > getutcdate()))
		begin
			if exists (select * from [LockOwnersTable] where [SurrogateLockOwnerId] = @surrogateLockOwnerId)
				set @result = 11
			else
				set @result = 12
			
			select @result as 'Result'
			return 0
		end
		
		select @currentLockOwnerId = [SurrogateLockOwnerId],
			   @isCompleted = [IsCompleted],
			   @currentVersion = [Version]
		from [InstancesTable]
		where [Id] = @instanceId
	
		if (@@rowcount = 1)
		begin
			if (@isCompleted = 1)
				set @result = 7
			else if (@currentLockOwnerId = @surrogateLockOwnerId)
			begin
				if (@isLoadCommand = 0)
					set @result = 10
				else
					set @result = 14
			end
			else if (@isNotExpectedToBeLocked = 1)
				set @result = 2
			else
				set @result = 6
		end
		else if (@@rowcount = 0 and @isNotExpectedToBeLocked = 0)
			set @result = 6
	end

	if (@result != 0 and @result != 2)
		select @result as 'Result', @instanceId, @currentVersion
	else if (@result = 2)
	begin
		select @result as 'Result', @instanceId, [LockOwnersTable].[Id], [LockOwnersTable].[SerializationMethod], [PrimitiveLockOwnerData], [ComplexLockOwnerData]
		from [LockOwnersTable]
		join [InstancesTable] on [InstancesTable].[SurrogateLockOwnerId] = [LockOwnersTable].[SurrogateLockOwnerId]
		where [InstancesTable].[SurrogateLockOwnerId] = @currentLockOwnerId and
			  [InstancesTable].[Id] = @instanceId
		
		if (@@rowcount = 0)
			goto TryLockInstance
	end
	
	return @surrogateInstanceId
end
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[UnlockInstance]') and type in (N'P', N'PC'))
	drop procedure [System.Activities.DurableInstancing].[UnlockInstance]
go
create procedure [System.Activities.DurableInstancing].[UnlockInstance]
	@surrogateLockOwnerId bigint,
	@instanceId uniqueidentifier
as
begin
	set nocount on
	set transaction isolation level read committed
	
	begin transaction
	
	declare @result int
	declare @surrogateInstanceId bigint
	
	exec @surrogateInstanceId = [System.Activities.DurableInstancing].[LockInstance] @instanceId, @surrogateLockOwnerId, -1, 1, 0, @result output
	
	if (@surrogateInstanceId > 0 and @result = 0)
	begin
		update [InstancesTable]
		set [SurrogateLockOwnerId] = null
		where [Id] = @instanceId
	end
	
	if (@result = 0)
	begin
		commit transaction
		select @result as 'Result'
	end
	else
		rollback transaction
end
go

grant execute on [System.Activities.DurableInstancing].[UnlockInstance] to [System.Activities.DurableInstancing.InstanceStoreUsers] 
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[LoadInstance]') and type in (N'P', N'PC'))
	drop procedure [System.Activities.DurableInstancing].[LoadInstance]
go
create procedure [System.Activities.DurableInstancing].[LoadInstance]
	@surrogateLockOwnerId bigint,
	@operationType tinyint,
	@version bigint,
	@keyToLoadBy uniqueidentifier = null,
	@instanceId uniqueidentifier = null,
	@keysToAssociate xml = null,
	@concatenatedBinaryBlob varbinary(max) = null,
	@operationTimeout int
as
begin
	set nocount on
	set transaction isolation level read committed
	set deadlock_priority low
	begin transaction
	
	declare @lockAcquired bigint
	declare @isInitialized bit
	declare @createKey bit
	declare @createdInstance bit
	declare @keyIsAssociated bit
	declare @loadedByKey bit
	declare @now datetime
	declare @result int
	declare @surrogateInstanceId bigint

	set @createdInstance = 0
	set @isInitialized = 0
	set @keyIsAssociated = 0
	set @result = 0
	set @surrogateInstanceId = null
	
	exec @lockAcquired = sp_getapplock @Resource = 'InstanceStoreLock', @LockMode = 'Shared', @LockTimeout = @operationTimeout
	
	if (@lockAcquired < 0)
	begin
		set @result = 13
		select @result as 'Result'
	end
	
	set @now = getutcdate()
	
	if (@operationType = 0) or (@operationType = 2)
	begin
MapKeyToInstanceId:
		set @loadedByKey = 0
		set @createKey = 0
		
		select @instanceId = [InstancesTable].[Id],
			   @keyIsAssociated = [IsAssociated]
		from [KeysTable]
		join [InstancesTable] on [KeysTable].[SurrogateInstanceId] = [InstancesTable].[SurrogateInstanceId]
		where [KeysTable].[Id] = @keyToLoadBy
		
		if (@@rowcount = 0)
		begin
			if (@operationType = 2)
			begin
				set @result = 4
				select @result as 'Result', @keyToLoadBy 
			end
				set @createKey = 1
		end
		else if (@keyIsAssociated = 0)
		begin
			set @result = 8
			select @result as 'Result', @keyToLoadBy
		end
		else
			set @loadedByKey = 1
	end

	if (@result = 0)
	begin
LockOrCreateInstance:
		exec @surrogateInstanceId = [System.Activities.DurableInstancing].[LockInstance] @instanceId, @surrogateLockOwnerId, @version, 1, 1, @result output
														  
		if (@result = 0 and @surrogateInstanceId = 0)
		begin
			if (@loadedByKey = 1)
				goto MapKeyToInstanceId
			
			if (@operationType > 1)
			begin
				set @result = 1
				select @result as 'Result', @instanceId as 'InstanceId'
			end
			else
			begin
				exec @surrogateInstanceId = [System.Activities.DurableInstancing].[CreateInstance] @instanceId, @surrogateLockOwnerId, @result output					
			
				if (@result = 0 and @surrogateInstanceId = 0)
					goto LockOrCreateInstance
				else if (@surrogateInstanceId > 0)
					set @createdInstance = 1
			end
		end
	end
		
	if (@result = 0)
	begin
		if (@createKey = 1) 
		begin
			select @isInitialized = [IsInitialized]
			from [InstancesTable]
			where [SurrogateInstanceId] = @surrogateInstanceId
			
			if (@isInitialized = 1)
			begin
				set @result = 5
				select @result as 'Result', @instanceId
			end
			else
			begin													
				insert into [KeysTable] ([Id], [SurrogateInstanceId], [IsAssociated])
				values (@keyToLoadBy, @surrogateInstanceId, 1)
				
				if (@@rowcount = 0)
				begin
					if (@createdInstance = 1)
					begin
						delete [InstancesTable]
						where [SurrogateInstanceId] = @surrogateInstanceId
					end
					else
					begin
						update [InstancesTable]
						set [SurrogateLockOwnerId] = null
						where [SurrogateInstanceId] = @surrogateInstanceId
					end
					
					goto MapKeyToInstanceId
				end
			end
		end
		else if (@loadedByKey = 1 and not exists(select [Id] from [KeysTable] where ([Id] = @keyToLoadBy) and ([IsAssociated] = 1)))
		begin
			set @result = 8
			select @result as 'Result', @keyToLoadBy
		end
		
		if (@operationType > 1 and not exists(select [Id] from [InstancesTable] where ([Id] = @instanceId) and ([IsInitialized] = 1)))
		begin
			set @result = 1
			select @result as 'Result', @instanceId as 'InstanceId'
		end
		
		if (@result = 0)
			exec @result = [System.Activities.DurableInstancing].[AssociateKeys] @surrogateInstanceId, @keysToAssociate, @concatenatedBinaryBlob
	end
	
	if (@result != 13)
		exec sp_releaseapplock @Resource = 'InstanceStoreLock'
		
	if (@result = 0)
	begin
		select @result as 'Result',
			   [Id],
			   [SurrogateInstanceId],
			   [PrimitiveDataProperties],
			   [ComplexDataProperties],
			   [MetadataProperties],
			   [SerializationMethod],
			   [Version],
			   [IsInitialized],
			   @createdInstance
		from [InstancesTable]
		where [SurrogateInstanceId] = @surrogateInstanceId
		
		if (@createdInstance = 0)
		begin				  
			select 0 as 'Result',
				   [KeysTable].[Id],
				   [KeysTable].[IsAssociated],
				   [StringsTable1].[String],
				   [StringsTable2].[String],
				   [KeyPropertiesTable].[PropertyValue],
				   [KeyPropertiesTable].[PropertyBinaryValue]
			from [KeysTable]
			left outer join [KeyPropertiesTable] on [KeyPropertiesTable].[SurrogateKeyId] = [KeysTable].[SurrogateKeyId]
			left outer join [PropertyNamesTable] on [KeyPropertiesTable].[PropertyNameId] = [PropertyNamesTable].[Id]
			left outer join [StringsTable] as [StringsTable1] on [StringsTable1].[Id] = [PropertyNamesTable].[PropertyNameId]
			left outer join [StringsTable] as [StringsTable2] on [StringsTable2].[Id] = [PropertyNamesTable].[PropertyNamespaceId]
			where ([KeysTable].[SurrogateInstanceId] = @surrogateInstanceId) and
				  ([IsWriteOnly] = 0 or [IsWriteOnly] is null)
			
			if (@@rowcount = 0)
				select 0 as 'Result', null, null, null, null, null, null
		end

		commit transaction
	end
	else if (@result = 2)
		commit transaction
	else
		rollback transaction
end
go

grant execute on [System.Activities.DurableInstancing].[LoadInstance] to [System.Activities.DurableInstancing.InstanceStoreUsers] 
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[DeleteInstance]') and type in (N'P', N'PC'))
	drop procedure [System.Activities.DurableInstancing].[DeleteInstance]
go
create procedure [System.Activities.DurableInstancing].[DeleteInstance]
	@surrogateInstanceId bigint = null,
	@instanceId uniqueidentifier = null,
	@operationTimeout int = 10000
as
begin
	set nocount on
	set transaction isolation level read committed
	begin transaction

	declare @externallyCalled bit	
	declare @lockAcquired bigint
	declare @result int
	
	set @externallyCalled = 0
	set @result = 0
	
	if (@surrogateInstanceId is null)
	begin
		set @externallyCalled = 1
		
		-- We were not called via the SaveInstance stored procedure; therefore, we need to take an application
		-- lock on the store.
		exec @lockAcquired = sp_getapplock @Resource = 'InstanceStoreLock', @LockMode = 'Shared', @LockTimeout = @operationTimeout

		if (@lockAcquired < 0)
		begin
			set @result = 13
			select @result as 'Result'
		end
	
		if (@result = 0)
		begin
			select @surrogateInstanceId = [SurrogateInstanceId]
			from [InstancesTable]
			where [Id] = @instanceId
		end
		
		if (@@rowcount = 0)
		begin
			set @result = 1
			select @result as 'Result', @instanceId
		end
	end
	
	if (@result = 0)
	begin
		delete [InstancePromotedPropertiesTable]
		where [SurrogateInstanceId] = @surrogateInstanceId
		
		delete [KeyPropertiesTable]
		from [KeysTable]
		join [KeyPropertiesTable] on [KeysTable].[SurrogateKeyId] = [KeyPropertiesTable].[SurrogateKeyId]
		where [KeysTable].[SurrogateInstanceId] = @surrogateInstanceId
		
		delete [KeysTable]
		where [SurrogateInstanceId] = @surrogateInstanceId
	
		delete [InstancesTable] 
		where [SurrogateInstanceId] = @surrogateInstanceId
		exec [System.Activities.DurableInstancing].[DeleteWorkflowInstanceCommands] @surrogateInstanceId
	end
	
	if (@result != 13 and @externallyCalled = 1)
		exec sp_releaseapplock @Resource = 'InstanceStoreLock'
		
	commit transaction
end
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[SaveInstance]') and type in (N'P', N'PC'))
	drop procedure [System.Activities.DurableInstancing].[SaveInstance]
go
create procedure [System.Activities.DurableInstancing].[SaveInstance]
	@instanceId uniqueidentifier,
	@surrogateLockOwnerId bigint,
	@instanceVersion bigint,
	@primitiveDataProperties varbinary(max),
	@complexDataProperties varbinary(max),
	@writeOnlyPrimitiveDataProperties varbinary(max),
	@writeOnlyComplexDataProperties varbinary(max),
	@metadataProperties varbinary(max),
	@serializationMethod tinyint,
	@pendingTimer datetime,
	@suspensionStateChange tinyint,
	@suspensionReason nvarchar(450),
	@keysToAssociate xml,
	@keysToComplete xml,
	@keysToFree xml,
	@keyPropertiesToAdd xml,
	@keyPropertiesToRemove xml,
	@concatenatedBinaryBlob varbinary(max),
	@isNotExpectedToBeLocked bit,
	@unlockInstance bit,
	@isReadyToRun bit,
	@isCompleted bit,
	@lastMachineRunOn nvarchar(450),
	@executionStatus nvarchar(450),
	@blockingBookmarks nvarchar(450),
	@operationTimeout int
as
begin
	set nocount on
	set transaction isolation level read committed

	declare @currentInstanceVersion bigint
	declare @deleteInstanceOnCompletion bit
	declare @enqueueCommand bit
	declare @lockAcquired bigint
	declare @metadataUpdateOnly bit
	declare @now datetime
	declare @result int
	declare @surrogateInstanceId bigint
	
	set @result = 0
	set @metadataUpdateOnly = 0
	
	exec @lockAcquired = sp_getapplock @Resource = 'InstanceStoreLock', @LockMode = 'Shared', @LockTimeout = @operationTimeout
		
	if (@lockAcquired < 0)
	begin
		select @result as 'Result'
		set @result = 13
	end
	
	set @now = getutcdate()
	
	if (@primitiveDataProperties is null and @complexDataProperties is null and @writeOnlyPrimitiveDataProperties is null and @writeOnlyComplexDataProperties is null)
		set @metadataUpdateOnly = 1

LockOrCreateInstance:
	if (@result = 0)
	begin
		exec @surrogateInstanceId = [System.Activities.DurableInstancing].[LockInstance] @instanceId, @surrogateLockOwnerId, @instanceVersion, @isNotExpectedToBeLocked, 0, @result output
															  
		if (@result = 0 and @surrogateInstanceId = 0)
		begin
			exec @surrogateInstanceId = [System.Activities.DurableInstancing].[CreateInstance] @instanceId, @surrogateLockOwnerId, @result output
		
			if (@result = 0 and @surrogateInstanceId = 0)
				goto LockOrCreateInstance		
		end
	end
	
	if (@result = 0)
	begin
		select @enqueueCommand = [EnqueueCommand],
			   @deleteInstanceOnCompletion = [DeletesInstanceOnCompletion]
		from [LockOwnersTable]
		where ([SurrogateLockOwnerId] = @surrogateLockOwnerId)

		update [InstancesTable] 
		set @instanceId = [InstancesTable].[Id],
			@currentInstanceVersion = [InstancesTable].[Version],
			[SurrogateLockOwnerId] = case when (@unlockInstance = 1 or @isCompleted = 1)
										then null
										else @surrogateLockOwnerId
									 end,
			[PrimitiveDataProperties] = case when (@metadataUpdateOnly = 1)
										then [PrimitiveDataProperties]
										else @primitiveDataProperties
									   end,
			[ComplexDataProperties] = case when (@metadataUpdateOnly = 1)
										then [ComplexDataProperties]
										else @complexDataProperties
									   end,
			[WriteOnlyPrimitiveDataProperties] = case when (@metadataUpdateOnly = 1)
										then [WriteOnlyPrimitiveDataProperties]
										else @writeOnlyPrimitiveDataProperties
									   end,
			[WriteOnlyComplexDataProperties] = case when (@metadataUpdateOnly = 1)
										then [WriteOnlyComplexDataProperties]
										else @writeOnlyComplexDataProperties
									   end,
			[MetadataProperties] = @metadataProperties,
			[PendingTimer] = case 
								when (@metadataUpdateOnly = 1) then [PendingTimer]
								when (@unlockInstance = 1) then @pendingTimer
								else null
							 end,
			[SuspensionReason] = case
									when (@suspensionStateChange = 0) then [SuspensionReason]
									when (@suspensionStateChange = 1) then @suspensionReason
									else null
								 end,
			[IsReadyToRun] =  case 
								when (@metadataUpdateOnly = 1) then [IsReadyToRun]
								when (@unlockInstance = 1) then @isReadyToRun
								else 0
							 end,
			[IsSuspended] = case
								when (@suspensionStateChange = 0) then [IsSuspended]
								when (@suspensionStateChange = 1) then 1
								else 0
							end,
			[IsCompleted] = @isCompleted,
			[IsInitialized] = case
								when (@metadataUpdateOnly = 0) then 1
								else [IsInitialized]
							  end,
			[SerializationMethod] = @serializationMethod,
			[BlockingBookmarks] = case
									when (@metadataUpdateOnly = 0) then @blockingBookmarks
									else [BlockingBookmarks]
								  end,
			[LastUpdated] = @now,
			[LastMachineRunOn] = case
									when (@metadataUpdateOnly = 0) then @lastMachineRunOn
									else [LastMachineRunOn]
								 end,
			[ExecutionStatus] = case
									when (@metadataUpdateOnly = 0) then @executionStatus
									else [ExecutionStatus]
								end
		from [InstancesTable]
		where ([InstancesTable].[SurrogateInstanceId] = @surrogateInstanceId) 
		
		if (@@rowcount = 0)
		begin
			set @result = 99
			select @result as 'Result' 
		end
		else
		begin		
			if (@isCompleted = 1 and @deleteInstanceOnCompletion = 1)
				exec [System.Activities.DurableInstancing].[DeleteInstance] @surrogateInstanceId
			else
			begin
				exec @result = [System.Activities.DurableInstancing].[AssociateKeys] @surrogateInstanceId, @keysToAssociate, @concatenatedBinaryBlob
				
				if (@result = 0)
					exec @result = [System.Activities.DurableInstancing].[CompleteKeys] @surrogateInstanceId, @keysToComplete
				
				if (@result = 0)
					exec @result = [System.Activities.DurableInstancing].[FreeKeys] @surrogateInstanceId, @keysToFree
				
				if (@result = 0)
					exec @result = [System.Activities.DurableInstancing].[UpdateKeyProperties] @surrogateInstanceId, @keyPropertiesToAdd, @keyPropertiesToRemove, @concatenatedBinaryBlob
				
				if (@result = 0) and (@metadataUpdateOnly = 0)
				begin
					delete from [InstancePromotedPropertiesTable]
					where [SurrogateInstanceId] = @surrogateInstanceId
				end
			
				if (@enqueueCommand = 1)
				begin
					if (@isCompleted = 1)
						exec [System.Activities.DurableInstancing].[DeleteWorkflowInstanceCommands] @surrogateInstanceId
					else
					begin
						if (@metadataUpdateOnly = 0 and @unlockInstance = 1)
						begin
							if (@isReadyToRun = 1)
								exec [System.Activities.DurableInstancing].[AdjustWorkflowInstanceRunCommand] @surrogateInstanceId, @now
							else if (@pendingTimer is not null)
								exec [System.Activities.DurableInstancing].[AdjustWorkflowInstanceRunCommand] @surrogateInstanceId, @pendingTimer
							else
								exec [System.Activities.DurableInstancing].[AdjustWorkflowInstanceRunCommand] @surrogateInstanceId, null
						end			
					end
				end
			end			
		end
	end
	
	if (@result != 13)
		exec sp_releaseapplock @Resource = 'InstanceStoreLock'
	
	if (@result = 0)
		select @result as 'Result', @currentInstanceVersion

	return @result
end
go

grant execute on [System.Activities.DurableInstancing].[SaveInstance] to [System.Activities.DurableInstancing.InstanceStoreUsers] 
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[CleanupServiceDeploymentTable]') and type in (N'P', N'PC'))
	drop procedure [System.Activities.DurableInstancing].[CleanupServiceDeploymentTable]
go

create procedure [System.Activities.DurableInstancing].[CleanupServiceDeploymentTable]
as
begin
	declare @serviceDeploymentCount bigint
	declare @lockAcquired bigint
	declare @candidateDeploymentIdsPass1 table([Id] bigint primary key)
	declare @candidateDeploymentIdsPass2 table([Id] bigint primary key)
	
	select @serviceDeploymentCount = count(1)
	from [ServiceDeploymentTable] with (nolock)
	
	if (@serviceDeploymentCount > 128)
	begin
		begin transaction
		exec @lockAcquired = sp_getapplock @Resource = 'InstanceStoreLock', @LockMode = 'Exclusive', @LockTimeout= 25000
		
		if (@lockAcquired < 0)
		begin
			rollback transaction
			return -1
		end

		insert into @candidateDeploymentIdsPass1
		select [Id] from [ServiceDeploymentTable]
		except
		select [ServiceDeploymentId] from [LockOwnersTable]
		
		insert into @candidateDeploymentIdsPass2
		select [Id] from @candidateDeploymentIdsPass1
		except
		select [ServiceDeploymentId] from [InstancesTable]
		
		delete [ServiceDeploymentTable]
		from [ServiceDeploymentTable]
		join @candidateDeploymentIdsPass2 as [OrphanedIds] on [OrphanedIds].[Id] = [ServiceDeploymentTable].[Id]
		
		exec sp_releaseapplock @Resource = 'InstanceStoreLock'
		commit transaction
	end
	
	return 0
end
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[CleanupPropertyNames]') and type in (N'P', N'PC'))
	drop procedure [System.Activities.DurableInstancing].[CleanupPropertyNames]
go

create procedure [System.Activities.DurableInstancing].[CleanupPropertyNames]
as
begin
	declare @stringCount bigint
	declare @propertyNameCount bigint
	declare @candidatePropertyNameIds table([Id] bigint primary key)
	declare @candidateStringIdsPass1 table([Id] bigint primary key)
	declare @candidateStringIdsPass2 table([Id] bigint primary key)
	declare @lockAcquired bigint
	
	select @stringCount = count(1)
	from [StringsTable] with (nolock)
	
	select @propertyNameCount = count(1)
	from [PropertyNamesTable] with (nolock)
	
	if (@stringCount > 1024 or @propertyNameCount > 1024)
	begin
		set transaction isolation level read committed
		begin transaction
		
		exec @lockAcquired = sp_getapplock @Resource = 'InstanceStoreLock', @LockMode = 'Exclusive', @LockTimeout = 10000
		
		if (@lockAcquired < 0)
		begin
			rollback transaction
			return -1
		end
		
		insert into @candidatePropertyNameIds
		select [Id] from [PropertyNamesTable]
		except
		select [PropertyNameId] from [KeyPropertiesTable]
		
		delete [PropertyNamesTable]
		from [PropertyNamesTable]
		join @candidatePropertyNameIds as [OrphanedIds] on [OrphanedIds].[Id] = [PropertyNamesTable].[PropertyNameId]
		
		insert into @candidateStringIdsPass1
		select [Id] from [StringsTable]
		except
		select [PropertyNameId] from [PropertyNamesTable]
		
		insert into @candidateStringIdsPass2
		select [Id] from @candidateStringIdsPass1
		except
		select [PropertyNamespaceId] from [PropertyNamesTable]
		
		delete [StringsTable]
		from [StringsTable]
		join @candidateStringIdsPass2 as [OrphanedIds] on [OrphanedIds].[Id] = [StringsTable].[Id]
		
		exec sp_releaseapplock @Resource = 'InstanceStoreLock'
		commit transaction
	end
	
	return 0
end
go

if exists (select * from sys.objects where object_id = OBJECT_ID(N'[System.Activities.DurableInstancing].[InsertPromotedProperties]') and type in (N'P', N'PC'))
	drop procedure [System.Activities.DurableInstancing].[InsertPromotedProperties]
go

create procedure [System.Activities.DurableInstancing].[InsertPromotedProperties]
	@instanceId uniqueidentifier,
	@promotionName nvarchar(400),
	@value1 sql_variant = null,
	@value2 sql_variant = null,
	@value3 sql_variant = null,
	@value4 sql_variant = null,
	@value5 sql_variant = null,
	@value6 sql_variant = null,
	@value7 sql_variant = null,
	@value8 sql_variant = null,
	@value9 sql_variant = null,
	@value10 sql_variant = null,
	@value11 sql_variant = null,
	@value12 sql_variant = null,
	@value13 sql_variant = null,
	@value14 sql_variant = null,
	@value15 sql_variant = null,
	@value16 sql_variant = null,
	@value17 sql_variant = null,
	@value18 sql_variant = null,
	@value19 sql_variant = null,
	@value20 sql_variant = null,
	@value21 sql_variant = null,
	@value22 sql_variant = null,
	@value23 sql_variant = null,
	@value24 sql_variant = null,
	@value25 sql_variant = null,
	@value26 sql_variant = null,
	@value27 sql_variant = null,
	@value28 sql_variant = null,
	@value29 sql_variant = null,
	@value30 sql_variant = null,
	@value31 sql_variant = null,
	@value32 sql_variant = null,
	@value33 varbinary(max) = null,
	@value34 varbinary(max) = null,
	@value35 varbinary(max) = null,
	@value36 varbinary(max) = null,
	@value37 varbinary(max) = null,
	@value38 varbinary(max) = null,
	@value39 varbinary(max) = null,
	@value40 varbinary(max) = null,
	@value41 varbinary(max) = null,
	@value42 varbinary(max) = null,
	@value43 varbinary(max) = null,
	@value44 varbinary(max) = null,
	@value45 varbinary(max) = null,
	@value46 varbinary(max) = null,
	@value47 varbinary(max) = null,
	@value48 varbinary(max) = null,
	@value49 varbinary(max) = null,
	@value50 varbinary(max) = null,
	@value51 varbinary(max) = null,
	@value52 varbinary(max) = null,
	@value53 varbinary(max) = null,
	@value54 varbinary(max) = null,
	@value55 varbinary(max) = null,
	@value56 varbinary(max) = null,
	@value57 varbinary(max) = null,
	@value58 varbinary(max) = null,
	@value59 varbinary(max) = null,
	@value60 varbinary(max) = null,
	@value61 varbinary(max) = null,
	@value62 varbinary(max) = null,
	@value63 varbinary(max) = null,
	@value64 varbinary(max) = null
as
begin
	set nocount on
	set transaction isolation level read committed

	declare @surrogateInstanceId bigint

	select @surrogateInstanceId = [SurrogateInstanceId]
	from [InstancesTable]
	where [Id] = @instanceId

	insert into [System.Activities.DurableInstancing].[InstancePromotedPropertiesTable]
	values (@surrogateInstanceId, @promotionName, @value1, @value2, @value3, @value4, @value5, @value6, @value7, @value8,
			@value9, @value10, @value11, @value12, @value13, @value14, @value15, @value16, @value17, @value18, @value19,
			@value20, @value21, @value22, @value23, @value24, @value25, @value26, @value27, @value28, @value29, @value30,
			@value31, @value32, @value33, @value34, @value35, @value36, @value37, @value38, @value39, @value40, @value41,
			@value42, @value43, @value44, @value45, @value46, @value47, @value48, @value49, @value50, @value51, @value52,
			@value53, @value54, @value55, @value56, @value57, @value58, @value59, @value60, @value61, @value62, @value63,
			@value64)
end
go