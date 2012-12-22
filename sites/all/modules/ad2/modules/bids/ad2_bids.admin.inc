<?php



function ad2_bids_admin_autobids($form_state) {
	
	$autobids = variable_get('ad2_autobids_table' , array());
	
	//$autobids = _had_auto_default_values_info();
	
	//tr( $autobids );
	
	//tr( $positions );
	
	$form['autobids'] = array(
		'#type' => 'container',
		'#tree' => true,
	);
	
	
	for($x = 0 ; $x < 20 ; $x++ ){
		
		$form['autobids'][$x] =  array(
			'#type' => 'container',
			'#tree' => true,
		);
		
		$form['autobids'][$x]['min'] = array(
			'#type' => 'textfield',
			'#title' => 'מינימום ' ,
			'#title_display' => 'invisible',
			'#default_value' => $autobids[$x]['min'], 
			'#size' => 5,
		);
		
		$form['autobids'][$x]['max'] = array(
			'#type' => 'textfield',
			'#title' => 'מקיסימום' ,
			'#title_display' => 'invisible',
			'#default_value' => $autobids[$x]['max'], 
			'#size' => 5,
		);
		
		$form['autobids'][$x]['bid'] = array(
			'#type' => 'textfield',
			'#title' => 'מחיר בסיס ' ,
			'#title_display' => 'invisible',
			'#default_value' => $autobids[$x]['bid'], 
			'#size' => 5,
		);
		
		$form['autobids'][$x]['bid_max'] = array(
			'#type' => 'textfield',
			'#title' => 'מחיר מקסימלי ' ,
			'#title_display' => 'invisible',
			'#default_value' =>$autobids[$x]['bid_max'], 
			'#size' => 5,
		);
		
	}
	
	$form['save'] = array(
		'#type' => 'submit',
		'#value' => t('Save'),
	);
	
	return $form;
}

function ad2_bids_admin_autobids_submit($form , &$form_state) {
	
	variable_set('ad2_autobids_table' , $form_state['values']['autobids'] );
	drupal_set_message('Data saved');
	
}

function theme_ad2_bids_admin_autobids( &$vars ){
	
	$form = $vars['form'];
	$rows = array();
	
	for($x = 0 ; $x < 20 ; $x++ ){
		//unset($form['autobids'][$x]['#title'] );
		
		$rows[] = array(
			 ($x+1),
			drupal_render($form['autobids'][$x]['min']),
			drupal_render($form['autobids'][$x]['max']),
			drupal_render($form['autobids'][$x]['bid']),
			drupal_render($form['autobids'][$x]['bid_max']),
		);
	}
	
	$html = "";
	$html .= theme('table' , array('rows' => $rows , 'header' => array('#'  ,  'תקציב מינימום' , 'תקציב מקסימום' , 'מחיר בסיס' , 'מחיר מקסימלי' ) ) );
	$html .= drupal_render($form['submit']);
	$html .= drupal_render_children( $form );
	
	return $html;
	
}



function ad2_bids_bysite_report(){

	
	$build['form'] = drupal_get_form('ad2_bids_bysite_form');
	
	$nid = isset($_GET['nid']) ? $_GET['nid'] : null;
	
	if( $nid ){
		$q = db_select('ad2_bids_auto_log' , 'a');
		$q->fields('a');
		$q->condition('nid' , $nid );
		$q->orderBy('timestamp' , 'desc');
		$results = $q->execute()->fetchAll();
		
		$hours = _had_auto_clicks_by_hour_info();
		
		$rows = array();
		foreach ($results as $result ) {
				
			$h = date('G' , $result->timestamp );
			$p = $hours[$h];
			
			$row = array(
				date('d/m/Y H:i' , $result->timestamp),
				$result->roof,
				$result->done,
				round( 100 * $result->done / $result->roof , 2 ),
				$p,
				$result->bid,
			);
			
			$rows[] = $row;
		}
		
		$build['table'] = array(
			'#theme' => 'table',
			'#rows' => $rows,
			'#header' => array('time' ,'roof' , 'done' , '% done' , '% hour' , 'bid'),
			'#empty' => 'no data',
		);
	}
	
	
	return $build;
}



function ad2_bids_bysite_form(&$form , &$form_state ){
	
	
	$options = db_query('select nid, title from node where type=:type and status = 1 order by title' , array(':type' => 'site'))->fetchAllKeyed();
	
	//tr( $options );
	
	$form['nid'] = array(
		'#type' => 'select',
		'#options' => $options,
		'#default_value' => isset($_GET['nid']) ? $_GET['nid'] : null,
		'#title' => 'בחר צימר',
	);
	
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Go'),
	);
	
	return $form;
}


function ad2_bids_bysite_form_submit($form , &$form_state ){
		
	$nid = $form_state['values']['nid'];
	
	
	$form_state['redirect'] = array(
    	request_path(),
    	array('query' => array('nid' => $nid)),
  );
  
}
	

function ad2_bids_report(){
	
		
	$q = db_select('ad2_bids_auto_log' , 'a');
	$q->fields('a');
	//$q->condition('nid' , $nid );
	
	$q->join('node' , 'n' , 'a.nid=n.nid');
	$q->fields('n' , array('title'));
	
	$q->orderBy('title');
	$q->orderBy('timestamp' , 'desc');
	
	$q->condition('timestamp' , time() - 24*60*60 , '>' );
	//$q->range(0,20000);
	$results = $q->execute()->fetchAll();
	
	$sets = array();
	foreach ($results as $result ) {
		$sets[$result->nid][] = $result;
	}

	$hours = ad2_bids_clicks_by_hour_info();// _had_auto_clicks_by_hour_info();
				
	$rows = array();
	foreach ($sets as $nid => $set ) {
		
		//$p = 0;
		
		$h = date('G' , $set[0]->timestamp );
		$p = $hours[$h];
		
		$row = array(
			$nid,
			$set[0]->title,
			date('d/m/Y H:i' , $set[0]->timestamp),
			$set[0]->roof,
			$set[0]->done,
			round( 100 * $set[0]->done / $set[0]->roof , 2 ),
			$p,
			$set[0]->bid,
			l('history' , 'admin/reports/had/auto/bysite' , array('query' => array('nid' => $nid)) ),
		);
		
		$rows[] = $row;
	}
	
	$build['cron'] = array(
		'#prefix' => 'ריצת קרון אחרונה: ',
		'#markup' => date('d-m-Y H:i:s' , variable_get('had_bids_auto_cron_last' , 0 ) ),
	);
	$build['table'] = array(
		'#theme' => 'table',
		'#rows' => $rows,
		'#header' => array('#' , 'site' , 'time' , 'roof' , 'done' , '% done' , '% hour' , 'bid' , ''),
		'#empty' => 'no data',
	);

	return $build;
}
	