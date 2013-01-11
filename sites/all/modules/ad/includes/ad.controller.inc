<?php

class AdController extends EntityAPIController {

	
	
  public function create(array $values = array()) {
    global $user;
    $values += array(
      'title' => '',
      'url' => '',
      'weight'	=> 1,
      'size'	=> '',
      'created' => REQUEST_TIME,
      'changed' => REQUEST_TIME,
      'uid' => $user->uid,
      'status' => 1,
    );
    return parent::create($values);
  }

  public function buildContent($entity, $view_mode = 'full', $langcode = NULL, $content = array()) {
    // Remove previously built content, if exists.
    $entity->content = $content;
    $langcode = isset($langcode) ? $langcode : $GLOBALS['language_content']->language;

    $wrapper = entity_metadata_wrapper('ad', $entity);
	//tr( get_class_methods( $wrapper ));
	
    //$entity->content['author'] = array('#markup' => t('Created by: !author', array('!author' => $wrapper->uid->name->value(array('sanitize' => TRUE)))));

    // Make Description and Status themed like default fields.
    // $entity->content['description'] = array(
      // '#theme' => 'field',
      // '#weight' => 0,
      // '#title' =>t('Description'),
      // '#access' => TRUE,
      // '#label_display' => 'above',
      // '#view_mode' => 'full',
      // '#language' => LANGUAGE_NONE,
      // '#field_name' => 'field_fake_description',
      // '#field_type' => 'text',
      // '#entity_type' => 'ad2',
      // '#bundle' => $entity->type,
      // '#items' => array(array('value' => $entity->description)),
      // '#formatter' => 'text_default',
      // 0 => array('#markup' => check_plain($entity->description))
    // );

    // $entity->content['mail'] = array(
      // '#theme' => 'field',
      // '#weight' => 1,
      // '#title' =>t('Mail'),
      // '#access' => TRUE,
      // '#label_display' => 'inline',
      // '#view_mode' => 'full',
      // '#language' => LANGUAGE_NONE,
      // '#field_name' => 'field_fake_mail',
      // '#field_type' => 'text',
      // '#entity_type' => 'ad2',
      // '#bundle' => $entity->type,
      // '#items' => array(array('value' => $entity->mail)),
      // '#formatter' => 'text_default',
      // 0 => array('#markup' => check_plain($entity->mail))
    // );


    // $entity->content['phone'] = array(
      // '#theme' => 'field',
      // '#weight' => 1,
      // '#title' =>t('Phone'),
      // '#access' => TRUE,
      // '#label_display' => 'inline',
      // '#view_mode' => 'full',
      // '#language' => LANGUAGE_NONE,
      // '#field_name' => 'field_fake_phone',
      // '#field_type' => 'text',
      // '#entity_type' => 'ad2',
      // '#bundle' => $entity->type,
      // '#items' => array(array('value' => $entity->phone)),
      // '#formatter' => 'text_default',
      // 0 => array('#markup' => check_plain($entity->phone))
    // );


    // $entity->content['cuid'] = array(
      // '#theme' => 'field',
      // '#weight' => 1,
      // '#title' =>t('User'),
      // '#access' => TRUE,
      // '#label_display' => 'inline',
      // '#view_mode' => 'full',
      // '#language' => LANGUAGE_NONE,
      // '#field_name' => 'field_fake_cuid',
      // '#field_type' => 'text',
      // '#entity_type' => 'ad2',
      // '#bundle' => $entity->type,
      // '#items' => array(array('value' => $entity->cuid)),
      // '#formatter' => 'text_default',
      // 0 => array('#markup' => check_plain($entity->cuid))
    // );


    // Add in fields.
    if (!empty($this->entityInfo['fieldable'])) {
      $entity->content += field_attach_view($this->entityType, $entity, $view_mode, $langcode);
    }

    // Invoke hook_ENTITY_view() to allow modules to add their additions.
    if (module_exists('rules')) {
      rules_invoke_all($this->entityType . '_view', $entity, $view_mode, $langcode);
    }
    else {
      module_invoke_all($this->entityType . '_view', $entity, $view_mode, $langcode);
    }
    module_invoke_all('entity_view', $entity, $this->entityType, $view_mode, $langcode);
    $build = $entity->content;
    unset($entity->content);
    return $build;
  }


}
       
 class AdTypeController extends EntityAPIControllerExportable {
   public function create(array $values = array()) {
    $values += array(
      'label' => '',
      'description' => '',
    );
    return parent::create($values);
  }

  /**
   * Save Task Type.
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    parent::save($entity, $transaction);
    // Let FieldAPI know that new bundle created.
    field_attach_create_bundle('ad', $entity->type);
    // Rebuild menu registry.
    menu_rebuild();
  }
 }

/**
 * UI controller for Task Type.
 */
class AdTypeUIController extends EntityDefaultUIController {
  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['description'] = 'Manage Ads types.';
    return $items;
  }
}

/**
 * Task class.
 */
class Ad extends Entity {
  protected function defaultLabel() {
    return $this->title;
  }

  protected function defaultUri() {
    return array('path' => 'ad/' . $this->identifier());
  }
}

/**
 * Task Type class.
 */
class AdType extends Entity {
  public $type;
  public $label;
  public $weight = 0;

  public function __construct($values = array()) {
    parent::__construct($values, 'ad_type');
  }

  function isLocked() {
    return isset($this->status) && empty($this->is_new) && (($this->status & ENTITY_IN_CODE) || ($this->status & ENTITY_FIXED));
  }
}
