<?php

require_once("include/DB.php");

class Topology {

static function get_all_parents()
{
	global $dbcon;

	$sql = "SELECT DISTINCT childname, childobjectid FROM topology.relation ORDER BY childobjectid ASC";

	$stmt = $dbcon->prepare($sql);
	
	if (!$stmt->execute()) {
		print_r($dbcon->errorInfo());
		die("Error query parent list.");
	}

	$list = array();
	while ($row = $stmt->fetch(PDO::FETCH_OBJ))
		$list[] = $row;

	return $list;
}


}

?>
