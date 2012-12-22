<?php

function ad2_roof_settings_form( &$form , &$form_state){
	
	
	$form['ad2_roof_min_default'] = array(
		'#type' => 'textfield',
		'#title' => t('Roof Minimum'),
		'#default_value' => variable_get('ad2_roof_min_default' , 20),
		'#size' => 5,
	);
	
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Go'),
	);
	
	return $form;
}


function ad2_roof_settings_form_validate($form , &$form_state){
	$v = $form_state['values']['ad2_roof_min_default'];
	if (! is_numeric($v)){
	  form_error($form['ad2_roof_min_default'] ,t('נא להזין ערך מספרי'));
	}
}

function ad2_roof_settings_form_submit($form , &$form_state){
	
	$v = $form_state['values']['ad2_roof_min_default'];
	variable_set('ad2_roof_min_default' , $v );
	
	drupal_set_message('Data Saved');
		
}

function ad2_roof_tasks_form( $form , &$form_state  ){
	
	$form['batch'] = array(
		'#type' => 'checkboxes',
		'#title' => t('Create Accounts'),
		'#options' => array(
			'run-daily-script' => 'run the daily script',
			'reset-non-auto' => 'reset non auto roofs',
		),
		'#size' => 5,
	);
	
	$form['actions']['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Go'),
	);
	
	return $form;
}


function ad2_roof_tasks_form_submit(  $form, &$form_state ){
	
	$operations = array();

	
	$i = 0;
	foreach ($form_state['values']['batch'] as $action => $do ) {
		if( $do ){
			switch ($action) {
				case 'run-daily-script':
					ad2_roof_sites_ajastment();
					break;
				case 'reset-non-auto':
					$rows = db_query("SELECT * FROM node n join ad_roof ar on ar.nid = n.nid where n.type = 'site' and ar.auto = 0 ")->fetchAll();
					foreach ($rows as $row) {
						$nid = $row->nid;
						$balance = account_balance($nid);	
		
						$n = date('t') - date('d') + 1 ;
						
						$roof = round(  $balance / $n , 2 );
			
						if( $roof < 0 )
							$roof = 0;
						
						//tr($nid . ' ' . $balance . ' ' . $roof . ' ' . $row->roof);
						
						db_update('ad2_roof')
							->condition('nid' ,  $nid )
							->fields(array(
								'roof' => $roof,
							))
							->execute()
							;
					}
					//tr( $rows );
					//die();
					break;
				default:
					break;
			}
		}
	}
	
}


function ad2_roof_report_page(){
	
	//ad2_roof_mark_ads_over();
	// $aids = array();
			// $query = new EntityFieldQuery();
			// $query	->entityCondition('entity_type', 'ad2')
					// //->entityCondition('bundle', 'article')
					// ->propertyCondition('over', 1)
					// //->propertyCondition('adstatus', 'active')
					// //->fieldCondition(AD2_SITE_FIELD, 'nid', $row->nid )
					// //->range(0, 10)
					// ->addMetaData('account', user_load(1))
					// ; // run the query as user 1
// 
			// $result = $query->execute();
			// if (isset($result['ad2'])) {
			  // $aids = array_keys($result['ad2']);
			  // //$news_items = entity_load('node', $news_items_nids);
			// }
			// tr( $aids );
			
	// $ad =ad2_load(3305 );
	// tr($ad->over);
	// // $ad->over = 1;
	// // ad2_save($ad );
	// // $ad2 = ad2_load(3305);
	// // tr($ad2->over );
	// //tr($ad);
	// die();
	
	$q = db_select('ad2_roof' , 'ar');
	$q->fields('ar');
	$q->join('node' , 'n' , 'ar.nid = n.nid');
	$q->fields('n' , array('title'));
	$q->condition('n.status' , 1 );
	$r = $q->execute()->fetchAll();
	
	$rows = array();
	
	$total = 0;
	
	foreach ($r as $v) {
		//tr( $r );
		
		$total += $v->roof;
		
		$rows[] = array(
			$v->nid,
			$v->title,
			$v->roof,
			$v->auto,
			//$v->over,
			// $v->sunday,
			// $v->monday,
			// $v->tuesday,
			// $v->wednesday,
			// $v->thursday,
			// $v->friday,
			// $v->saturday,
		);
	}
	
	$rows[] = array(
		'',
		'',
		$total,
	);
		
		
	$build['table'] = array(
		'#theme' => 'table',
		'#rows' => $rows,
		'#header' => array(
			'#', 
			'שם הצימר' ,
			 'תקרה יומית' ,
			 'ניהול אוטומטי',
			// 'עבר תקרה יומית'
			//'יום א',
			//'יום ב',
			//'יום ג',
			//'יום ד',
			//'יום ה',
			//'יום ו',
			//'יום ש',
		)
	);	
	
	return $build;
}

