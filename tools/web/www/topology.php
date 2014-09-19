<?php
require_once("include/Topology.php");
$site_title = "Topologic Overview";
$site_description = "SIOX topology stored in the database.";
require_once("header.php");

$top    = isset($_GET["top"])    ? $_GET["top"]    : 0;
$levels = isset($_GET["levels"]) ? $_GET["levels"] : 2;

$parents = Topology::get_all_parents(); 

?>

<h1>Current Topology</h1>

<form name="topology_frm" method="post" action="index.php">
	<input type="submit" value="Program overview" />
</form>

<img src="tree.php?top=<?php echo $top?>&amp;levels=<?php echo $levels?>" title="Topology tree" alt="Topology tree" />

<?php
require_once("footer.php");
?>
