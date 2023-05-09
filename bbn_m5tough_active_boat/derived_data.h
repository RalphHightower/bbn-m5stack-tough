#ifndef DERIVED_DATA_H
#define DERIVED_DATA_H

/*
 Also look at https://github.com/SignalK/signalk-derived-data
*/

#include "leeway.h"

#ifdef __cplusplus
extern "C" {
#endif

  float norm_deg(float deg) {
    if (deg < 0.0) {
      return 360 + deg;
    } else if (deg > 360.0) {
      return deg - 360;
    } else {
      return deg;
    }
  }

  float norm180_deg(float deg) {
    if (deg < -180.0) {
      return 360 + deg;
    } else if (deg > 180.0) {
      return deg - 360;
    } else {
      return deg;
    }
  }

  static void sunrise_sunset() {
    if ((!fresh(shipDataModel.environment.sunrise.age, TWO_MINUTES) || !fresh(shipDataModel.environment.sunset.age, TWO_MINUTES))
        && fresh(shipDataModel.navigation.position.lat.age)
        && fresh(shipDataModel.navigation.position.lon.age)) {
      M5.Rtc.GetDate(&RTCdate);
      float lon = shipDataModel.navigation.position.lon.deg;
      float lat = shipDataModel.navigation.position.lat.deg;

      int year = (RTCdate.Year % 100) + 1900;
      int month = RTCdate.Month + 1;
      int day = RTCdate.Date;

      float daylen = day_length(year, month, day, lon, lat);                     // Day length (hr)
      float nautlen = day_nautical_twilight_length(year, month, day, lon, lat);  // Day length with With nautical twilight (hr)
      float nauttwilight = (nautlen - daylen) / 2.0;                             // Length of nautical twilight (hr)

      shipDataModel.environment.daylight_duration.hr = daylen;
      shipDataModel.environment.daylight_duration.age = millis();
      shipDataModel.environment.nautical_twilight_duration.hr = nauttwilight;
      shipDataModel.environment.nautical_twilight_duration.age = millis();

      float rise, set, naut_start, naut_end;
      int rs = sun_rise_set(year, month, day, lon, lat, &rise, &set);                    // 1, 0, or -1
      int naut = nautical_twilight(year, month, day, lon, lat, &naut_start, &naut_end);  // 1, 0, or -1

      if (rs == 0) {
        shipDataModel.environment.sunrise.hr = rise;
        shipDataModel.environment.sunrise.age = millis();
        shipDataModel.environment.sunset.hr = set;
        shipDataModel.environment.sunset.age = millis();
      }
      shipDataModel.environment.no_sunset_flag = rs;  //  1 - above, -1 - below, 0 - rises and sets

      if (naut == 0) {
        shipDataModel.environment.nautical_twilight_start.hr = naut_start;
        shipDataModel.environment.nautical_twilight_start.age = millis();
        shipDataModel.environment.nautical_twilight_end.hr = naut_end;
        shipDataModel.environment.nautical_twilight_end.age = millis();
      }
      shipDataModel.environment.no_dark_flag = naut;
    }
  }

  static void derive_data() {
    // derive magnetic variation
    if (fresh(shipDataModel.navigation.position.lat.age)
        && fresh(shipDataModel.navigation.position.lon.age)) {
      M5.Rtc.GetDate(&RTCdate);
      float longitude = shipDataModel.navigation.position.lon.deg;
      float latitude = shipDataModel.navigation.position.lat.deg;
      float mag_var_deg = myDeclination.magneticDeclination(latitude, longitude, RTCdate.Year % 100, 1 + RTCdate.Month, RTCdate.Date);
      if (abs(mag_var_deg) > 0.00001) {  // do not trust 0
        shipDataModel.navigation.mag_var.deg = mag_var_deg;
        shipDataModel.navigation.mag_var.age = millis();
      }
    }

    if (fresh(shipDataModel.navigation.mag_var.age, LONG_EXPIRE_TO)) {
      if (fresh(shipDataModel.navigation.course_over_ground_true.age)) {
        shipDataModel.navigation.course_over_ground_mag.deg = norm_deg(shipDataModel.navigation.course_over_ground_true.deg - shipDataModel.navigation.mag_var.deg);
        shipDataModel.navigation.course_over_ground_mag.age = millis();
      }
      if (fresh(shipDataModel.navigation.heading_mag.age)) {
        shipDataModel.navigation.heading_true.deg = norm_deg(shipDataModel.navigation.heading_mag.deg + shipDataModel.navigation.mag_var.deg);
        shipDataModel.navigation.heading_true.age = millis();
      }
      if (fresh(shipDataModel.navigation.course_rhumbline.bearing_track_true.age)) {
        shipDataModel.navigation.course_rhumbline.bearing_track_mag.deg = norm_deg(shipDataModel.navigation.course_rhumbline.bearing_track_true.deg - shipDataModel.navigation.mag_var.deg);
        shipDataModel.navigation.course_rhumbline.bearing_track_mag.age = millis();
      }
    }

    // ground wind calc
    if (fresh(shipDataModel.navigation.course_over_ground_true.age)
        && fresh(shipDataModel.navigation.speed_over_ground.age)
        && fresh(shipDataModel.navigation.heading_true.age)
        && fresh(shipDataModel.environment.wind.apparent_wind_speed.age)
        && fresh(shipDataModel.environment.wind.apparent_wind_angle.age)) {
      float heading_speed_over_ground = shipDataModel.navigation.speed_over_ground.kn * cos((shipDataModel.navigation.course_over_ground_true.deg - shipDataModel.navigation.heading_true.deg) * PI / 180.0);
      float head_wind = abs(-heading_speed_over_ground);
      float aws = shipDataModel.environment.wind.apparent_wind_speed.kn;
      float awa = norm180_deg(shipDataModel.environment.wind.apparent_wind_angle.deg) * PI / 180.0;
      float ground_wind_speed =
        sqrt(abs(aws * aws + head_wind * head_wind - 2.0 * aws * head_wind * cos(awa)));
      float r = (aws * cos(awa) - head_wind) / ground_wind_speed;
      if (r >= -1 && r <= 1) {
        if (ground_wind_speed > 0.01) {
          float ground_wind_angle_rad = acos(r);
          if (awa < 0 || awa > PI) {  // port side
            ground_wind_angle_rad = (-ground_wind_angle_rad);
          }
          float ground_wind_angle_deg = ground_wind_angle_rad * 180.0 / PI;
          float ground_wind_dir_true_deg = norm_deg(ground_wind_angle_deg + shipDataModel.navigation.heading_true.deg);
          shipDataModel.environment.wind.ground_wind_angle.deg = norm180_deg(ground_wind_angle_deg);
          shipDataModel.environment.wind.ground_wind_angle.age = millis();
          shipDataModel.environment.wind.ground_wind_dir_true.deg = ground_wind_dir_true_deg;
          shipDataModel.environment.wind.ground_wind_dir_true.age = millis();
        }
        shipDataModel.environment.wind.ground_wind_speed.kn = ground_wind_speed;
        shipDataModel.environment.wind.ground_wind_speed.age = millis();
      }
    }

    if (fresh(shipDataModel.environment.wind.ground_wind_dir_true.age) && fresh(shipDataModel.navigation.mag_var.age, LONG_EXPIRE_TO)) {
      shipDataModel.environment.wind.ground_wind_dir_mag.deg = norm_deg(shipDataModel.environment.wind.ground_wind_dir_true.deg - shipDataModel.navigation.mag_var.deg);
      shipDataModel.environment.wind.ground_wind_dir_mag.age = millis();
    }

    // derive leeway
    if (fresh(shipDataModel.navigation.speed_through_water.age)
        && fresh(shipDataModel.navigation.attitude.heel.age)) {
      if (shipDataModel.navigation.speed_through_water.kn > 1) {
        int k_factor = 12;  // TODO:
        shipDataModel.navigation.leeway.deg = leeway_deg(shipDataModel.navigation.attitude.heel.deg,
                                                         shipDataModel.navigation.speed_through_water.kn, k_factor);
        shipDataModel.navigation.leeway.age = millis();
      }
    }

    // derive mag variation if unknown
    if (!fresh(shipDataModel.navigation.mag_var.age, LONG_EXPIRE_TO) && abs(shipDataModel.navigation.heading_true.deg - shipDataModel.navigation.heading_mag.deg) > 0.00001) {
      if (fresh(shipDataModel.navigation.heading_true.age) && fresh(shipDataModel.navigation.heading_mag.age)) {
        shipDataModel.navigation.mag_var.deg = norm_deg(shipDataModel.navigation.heading_true.deg - shipDataModel.navigation.heading_mag.deg);
        shipDataModel.navigation.mag_var.age = millis();
      }
    }

    sunrise_sunset();
  }

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
