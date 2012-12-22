<?php

function ad2_batch_view_page($id ){
	//tr( $id );
	
	$q = db_select('ad2_batch' , 'ab');
	$q->fields('ab');
	$q->condition('id' , $id);
	
	$row = $q->execute()->fetchObject();
	
	$row->data = unserialize($row->data);
	
	//tr( $row );
	
	return 'data = ' . '<pre dir="ltr">' . print_r($row,true) . '</pre>';
}

function ad2_batch_page( ){
	
	
	
	
	$q = db_select('ad2_batch' , 'ab')->extend('PagerDefault');
	
	$q->fields('ab' );
	$q->orderby('id' , 'desc');
	$q->limit(15);
	
	$results = $q->execute()->fetchAll();
	
	$rows = array();
	foreach ($results as $line) {
		$row = array(
			$line->id,
			date('d-m-Y H:i:s' , $line->created),
			'<pre dir="ltr">' . print_r(unserialize($line->data),true) . '</pre>',
			//l('view' , 'admin/reports/had/batch1/' . $line->id ),

		);
		
		$rows[] = $row;
	}
	//tr( $rows );
	
	$build['table'] = array(
		'#theme' => 'table',
		'#rows' => $rows,
		'#header' => array('#' , 'Date' , 'data'),
	);
	
	
	$build['pager'] = array(
		'#theme' => 'pager',
	);
	
	return $build;
}
