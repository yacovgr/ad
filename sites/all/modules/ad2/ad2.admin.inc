<?php

/**
 * Generates the task type editing form.
 */
function ad2_type_form($form, &$form_state, $task_type, $op = 'edit') {


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
      'exists' => 'ad2_types',
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
    // '#submit' => array('ad2_type_form_submit' , 'ad2_type_form_submit_fields')
  );

  if (!$task_type->isLocked() && $op != 'add' && $op != 'clone') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete task type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('ad2_type_form_submit_delete')
    );
  }
  return $form;
}

/**
 * Submit handler for creating/editing task_type.
 */
function ad2_type_form_submit(&$form, &$form_state) {
  $task_type = entity_ui_form_submit_build_entity($form, $form_state);
  // Save and go back.
  ad2_type_save($task_type);

// only if new !
	
	// $field = array(
    // 'field_name' => 'ad2_image',
    // 'type' => 'image',
    // 'entity_types' => array('ad2'),
    // 'cardinality' => 1,
    // 'translatable' => TRUE,
    // 'locked' => TRUE,
  // );
  // $field = field_create_field($field);
  // $instance = array(
    // 'field_name' => 'ad2_image',
    // 'bundle' => $task_type->type,
    // 'entity_type' => 'ad2',
    // 'label' => t('Ad Image'),
    // 'description' => t('This is the text of all messages of this type.'),
    // 'required' => TRUE,
    // 'settings' => array(
      // 'text_processing' => 1,
    // ),
  // );
  // field_create_instance($instance);
	
	
  // Redirect user back to list of task types.
  $form_state['redirect'] = 'admin/structure/ad2-types';
}

// 
// /**
 // * Submit handler for creating/editing task_type.
 // */
// function ad2_type_form_submit_fields(&$form, &$form_state) {
// 	
  // $task_type = entity_ui_form_submit_build_entity($form, $form_state);
  // // Save and go back.
  // ad2_type_save($task_type);
// 
  // // Redirect user back to list of task types.
  // $form_state['redirect'] = 'admin/structure/ad2-types';
// }



function ad2_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/ad2-types/' . $form_state['ad2_type']->type . '/delete';
}

/**
 * Task type delete form.
 */
function ad2_type_form_delete_confirm($form, &$form_state, $task_type) {
  $form_state['ad2'] = $task_type;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['ad2_id'] = array('#type' => 'value', '#value' => entity_id('ad2_type' ,$task_type));
  return confirm_form($form,
    t('Are you sure you want to delete ad2 type %title?', array('%title' => entity_label('ad2_type', $task_type))),
    'task/' . entity_id('ad2_type' ,$task_type),
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Task type delete form submit handler.
 */
function ad2_type_form_delete_confirm_submit($form, &$form_state) {
  $task_type = $form_state['ad2'];
  ad2_type_delete($task_type);

  watchdog('ad2_type', '@type: deleted %title.', array('@type' => $task_type->type, '%title' => $task_type->label));
  drupal_set_message(t('@type %title has been deleted.', array('@type' => $task_type->type, '%title' => $task_type->label)));

  $form_state['redirect'] = 'admin/structure/ad2-types';
}

/**
 * Page to select task Type to add new task.
 */
function ad2_admin_add_page() {
  $items = array();
  foreach (ad2_types() as $ad_type_key => $ad_type) {
    $items[] = l(entity_label('ad2_type', $ad_type), 'ad2/add/' . $ad_type_key);
  }
  return array('list' => array('#theme' => 'item_list', '#items' => $items, '#title' => t('Select type of Ad to create.')));
}

/**
 * Add new task page callback.
 */
function ad2_add($type) {
	//tr( $type );
  $task_type = ad2_types($type);

  $task = entity_create('ad2', array('type' => $type));
  drupal_set_title(t('Create @name', array('@name' => entity_label('ad2_type', $task_type))));

  $output = drupal_get_form('ad2_form', $task);

  return $output;
}

/**
 * Task Form.
 */
function ad2_form($form, &$form_state, $ad) {
	
	//entity_defaults_rebuild();
	
  $form_state['ad2'] = $ad;

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

// 
  // $form['mail'] = array(
    // '#type' => 'textfield',
    // '#title' => t('Mail'),
    // '#default_value' => $ad->mail,
  // );
// 
// 
  // $form['phone'] = array(
    // '#type' => 'textfield',
    // '#title' => t('Phone'),
    // '#default_value' => $ad->phone,
  // );


  // $form['cuid'] = array(
    // '#type' => 'textfield',
    // '#title' => t('User'),
    // '#default_value' => $contact->cuid,
  // );


  $form['uid'] = array(
    '#type' => 'value',
    '#value' => $ad->uid,
  );

  field_attach_form('ad2', $ad, $form, $form_state);

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
    '#submit' => $submit + array('ad2_form_submit'),
  );

  // Show Delete button if we edit task.
  $ad_id = entity_id('ad2' ,$ad);
  if (!empty($ad_id) && ad2_access('edit', $ad)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('ad2_form_submit_delete'),
    );
  }

  $form['#validate'][] = 'ad2_form_validate';
 

  return $form;
}

function ad2_form_validate($form, &$form_state) {

}

/**
 * Task submit handler.
 */
function ad2_form_submit($form, &$form_state) {
  $task = $form_state['ad2'];

  entity_form_submit_build_entity('ad2', $task, $form, $form_state);

  ad2_save($task);

  $task_uri = entity_uri('ad2', $task);

  $form_state['redirect'] = $task_uri['path'];

  drupal_set_message(t('Task %title saved.', array('%title' => entity_label('ad2', $task))));
}

function ad2_form_submit_delete($form, &$form_state) {
  $task = $form_state['ad2'];
  $task_uri = entity_uri('ad2', $task);
  $form_state['redirect'] = $task_uri['path'] . '/delete';
}

/**
 * Delete confirmation form.
 */
function ad2_delete_form($form, &$form_state, $task) {
  $form_state['ad2'] = $task;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['ad2_type_id'] = array('#type' => 'value', '#value' => entity_id('ad2' ,$task));
  $task_uri = entity_uri('ad2', $task);
  return confirm_form($form,
    t('Are you sure you want to delete task %title?', array('%title' => entity_label('ad2', $task))),
    $task_uri['path'],
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Delete form submit handler.
 */
function ad2_delete_form_submit($form, &$form_state) {
  $task = $form_state['ad2'];
  ad2_delete($task);

  drupal_set_message(t('Task %title deleted.', array('%title' => entity_label('ad2', $task))));

  $form_state['redirect'] = '<front>';
}


function ad2_config_page(){
	
	//field_delete_field('ad2_image');
	
	//tr( field_info_fields() );
	
	//field_delete_field('ad2_image');
	//tr( field_info_field('ad2_image') );
	
	// tr( module_implements('field_attach_create_bundle') );
	// tr( module_implements('field_attach_update') );
	// tr( module_implements('field_attach_insert') );
// 	
	
	return 'here config page';
}
