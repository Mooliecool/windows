Exemple ASPX Hosting
=======================
Cet exemple montre comment combiner les fonctionnalités de HttpListener pour créer un serveur Http qui route les appels vers l'application Aspx hébergée. La version 2.0 du .NET Framework introduit la classe HttpListener qui repose sur Http.Sys, ce qui permet aux utilisateurs de créer un serveur Http autonome.

Cet exemple utilise les fonctionnalités suivantes de HttpListener :
1. Authentification
2. Activation de SSL
3. Lecture des certificats clients sur des connexions sécurisées


Langages d'implémentation de l'exemple
===============================
     Cet exemple est disponible dans les implémentations de langage suivantes :
     C#


Pour générer l'exemple à partir de l'invite de commandes
=============================================

     1. Ouvrez la fenêtre d'invite de commandes du SDK et accédez au sous-répertoire CS du répertoire AspxHost.

     2. Tapez msbuild AspxHostCS.sln.


Pour générer l'exemple à l'aide de Visual Studio
=======================================

     1. Ouvrez l'Explorateur Windows et accédez au sous-répertoire CS sous le répertoire AspxHost.

     2. Double-cliquez sur l'icône du fichier solution (.sln) pour l'ouvrir dans Visual Studio.

     3. Dans le menu Générer, sélectionnez Générer la solution.
     L'application sera générée dans le répertoire par défaut \bin ou \bin\Debug.


Pour exécuter l'exemple
=================
     1. Accédez au répertoire qui contient le nouveau fichier exécutable à l'aide de l'invite de commandes ou de l'Explorateur Windows.
     2. Tapez AspxHostCS.exe à l'invite de commandes ou double-cliquez sur l'icône de AspxHostingCS.exe pour l'exécuter à partir de l'Explorateur Windows. 


Notes
======================
1. Informations de classe

Le fichier AspxHostCS.cs contient la classe principale qui crée et configure un écouteur et une application Aspx.

Le fichier AspxVirtualRoot.cs contient la classe qui configure un HttpListener pour écouter les préfixes et les schémas d'authentification pris en charge.

Le fichier AspxNetEngine.cs contient la classe qui configure une application Aspx en assignant un alias virtuel qui est mappé à un répertoire physique.

Le fichier AspxPage.cs contient la classe qui implémente la classe SimpleWorkerRequest et représente une page demandée par le client.

Le fichier AspxRequestInfo.cs contient la classe conteneur de données utilisée pour passer les données appropriées de HttpListenerContext à l'application hébergée.

Le fichier AspxException.cs contient la classe d'exception personnalisée.

Le répertoire Demopages contient les pages Aspx d'exemple.


2. Utilisation de l'exemple
 
Le fichier AspxHostCS.cs est la classe qui contient la méthode principale qui lancera un HttpListener et configurera un répertoire physique sous la forme d'une application ASPX hébergée. Par défaut, la classe essaie de configurer le répertoire DemoPages (qui se trouve dans le même répertoire samples) sous la forme d'une application hébergée sous alias virtuel /.  Dans la mesure où HttpListener dans cet exemple écoute le port 80, il se peut que vous deviez arrêter IIS pour exécuter cet exemple.

 
Modifiez le code pour votre utilisation personnelle : 

                //Crée un objet AspxVirtualRoot avec un port http et un port https si nécessaire
                
                AspxVirtualRoot virtualRoot = new AspxVirtualRoot(80);


                //Configure un répertoire physique sous la forme d'un alias virtuel.

                //TODO : remplacez le répertoire physique par le répertoire à configurer.

		virtualRoot.Configure("/", Path.GetFullPath(@"..\..\DemoPages"));


                //TODO : Si l'authentification doit être ajoutée, ajoutez-la ici

                //virtualRoot.AuthenticationSchemes = AuthenticationSchemes.Basic;


3. Définition du schéma d'authentification
 
Après avoir configuré un objet AspxVirtualRoot, définissez le schéma d'authentification requis en définissant le champ AuthenticationScheme de l'objet AspxVirtualRoot.

 
4. Activation de Ssl
 

Pour activer SSL, un certificat de serveur installé dans le magasin de l'ordinateur doit être configuré sur le port qui requiert SSL. Pour plus d'informations sur la configuration d'un certificat de serveur sur un port à l'aide de l'utilitaire Httpcfg.exe, reportez-vous au lien Httpcfg.

 
Remarque : Winhttpcertcfg peut également être utilisé pour configurer le certificat de serveur sur un port.


Problème connu
====================== 

Problème :
Lorsque je démarre l'application, le message d'erreur suivant s'affiche :

"System.IO.FileNotFoundException: Impossible de charger le fichier ou l'assembly 'AspxHostCS, Version=1.0.1809.19805, Culture=neutral, PublicKeyToken=null' ou une de ses dépendances. Le système ne trouve pas le fichier spécifié. Nom de fichier : 'AspxHostCS, Version=1.0.1809.19805, Culture=neutral, PublicKeyToken=null'"

Solution :
Le fichier AspxHostCs.exe n'est pas présent dans le répertoire bin du répertoire physique en cours de configuration. Copiez le fichier AspxHostcs.exe dans le répertoire bin.


Voir aussi
============
Consultez la documentation sur HttpListener et l'API Aspx Hosting dans la documentation du Kit de développement .NET Framework SDK et sur MSDN.

