<?php 

require_once "include/DB.php"; 
require_once "include/Activity.php"; 

header("Content-type: image/svg");
Activity::print_dot($_GET['unique_id']); 

?>