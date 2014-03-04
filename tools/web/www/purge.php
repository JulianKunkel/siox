<?php

require_once("include/DB.php"); 
require_once("include/SIOX.php"); 

SIOX::purge();

header("Location: index.php");
?>
