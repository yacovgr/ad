<?php

function ad2_page_blocks(){
	
	
	$build = array();
	
	$build['link'] = array(
		'#type' => 'link',
		'#href' => 'admin/structure/block/add-ad2-block',
		'#title' => 'הוסף בלוק',
	);
	
	$blocks = db_select('ad2_blocks' , 'b')
		->fields('b')
		->execute()
		->fetchAll()
		;
	$rows = array();
	
	//tr( $blocks );
	$view_modes = had_get_view_mode_options();
	
	
	
	foreach ($blocks as $block ) {
		
		$types = array();
		$names = node_type_get_names();
		foreach (explode(",", $block->types) as $type ) {
			$types[] = $type;
		}
	
		$block->types = implode(", ", $types);
		$block->view_mode = $view_modes[$block->view_mode];
		
		
		$row = array(
			$block->delta,
			$block->info,
			$block->quantity,
			$block->types,
			$block->pages,
			$block->view_mode,
			$block->selection,
			$block->paginator,
			$block->tids,
			l(t('Edit') , 'admin/structure/block/manage/ad2_blocks/' . $block->delta .'/configure' , array('query' => drupal_get_destination() )),
			//l(t('Delete') , 'admin/structure/block/manage/ads/' . $block->delta .'/configure'),
			$block->pager_title,
		);
		$rows[] = $row;
	}
	
	$build['table'] = array(
		'#theme' => 'table',
		'#rows' => $rows,
		'#header' => array('#'  , 'כותרת בדף בלוקים' , 'כמות' , 'סוגי מודעות' , 'מספר דפים' , 'פורמט המודעה' , 'פונקצית בחירה' , 'דפדוף' , 'קטגוריות'),
	);
	
	return $build;
	
}

// 
// function had_block_view_mode_translate( $s ){
	// $defs = array(
		// 'teaser' => 'תקציר',
		// 'big' => 'גדול',
		// 'compact' => 'קומפקטי',
// 	
	// );
// }


function ad2_add_block_form($form , $form_state){
	require_once drupal_get_path('module' , 'block') .  "/block.admin.inc";
	return block_admin_configure($form, $form_state, 'ad2_blocks', NULL);
}


function ad2_add_block_form_validate($form , &$form_state){
	
 $custom_block_exists = (bool) db_query_range('SELECT 1 FROM ad2_blocks WHERE info = :info', 0, 1, array(':info' => $form_state['values']['ad2_info']))->fetchField();

  if (empty($form_state['values']['ad2_info']) || $custom_block_exists) {
    form_set_error('info', t('Ensure that each ads block description is unique.'));
  }
}


function ad2_add_block_form_submit($form , &$form_state){
	
	$values = $form_state['values'];
		
	$types = array();
	foreach ($values['ad2_types'] as $k => $v ) {
		if( $v )
			$types[] = $k;
	}
	$types = implode(',',$types);
		
	
	$tids = array();
	foreach ($values['ad2_tids'] as $k => $v ) {
		if( $v )
			$tids[] = $k;
	}
	sort($tids);
	$tids = implode(',',$tids);
		
	$delta = db_insert('ad2_blocks')
    ->fields(array(
      //'body' => $form_state['values']['body']['value'],
      	
      	//'format' => $form_state['values']['body']['format'],
      	'info' 		=> $values['ad2_info'],
      	'quantity' 	=> $values['ad2_quantity'],
		'display' 	=> 'raw', //$values['had_display'],
		'view_mode' => $values['ad2_view_mode'],
		'paginator' => $values['ad2_paginator'],
		'selection' => $values['ad2_selection'],
		'types' 	=> $types,
		'tids' 		=> $tids ? $tids : '',
    ))
    ->execute();
  // Store block delta to allow other modules to work with new block.
  $form_state['values']['delta'] = $delta;

  $query = db_insert('block')->fields(array('visibility', 'pages', 'custom', 'title', 'module', 'theme', 'status', 'weight', 'delta', 'cache'));
  foreach (list_themes() as $key => $theme) {
    if ($theme->status) {
      $query->values(array(
        'visibility' => (int) $form_state['values']['visibility'],
        'pages' => trim($form_state['values']['pages']),
        'custom' => (int) $form_state['values']['custom'],
        'title' => $form_state['values']['title'],
        'module' => $form_state['values']['module'],
        'theme' => $theme->name,
        'status' => 0,
        'weight' => 0,
        'delta' => $delta,
        'cache' => DRUPAL_NO_CACHE,
      ));
    }
  }
  $query->execute();

  $query = db_insert('block_role')->fields(array('rid', 'module', 'delta'));
  foreach (array_filter($form_state['values']['roles']) as $rid) {
    $query->values(array(
      'rid' => $rid,
      'module' => $form_state['values']['module'],
      'delta' => $delta,
    ));
  }
  $query->execute();

  // Store regions per theme for this block
  foreach ($form_state['values']['regions'] as $theme => $region) {
    db_merge('block')
      ->key(array('theme' => $theme, 'delta' => $delta, 'module' => $form_state['values']['module']))
      ->fields(array(
        'region' => ($region == BLOCK_REGION_NONE ? '' : $region),
        'pages' => trim($form_state['values']['pages']),
        'status' => (int) ($region != BLOCK_REGION_NONE),
      ))
      ->execute();
  }

  drupal_set_message(t('The block has been created.'));
  cache_clear_all();
  $form_state['redirect'] = 'admin/config/ad2/blocks';
  
}
