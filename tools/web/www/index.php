<?php 
require_once("include/DB.php"); 
require_once("include/Activity.php"); 
$site_title = "Activity overview";
$site_description = "Overview of the stored activities";
require_once("header.php"); 
?>

<h1>Activity List</h1>

<?php $page_size = 200; ?>
<?php $total_activities = Activity::get_count(); ?>
<?php $total_pages = max(ceil($total_activities / $page_size), 1); ?>
<?php $current_page = isset($_GET['page']) ? $_GET['page'] : $total_pages; ?>
<?php $activities = Activity::get_list($current_page, $page_size); ?>

<form name="purge_frm" method="post" action="purge.php">
	<input type="submit" value="Purge all actvities" onclick="return confirm('Dude, you are gonna delete all data. Are you sure?')" />
</form>

<br /><br />

<table cellspacing="0" cellpadding="0">
<thead>
	<tr>
		<th colspan="2" style="text-align: left"><a href="?page=1">↶ first</a>&nbsp;&nbsp;<a href="?page=<?=$current_page == 1 ? 1 : $current_page-1?>">← previous</a></th>
		<th colspan="2" style="text-align: center"><?=$current_page?> / <?=$total_pages?></th>
		<th colspan="2" style="text-align: right"><a href="?page=<?=$current_page == $total_pages ? $current_page : $current_page+1?>">next →</a>&nbsp;&nbsp;<a href="?page=<?=$total_pages?>">last ↷</a></th>
	</tr>
	<tr>
		<th>#</th>
		<th>Function</th>
		<th>Time start</th>
		<th>Time stop</th>
		<th>Duration [µs]</th>
		<th>Error code</th>
	</tr>
</thead>
<tbody>
<?php if (count($activities) == 0): ?>
	<tr class="<?=$i++ % 2 == 0 ? "even" : "odd";?>" onclick="window.location='activity.php?unique_id=<?=$a->unique_id?>'">
		<td colspan="6" style="text-align: center;">The activity database is empty.</td>
	</tr>
<?php endif ?>
<?php $i = 0; ?>
<?php foreach ($activities as $a): ?>
	<tr class="<?=$i++ % 2 == 0 ? "even" : "odd";?>" onclick="window.location='activity.php?unique_id=<?=$a->unique_id?>'">
		<td><?=$a->unique_id?></td>
		<td><?=$a->name?></td>
		<td><?=date("d.m.Y H:i:s", floor($a->time_start / 1000000000)).".<b>".($a->time_start % 1000000000)."</b>"?></td>
		<td><?=date("d.m.Y H:i:s", floor($a->time_stop / 1000000000)).".<b>".($a->time_stop % 1000000000)."</b>"?></td>
		<td align="center"><?=round(($a->time_stop - $a->time_start) / 1000, 3)?></td>
		<td><?=$a->error_value?></td>
	</tr>
<?php endforeach ?>
</tbody>
<tfoot>
	<tr>
		<th colspan="2" style="text-align: left"><a href="?page=1">↶ first</a>&nbsp;&nbsp;<a href="?page=<?=$current_page == 1 ? 1 : $current_page-1?>">← previous</a></th>
		<th colspan="2" style="text-align: center"><?=$current_page?> / <?=$total_pages?></th>
		<th colspan="2" style="text-align: right"><a href="?page=<?=$current_page == $total_pages ? $current_page : $current_page+1?>">next →</a>&nbsp;&nbsp;<a href="?page=<?=$total_pages?>">last ↷</a></th>
	</tr>
</tfoot>
</table>

<?php require_once("footer.php"); ?>
