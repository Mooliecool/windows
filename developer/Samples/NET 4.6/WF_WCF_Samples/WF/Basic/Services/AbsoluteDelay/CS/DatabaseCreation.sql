-- Copyright (c) Microsoft Corporation.  All rights reserved.
USE [master]
GO
IF  EXISTS (SELECT name FROM sys.databases WHERE name = N'AbsoluteDelaySampleDB')
DROP DATABASE [AbsoluteDelaySampleDB]
GO
CREATE DATABASE [AbsoluteDelaySampleDB]
GO
