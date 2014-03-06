<?php

require_once("SIOX.php");

class Program {

static function get_count()
{
	global $dbcon;

	$sql = "SELECT count(*) FROM get_program_list()";
	$stmt = $dbcon->prepare($sql);

	if (!$stmt->execute()) {
		print_r($dbcon->errorInfo());
		die("Error getting program count.");
	}

	$row = $stmt->fetch(PDO::FETCH_OBJ);
	return $row->count;

}


static function get_attributes($procid)
{
	global $dbcon;

	$sql = "SELECT * FROM topology.get_attributes_by_proc_number(:pid)";

	$stmt = $dbcon->prepare($sql);
	$stmt->bindParam(':pid', $procid);

	if (!$stmt->execute()) {
		print_r($dbcon->errorInfo());
		die("Error getting program attributes.");
	}

	$attrs = array();

	while ($row = $stmt->fetch(PDO::FETCH_OBJ)) {
		$attrs[$row->key] = $row->val;
	}

	return $attrs;
}


static function get_list($page = 1, $page_size = 200)
{
	global $dbcon;

	$sql = "SELECT * FROM get_program_list() LIMIT :page_size OFFSET :page_offset";
	
	$stmt = $dbcon->prepare($sql);
	$stmt->bindParam(':page_size', $page_size);
	$stmt->bindValue(':page_offset', $page_size*($page-1));

	if (!$stmt->execute()) {
		print_r($dbcon->errorInfo());
		die("Error getting program list.");
	}

	$list = array();

	while ($row = $stmt->fetch(PDO::FETCH_OBJ)) {
		$row->attributes = self::get_attributes($row->childobjectid);
		$trimmed_id  = trim($row->childname, "()");
		$exploded_id = explode(',', $trimmed_id);
		$row->nid  = $exploded_id[0];
		$row->pid  = $exploded_id[1];
		$row->time = $exploded_id[2];
		$row->node = SIOX::get_node_name($row->nid);
		$list[] = $row;
	}

	return $list;

}


}

?>
