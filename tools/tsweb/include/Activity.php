<?php

class Activity {

static function get_list()
{
	global $dbcon;

	$sql = "SELECT * FROM activity.activities AS a, activity.activity_ids AS b WHERE a.unique_id = b.unique_id";
	$stmt = $dbcon->prepare($sql);
	if (!$stmt->execute())
		die("Error querying activity list.");

	$list = array();

	while ($row = $stmt->fetch(PDO::FETCH_OBJ)) {
		$list[] = $row;
	}

	return $list;
}


static function get_activity($unique_id)
{
	global $dbcon;

	$sql = "SELECT * FROM activity.activities AS a, activity.activity_ids AS b WHERE a.unique_id = b.unique_id AND a.unique_id = :unique_id";
	$stmt = $dbcon->prepare($sql);
	$stmt->bindParam(':unique_id', $unique_id);

	if (!$stmt->execute())
		die("Error querying activity.");

	$act = $stmt->fetch(PDO::FETCH_OBJ);

	$act->parents = self::get_parent_ids($unique_id);
	$act->children = self::get_child_ids($unique_id);
	$act->remote_calls = self::get_remote_call_ids($unique_id);

	return $act;
}


static function get_remote_call_ids($unique_id)
{
	global $dbcon;

	$sql = "SELECT unique_id FROM activity.remote_call_ids WHERE activity_uid = :unique_id";
	$stmt = $dbcon->prepare($sql);
	$stmt->bindParam(':unique_id', $unique_id);

	if (!$stmt->execute())
		die("Error querying activity.");

	$remote_calls = array();

	while ($row = $stmt->fetch(PDO::FETCH_OBJ)) 
		$remote_calls[] = $row->unique_id;

	return $remote_calls;
}


static function get_activity_id($unique_id, $recursive = 0)
{
	global $dbcon;

	$sql = "SELECT * FROM activity.activity_ids WHERE unique_id = :unique_id";
	$stmt = $dbcon->prepare($sql);
	$stmt->bindParam(':unique_id', $unique_id);

	if (!$stmt->execute())
		die("Error querying activity.");

	$act = $stmt->fetch(PDO::FETCH_OBJ);

	$act->parents = array();
	$act->children = array();

	if ($recursive == 1 || $recursive == 3) 
		foreach (self::get_parent_ids($unique_id) as $p) 
			$act->parents[] = &self::get_activity_id($p, 1);
		
	if ($recursive == 2 || $recursive == 3) 
		foreach (self::get_child_ids($unique_id) as $c) 
			$act->children[] = self::get_activity_id($c, 2);

	return $act;
}


static function get_parent_ids($unique_id)
{
	global $dbcon;

	$sql = "SELECT parent_id FROM activity.parents WHERE child_id = :unique_id";
	$stmt = $dbcon->prepare($sql);
	$stmt->bindParam(':unique_id', $unique_id);

	$parents = array();

	if (!$stmt->execute())
		die("Error querying parent list.");

	while ($row = $stmt->fetch(PDO::FETCH_OBJ))
		$parents[] = $row->parent_id;

	return $parents;

}


static function get_child_ids($unique_id)
{
	global $dbcon;

	$sql = "SELECT * FROM activity.parents WHERE parent_id = :unique_id";
	$stmt = $dbcon->prepare($sql);
	$stmt->bindParam(':unique_id', $unique_id);

	$children = array();

	if (!$stmt->execute())
		die("Error querying children list.");

	while ($row = $stmt->fetch(PDO::FETCH_OBJ))
		$children[] = $row->child_id;

	return $children;

}


static function add_child_edges(&$gv, $children, $parent)
{
	foreach ($children as $child) {
		$gv->addEdge(array($parent->unique_id => $child->unique_id));
		if (!empty($child->children))
			self::add_child_edges($gv, $child->children, $child);
	}
}


static function add_parent_edges(&$gv, $parents, $child)
{
	foreach ($parents as $parent) {
		$gv->addEdge(array($parent->unique_id => $child->unique_id));
		if (!empty($parent->parents))
			self::add_parent_edges($gv, $parent->parents, $parent);
	}
}

static function print_dot($unique_id)
{
	require_once "Image/GraphViz.php";

	$act = Activity::get_activity_id($unique_id, 3);

	$gv = new Image_GraphViz();

	$gv->addNode($unique_id, array('style' => 'filled', 'fillcolor' => 'greenyellow', 'label' => "**Function name**\nID: $unique_id\nTimestamps: (x, y)"));

	self::add_parent_edges($gv, $act->parents, $act);
	self::add_child_edges($gv, $act->children, $act);
	
	$gv->image();
}


}

?>