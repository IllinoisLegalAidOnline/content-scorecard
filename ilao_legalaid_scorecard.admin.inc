<?php

/**
 * @file
 */

/**
 * Provides configuration form for Legal Aid content scorecard.
 */
function ilao_legalaid_scorecard_settings_form($form, $form_state) {
  $form = array();

  $form['ilao_legalaid_scorecard_taxonomy_vocab_legal_problem'] = array(
    '#type'             => 'select',
    '#title'            => t('What taxonomy do you use for legal problem?'),
    '#options'          => _ilao_legalaid_scorecard_get_entities_as_options('machine_name', 'name', 'taxonomy_vocabulary'),
    '#default_value'    => variable_get('ilao_legalaid_scorecard_taxonomy_vocab_legal_problem'),
  );
  $form['ilao_legalaid_scorecard_node_type_legal_problem'] = array(
    '#type'             => 'select',
    '#title'            => t('What content type do you use for legal content?'),
    '#options'          => _ilao_legalaid_scorecard_get_entities_as_options('type', 'name', 'node'),
    '#default_value'    => variable_get('ilao_legalaid_scorecard_node_type_legal_problem'),
    '#ajax'             => array(
      'callback' => 'ilao_legalaid_scorecard_get_field_instance',
      'wrapper' => 'field_ilao_legalaid_scorecard_last_reviewed_date_field',
    ),
  );
  $form['ilao_legalaid_scorecard_node_legal_issue_field'] = array(
    '#type'             => 'textfield',
    '#title'            => t('What field do you use for legal issue in your content?'),
    '#default_value'    => variable_get('ilao_legalaid_scorecard_node_legal_issue_field'),
    '#description' => t('Enter the machine name'),
  );
  if (module_exists('fivestar')) {
    $form['ilao_legalaid_scorecard_field_for_ratings'] = array(
      '#type'             => 'textfield',
      '#title'            => t('Field for ratings:'),
      '#description'      => t('If you allow users to rate your legal content, enter the field name used for ratings. Leave blank if you do not store the data.'),
      '#default_value'    => variable_get('ilao_legalaid_scorecard_field_for_ratings', ''),
    );
  }
  if (module_exists('google_analytics_reports')) {
    $form['ilao_legalaid_scorecard_page_views'] = array(
      '#type'             => 'textfield',
      '#title'            => t('Field for page views of legal content:'),
      '#description'      => t('If you regularly pull in page views and have attached a field to your legal content type to store those page views, enter the machine name of the field. Leave blank if you do not store the data.'),
      '#default_value'    => variable_get('ilao_legalaid_scorecard_page_views', ''),
    );
  }
  $form['ilao_legalaid_scorecard_maximum_time'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum time (in months) that should be allowed between reviews of legal content, absent a law change?'),
    '#required' => 1,
    '#default_value'    => variable_get('ilao_legalaid_scorecard_maximum_time'),
    '#element_validate' => array('element_validate_integer_positive'),
  );
  $form['ilao_legalaid_scorecard_last_reviewed_date'] = array(
    '#type' => 'radios',
    '#title' => t('Do you use a custom date field as your last reviewed date?'),
    '#default_value' => variable_get('ilao_legalaid_scorecard_last_reviewed_date'),
    '#options' => array(1 => 'Yes', 0 => 'No'),
    '#required' => 1,
    '#ajax'             => array(
      'callback' => 'ilao_legalaid_scorecard_get_field_instance',
      'wrapper' => 'field_ilao_legalaid_scorecard_last_reviewed_date_field',
    ),
  );

  $legal_content_type = variable_get('ilao_legalaid_scorecard_node_type_legal_problem', '');


  $form['ilao_legalaid_scorecard_last_reviewed_date_field'] = array(
    '#type' => 'textfield',
    '#title' => t('Last reviewed date field machine name:'),

    '#default_value' => variable_get('ilao_legalaid_scorecard_last_reviewed_date_field'),

    '#states' => array(
      'visible' => array(
        ':input[name=ilao_legalaid_scorecard_last_reviewed_date]' => array('value' => 1),
      ),
      'required' => array(
        ':input[name=ilao_legalaid_scorecard_last_reviewed_date]' => array('value' => 1),
      ),
    ),
    '#prefix' => "<div id='field_ilao_legalaid_scorecard_last_reviewed_date_field' >",
    '#suffix' => "</div>",
  );

  $form['#submit'][] = 'ilao_legalaid_scorecard_settings_form_submit';
  return system_settings_form($form);
}

/**
 * Submit handler for Legal Aid content scorecard.
 */
function ilao_legalaid_scorecard_settings_form_submit($form, $form_state) {
  // Get the bundle name of the legal problem vocabulary.
  $bundle = $form_state['values']['ilao_legalaid_scorecard_taxonomy_vocab_legal_problem'];
  variable_set('ilao_legalaid_scorecard_node_legal_issue_field',$form_state['values']['ilao_legalaid_scorecard_node_legal_issue_field']);

  // If legal problem vocabulary is not empty, then create the field base and
  // field instance if required.
  if (!empty($bundle)) {
    $legal_problem_fields = array('field_legal_issue_breadth', 'field_legal_issue_user_impact', 'field_existing_issue_coverage', 'field_effective_date', 'field_workload_impact');

    foreach ($legal_problem_fields as $legal_problem_field) {
      if (!field_info_field($legal_problem_field)) {
        _ilao_legalaid_scorecard_create_field_base($legal_problem_field);
      }
      if (!field_info_instance('taxonomy_term', $legal_problem_field, $bundle)) {
        _ilao_legalaid_scorecard_create_field_instance('taxonomy_term', $legal_problem_field, $bundle);
      }
    }

    $content_type = $form_state['values']['ilao_legalaid_scorecard_node_type_legal_problem'];
    if (!field_info_field('field_content_score')) {
      _ilao_legalaid_scorecard_create_field_base('field_content_score');
    }
    if (!field_info_instance('node', 'field_content_score', $content_type)) {
      _ilao_legalaid_scorecard_create_field_instance('node', 'field_content_score', $content_type);
    }
  }
}
