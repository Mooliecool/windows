-- Program O SQL Dump
-- version 2.0.3
-- http://www.program-o.com
--
-- Host: localhost
-- Generation Time: Nov 29, 2011 at 10:52 PM
-- Server version: 5.1.60
-- PHP Version: 5.3.8

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";

--
-- Created for: Dave Morton
--

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;


-- --------------------------------------------------------

--
-- Table structure for table `wordcensor`
--

CREATE TABLE IF NOT EXISTS `wordcensor` (
  `censor_id` int(11) NOT NULL AUTO_INCREMENT,
  `word_to_censor` varchar(50) NOT NULL,
  `replace_with` varchar(50) NOT NULL DEFAULT '****',
  `bot_exclude` varchar(255) NOT NULL,
  PRIMARY KEY (`censor_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=31 ;

--
-- Dumping data for table `wordcensor`
--

INSERT INTO `wordcensor` (`censor_id`, `word_to_censor`, `replace_with`, `bot_exclude`) VALUES
(1, 'fucking', 'f^@%^^g', ''),
(2, 'fucker', 'f%%!^r', ''),
(3, 'fucked', 'f!^#&d', ''),
(4, 'fuck', 'f%*k', ''),
(5, 'shit', 's**t', ''),
(6, 'tits', 't@&s', ''),
(7, 'tit', 't!t', ''),
(8, 'cunts', 'c@!#s', ''),
(9, 'cunt', 'c##t', ''),
(10, 'bastards', 'b#^^%*@s', ''),
(11, 'bastard', 'b*%@@!d', ''),
(12, 'piss', 'p%*s', ''),
(13, 'cock', 'c$%k', ''),
(14, 'slut', 's@$t', ''),
(15, 'bitch', 'b&^*h', ''),
(16, 'penis', 'p^*!s', ''),
(17, 'pussies', 'p#*$%%s', ''),
(18, 'pussy', 'p!@*y', ''),
(19, 'whores', 'w#$&#s', ''),
(20, 'whore', 'w!&!e', ''),
(21, 'twats', 't@@^s', ''),
(22, 'twat', 't^&t', ''),
(23, 'dick', 'd@^k', ''),
(24, 'nigga', 'n#%^a', ''),
(25, 'niggers', 'n#^%&#s', ''),
(26, 'nigger', 'n&%$!r', ''),
(27, 'vaginas', 'v&%^%!s', ''),
(28, 'vagina', 'v!@*^a', ''),
(29, 'viagra', 'v$$!&a', ''),
(30, 'cialis', 'c*&$!s', '');
