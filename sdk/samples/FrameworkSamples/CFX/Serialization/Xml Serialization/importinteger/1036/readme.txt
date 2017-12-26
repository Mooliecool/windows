Exemple SchemaImporterExtension Import Integer
=============================================
     Montre comment écrire un SchemaImporterExtension pour importer les types d'entiers du schéma XML sous la forme long et ulong au lieu de string.


Langages d'implémentation de l'exemple
===============================
     Les implémentations de cet exemple sont disponibles dans les langages suivants :
     C#


Pour générer l'exemple à partir de l'invite de commandes
=============================================
     1. Ouvrez la fenêtre d'invite de commandes et accédez au répertoire Technologies\Serialization\Xml Serialization\SchemaImporterExtension\ImportInteger.
     2. Tapez msbuild [Nom de fichier de la solution].


Pour générer l'exemple à l'aide de Visual Studio
=======================================
     1. Ouvrez l'Explorateur Windows et accédez au répertoire Technologies\Serialization\Xml Serialization\SchemaImporterExtension\ImportInteger.
     2. Double-cliquez sur l'icône du fichier solution (.sln) pour l'ouvrir dans Visual Studio.
     3. Dans le menu Générer, sélectionnez Générer la solution.
     L'application sera générée dans le répertoire par défaut \bin ou \bin\Debug.

     Éléments supplémentaires à prendre en compte lors de la génération de cet exemple :
     1.  Générez un nom fort pour l'assembly
     2.  Ajoutez l'assembly au GAC
     3.  Ajoutez l'assembly à machine.config dans une section schemas.xml.serialization/schemaImporterExtension


Pour exécuter l'exemple
=================
     1. Accédez au répertoire qui contient le nouveau fichier exécutable à l'aide de l'invite de commandes ou de l'Explorateur Windows.
     2. Tapez [Nom_fichier_exe] à l'invite de commandes ou double-cliquez sur l'icône de [Nom_Exe_Exemple] pour l'exécuter à partir de l'Explorateur Windows.

     L'exemple peut être généré avec des autorisations limitées, mais l'installation requiert des privilèges d'administrateur, car l'assembly doit être ajouté au GAC et machine.config doit être modifié.

     Exemple d'entrée dans machine.config :

     system.xml.serialization
     	schemaImporterExtensions
     		add name="RandomString" type="Microsoft.Samples.Xml.Serialization.SchemaImporterExtension.ImportInteger, ImportInteger, Version=0.0.0.0, Culture=neutral, PublicKeyToken=3c3789dee90b3265"
     	schemaImporterExtensions
     system.xml.serialization


Notes
=================
     1.  Exécutez xsd.exe, wsdl.exe ou Ajouter une référence Web pour WSDL qui utilise xs:integer, xs:negativeInteger, xs:nonNegativeInteger, xs:positiveInteger ou xs:nonPositiveInteger
2.  Remarquez que la classe générée utilise long ou ulong au lieu de string pour les types d'entiers du schéma XML

