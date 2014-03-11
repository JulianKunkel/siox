<?php

require_once("SIOX.php");

class Stats {

static function get_list()
{
	global $dbcon;

	$sql = "SELECT * FROM topology.get_statistics()";

	$stmt = $dbcon->prepare($sql);

	if (!$stmt->execute()) {
		print_r($dbcon->errorInfo());
		die("Error getting statistics.");
	}

	$list = array();

	while ($row = $stmt->fetch(PDO::FETCH_OBJ))
		$list[] = $row;

	return $list;
}


}

?>
