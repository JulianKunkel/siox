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

static function get_node_list()
{
	global $dbcon;

	$sql = "SELECT get_all_nodes() AS node";

	$stmt = $dbcon->prepare($sql);
	
	if (!$stmt->execute()) {
		print_r($dbcon->errorInfo());
		die("Error querying node list.");
	}

	$list = array();

	while ($row = $stmt->fetch(PDO::FETCH_OBJ))
		$list[] = $row->node;

	return $list;
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


static function get_user_list()
{
	global $dbcon;

	$sql = "SELECT get_all_users() AS user";

	$stmt = $dbcon->prepare($sql);
	
	if (!$stmt->execute()) {
		print_r($dbcon->errorInfo());
		die("Error querying user list.");
	}

	$list = array();

	while ($row = $stmt->fetch(PDO::FETCH_OBJ))
		$list[] = $row->user;

	return $list;

}

static function get_cmd_list()
{
	global $dbcon;

	$sql = "SELECT get_all_cmds() AS cmd";

	$stmt = $dbcon->prepare($sql);
	
	if (!$stmt->execute()) {
		print_r($dbcon->errorInfo());
		die("Error querying command list.");
	}

	$list = array();

	while ($row = $stmt->fetch(PDO::FETCH_OBJ)) {
		$cmd = basename($row->cmd, " ");
		$list[] = $cmd;
	}

	return $list;
}


}

?>
