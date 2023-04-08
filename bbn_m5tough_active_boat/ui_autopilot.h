#ifndef UI_AUTOPILOT_H
#define UI_AUTOPILOT_H

#ifdef __cplusplus
extern "C" {
#endif

  lv_updatable_screen_t autopilotScreen;

  static lv_obj_t *autopilot_led;
  static lv_obj_t *heading_l;
  static lv_obj_t *command_l;
  static lv_obj_t *autopilot_btnm;
  static lv_obj_t *autopilot_list_modes;

  static const char *autopilot_btnm_map[] = {
    LV_SYMBOL_DOUBLE_LEFT, LV_SYMBOL_DOUBLE_RIGHT, "\n",
    LV_SYMBOL_LEFT, LV_SYMBOL_RIGHT, "\n",
    "STANDBY", "AUTO", "\n",
    AP_MODE_COMPASS, LV_SYMBOL_EYE_OPEN, ""
  };

  static void event_handler_ap_mode(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED) {
      const char *mode = (const char *)lv_event_get_user_data(e);
      pypilot_send_mode(pypClient, mode);
      lv_obj_add_flag(autopilot_list_modes, LV_OBJ_FLAG_HIDDEN);
    }
  }

  void lv_autopilot_list_modes(lv_obj_t *parent) {
    autopilot_list_modes = lv_list_create(parent);
    lv_obj_set_size(autopilot_list_modes, 180, 160);
    lv_obj_center(autopilot_list_modes);

    lv_obj_t *btn;
    btn = lv_list_add_btn(autopilot_list_modes, NULL, "Compass");
    lv_obj_add_event_cb(btn, event_handler_ap_mode, LV_EVENT_CLICKED, (void *)AP_MODE_COMPASS);
    btn = lv_list_add_btn(autopilot_list_modes, NULL, "GPS");
    lv_obj_add_event_cb(btn, event_handler_ap_mode, LV_EVENT_CLICKED, (void *)AP_MODE_GPS);
    btn = lv_list_add_btn(autopilot_list_modes, NULL, "Wind");
    lv_obj_add_event_cb(btn, event_handler_ap_mode, LV_EVENT_CLICKED, (void *)AP_MODE_WIND);
    btn = lv_list_add_btn(autopilot_list_modes, NULL, "True Wind");
    lv_obj_add_event_cb(btn, event_handler_ap_mode, LV_EVENT_CLICKED, (void *)AP_MODE_WIND_TRUE);
  }

  static void autopilot_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_VALUE_CHANGED) {
      uint32_t id = lv_btnmatrix_get_selected_btn(obj);
      const char *txt = lv_btnmatrix_get_btn_text(obj, id);
      if (txt != NULL) {
        if (strcmp("AUTO", txt) == 0) {
          pypilot_send_engage(pypClient);
        } else if (strcmp("STANDBY", txt) == 0) {
          pypilot_send_disengage(pypClient);
        } else if (strcmp(LV_SYMBOL_DOUBLE_LEFT, txt) == 0) {
          pypilot_send_command(pypClient, shipDataModel.steering.autopilot.command.deg - 10.0);
        } else if (strcmp(LV_SYMBOL_LEFT, txt) == 0) {
          pypilot_send_command(pypClient, shipDataModel.steering.autopilot.command.deg - 2.0);
        } else if (strcmp(LV_SYMBOL_DOUBLE_RIGHT, txt) == 0) {
          pypilot_send_command(pypClient, shipDataModel.steering.autopilot.command.deg + 10.0);
        } else if (strcmp(LV_SYMBOL_RIGHT, txt) == 0) {
          pypilot_send_command(pypClient, shipDataModel.steering.autopilot.command.deg + 2.0);
        } else if (strcmp(LV_SYMBOL_EYE_OPEN, txt) == 0) {
        } else {
          lv_obj_clear_flag(autopilot_list_modes, LV_OBJ_FLAG_HIDDEN);
        }
      }
    }
  }

  void lv_autopilot_buttons(lv_obj_t *parent) {
    autopilot_led = lv_led_create(parent);
    lv_obj_align(autopilot_led, LV_ALIGN_OUT_LEFT_TOP, 8, 13);
    lv_led_set_color(autopilot_led, lv_palette_main(LV_PALETTE_GREY));
    lv_led_on(autopilot_led);

    int width_l = 134;
    int left_l = 52;
    int top_l = 14;

    heading_l = lv_label_create(parent);
    lv_label_set_text(heading_l, "HDG:  n/a");
    lv_obj_set_width(heading_l, width_l);
    lv_obj_set_style_text_align(heading_l, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_align(heading_l, LV_ALIGN_TOP_LEFT, left_l, top_l);
#if LV_FONT_MONTSERRAT_20
    lv_obj_set_style_text_font(heading_l, &lv_font_montserrat_20, NULL);
#endif

    command_l = lv_label_create(parent);
    lv_label_set_text(command_l, "CMD:  n/a");
    lv_obj_set_width(command_l, width_l);
    lv_obj_set_style_text_align(command_l, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_align(command_l, LV_ALIGN_TOP_LEFT, left_l + width_l, top_l);
#if LV_FONT_MONTSERRAT_20
    lv_obj_set_style_text_font(command_l, &lv_font_montserrat_20, NULL);
#endif

    autopilot_btnm = lv_btnmatrix_create(parent);
    lv_btnmatrix_set_map(autopilot_btnm, autopilot_btnm_map);
    lv_btnmatrix_set_btn_width(autopilot_btnm, 6, 3);
    lv_obj_set_size(autopilot_btnm, 320, 190);
    lv_obj_add_event_cb(autopilot_btnm, autopilot_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_align(autopilot_btnm, LV_ALIGN_CENTER, 0, 25);

    lv_autopilot_list_modes(parent);
    lv_obj_add_flag(autopilot_list_modes, LV_OBJ_FLAG_HIDDEN);
  }

#define LONG_EXPIRE_TO 172800000

  static void autopilot_update_cb() {
    if (shipDataModel.steering.autopilot.ap_state.st == ap_state_e::STANDBY
        && fresh(shipDataModel.steering.autopilot.ap_state.age, LONG_EXPIRE_TO)) {
      lv_led_set_color(autopilot_led, lv_palette_main(LV_PALETTE_RED));
    } else if (shipDataModel.steering.autopilot.ap_state.st == ap_state_e::ENGAGED
               && fresh(shipDataModel.steering.autopilot.ap_state.age, LONG_EXPIRE_TO)) {
      lv_led_set_color(autopilot_led, lv_palette_main(LV_PALETTE_GREEN));
    }
    if (!fresh(shipDataModel.steering.autopilot.ap_state.age, LONG_EXPIRE_TO)) {
      lv_led_set_color(autopilot_led, lv_palette_main(LV_PALETTE_GREY));
    }
    lv_label_set_text(heading_l,
                      ("HDG:  " + (fresh(shipDataModel.steering.autopilot.heading.age) ? String(shipDataModel.steering.autopilot.heading.deg, 0) + LV_SYMBOL_DEGREES : "n/a")).c_str());
    lv_label_set_text(command_l,
                      ("CMD:  " + (fresh(shipDataModel.steering.autopilot.command.age, LONG_EXPIRE_TO) ? String(shipDataModel.steering.autopilot.command.deg, 0) + LV_SYMBOL_DEGREES : "n/a")).c_str());

    if (fresh(shipDataModel.steering.autopilot.ap_mode.age, LONG_EXPIRE_TO)) {
      if (shipDataModel.steering.autopilot.ap_mode.mode == ap_mode_e::COG_TRUE) {
        autopilot_btnm_map[9] = AP_MODE_GPS;
      } else if (shipDataModel.steering.autopilot.ap_mode.mode == ap_mode_e::APP_WIND) {
        autopilot_btnm_map[9] = AP_MODE_WIND;
      } else if (shipDataModel.steering.autopilot.ap_mode.mode == ap_mode_e::HEADING_MAG) {
        autopilot_btnm_map[9] = AP_MODE_COMPASS;
      } else if (shipDataModel.steering.autopilot.ap_mode.mode == ap_mode_e::TRUE_WIND) {
        autopilot_btnm_map[9] = AP_MODE_WIND_TRUE;
      }
      lv_btnmatrix_set_map(autopilot_btnm, autopilot_btnm_map);
      lv_event_send(autopilot_btnm, LV_EVENT_REFRESH, NULL);
    }
  }

  void init_autopilotScreen() {
    autopilotScreen.screen = lv_obj_create(NULL);  // Creates a Screen object
    lv_autopilot_buttons(autopilotScreen.screen);
    autopilotScreen.update_cb = autopilot_update_cb;
  }

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
