<?php

/**
 * Task view callback.
 */
function ad_view($ad) {
  drupal_set_title(entity_label('ad', $ad));
  return entity_view('ad', array(entity_id('ad', $ad) => $ad), 'full');
}



function ad_report_page(){
	
	
	//$res = db_query('select count(*) as total , type , adstatus from {ad} group by type, adstatus' )->fetchAll();
	$res = db_query('select count(*) as total , type  from {ad} group by type' )->fetchAll();
	//tr( $res );
	
	$rows = array();
	foreach ($res as $r) {
		$rows[] = array(
			$r->type,
			//$r->adstatus,
			$r->total,
		);
	}
	
	$build = array(
		'#theme' => 'table',
		'#rows' => $rows ,
		'#header' => array('ad type' ,/* 'ad status' ,*/ 'ad count'),
	);
	
	return $build;
	
}
