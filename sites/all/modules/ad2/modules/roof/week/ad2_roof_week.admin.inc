<?php




function theme_ad2_roof_week_table( $variables ){
  
  

  $days = $variables['form']['#days'];
  //tr($days);
  $row 	= array();
  $rows 	= array();

  foreach ($days as $day => $value){
    //tr($variables['form'][$day]);
    array_push($row,drupal_render( $variables['form'][$day] ));
  }
  //tr($variables['form']['submit']);
  $submit = drupal_render($variables['form']['submit']);
  $help = drupal_render($variables['form']['help']);
  $rows[] = array('data' => $row);
  
  $header = array(t('ראשון'),t('שני'),t('שלישי'),t('רבעי'),t('חמישי'),t('שישי'),t('שבת'),
  );

	$build = array(
		'#theme' => 'table',
		'#rows' => $rows,
		'#header' => $header,
		'#prefix' => '<div id="had-week-roof-results">',
		'#suffix' => $help .'<div id="had-week-roof-submit">'. $submit .'</div></div>',
		//'#footer' => drupal_render($variables['form']['submit']),
	);

	return  drupal_render($build);
}

function ad2_roof_week_report_page(){
	
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
			$v->sunday,
			$v->monday,
			$v->tuesday,
			$v->wednesday,
			$v->thursday,
			$v->friday,
			$v->saturday,
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
			// 'עבר תקרה יומית',
			'יום א',
			'יום ב',
			'יום ג',
			'יום ד',
			'יום ה',
			'יום ו',
			'יום ש',
		)
	);	
	
	return $build;
}

