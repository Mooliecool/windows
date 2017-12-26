Esempio ASPX Hosting
=======================
In questo esempio viene illustrato come combinare le funzionalità di HttpListener per creare un server Http per l'inoltro di chiamate all'applicazione ASPX host. Nella versione 2.0 di .NET Framework è inclusa la classe HttpListener basata su Http.Sys, che consente agli utenti di creare un server Http autonomo.

Nell'esempio vengono utilizzate le seguenti funzionalità di HttpListener:
1. Autenticazione
2. Attivazione di SSL
3. Lettura di certificati client in connessioni protette


Implementazioni di linguaggi disponibili per l'esempio
===============================
     L'esempio è disponibile nelle seguenti implementazioni di linguaggi:
     C#


Per generare l'esempio utilizzando il prompt dei comandi
=============================================

     1. Aprire una finestra del prompt dei comandi di SDK, quindi passare alla sottodirectory CS della directory AspxHost.

     2. Digitare msbuild AspxHostCS.sln.


Per generare l'esempio utilizzando Visual Studio
=======================================

     1. Aprire Esplora risorse, quindi passare alla sottodirectory CS della directory AspxHost.

     2. Fare doppio clic sull'icona relativa al file con estensione sln (soluzione) per aprire il file in Visual Studio.

     3. Scegliere Genera soluzione dal menu Genera.
     L'applicazione verrà generata nella directory \bin o \bin\Debug predefinita.


Per eseguire l'esempio
=================
     1. Passare alla directory contenente il nuovo eseguibile, utilizzando il prompt dei comandi o Esplora risorse.
     2. Digitare AspxHostCS.exe alla riga di comando oppure fare doppio clic sull'icona del file AspxHostingCS.exe in Esplora risorse. 


Osservazioni
======================
1. Informazioni sulle classi

Il file AspxHostCS.cs contiene la classe principale che consente di creare e configurare un listener e un'applicazione ASPX.

Il file AspxVirtualRoot.cs contiene la classe che consente di configurare un HttpListener che rimanga in attesa su prefissi e schemi di autenticazione supportati.

Il file AspxNetEngine.cs contiene la classe che consente di configurare un'applicazione ASPX mediante l'assegnazione di un alias virtuale mappato a una directory fisica.

Il file AspxPage.cs contiene la classe che implementa la classe SimpleWorkerRequest e rappresenta una pagina richiesta dal client.

Il file AspxRequestInfo.cs contiene una classe contenitore dati utilizzata per passare dati pertinenti da HttpListenerContext all'applicazione host.

Il file AspxException.cs contiene la classe di eccezioni personalizzata.

La directory Demopages contiene le pagine ASPX di esempio.


2. Utilizzo dell'esempio
 
Il file AspxHostCS.cs rappresenta la classe che contiene il metodo principale per l'avvio di un HttpListener e la configurazione di una directory fisica come applicazione ASPX host. Per impostazione predefinita, la classe tenta di configurare la directory DemoPages, inclusa nella stessa directory degli esempi, come applicazione host con l'alias virtuale /.  Poiché in questo esempio HttpListener rimane in attesa sulla porta 80, per eseguire l'esempio potrebbe essere necessario interrompere IIS.

 
Modificare il codice per adattarlo alle proprie esigenze: 

                //Creare un oggetto AspxVirtualRoot con una porta http e una porta https, se necessario
                
                AspxVirtualRoot virtualRoot = new AspxVirtualRoot(80);


                //Configurare una directory fisica come alias virtuale.

                //TODO: sostituire la directory fisica con la directory da configurare.

		virtualRoot.Configure("/", Path.GetFullPath(@"..\..\DemoPages"));


                //TODO: aggiungere qui la funzionalità di autenticazione, se necessaria

                //virtualRoot.AuthenticationSchemes = AuthenticationSchemes.Basic;


3. Impostazione dello schema di autenticazione
 
Dopo aver configurato un oggetto AspxVirtualRoot, definire lo schema di autenticazione necessario impostando il campo AuthenticationScheme sull'oggetto AspxVirtualRoot.

 
4. Attivazione di SSL
 

Per attivare SSL, è necessario configurare un certificato server installato nell'archivio del computer sulla porta per la quale si richiede SSL. Per ulteriori informazioni sulla configurazione di un certificato server su una porta tramite l'utilità Httpcfg.exe, vedere il collegamento Httpcfg.

 
Nota: per configurare un certificato server su una porta, è anche possibile utilizzare Winhttpcertcfg.


Problema noto
====================== 

Problema:
All'avvio dell'applicazione viene visualizzato un messaggio di errore simile al seguente:

"System.IO.FileNotFoundException: Impossibile caricare il file o l'assembly 'AspxHostCS, Version=1.0.1809.19805, Culture=neutral, PublicKeyToken=null' o una delle relative dipendenze. Impossibile trovare il nome file specified.File: 'AspxHostCS, Version=1.0.1809.19805, Culture=neutral, PublicKeyToken=null'”

Soluzione:
Il file AspxHostCs.exe non è presente nella directory bin della directory fisica configurata. Copiare il file AspxHostcs.exe nella directory bin.


Argomenti correlati
============
Vedere gli argomenti relativi alle API di hosting ASPX e a HttpListener nella documentazione di .NET Framework SDK e in MSDN.
