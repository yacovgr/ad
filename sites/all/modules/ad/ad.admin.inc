<?php

/**
 * Generates the task type editing form.
 */
function ad_type_form($form, &$form_state, $task_type, $op = 'edit') {


  if ($op == 'clone') {
    $task_type->label .= ' (cloned)';
    $task_type->type = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $task_type->label,
    '#description' => t('The human-readable name of this task type.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  // Machine-readable type name.
  $form['type'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($task_type->type) ? $task_type->type : '',
    '#maxlength' => 32,
    '#disabled' => $task_type->isLocked() && $op != 'clone',
    '#machine_name' => array(
      'exists' => 'ad_types',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this task type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['description'] = array(
    '#type' => 'textarea',
    '#default_value' => isset($task_type->description) ? $task_type->description : '',
    '#description' => t('Description about the task type.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Ad type'),
    '#weight' => 40,
    // '#submit' => array('ad_type_form_submit' , 'ad_type_form_submit_fields')
  );

  if (!$task_type->isLocked() && $op != 'add' && $op != 'clone') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete task type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('ad_type_form_submit_delete')
    );
  }
  return $form;
}

/**
 * Submit handler for creating/editing task_type.
 */
function ad_type_form_submit(&$form, &$form_state) {
  $task_type = entity_ui_form_submit_build_entity($form, $form_state);
  // Save and go back.
  ad_type_save($task_type);

// only if new !
	
	// $field = array(
    // 'field_name' => 'ad_image',
    // 'type' => 'image',
    // 'entity_types' => array('ad'),
    // 'cardinality' => 1,
    // 'translatable' => TRUE,
    // 'locked' => TRUE,
  // );
  // $field = field_create_field($field);
  // $instance = array(
    // 'field_name' => 'ad_image',
    // 'bundle' => $task_type->type,
    // 'entity_type' => 'ad',
    // 'label' => t('Ad Image'),
    // 'description' => t('This is the text of all messages of this type.'),
    // 'required' => TRUE,
    // 'settings' => array(
      // 'text_processing' => 1,
    // ),
  // );
  // field_create_instance($instance);
	
	
  // Redirect user back to list of task types.
  $form_state['redirect'] = 'admin/structure/ad-types';
}

// 
// /**
 // * Submit handler for creating/editing task_type.
 // */
// function ad_type_form_submit_fields(&$form, &$form_state) {
// 	
  // $task_type = entity_ui_form_submit_build_entity($form, $form_state);
  // // Save and go back.
  // ad_type_save($task_type);
// 
  // // Redirect user back to list of task types.
  // $form_state['redirect'] = 'admin/structure/ad-types';
// }



function ad_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/ad-types/' . $form_state['ad_type']->type . '/delete';
}

/**
 * Task type delete form.
 */
function ad_type_form_delete_confirm($form, &$form_state, $task_type) {
  $form_state['ad'] = $task_type;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['ad_id'] = array('#type' => 'value', '#value' => entity_id('ad_type' ,$task_type));
  return confirm_form($form,
    t('Are you sure you want to delete ad type %title?', array('%title' => entity_label('ad_type', $task_type))),
    'task/' . entity_id('ad_type' ,$task_type),
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Task type delete form submit handler.
 */
function ad_type_form_delete_confirm_submit($form, &$form_state) {
  $task_type = $form_state['ad'];
  ad_type_delete($task_type);

  watchdog('ad_type', '@type: deleted %title.', array('@type' => $task_type->type, '%title' => $task_type->label));
  drupal_set_message(t('@type %title has been deleted.', array('@type' => $task_type->type, '%title' => $task_type->label)));

  $form_state['redirect'] = 'admin/structure/ad-types';
}

/**
 * Page to select task Type to add new task.
 */
function ad_admin_add_page() {
  $items = array();
  foreach (ad_types() as $ad_type_key => $ad_type) {
    $items[] = l(entity_label('ad_type', $ad_type), 'ad/add/' . $ad_type_key);
  }
  return array('list' => array('#theme' => 'item_list', '#items' => $items, '#title' => t('Select type of Ad to create.')));
}

/**
 * Add new task page callback.
 */
function ad_add($type) {
	//tr( $type );
  $task_type = ad_types($type);

  $task = entity_create('ad', array('type' => $type));
  drupal_set_title(t('Create @name', array('@name' => entity_label('ad_type', $task_type))));

  $output = drupal_get_form('ad_form', $task);

  return $output;
}

/**
 * Task Form.
 */
function ad_form($form, &$form_state, $ad) {
	
	//entity_defaults_rebuild();
	
  $form_state['ad'] = $ad;

  $form['title'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Name'),
    '#default_value' => $ad->title,
  );

  $form['status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Status'),
    '#default_value' => $ad->status,
  );

/*
  $form['url'] = array(
    '#type' => 'textfield',
    '#title' => t('Url'),
    '#default_value' => $ad->url,
    '#required' => true,
    '#description' => t('the url to redirect when clicking the ad. put <self> to redirect to ad entity view' ),
  );
*/

  // $form['weight'] = array(
    // '#type' => 'textfield',
    // '#title' => t('Weight'),
    // '#default_value' => $ad->weight,
    // '#size' => 5,
    // '#required' => true,
  // );

/*
  $form['size'] = array(
    '#type' => 'textfield',
    '#title' => t('Size'),
    '#default_value' => $ad->size,
    '#required' => true,
  );
*/

  $form['uid'] = array(
    '#type' => 'value',
    '#value' => $ad->uid,
  );

  field_attach_form('ad', $ad, $form, $form_state);

  $submit = array();
  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions'] = array(
    '#weight' => 100,
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Ad'),
    '#submit' => $submit + array('ad_form_submit'),
  );

  // Show Delete button if we edit task.
  $ad_id = entity_id('ad' ,$ad);
  if (!empty($ad_id) && ad_access('edit', $ad)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('ad_form_submit_delete'),
    );
  }

  $form['#validate'][] = 'ad_form_validate';
 

  return $form;
}

function ad_form_validate($form, &$form_state) {
	// weight should be greater then zero , if it is zero then it will not show
}

/**
 * Task submit handler.
 */
function ad_form_submit($form, &$form_state) {
  $task = $form_state['ad'];

  entity_form_submit_build_entity('ad', $task, $form, $form_state);

  ad_save($task);

  $task_uri = entity_uri('ad', $task);

  $form_state['redirect'] = $task_uri['path'];

  drupal_set_message(t('Ad %title saved.', array('%title' => entity_label('ad', $task))));
}

function ad_form_submit_delete($form, &$form_state) {
  $task = $form_state['ad'];
  $task_uri = entity_uri('ad', $task);
  $form_state['redirect'] = $task_uri['path'] . '/delete';
}

/**
 * Delete confirmation form.
 */
function ad_delete_form($form, &$form_state, $task) {
  $form_state['ad'] = $task;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['ad_type_id'] = array('#type' => 'value', '#value' => entity_id('ad' ,$task));
  $task_uri = entity_uri('ad', $task);
  return confirm_form($form,
    t('Are you sure you want to delete task %title?', array('%title' => entity_label('ad', $task))),
    $task_uri['path'],
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Delete form submit handler.
 */
function ad_delete_form_submit($form, &$form_state) {
  $task = $form_state['ad'];
  ad_delete($task);

  drupal_set_message(t('Task %title deleted.', array('%title' => entity_label('ad', $task))));

  $form_state['redirect'] = '<front>';
}


function ad_config_page(){
	
	//field_delete_field('ad_image');
	
	//tr( field_info_fields() );
	
	//field_delete_field('ad_image');
	//tr( field_info_field('ad_image') );
	
	// tr( module_implements('field_attach_create_bundle') );
	// tr( module_implements('field_attach_update') );
	// tr( module_implements('field_attach_insert') );
// 	
	
	return 'here config page';
}



function ad_page_blocks(){
	
	
	$build = array();
	
	$build['link'] = array(
		'#type' => 'link',
		'#href' => 'admin/structure/block/add-ad-block',
		'#title' => 'הוסף בלוק',
	);
	
	$blocks = db_select('ad_block' , 'b')
		->fields('b')
		->execute()
		->fetchAll()
		;
	$rows = array();
	
	//tr( $blocks );
	$view_modes = array(); // had_get_view_mode_options();
	
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
			l(t('Edit') , 'admin/structure/block/manage/ad/' . $block->delta .'/configure' , array('query' => drupal_get_destination() )),
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


function ad_add_block_form($form , $form_state){
	require_once drupal_get_path('module' , 'block') .  "/block.admin.inc";
	return block_admin_configure($form, $form_state, 'ad', NULL);
}


function ad_add_block_form_validate($form , &$form_state){
	
 $custom_block_exists = (bool) db_query_range('SELECT 1 FROM ad_block WHERE info = :info', 0, 1, array(':info' => $form_state['values']['ad_info']))->fetchField();

  if (empty($form_state['values']['ad_info']) || $custom_block_exists) {
    form_set_error('info', t('Ensure that each ads block description is unique.'));
  }
}


function ad_add_block_form_submit($form , &$form_state){
	
	$values = $form_state['values'];
		
	$types = array();
	foreach ($values['ad_types'] as $k => $v ) {
		if( $v )
			$types[] = $k;
	}
	$types = implode(',',$types);
		
	
	$tids = array();
	foreach ($values['ad_tids'] as $k => $v ) {
		if( $v )
			$tids[] = $k;
	}
	sort($tids);
	$tids = implode(',',$tids);
		
	$delta = db_insert('ad_block')
    ->fields(array(
      //'body' => $form_state['values']['body']['value'],
      	
      	//'format' => $form_state['values']['body']['format'],
      	'info' 		=> $values['ad_info'],
      	'quantity' 	=> $values['ad_quantity'],
		'display' 	=> 'raw', //$values['had_display'],
		'view_mode' => $values['ad_view_mode'],
		'paginator' => $values['ad_paginator'],
		'selection' => $values['ad_selection'],
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
  $form_state['redirect'] = 'admin/config/ad/blocks';
  
}

