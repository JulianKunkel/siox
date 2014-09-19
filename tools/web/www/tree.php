<?php

require_once "Image/GraphViz.php";
require_once "include/DB.php";

$top    = $_GET["top"];
$levels = $_GET["levels"];

function add_nodes(&$dot, $parent = 0)
{
	global $dbcon;

	$sql = "SELECT * FROM topology.relation WHERE parentobjectid = :parent ORDER BY childobjectid ASC";

	$stmt = $dbcon->prepare($sql);
	$stmt->bindParam(':parent', $parent);

	if (!$stmt->execute()) {
		print_r($dbcon->errorInfo());
		die("Error querying topology. Parent = $parent.");
	}

	while ($row = $stmt->fetch(PDO::FETCH_OBJ)) {
		$dot->addNode($row->childobjectid, array('style' => 'filled', 'fillcolor' => 'greenyellow', 'label' => $row->childname));
		$dot->addEdge(array($parent => $row->childobjectid));	
		add_nodes($dot, $row->childobjectid);
	}
}

$gv = new Image_GraphViz();
$gv->addNode($top, array('style' => 'filled', 'fillcolor' => 'black', 'label' => 'root'));
add_nodes($gv, $top);

header("Content-type: image/svg");
$gv->image();
?>
