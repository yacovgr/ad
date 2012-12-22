<?php

/**
 * Task view callback.
 */
function ad2_view($ad) {
  drupal_set_title(entity_label('ad2', $ad));
  return entity_view('ad2', array(entity_id('ad2', $ad) => $ad), 'full');
}



function ad2_report_page(){
	
	
	$res = db_query('select count(*) as total , type , adstatus from ad2 group by type, adstatus' )->fetchAll();
	//tr( $res );
	
	$rows = array();
	foreach ($res as $r) {
		$rows[] = array(
			$r->type,
			$r->adstatus,
			$r->total,
		);
	}
	
	$build = array(
		'#theme' => 'table',
		'#rows' => $rows ,
		'#header' => array('ad type' , 'ad status' , 'ad count'),
	);
	
	return $build;
	return 'here report page: put number of ads and thier types / statuses';
}
