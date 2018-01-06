Note: this repository exists for the archival purposes only.  
The up-to-date version of .NET is located here: https://github.com/dotnet.

# Shared Source Common Language Infrastructure 1.0 Release

The Shared Source CLI is a compressed archive of the source code to a working implementation of the ECMA CLI and the ECMA C# language specification. This implementation builds and runs on Windows XP, the FreeBSD operating system, and Mac OS X 10.2.

## Quick details

* Version:	1.0	Date Published:	11/5/2002
* Language:	English
* Original URL: [http://www.microsoft.com/download/en/details.aspx?displaylang=en&id=14124](http://web.archive.org/web/20111103055104/http://www.microsoft.com/download/en/details.aspx?displaylang=en&id=14124)

## Files in this download

The links in this section correspond to files available for this download. Download the files appropriate for you.

| File | Name |	Size |
|------|------|------|
|sscli_20021101.tgz |	15.0 MB |	[Download](https://github.com/SSCLI/sscli_20021101/raw/master/archive/sscli_20021101.tgz) |
|sscli_ref_20021101.tgz	|3.0 MB	| [Download](https://github.com/SSCLI/sscli_ref_20021101/raw/master/archive/sscli_ref_20021101.tgz) |

## Overview

The Common Language Infrastructure (CLI) is the ECMA standard that describes the core of the .NET Framework world. The Shared Source CLI is a compressed archive of the source code to a working implementation of the ECMA CLI and the ECMA C# language specification.

This implementation builds and runs on Windows XP, the FreeBSD operating system, and Mac OS X 10.2. It is released under a shared source initiative. Please see the accompanying [license](https://github.com/SSCLI/sscli_20021101/blob/master/license.txt).

The Shared Source CLI goes beyond the printed specification of the ECMA standards, providing a working implementation for CLI developers to explore and understand. It will be of interest to academics and researchers wishing to teach and explore modern programming language concepts, and to .NET developers interested in how the technology works. 

## Features

The Shared Source CLI archive contains the following technologies in source code form:

* An implementation of the runtime for the Common Language Infrastructure (ECMA-335) that builds and runs on Windows XP, the FreeBSD operating system, and Mac OS X 10.2.
* Compilers that work with the Shared Source CLI for C# (ECMA-334) and JScript.
* Development tools for working with the Shared Source CLI such as assembler/disassemblers (ilasm, ildasm), a debugger (cordbg), metadata introspection (metainfo), and other utilities.
* The Platform Adaptation Layer (PAL) used to port the Shared Source CLI from Windows XP to FreeBSD and Mac OS X.
* Build environment tools (nmake, build, and others).
* Documentation for the implementation.
* Test suites used to verify the implementation.
* A rich set of sample code and tools for working with the Shared Source CLI.

## New in this Release

A more detailed list of what's new in this release is included in the FAQ (below).

* Support for Mac OS X 10.2.
* Additional code clean-up and bug fixes.
* Debugger improvements.
* Class reference documentation (separate archive) and additional samples.
* Build system improvements and additional test cases and tool improvements.

# What can I do with the Shared Source CLI? 

There is a wealth of programming language technology in the Shared Source CLI. It is likely to be of interest to a wide audience, including:

* Developers interested in the internal workings of the .NET Framework can explore this implementation of the CLI to see how garbage collection works, JIT compilation and verification is handled, security protocols implemented, and the organization of frameworks and virtual object systems.
* Teachers and researchers doing work with advanced compiler technology. Research projects into language extensions, JIT optimizations, and modern garbage collection all have a basis in the Shared Source CLI. Modern compiler courses can be based on the C# or JScript languages implemented on the CLI.
* People developing their own CLI implementations will find the Shared Source CLI an indispensable guide and adjunct to the ECMA standards.

## System requirements

Supported Operating Systems: Windows XP

On Windows you will need:

* Windows XP or Windows 2000. We recommend Windows XP.
* Microsoft Visual Studio .NET.
* Perl 5.6 (available from http://www.perl.org.)
* Archiving utility of choice—WinZip or other.
* 256 MB of memory.
* One gigabyte of free disk space.

On FreeBSD you will need:

* FreeBSD 4.7 (recommended).
* The developer distribution installed.
* 512 MB of memory.
* One gigabyte of free disk space.

On Mac OS X you will need:

* Mac OS X version 10.2.
* Apple Developer Tools.
* The BSD subsystem installed.
* 256 MB memory, 512 MB (recommended).
* One gigabyte of free disk space.

## Instructions

### Download

The archive is a compressed tarball. Download it and then uncompress and extract it using the appropriate tools on your system. On Mac OS X, you should use gnutar instead of tar.

### Support

While Technical Support is not provided for the Shared Source Common Language Infrastructure, there are several newsgroups where discussion with your peers can take place:

* The Microsoft Shared Source CLI newsgroup.
* A Shared Source CLI moderated discussion list has been created by the University of Pisa at: https://mailserver.di.unipi.it/mailman/listinfo/dotnet-sscli.
* A Shared Source CLI discussion group exists at DevelopMentor. To subscribe, either go to http://discuss.develop.com and join the list, or send an email from the account you'll be posting with to: listserv@discuss.develop.com, with a message body of: subscribe dotnet-rotor.

## Additional information

* FAQ and Release Notes are available.
* Microsoft Research has a web site dedicated to research projects using the Shared Source CLI.
* The original MSDN article by David Stutz.
* O'Reilly Network has a number of articles on the Shared Source CLI (codenamed Rotor) at: http://www.oreillynet.com/dotnet.
* Microsoft continues to commit to the CLI and C# standardization efforts; please see: http://msdn.microsoft.com/net/ecma.
* ECMA is the official publication site for ECMA-334 (The C# Language Specification) and ECMA-335 (The Common Language Infrastructure). Please see: ECMA-334 and ECMA-335.
* For more information on Microsoft's Shared Source initiatives, including the Shared Source CLI, please see: http://www.microsoft.com/sharedsource.

## Related resources

* Shared Source Common Language Infrastructure 2.0 Releaseversion 2.0
* FAQ and Release Notes for This Download
* Microsoft Shared Source CLI Newsgroup
* Home Page for Microsoft Shared Source Initiative
