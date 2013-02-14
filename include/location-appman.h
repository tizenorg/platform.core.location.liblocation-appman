/*
 * liblocation-appman
 *
 * Copyright (c) 2012-2013 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Youngae Kang <youngae.kang@samsung.com>, Minjune Kim <sena06.kim@samsung.com>
 *          Genie Kim <daejins.kim@samsung.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LOCATION_APPMAN_H_
#define LOCATION_APPMAN_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup liblocation-appman
 */

typedef struct{
	char		package[64];
	char		appname[64];
	char		appicon[256];
	int			appid;
	int			installed_date;
	int			recently_used;
	int			enable;
} location_appman_s;

typedef enum {
	LOCATION_APPMAN_COLUMN_PACKAGE = 0,
	LOCATION_APPMAN_COLUMN_APPICON,
	LOCATION_APPMAN_COLUMN_APPID,
	LOCATION_APPMAN_COLUMN_INSTALLED_DATE,
	LOCATION_APPMAN_COLUMN_RECENTLY_USED,
	LOCATION_APPMAN_COLUMN_ENABLE
}location_appman_column_e;

typedef enum {
	LOCATION_APPMAN_ERROR_NONE = 0,
	LOCATION_APPMAN_ERROR_SQLITE_FAIL = -100,
	LOCATION_APPMAN_ERROR_INVALID_PARAMETER = LOCATION_APPMAN_ERROR_SQLITE_FAIL | 0x01,
	LOCATION_APPMAN_ERROR_INTERNAL = LOCATION_APPMAN_ERROR_SQLITE_FAIL | 0x02,
	LOCATION_APPMAN_ERROR_UNKNOWN = LOCATION_APPMAN_ERROR_SQLITE_FAIL | 0x03,
	LOCATION_APPMAN_ERROR_INVALID_XML = LOCATION_APPMAN_ERROR_SQLITE_FAIL | 0x04,
}Location_appman_error_e;

typedef enum {
	LOCATION_APPMAN_ENABLE_OFF = 0,
	LOCATION_APPMAN_ENABLE_ON,
	LOCATION_APPMAN_PACKAGE_NOTFOUND
}Location_appman_onoff_e;

/**
 * @}
 */

int location_appman_check_developer_mode(void);

int location_appman_get_total_count(int *count);
int location_appman_insert(location_appman_s *appman);
int location_appman_delete(const char *package);
int location_appman_reset(void);
int location_appman_update(location_appman_s *appman);

// GET
int location_appman_get_app_list(location_appman_column_e sort_by, int *list_count, location_appman_s **appman);
int location_appman_get_name(const char *package, char **appname);
int location_appman_get_icon(const char *package, char **appicon);
int location_appman_get_appid(const char *package, int *appid);
int location_appman_get_installed_date(const char *package, unsigned int *installed_date);
int location_appman_get_recently_used(const char *package, unsigned int *recently_used);
int location_appman_is_enabled(const char *package, int *enable);

// SET
int location_appman_set_icon(const char *package, char *appicon);
int location_appman_set_appid(const char *package, int appid);
int location_appman_set_installed_date(const char *pakage, unsigned int installed_date);
int location_appman_set_recently_used(const char *package, unsigned int recently_used);
int location_appman_set_on(const char *package, int onoff);

// For manifest
int location_appamn_parse_manifest(const char *xml, location_appman_s **appman);
int location_appman_get_package_by_pid(pid_t pid, location_appman_s **appman);
int location_appman_find_package(const char *package, int *findpackage);
int location_appman_register_package(location_appman_s *appman);

#ifdef __cplusplus
}
#endif
#endif				/* LOCATION_APPMAN_H_ */
