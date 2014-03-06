<?php 
require_once("include/DB.php"); 
require_once("include/Activity.php"); 
require_once("include/Program.php"); 
$site_title = "Program Execution Overview";
$site_description = "Overview of the stored program runs";
require_once("header.php"); 
?>

<h1>Program Execution List</h1>

<?php $page_size = 200; ?>
<?php $total_runs = Program::get_count(); ?>
<?php $total_pages = max(ceil($total_runs / $page_size), 1); ?>
<?php $current_page = isset($_GET['page']) ? $_GET['page'] : $total_pages; ?>
<?php $runs = Program::get_list($current_page, $page_size); ?>

<form name="purge_frm" method="post" action="purge.php">
	<input type="submit" value="Purge database" onclick="return confirm('You are about to delete all data. Are you sure?')" />
</form>

<br /><br />

<table cellspacing="0" cellpadding="0">
<thead>
	<tr>
		<th colspan="1" style="text-align: left"><a href="?page=1">↶ first</a>&nbsp;&nbsp;<a href="?page=<?=$current_page == 1 ? 1 : $current_page-1?>">← previous</a></th>
		<th colspan="3" style="text-align: center"><?=$current_page?> / <?=$total_pages?></th>
		<th colspan="1" style="text-align: right"><a href="?page=<?=$current_page == $total_pages ? $current_page : $current_page+1?>">next →</a>&nbsp;&nbsp;<a href="?page=<?=$total_pages?>">last ↷</a></th>
	</tr>
	<tr>
		<th>Start</th>
		<th>Command</th>
		<th>Duration</th>
		<th>Node</th>
		<th>User</th>
	</tr>
</thead>
<tbody>
<?php if (count($runs) == 0): ?>
	<tr class="even">
		<td colspan="6" style="text-align: center;">The activity database is empty.</td>
	</tr>
<?php endif ?>
<?php $i = 0; ?>
<?php foreach ($runs as $r): ?>
	<tr class="<?=$i++ % 2 == 0 ? "even" : "odd";?>" onclick="window.location='activities.php?nid=<?=$r->nid?>&amp;pid=<?=$r->pid?>&amp;time=<?=$r->time?>'">
		<td></td>
		<td><?=$r->attributes['description/commandLine'];?></td>
		<td></td>
		<td><?=$r->node?></td>
		<td><?=$r->attributes['description/user-name'];?></td>
	</tr>
<?php endforeach ?>
</tbody>
<tfoot>
	<tr>
		<th colspan="1" style="text-align: left"><a href="?page=1">↶ first</a>&nbsp;&nbsp;<a href="?page=<?=$current_page == 1 ? 1 : $current_page-1?>">← previous</a></th>
		<th colspan="3" style="text-align: center"><?=$current_page?> / <?=$total_pages?></th>
		<th colspan="1" style="text-align: right"><a href="?page=<?=$current_page == $total_pages ? $current_page : $current_page+1?>">next →</a>&nbsp;&nbsp;<a href="?page=<?=$total_pages?>">last ↷</a></th>
	</tr>
</tfoot>
</table>

<?php require_once("footer.php"); ?>
