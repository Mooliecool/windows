Ejemplo ASPX Hosting
=======================
En este ejemplo se muestra cómo combinar las características de HttpListener para crear un servidor Http que enrute las llamadas a la aplicación Aspx alojada. La versión 2.0 de .NET Framework presenta la clase HttpListener generada sobre Http.Sys, que permite a los usuarios crear un servidor Http independiente.

En este ejemplo se utilizan las siguientes funciones de HttpListener:
1. Autenticación
2. Habilitar SSL
3. Leer certificados de cliente sobre conexiones seguras


Implementaciones de lenguaje del ejemplo
===============================
     El ejemplo está disponible en las siguientes implementaciones de lenguaje:
     C#


Para generar el ejemplo mediante el símbolo del sistema:
=============================================

     1. Abra la ventana Símbolo del sistema de SDK y vaya al subdirectorio CS del directorio AspxHost.

     2. Escriba msbuild AspxHostCS.sln.


Para generar el ejemplo mediante Visual Studio:
=======================================

     1. Abra el Explorador de Windows y vaya al subdirectorio CS del directorio AspxHost.

     2. Haga doble clic en el icono del archivo .sln (solución) para abrir el archivo en Visual Studio.

     3. En el menú Generar, seleccione Generar solución.
     La aplicación se generará en el directorio \bin o \bin\Debug predeterminado.


Para ejecutar el ejemplo:
=================
     1. Desplácese hasta el directorio que contiene el nuevo archivo ejecutable mediante el símbolo del sistema o el Explorador de Windows.
     2. Escriba AspxHostCS.exe en la línea de comandos o haga doble clic en el icono de AspxHostCS.exe para iniciarlo desde el Explorador de Windows. 


Comentarios
======================
1. Información de la clase

El archivo AspxHostCS.cs contiene la clase principal que crea y configura un agente de escucha y una aplicación Aspx.

El archivo AspxVirtualRoot.cs contiene la clase que configura una clase HttpListener para escuchar en prefijos y esquemas de autenticación compatibles.

El archivo AspxNetEngine.cs contiene la clase que configura una aplicación Aspx asignándole un alias virtual que se asigna a un directorio físico.

El archivo AspxPage.cs contiene la clase que implementa la clase SimpleWorkerRequest y representa una página solicitada por el cliente.

El archivo AspxRequestInfo.cs contiene la clase del contenedor de datos que se utiliza para pasar datos importantes desde HttpListenerContext a la aplicación alojada.

El archivo AspxException.cs contiene la clase de excepción personalizada.

El directorio DemoPages contiene las páginas del ejemplo Aspx.


2. Uso del ejemplo
 
El archivo AspxHostCS.cs es la clase que contiene el método principal que iniciará una clase HttpListener y que configura un directorio físico como una aplicación Aspx alojada. De manera predeterminada, la clase intenta configurar el directorio DemoPages (que se encuentra en el mismo directorio de ejemplos) como una aplicación alojada bajo un alias virtual /.  Como la clase HttpListener de este ejemplo escucha en el puerto 80, es posible que tenga que detener IIS para ejecutar este ejemplo.

 
Cambiar el código para uso individual: 

                //Crear un objeto AspxVirtualRoot con un puerto http y un puerto https si fuese necesario
                
                AspxVirtualRoot virtualRoot = new AspxVirtualRoot(80);


                //Configurar un directorio físico como alias virtual.

                //TODO: Sustituir el directorio físico por el directorio que se va a configurar.

		virtualRoot.Configure("/", Path.GetFullPath(@"..\..\DemoPages"));


                //TODO: Si es necesario agregar autenticación, agregarla aquí

                //virtualRoot.AuthenticationSchemes = AuthenticationSchemes.Basic;


3. Establecer el esquema de autenticación
 
Después de configurar un objeto AspxVirtualRoot, establezca el esquema de autenticación necesario estableciendo el campo AuthenticationScheme en el objeto AspxVirtualRoot.

 
4. Habilitar Ssl
 

Para habilitar SSL, un certificado de servidor instalado en el almacén de la máquina debe estar configurado en el puerto en el que se requiere SSL. Para obtener más información sobre el modo de configurar certificados de servidor en un puerto mediante la utilidad Httpcfg.exe, consulte el vínculo Httpcfg.

 
Nota: Winhttpcertcfg también se puede utilizar para configurar certificados de servidor en un puerto. Consulte el vínculo Winhttpcertcfg.


Problema conocido
====================== 

Problema:
Cuando inicio la aplicación aparece el siguiente mensaje de error:

"System.IO.FileNotFoundException: No se puede cargar el archivo o ensamblado 'AspxHostCS, Version=1.0.1809.19805, Culture=neutral, PublicKeyToken=null' ni una de sus dependencias. El sistema no encuentra el archivo especificado. Nombre del archivo: 'AspxHostCS, Version=1.0.1809.19805, Culture=neutral, PublicKeyToken=null'"

Solución:
El archivo AspxHostCs.exe no está en el directorio bin del directorio físico que se está configurando. Copie el archivo AspxHostcs.exe en el directorio bin.


Vea también
============
Consulte la documentación de las API de HttpListener y Aspx Hosting en el SDK de .NET Framework SDK y consulte también MSDN.
