Esempio SchemaImporterExtension Import Integer
=============================================
     In questo esempio viene illustrato come scrivere un oggetto SchemaImporterExtension per importare i tipi di valori interi dello schema XML come valori long e ulong anziché come valori string.


Implementazioni di linguaggi disponibili per l'esempio
===============================
     L'esempio è disponibile nelle seguenti implementazioni di linguaggi:
     C#


Per generare l'esempio utilizzando il prompt dei comandi
=============================================
     1. Aprire la finestra del prompt dei comandi, quindi passare alla directory Technologies\Serialization\Xml Serialization\SchemaImporterExtension\ImportInteger.
     2. Digitare msbuild [Nome file della soluzione].


Per generare l'esempio utilizzando Visual Studio
=======================================
     1. Aprire Esplora risorse, quindi passare alla directory Technologies\Serialization\Xml Serialization\SchemaImporterExtension\ImportInteger.
     2. Fare doppio clic sull'icona relativa al file con estensione sln (soluzione) per aprire il file in Visual Studio.
     3. Scegliere Genera soluzione dal menu Genera.
     L'applicazione verrà generata nella directory \bin o \bin\Debug predefinita.

     Ulteriori fattori da considerare per la generazione dell'esempio:
     1.  Generare un nome sicuro per l'assembly
     2.  Aggiungere l'assembly alla Global Assembly Cache
     3.  Aggiungere l'assembly a machine.config in una sezione schemas.xml.serialization/schemaImporterExtension


Per eseguire l'esempio
=================
     1. Passare alla directory contenente il nuovo eseguibile, utilizzando il prompt dei comandi o Esplora risorse.
     2. Digitare [FileEseguibile] alla riga di comando oppure fare doppio clic sull'icona di [EseguibileEsempio] in Esplora risorse.

     È possibile generare l'esempio con autorizzazioni limitate, tuttavia per l'installazione sono richiesti privilegi amministrativi perché è necessario aggiungere l'assembly alla Global Assembly Cache e modificare il file machine.config.

     Esempio di voce di machine.config:

     system.xml.serialization
     	schemaImporterExtensions
     		add name="RandomString" type="Microsoft.Samples.Xml.Serialization.SchemaImporterExtension.ImportInteger, ImportInteger, Version=0.0.0.0, Culture=neutral, PublicKeyToken=3c3789dee90b3265"
     	schemaImporterExtensions
     system.xml.serialization


Osservazioni
=================
     1.  Eseguire xsd.exe, wsdl.exe o Aggiungi riferimento Web nel servizio WSDL in cui viene utilizzato xs:integer, xs:negativeInteger, xs:nonNegativeInteger, xs:positiveInteger o xs:nonPositiveInteger.
2.  Si noti che nella classe generata vengono utilizzati valori long o ulong anziché i valori string per i tipi di valori interi dello schema XML.

