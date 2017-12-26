<?php
/***************************************
* www.program-o.com
* PROGRAM O 
* Version: 2.5.3
* FILE: chatbot/core/conversation/make_conversation.php
* AUTHOR: Elizabeth Perreau and Dave Morton
* DATE: MAY 17TH 2014
* DETAILS: this file contains the functions control the creation of the conversation 
***************************************/

  /**
   * function make_conversation()
   * A controller function to run the instructions to make the conversation
   *
   * @link http://blog.program-o.com/?p=1209
   * @param  array $convoArr - the current state of the conversation array
   * @return array $convoArr (updated)
   */
function make_conversation($convoArr){
	
	runDebug( __FILE__, __FUNCTION__, __LINE__, "Making conversation",4);
	//get the user input and clean it
	$convoArr['aiml']['lookingfor'] =  normalize_text($convoArr['user_say'][1]);
	//find an aiml match in the db
	$convoArr = get_aiml_to_parse($convoArr);
	$convoArr = parse_matched_aiml($convoArr,'normal');
		
	//parse the aiml to build a response
	//store the conversation
	$convoArr = push_on_front_convoArr('parsed_template',$convoArr['aiml']['parsed_template'],$convoArr);
	$convoArr = push_on_front_convoArr('template',$convoArr['aiml']['template'],$convoArr);
	//display conversation vars to user.
	$convoArr['conversation']['totallines']++;
	return $convoArr;
}

  /**
   * function buildNounList()
   * loads a nounlist from file and inserts it into the convo array
   *
   * @link     http://blog.program-o.com/?p=1212
   * @param array $convoArr
   * @internal param int $person
   * @internal param string $in
   * @return array $convoArr
   */

  function buildNounList($convoArr)
  {
    $fileName = _CONF_PATH_ . 'nounList.dat';
    $nounList = file($fileName,FILE_IGNORE_NEW_LINES);
    $convoArr['nounList'] = $nounList;
    return $convoArr;
  }


?>
