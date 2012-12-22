<?

/**
 * Form builder; Configure access logging.
 *
 * @ingroup forms
 * @see system_settings_form()
 */
function ad2_log_settings_form() {
	
  $form['access'] = array(
    '#type' => 'fieldset',
    '#title' => t('Had log settings'),
  );
  $form['access']['ad2_log_enable_node_access_log'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Ad2 log'),
    '#default_value' => variable_get('ad2_log_enable_node_access_log', 0),
    '#description' => t('Log each page access. Required for referrer statistics.'),
  );
  
  return system_settings_form($form);
}


/**
 * @file
 * Admin page callbacks for the statistics module.
 */

/**
 * Menu callback; presents the "recent hits" page.
 */
function ad2_log_pending() {
	
	//$form = drupal_get_form('ad2_log_recent_filter_form');
	
	//$num = had_log_processor_process();
	
	$q = db_select('ad2_log' , 'a')->extend('PagerDefault');
	$q	->fields('a')
		->condition('done' , 0 )
		->orderBy('timestamp' , 'desc')
		->limit(100)
		;
	if( isset($_GET['nid']) ){
		$q->condition('nid' , $_GET['nid'] );
	}
	$lines = $q->execute()->fetchAll();
	
	$rows = array();
	foreach ($lines as $line ) {
		$rows[] =  array(
			$line->id, 
			$line->nid, 
			$line->aid, 
			$line->bid, 
			$line->uid,
			date('d-m-Y H:i:s' , $line->timestamp), 
			//$line->filter, 
			//$line->action, 
			//$line->description, 
			$line->referer, 
			//$line->done, 
			//$line->hostname, 
			//$line->sid, 
		);
	}	
	//tr( $rows );
	
	$headers = array(
		'id',
		'nid',
		'aid',
		'bid',
		'uid',
		'date',
		//'filtered',
		//'action',
		//'description',
		'referer',
		//'done',
	);
	
	$build['process'] = array(
		'#markup' => l('Process now' , 'admin/reports/ad2/log/process')
	);
	//$build['form'] = $form;
	$build['table'] = array(
		'#theme' => 'table',
		'#rows' => $rows,
		'#header' => $headers,
	);
	$build['had_log_pager'] = array('#theme' => 'pager');
	return $build;
}



/**
 * Menu callback; presents the "recent hits" page.
 */
function ad2_log_done() {
	
	$form = drupal_get_form('ad2_log_recent_filter_form');
	
	//$num = had_log_processor_process();
	
	$q = db_select('ad2_log' , 'a')->extend('PagerDefault');
	$q	->fields('a')
		->condition('done' , 1 )
		->orderBy('timestamp' , 'desc')
		->limit(100)
		;
	if( isset($_GET['nid']) ){
		$q->condition('nid' , $_GET['nid'] );
	}
	$lines = $q->execute()->fetchAll();
	
	$rows = array();
	foreach ($lines as $line ) {
		$rows[] =  array(
			$line->id, 
			$line->nid, 
			$line->aid, 
			$line->bid, 
			$line->uid,
			date('d-m-Y H:i:s' , $line->timestamp), 
			$line->filter, 
			$line->action, 
			$line->description, 
			$line->referer, 
			//$line->done, 
			//$line->hostname, 
			//$line->sid, 
		);
	}	
	//tr( $rows );
	
	$headers = array(
		'id',
		'nid',
		'aid',
		'bid',
		'uid',
		'date',
		'filtered',
		'action',
		'description',
		'referer',
		//'done',
	);
	
	// $build['process'] = array(
		// '#markup' => l('Process now' , 'admin/reports/ad2/log/process')
	// );
	$build['form'] = $form;
	$build['table'] = array(
		'#theme' => 'table',
		'#rows' => $rows,
		'#header' => $headers,
	);
	$build['had_log_pager'] = array('#theme' => 'pager');
	return $build;
}


/**
 * Menu callback; Displays recent page accesses.
 */
function ad2_log_log($aid) {
	
	die();
  $access = db_query('SELECT a.*, u.name FROM {accesslog} a LEFT JOIN {users} u ON a.uid = u.uid WHERE aid = :aid', array(':aid' => $aid))->fetch();
  if ($access) {
    $rows[] = array(
      array('data' => t('URL'), 'header' => TRUE),
      l(url($access->path, array('absolute' => TRUE)), $access->path)
    );
    // It is safe to avoid filtering $access->title through check_plain because
    // it comes from drupal_get_title().
    $rows[] = array(
      array('data' => t('Title'), 'header' => TRUE),
      $access->title
    );
    $rows[] = array(
      array('data' => t('Referrer'), 'header' => TRUE),
      ($access->url ? l($access->url, $access->url) : '')
    );
    $rows[] = array(
      array('data' => t('Date'), 'header' => TRUE),
      format_date($access->timestamp, 'long')
    );
    $rows[] = array(
      array('data' => t('User'), 'header' => TRUE),
      theme('username', array('account' => $access))
    );
    $rows[] = array(
      array('data' => t('Hostname'), 'header' => TRUE),
      check_plain($access->hostname)
    );

    $build['statistics_table'] = array(
      '#theme' => 'table',
      '#rows' => $rows,
    );
    return $build;
  }
  else {
    drupal_not_found();
  }
}

function ad2_log_recent_filter_form( &$form , &$form_state ){
	
	$sites = db_query("select nid,title from node where type = :type order by title"  , array(':type' => 'site') )->fetchAllKeyed();
	
	
	
	$form['nid'] = array(
	'#title' => 'בחר צימר',
    '#type' => 'select',
    '#default_value' => isset( $_GET['nid'] ) ? (int) $_GET['nid'] : '0',
    '#options' => array('0' => 'all') +  $sites,
    //'#value' => t('Filter'),
  );
	
	
	
	// $form['date'] = array(
	// '#title' => 'בחר צימר',
    // '#type' => 'select',
    // '#default_value' => isset( $_GET['nid'] ) ? (int) $_GET['nid'] : '0',
    // '#options' => array('0' => 'all') +  $sites,
    // //'#value' => t('Filter'),
  // );
	
	
	$form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Filter'),
  );
	
	
	return $form;
}


function ad2_log_recent_filter_form_submit( $form , &$form_state ){
	
	$nid = $form_state['values']['nid'];
	
	$options = array('query' => array());
	
	if( $nid ){
		$options['query']['nid'] =(int) $nid;
	}
	
	$form_state['redirect'] = array(request_path(),$options);
  
}


