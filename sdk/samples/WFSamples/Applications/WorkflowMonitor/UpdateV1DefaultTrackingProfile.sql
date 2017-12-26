-----------------------------------------------------------------------
--  This file is part of the Windows Workflow Foundation SDK Code Samples.
-- 
--  Copyright (C) Microsoft Corporation.  All rights reserved.
-- 
--This source code is intended only as a supplement to Microsoft
--Development Tools and/or on-line documentation.  See these other
--materials for detailed information regarding Microsoft code samples.
-- 
--THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
--KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
--IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
--PARTICULAR PURPOSE.
-----------------------------------------------------------------------

------------
--Updates V1.0.0 of DefaultTrackingProfile to extract Member Name
------------
Update [dbo].[DefaultTrackingProfile] 
Set 
[TrackingProfileXml] = N'<?xml version="1.0" encoding="utf-16" standalone="yes"?>
<TrackingProfile xmlns="http://schemas.microsoft.com/winfx/2006/workflow/trackingprofile" version="1.0.0">
    <TrackPoints>
        <ActivityTrackPoint>
            <MatchingLocations>
                <ActivityTrackingLocation>
                    <Activity>
                        <Type>System.Workflow.ComponentModel.Activity, System.Workflow.ComponentModel, Version=3.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35</Type>
                        <MatchDerivedTypes>true</MatchDerivedTypes>
                    </Activity>
                    <ExecutionStatusEvents>
                        <ExecutionStatus>Initialized</ExecutionStatus>
                        <ExecutionStatus>Executing</ExecutionStatus>
                        <ExecutionStatus>Compensating</ExecutionStatus>
                        <ExecutionStatus>Canceling</ExecutionStatus>
                        <ExecutionStatus>Closed</ExecutionStatus>
                        <ExecutionStatus>Faulting</ExecutionStatus>
                    </ExecutionStatusEvents>
                </ActivityTrackingLocation>
            </MatchingLocations>
            <Extracts>
                <WorkflowDataTrackingExtract>
                    <Member>Name</Member>
                </WorkflowDataTrackingExtract>
            </Extracts>
        </ActivityTrackPoint>
		<WorkflowTrackPoint>
			<MatchingLocation>
				<WorkflowTrackingLocation>
					<TrackingWorkflowEvents>
						<TrackingWorkflowEvent>Created</TrackingWorkflowEvent>						
						<TrackingWorkflowEvent>Completed</TrackingWorkflowEvent>
						<TrackingWorkflowEvent>Idle</TrackingWorkflowEvent>
						<TrackingWorkflowEvent>Suspended</TrackingWorkflowEvent>
						<TrackingWorkflowEvent>Resumed</TrackingWorkflowEvent>
						<TrackingWorkflowEvent>Persisted</TrackingWorkflowEvent>
						<TrackingWorkflowEvent>Unloaded</TrackingWorkflowEvent>
						<TrackingWorkflowEvent>Loaded</TrackingWorkflowEvent>
						<TrackingWorkflowEvent>Exception</TrackingWorkflowEvent>
						<TrackingWorkflowEvent>Terminated</TrackingWorkflowEvent>
						<TrackingWorkflowEvent>Aborted</TrackingWorkflowEvent>
						<TrackingWorkflowEvent>Changed</TrackingWorkflowEvent>
						<TrackingWorkflowEvent>Started</TrackingWorkflowEvent>
					</TrackingWorkflowEvents>
				</WorkflowTrackingLocation>
			</MatchingLocation>
		</WorkflowTrackPoint>
        <UserTrackPoint>
            <MatchingLocations>
                <UserTrackingLocation>
                    <Activity>
                        <Type>System.Workflow.ComponentModel.Activity, System.Workflow.ComponentModel, Version=3.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35</Type>
                        <MatchDerivedTypes>true</MatchDerivedTypes>
                    </Activity>
                    <Argument>
                        <Type>System.Object, mscorlib, Version=2.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35</Type>
                        <MatchDerivedTypes>true</MatchDerivedTypes>
                    </Argument>
                </UserTrackingLocation>
            </MatchingLocations>
        </UserTrackPoint>
    </TrackPoints>
</TrackingProfile>' 

Where [Version] = '1.0.0'