#ifndef UI_GPS_H
#define UI_GPS_H

#ifdef __cplusplus
extern "C" {
#endif

  lv_updatable_screen_t gpsScreen;

  static lv_obj_t *gps_time_label;
  static lv_obj_t *gps_cog_label;
  static lv_obj_t *gps_lat_label;
  static lv_obj_t *gps_lon_label;

  /**
   * A GPS display 
   */
  void lv_gps_display(lv_obj_t *parent) {

    lv_obj_t *main_label = lv_label_create(parent);
    lv_obj_align(main_label, LV_ALIGN_CENTER, 0, -110);
    lv_label_set_text(main_label, "GPS");

    gps_time_label = lv_label_create(parent);
    lv_obj_align(gps_time_label, LV_ALIGN_TOP_LEFT, 10, 40);
#if LV_FONT_MONTSERRAT_20
    lv_obj_set_style_text_font(gps_time_label, &lv_font_montserrat_20, NULL);
#endif
    lv_label_set_text(gps_time_label, "GPS TIME:        --");

    gps_lat_label = lv_label_create(parent);
    lv_obj_align(gps_lat_label, LV_ALIGN_TOP_LEFT, 10, 80);
#if LV_FONT_MONTSERRAT_20
    lv_obj_set_style_text_font(gps_lat_label, &lv_font_montserrat_20, NULL);
#endif
    lv_label_set_text(gps_lat_label, "LAT:                    --");

    gps_lon_label = lv_label_create(parent);
    lv_obj_align(gps_lon_label, LV_ALIGN_TOP_LEFT, 10, 120);
#if LV_FONT_MONTSERRAT_20
    lv_obj_set_style_text_font(gps_lon_label, &lv_font_montserrat_20, NULL);
#endif
    lv_label_set_text(gps_lon_label, "LON:                   --");

    gps_cog_label = lv_label_create(parent);
    lv_obj_align(gps_cog_label, LV_ALIGN_TOP_LEFT, 10, 160);
#if LV_FONT_MONTSERRAT_20
    lv_obj_set_style_text_font(gps_cog_label, &lv_font_montserrat_20, NULL);
#endif
    lv_label_set_text(gps_cog_label, "COGT:                 --");
  }

  static String lat_print(float lat_deg) {
    return String(lat_deg, 6) + String(LV_SYMBOL_DEGREES);
  }

  static String lon_print(float lon_deg) {
    return String(lon_deg, 6) + String(LV_SYMBOL_DEGREES);
  }

  static String time_format(tm &t) {
    char bufferTime[9];
    sprintf(bufferTime, "%02d:%02d:%02d", t.tm_hour, t.tm_min, t.tm_sec);
    return String(bufferTime);
  }

  static void gps_update_cb() {
    lv_label_set_text(gps_time_label,
                      (String("GPS TIME:        ")
                       + (fresh(shipDataModel.environment.time_gps.age) ? time_format(shipDataModel.environment.time_gps.t) : String("--")))
                        .c_str());
    lv_label_set_text(gps_lat_label,
                      (String("LAT:                    ")
                       + (fresh(shipDataModel.navigation.position.lat.age) ? lat_print(shipDataModel.navigation.position.lat.deg) : String("--")))
                        .c_str());
    lv_label_set_text(gps_lon_label,
                      (String("LON:                    ")
                       + (fresh(shipDataModel.navigation.position.lon.age) ? lon_print(shipDataModel.navigation.position.lon.deg) : String("--")))
                        .c_str());
    lv_label_set_text(gps_cog_label,
                      (String("COGT:                 ")
                       + (fresh(shipDataModel.navigation.course_over_ground_true.age) ? String(shipDataModel.navigation.course_over_ground_true.deg, 1) + String(LV_SYMBOL_DEGREES) : String("--")))
                        .c_str());
  }

  void init_gpsScreen() {
    gpsScreen.screen = lv_obj_create(NULL);  // Creates a Screen object
    lv_gps_display(gpsScreen.screen);
    gpsScreen.update_cb = gps_update_cb;
  }

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
