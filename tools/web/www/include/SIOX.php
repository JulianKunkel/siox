<?php

class SIOX {

static function purge()
{
	global $dbcon;

	$sql = "SELECT reset_all_tables()";
	$stmt = $dbcon->prepare($sql);

	if (!$stmt->execute()) {
		print_r($dbcon->errorInfo());
		die("Error purging SIOX database.");
	}

}

static function get_node_name($nid)
{
	global $dbcon;

	$sql = "SELECT * FROM topology.get_node_by_id(:nid)";
	
	$stmt = $dbcon->prepare($sql);
	$stmt->bindParam(':nid', $nid);

	if (!$stmt->execute()) {
		print_r($dbcon->errorInfo());
		die("Error querying node. Id=$nid.");
	}

	$row = $stmt->fetch(PDO::FETCH_OBJ);

	return $row->value;
}


}

?>
