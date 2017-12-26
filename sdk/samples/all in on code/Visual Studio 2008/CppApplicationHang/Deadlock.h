/****************************** Module Header ******************************\
Module Name:  Deadlock.h
Project:      CppApplicationHang
Copyright (c) Microsoft Corporation.

Deadlock refers to a specific condition when two or more processes are each 
waiting for each other to release a resource, or more than two processes 
are waiting for resources in a circular chain. Because none of threads are 
willing to release their protected resources, what ultimately happens is 
that none of the threads will ever make any progress. They simply sit there 
and wait for the others to make a move, and a deadlock ensues.

In this sample, two threads are running TriggerDeadlock and 
DeadlockThreadProc respectively. The thread that runs TriggerDeadlock 
acquires resource2 first, and waits for resource1. The thread running 
DeadlockThreadProc acquires resource1, and waits for the leave of resource2 
in the first thread. Neither thread is willing to release its protected 
resource first, so a deadlock occurs.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma once


//
//  FUNCTION: TriggerDeadlock()
//
//  PURPOSE: Triggers a basic deadlock.
//
void TriggerDeadlock();