ASPX Hosting-Beispiel
=====================
Dieses Beispiel zeigt, wie die Features von HttpListener kombiniert werden, um einen HTTP-Server zu erstellen, der Aufrufe an eine gehostete ASPX-Anwendung weiterleitet. Version 2.0 von .NET Framework führt die HttpListener-Klasse ein, die auf "Http.Sys" aufgebaut ist, die Benutzern ermöglicht, einen eigenständigen HTTP-Server zu erstellen.

Dieses Beispiel verwendet folgende Features von HttpListener:
1. Authentifizierung
2. Aktivieren von SSL
3. Lesen von Clientzertifikaten über sichere Verbindungen


Sprachimplementierungen dieses Beispiels
========================================
     Dieses Beispiel ist in den folgenden Programmiersprachen verfügbar:
     C#


So erstellen Sie das Beispiel mithilfe der Eingabeaufforderung
==============================================================

     1. Öffnen Sie das SDK-Eingabeaufforderungsfenster, und wechseln Sie im Verzeichnis "AspxHost" zum Unterverzeichnis "CS".

     2. Geben Sie "msbuild AspxHostCS.sln" ein (ohne Anführungszeichen).


So erstellen Sie das Beispiel mithilfe von Visual Studio
========================================================

     1. Öffnen Sie Windows Explorer, und wechseln Sie im Verzeichnis "AspxHost" zum Unterverzeichnis "CS".

     2. Doppelklicken Sie auf das Symbol für die SLN-Datei (Projektmappe), um die Datei in Visual Studio zu öffnen.

     3. Wählen Sie im Menü "Erstellen" die Option "Projektmappe erstellen" aus.
     Die Anwendung wird im Standardverzeichnis "\bin" oder "\bin\debug" erstellt.


So führen Sie das Beispiel aus
==============================
     1. Wechseln Sie mithilfe der Eingabeaufforderung oder in Windows Explorer zu dem Verzeichnis, das die neue ausführbare Datei enthält.
     2. Geben Sie an der Befehlszeile "AspxHostCS.exe" ein, oder doppelklicken Sie in Windows Explorer auf das Symbol für "AspxHostingCS.exe", um die Datei zu starten. 


Hinweise
========
1. Klasseninformationen

Die Datei "AspxHostCS.cs" enthält die Hauptklasse, die einen Listener und eine ASPX-Anwendung erstellt und konfiguriert.

Die Datei "AspxVirtualRoot.cs" enthält die Klasse, die einen HttpListener so konfiguriert, dass er Präfixe und unterstützte Authentifizierungsschemas überwacht.

Die Datei "AspxNetEngine.cs" enthält die Klasse, die eine ASPX-Anwendung konfiguriert, indem ein virtueller Alias zugeordnet wird, dem ein physikalisches Verzeichnis zuordnet ist.

Die Datei "AspxPage.cs" enthält die Klasse, die die SimpleWorkerRequest-Klasse implementiert und eine vom Client angeforderte Seite darstellt.

Die Datei "AspxRequestInfo.cs" enthält die Datenhalterklasse, die verwendet wird, um wichtige Daten von HttpListenerContext an die gehostete Anwendung zu übergeben.

Die Datei "AspxException.cs" enthält die benutzerdefinierte Ausnahmeklasse.

Das Verzeichnis "Demopages" enthält ASPX-Beispielseiten.


2. Verwendung des Beispiels
 
Die Datei "AspxHostCS.cs" ist die Klasse, die die Hauptmethode enthält, die einen HttpListener startet und ein physikalisches Verzeichnis als gehostete ASPX-Anwendung konfiguriert. Standardmäßig versucht die Klasse, das Verzeichnis "DemoPages" (das sich im selben Beispielverzeichnis befindet) als gehostete Anwendung unter dem virtuellen Alias / zu konfigurieren. Da HttpListener in diesem Beispiel den Anschluss 80 überwacht, darf IIS dieses Beispiel möglicherweise nicht mehr ausführen.

 
Ändern Sie den Code für die individuelle Verwendung:

                //Ein AspxVirtualRoot-Objekt mit einem HTTP-Anschluss und einem HTTPS-Anschluss erstellen, falls erforderlich.
                
                AspxVirtualRoot virtualRoot = new AspxVirtualRoot(80);


                //Ein physikalisches Verzeichnis als virtuellen Alias konfigurieren.

                //TODO: Ersetzen Sie das physikalische Verzeichnis durch das zu konfigurierende Verzeichnis.

		virtualRoot.Configure("/", Path.GetFullPath(@"..\..\DemoPages"));


                //TODO: Authentifizierung ggf. hier hinzufügen.

                //virtualRoot.AuthenticationSchemes = AuthenticationSchemes.Basic;


3. Festlegen des Authentifizierungsschemas
 
Legen Sie nach dem Konfigurieren eines AspxVirtualRoot-Objekts das erforderliche Authentifizierungsschema fest, indem Sie das AuthenticationScheme-Feld für das AspxVirtualRoot-Objekt festlegen.

 
4. Aktivieren von SSL
 

Zum Aktivieren von SSL muss ein im Computerspeicher installiertes Serverzertifikat auf dem Anschluss, auf dem SSL erforderlich ist, konfiguriert werden. Weitere Informationen über das Konfigurieren eines Serverzertifikats auf einem Anschluss mithilfe des Dienstprogramms "Httpcfg.exe" erhalten Sie über den Link für Httpcfg.

 
Hinweis: Winhttpcertcfg kann ebenfalls für das Konfigurieren eines Serverzertifikats auf einem Anschluss verwendet werden.


Bekannte Probleme
=================

Problem:
Wenn ich die Anwendung starte, wird folgende Fehlermeldung angezeigt:

"System.IO.FileNotFoundException: Die Datei oder Assembly "AspxHostCS, Version=1.0.1809.19805, Culture=neutral, PublicKeyToken=null" oder eine Abhängigkeit davon wurde nicht gefunden. Das System kann die angegebene Datei nicht finden. Dateiname: "AspxHostCS, Version=1.0.1809.19805, Culture=neutral, PublicKeyToken=null""

Lösung:
Die Datei "AspxHostCs.exe" ist im Verzeichnis "bin" des physischen Verzeichnisses, das konfiguriert wird, nicht vorhanden. Kopieren Sie die Datei "AspxHostcs.exe" in das Verzeichnis "bin".


Siehe auch
==========
Siehe auch API-Dokumentation zu HttpListener und ASPX Hosting in der .NET Framework SDK-Dokumentation und in MSDN.