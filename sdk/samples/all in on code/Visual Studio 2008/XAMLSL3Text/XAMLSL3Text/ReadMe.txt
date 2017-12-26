========================================================================
    SILVERLIGHT APPLICATION : XAMLSL3Text Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This example demonstrates how to work with text in Silverlight using XAML.
It includes the following features:

	Work with paragraphs.
	Customize text format.
	Select text.
	Work with font.
	Work with text input.

Note: Please run the XAMLSL3Text_Web instead of the XAMLSL3Text project.
This project requires the http protocol.

In the sample, you can also find the show case of VideoBrush to demonstrate 
how to paint a text using a video. Due to the size of video (larger than 1MB), 
we do not embed the video in the project folder as a Resource, though it can 
make the video to show up faster since no web request is needed. When playing 
online videos, Silverlight requires you to use the http protocol. Otherwise 
you'll encounter a cross scheme issue, and the video cannot be played. 
Similarly, if you need to invoke web services, you have to use the http 
protocol, therefore we add a web project (XamlSL3Text_Web), and the users are 
required to run the web application rather than the Silverlight application 
to see the effect of VideoBrush. (Please set web project is the start project 
of the solution).


/////////////////////////////////////////////////////////////////////////////
Project Relation:

XAMLSL3Text - CSSL3Text - VBSL3Text

CSSL3Text and VBSL3Text expose the same UI and the same set of properties,
methods, and events, but they are implemented in different languages.
XAMLSL3Text is different in that it shows you how to work with text in XAML
rather than code behind.


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

Silverlight 3 Chained Installer
http://www.microsoft.com/downloads/details.aspx?familyid=9442b0f2-7465-417a-88f3-5e7b5409e9dd&displaylang=en


/////////////////////////////////////////////////////////////////////////////
Code Logic:

For a full description of how to work with text in XAML, please run the 
project. The sample itself is an interactive document.    


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Work with text
http://msdn.microsoft.com/en-us/library/cc189010(VS.95).aspx


/////////////////////////////////////////////////////////////////////////////
