Beispiel "SchemaImporterExtension Import Integer"
=================================================
     Zeigt, wie Sie eine SchemaImporterExtension schreiben, um XML-Schemaganzzahltypen als "long" oder "ulong" anstatt als "string" zu importieren.


Sprachimplementierungen dieses Beispiels
========================================
     Dieses Beispiel ist in den folgenden Programmiersprachen verfügbar:
     C#


So erstellen Sie das Beispiel mithilfe der Eingabeaufforderung
==============================================================
     1. Öffnen Sie das Eingabeaufforderungsfenster, und wechseln Sie zum Verzeichnis "Technologies\Serialization\Xml Serialization\SchemaImporterExtension\ImportInteger".
     2. Geben Sie "msbuild [Name der Projektmappendatei]" ein.


So erstellen Sie das Beispiel mithilfe von Visual Studio
========================================================
     1. Öffnen Sie das Eingabeaufforderungsfenster, und wechseln Sie zum Verzeichnis "Technologies\Serialization\Xml Serialization\SchemaImporterExtension\ImportInteger".
     2. Doppelklicken Sie auf das Symbol für die SLN-Datei (Projektmappe), um die Datei in Visual Studio zu öffnen.
     3. Wählen Sie im Menü "Erstellen" die Option "Projektmappe erstellen" aus.
     Die Anwendung wird im Standardverzeichnis "\bin" oder "\bin\debug" erstellt.

     Weitere Punkte, die Sie beim Erstellen des Beispiels beachten sollten:
     1. Erzeugen Sie einen starken Namen für die Assembly.
     2. Fügen Sie die Assembly dem GAC hinzu.
     3. Fügen Sie die Assembly in einem Abschnitt "schemas.xml.serialization/schemaImporterExtension" zu "machine.config" hinzu.


So führen Sie das Beispiel aus
==============================
     1. Wechseln Sie mithilfe der Eingabeaufforderung oder in Windows Explorer zu dem Verzeichnis, das die neue ausführbare Datei enthält.
     2. Geben Sie an der Befehlszeile "[AusführbareDatei]" ein, oder doppelklicken Sie in Windows Explorer auf das Symbol für [BeispielAusführbareDatei], um das Beispiel zu starten.

     Das Beispiel kann mit eingeschränkter Berechtigung erstellt werden. Für das Installieren sind jedoch Administratorrechte erforderlich, da die Assembly dem GAC hinzugefügt und "machine.config" bearbeitet werden muss.

     Beispieleintrag in "machine.config":

     system.xml.serialization
     	schemaImporterExtensions
     		add name="RandomString" type="Microsoft.Samples.Xml.Serialization.SchemaImporterExtension.ImportInteger, ImportInteger, Version=0.0.0.0, Culture=neutral, PublicKeyToken=3c3789dee90b3265"
     	schemaImporterExtensions
     system.xml.serialization


Hinweise
========
     1. Führen Sie "xsd.exe", "wsdl.exe" oder "Webverweis hinzufügen" in WSDL aus, das xs:integer, xs:negativeInteger, xs:nonNegativeInteger, xs:positiveInteger oder xs:nonPositiveInteger verwendet.
2. Beachten Sie, dass die erzeugte Klasse "long" oder "ulong" anstelle von "string" für die XML-Schemaganzzahltypen verwendet.
