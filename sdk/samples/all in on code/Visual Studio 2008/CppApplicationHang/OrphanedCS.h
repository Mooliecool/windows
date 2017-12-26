/****************************** Module Header ******************************\
Module Name:  OrphanedCS.h
Project:      CppApplicationHang
Copyright (c) Microsoft Corporation.

Examination of the critical section in question leads to a thread ID that 
does not exist. This is referred to as an orphaned critical section. 

Here, we demonstrate two typical scenarios of orphaned critical section:

Orphaned Critical Section Scenario 1:
An exception filter catches an exception during execution of code 
(typically it is some third party code written for RPC or COM server) that 
is holding a critical section.

Orphaned Critical Section Scenraio 2:
The main thread calls TerminateThread to terminate a thread that owns a 
critical section.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma once


void TriggerOrphanedCS1();


void TriggerOrphanedCS2();