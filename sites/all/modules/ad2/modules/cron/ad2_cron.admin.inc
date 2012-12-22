<?

function _ad2_cron_rehash(){
	
	// These are the jobs the function will return.
  $jobs = array();
  // These are the jobs defined by code and modified by the database.
  $current_jobs = array();
  // These are {job}.id values to be kept.
  $ids = array();
  $or = db_or();
  foreach (module_implements('ad2_cron_info') as $module) {
    $module_jobs = module_invoke($module, 'ad2_cron_info');
    foreach ($module_jobs as $delta => $job) {
    	//$job = array();
		$job['title'] = $job['info'];
      // Compile a condition to retrieve this job from the database.
      $condition = db_and()
        ->condition('module', $module)
        ->condition('delta', $delta);
      $or->condition($condition);
      // Add identifiers.
      $job['module'] = $module;
      $job['delta']  = $delta;
      //$block['theme']  = $theme;
      $current_jobs[$module][$delta] = $job;
    }
  }
  //tr($current_jobs);
  $code_jobs = $current_jobs;
  
  if( count($current_jobs)){
  $database_jobs = db_select('ad2_cron', 'ac')
    ->fields('ac')
    ->condition($or)
    //->condition('theme', $theme)
    ->execute();
  } else {
  	$database_jobs = array();
  }
	
	
  foreach ($database_jobs as $job) {
    // Preserve info which is not in the database.
    $job->info = $current_jobs[$job->module][$job->delta]['info'];
    // The cache mode can only by set from hook_block_info(), so that has
    // precedence over the database's value.
    // if (isset($current_blocks[$block->module][$block->delta]['cache'])) {
      // $block->cache = $current_blocks[$block->module][$block->delta]['cache'];
    // }
    // Blocks stored in the database override the blocks defined in code.
    $current_jobs[$job->module][$job->delta] = get_object_vars($job);
    // Preserve this block.
    $ids[$job->id] = $job->id;
  }
  //tr( $ids );
  foreach ($current_jobs as $module => $module_jobs) {
    foreach ($module_jobs as $delta => $job) {
      // if (!isset($block['pages'])) {
        // // {block}.pages is type 'text', so it cannot have a
        // // default value, and not null, so we need to provide
        // // value if the module did not.
        // $block['pages']  = '';
      // }
      // Make sure weight is set.
      if (!isset($job['weight'])) {
        $job['weight'] = 0;
      }
	  
	  
      if (!isset($job['status'])) {
        $job['status'] = 0;
      }
	  
	  
      // if (!empty($block['region']) && $block['region'] != BLOCK_REGION_NONE && !isset($regions[$block['region']])) {
        // drupal_set_message(t('The block %info was assigned to the invalid region %region and has been disabled.', array('%info' => $block['info'], '%region' => $block['region'])), 'warning');
        // // Disabled modules are moved into the BLOCK_REGION_NONE later so no
        // // need to move the bock to another region.
        // $block['status'] = 0;
      // }
      // Set region to none if not enabled and make sure status is set.
      // if (empty($block['status'])) {
        // $block['status'] = 0;
        // $block['region'] = BLOCK_REGION_NONE;
      // }
      // There is no point saving disabled blocks. Still, we need to save them
      // because the 'title' attribute is saved to the {blocks} table.
      if (isset($job['id'])) {
        // If the block has a bid property, it comes from the database and
        // the record needs to be updated, so set the primary key to 'bid'
        // before passing to drupal_write_record().
        $primary_keys = array('id');
        // Remove a block from the list of blocks to keep if it became disabled.
        unset($ids[$job['id']]);
      }
      else {
        $primary_keys = array();
      }
	  
      drupal_write_record('ad2_cron', $job, $primary_keys);
      // Add to the list of blocks we return.
      $jobs[] = $job;
    }
  }
//tr( $ids );
	if ($ids) {
		
    // Remove disabled that are no longer defined by the code from the
    // database.
    db_delete('ac2_cron')
      ->condition('id', $ids, 'NOT IN')
      //->condition('theme', $theme)
      //->execute()
	  ;
  }

  return $jobs;
}


function ad2_cron_page(){
	$jobs = _ad2_cron_rehash();
	usort($jobs, '_ad2_cron_compare');
	$build['form'] = drupal_get_form('ad2_cron_page_form' , $jobs );
	return $build;
}

function ad2_cron_page_form( &$form , &$form_state , $jobs ){
	
	$weights = array();
	
	for( $x = 0 ; $x < 50 ; $x++ ){
		$weights[$x] = $x;
	}
	
	$form['#jobs'] = $jobs;
	
	$form['jobs'] = array(
		'#type' => 'items',
		'#tree' => true,
	);
	foreach ($jobs as $job ) {
		//tr( $job );
		$id = $job['id'];
		$form['jobs'][$id] = array(
			'#type' => 'items',
		);
		
		$form['jobs'][$id]['weight'] = array(
			'#type' => 'select',
			'#options' => $weights,
			'#default_value' => $job['weight'],
		);
		
		$form['jobs'][$id]['status'] = array(
			'#type' => 'checkbox',
			'#default_value' => $job['status'],
			//'#options' => array(1=>1,2=>2),
		);
		
	}
	
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save'),
	);
	
	
	
	$form['run'] = array(
		'#type' => 'submit',
		'#value' => t('Run Now'),
		'#submit' => array('ad2_cron_run_now'),
	);
	
	
	return $form;
}

function ad2_cron_page_form_submit( &$form , &$form_state ){
		
	foreach ($form_state['values']['jobs'] as $id => $fields) {
		db_update('ad2_cron')
			->condition('id' , $id )
			->fields($fields)
			->execute()
			;
	}
	
	drupal_set_message('Data Saved');
}


function ad2_cron_run_now(&$form , &$form_state){
	timer_start(__FUNCTION__);
	_ad2_cron_run_now();
	drupal_set_message( t('Run Complete time:@time sec' , array('@time' => round(timer_read(__FUNCTION__) / 1000  , 1 ) )) );
}




function theme_ad2_cron_page_form( $vars ){
	
	$form = $vars['form'];
	$jobs = $form['#jobs'];
	
	$rows = array();
	
	//$last = variable_get("bycat_$delta_last_had_cron_run" , 0 );
	
	
	foreach ($jobs as $job ) {
		
		$module = $job['module'];
		$delta = $job['delta'];
		$v = $module . '_' . $delta . '_last_ad2_cron_run';
		$last = variable_get($v , 0 );
		
		$id = $job['id'];
		$rows[] = array(
			$job['id'],
			$job['title'],
			$last > 0 ? date('d-m-Y H:i:s' , $last ) : '-',
			$job['module'],
			$job['delta'],
			drupal_render($form['jobs'][$id]['weight']),
			drupal_render($form['jobs'][$id]['status']),
		);
	}
	$build['table'] = array(
		'#theme' 	=> 'table',
		'#rows' 	=> $rows,
		'#header' 	=> array('#' , 'title', 'last run', 'module' , 'delta' , 'weight' , 'status'  ),
	);
	
	//tr( $form );
	
	return  drupal_render($build) . drupal_render_children($form);
}



/**
 * Sorts active blocks by region, then by weight; sorts inactive blocks by name.
 *
 * Callback for usort() in block_admin_display_prepare_blocks().
 */
function _ad2_cron_compare($a, $b) {
 
	$weight = $a['weight'] - $b['weight'];
	if ($weight) {
      return $weight;
    }
	
	return strcmp($a['info'], $b['info']);
  
}
