<?php

  /***************************************
  * http://www.program-o.com
  * PROGRAM O
  * Version: 2.5.3
  * FILE: chatbot/core/aiml/load_aimlfunctions.php
  * AUTHOR: Elizabeth Perreau and Dave Morton
  * DATE: MAY 17TH 2014
  * DETAILS: this file contains all the includes that are needed to load all the aiml functions
  ***************************************/
  include_once ("find_aiml.php");
  include_once ("parse_aiml.php");
  include_once ("parse_aiml_as_XML.php");
  runDebug(__FILE__, __FUNCTION__, __LINE__, "AIML function include files loaded", 4);
?>