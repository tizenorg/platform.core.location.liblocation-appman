/*
 * liblocation-appman
 *
 * Copyright (c) 2010-2011 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Youngae Kang <youngae.kang@samsung.com>, Yunhan Kim <yhan.kim@samsung.com>,
 *          Genie Kim <daejins.kim@samsung.com>, Minjune Kim <sena06.kim@samsung.com>
 *          Hyuncheol Jung <hyuncheol.jung@samsung.com>
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

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <glib.h>
#include <sqlite3.h>
#include <libxml/parser.h>
#include <app_manager.h>
#include "location-appman.h"
#include "location-appman-log.h"

#define LOCATION_APPMAN_QUERY_MAX_LEN			1024

#define LOCATION_APPMAN_PATH_FILE			".location-appman.db"
#define LOCATION_APPMAN_PATH_DBSPACE			LOCATION_APPMAN_DBDIR
#define LOCATION_APPMAN_PATH_FULL			LOCATION_APPMAN_PATH_DBSPACE"/"LOCATION_APPMAN_PATH_FILE

#define LOCATION_APPMAN_PKG_NAME	"PKG_NAME"

sqlite3 *db_handle = NULL;

char *location_column_name[6] = { "package", "icon", "app_id", "installed_date", "recently_used", "enable" };

int location_appman_check_developer_mode(void)
{
	return FALSE;
}

void location_appman_close(void)
{
	sqlite3_close(db_handle);
	db_handle = NULL;
}

int location_appman_open(void)
{
	int rc = SQLITE_OK;

	if (NULL != db_handle) {
		LOCATION_APPMAN_LOG("location_appman_open & close\n");
		location_appman_close();
	}

	rc = sqlite3_open_v2(LOCATION_APPMAN_PATH_FULL, &db_handle, SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_READWRITE, NULL);
	if (SQLITE_OK != rc) {
		LOCATION_APPMAN_LOG("Failed to open : Error[%s]\n", sqlite3_errmsg(db_handle));
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	return LOCATION_APPMAN_ERROR_NONE;
}

int location_appman_query(char *dest, const char *src)
{
	if (NULL == dest || NULL == src) {
		LOCATION_APPMAN_LOG("location_appman_query : NULL\n");
		return FALSE;
	}

	g_strlcat(dest, src, strlen(src) + 1);
	return TRUE;
}

int location_appman_drop(void)
{
	int rc = SQLITE_OK;
	char query[LOCATION_APPMAN_QUERY_MAX_LEN] = { 0, };

	rc = location_appman_query(query, "DROP TABLE IF EXISTS LocationApp");
	if (FALSE == rc) {
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	rc = sqlite3_exec(db_handle, query, NULL, NULL, NULL);
	if (SQLITE_OK != rc) {
		LOCATION_APPMAN_LOG("Fail to connect to table. Error[%s]\n", sqlite3_errmsg(db_handle));
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	return LOCATION_APPMAN_ERROR_NONE;
}

int location_appman_insert(location_appman_s * appman)
{
	LOCATION_APPMAN_NULL_ARG_CHECK(appman);
	sqlite3_stmt *state;
	int rc = SQLITE_OK;
	int sql_param_index = 1;
	char query[LOCATION_APPMAN_QUERY_MAX_LEN] = { 0, };
	const char *tail;

	if (location_appman_open() != LOCATION_APPMAN_ERROR_NONE)
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;

	rc = location_appman_query(query,
				   "INSERT INTO LocationApp(package, icon, app_id, installed_date, recently_used, enable)"
				   "VALUES (?, ?, ?, ?, ?, ?)");
	if (FALSE == rc) {
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	rc = sqlite3_prepare_v2(db_handle, query, strlen(query), &state, &tail);
	if (SQLITE_OK != rc) {
		LOCATION_APPMAN_LOG("sqlite3_prepare_v2 failed : Error[%s]\n", sqlite3_errmsg(db_handle));
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	sqlite3_bind_text(state, sql_param_index, appman->package, strlen(appman->package), SQLITE_STATIC);
	sqlite3_bind_text(state, ++sql_param_index, appman->appicon, strlen(appman->appicon), SQLITE_STATIC);
	sqlite3_bind_int(state, ++sql_param_index, appman->appid);
	sqlite3_bind_int(state, ++sql_param_index, appman->installed_date);
	sqlite3_bind_int(state, ++sql_param_index, appman->recently_used);
	sqlite3_bind_int(state, ++sql_param_index, appman->enable);

	sqlite3_step(state);
	sqlite3_reset(state);

	sqlite3_finalize(state);
	location_appman_close();

	return LOCATION_APPMAN_ERROR_NONE;
}

int location_appman_delete(const char *package)
{
	LOCATION_APPMAN_NULL_ARG_CHECK(package);
	sqlite3_stmt *state = NULL;
	int rc = SQLITE_OK;
	char query[LOCATION_APPMAN_QUERY_MAX_LEN] = { 0, };

	if (location_appman_open() != LOCATION_APPMAN_ERROR_NONE)
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;

	rc = location_appman_query(query, "DELETE from LocationApp where package = ?");
	if (FALSE == rc) {
		LOCATION_APPMAN_LOG("location_app_delete. Error[%d]\n", rc);
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	rc = sqlite3_prepare_v2(db_handle, query, strlen(query), &state, NULL);
	if (SQLITE_OK != rc) {
		LOCATION_APPMAN_LOG("Fail to connect to table. Error[%s]\n", sqlite3_errmsg(db_handle));
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}
	// bind the values
	rc = sqlite3_bind_text(state, 1, package, strlen(package), SQLITE_STATIC);
	if (SQLITE_OK != rc) {
		LOCATION_APPMAN_LOG("Fail to bind string to query. Error[%s]\n", sqlite3_errmsg(db_handle));
		sqlite3_finalize(state);
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}
	// step
	rc = sqlite3_step(state);
	if (SQLITE_DONE != rc) {
		LOCATION_APPMAN_LOG("Fail to step. Error[%d]", rc);
		sqlite3_finalize(state);
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}
	sqlite3_finalize(state);
	location_appman_close();

	return LOCATION_APPMAN_ERROR_NONE;
}

int location_appman_reset(void)
{
	int rc = SQLITE_OK;
	char query[LOCATION_APPMAN_QUERY_MAX_LEN] = { 0, };

	if (location_appman_open() != LOCATION_APPMAN_ERROR_NONE)
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;

	if (location_appman_drop() != LOCATION_APPMAN_ERROR_NONE)
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;

	rc = location_appman_query(query, "CREATE TABLE LocationApp(\n "
				   "package VARCHAR(64) PRIMARY KEY,\n"
				   "name VARChAR(32),\n"
				   "icon VARCHAR(256),\n"
				   "app_id INTEGER,\n" "installed_date INTEGER,\n" "recently_used INTEGER,\n" "enable BOOL);");

	if (FALSE == rc) {
		LOCATION_APPMAN_LOG("location_appman_reset. Error[%d]\n", rc);
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	rc = sqlite3_exec(db_handle, query, NULL, NULL, NULL);
	if (SQLITE_OK != rc) {
		LOCATION_APPMAN_LOG("Fail to connect to table. Error[%s]\n", sqlite3_errmsg(db_handle));
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	location_appman_close();

	return LOCATION_APPMAN_ERROR_NONE;
}

int location_appman_update(location_appman_s * appman)
{
	LOCATION_APPMAN_NULL_ARG_CHECK(appman);
	sqlite3_stmt *state = NULL;
	int rc = SQLITE_OK;
	int sql_param_index = 1;
	char query[LOCATION_APPMAN_QUERY_MAX_LEN] = { 0, };

	if (location_appman_open() != LOCATION_APPMAN_ERROR_NONE)
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;

	rc = location_appman_query(query,
				   "UPDATE LocationApp SET icon = ?, app_id = ?, installed_date = ?, recently_used = ?, enable = ? where package = ? ");
	if (FALSE == rc) {
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	rc = sqlite3_prepare_v2(db_handle, query, strlen(query), &state, NULL);
	if (SQLITE_OK != rc) {
		LOCATION_APPMAN_LOG("Fail to connect to table. Error[%s]\n", sqlite3_errmsg(db_handle));
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}
	// bind the values
	// Icon
	rc = sqlite3_bind_text(state, sql_param_index++, appman->appicon, strlen(appman->appicon), SQLITE_STATIC);
	if (SQLITE_OK != rc) {
		LOCATION_APPMAN_LOG("Fail to bind string to query. Error[%s]\n", sqlite3_errmsg(db_handle));
		sqlite3_finalize(state);
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}
	// App ID
	rc = sqlite3_bind_int(state, sql_param_index++, appman->appid);
	if (SQLITE_OK != rc) {
		LOCATION_APPMAN_LOG("Fail to bind string to query. Error[%s]\n", sqlite3_errmsg(db_handle));
		sqlite3_finalize(state);
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}
	// Installed Date
	rc = sqlite3_bind_int(state, sql_param_index++, appman->installed_date);
	if (SQLITE_OK != rc) {
		LOCATION_APPMAN_LOG("Fail to bind string to query. Error[%s]\n", sqlite3_errmsg(db_handle));
		sqlite3_finalize(state);
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}
	// Recently used
	rc = sqlite3_bind_int(state, sql_param_index++, appman->recently_used);
	if (SQLITE_OK != rc) {
		LOCATION_APPMAN_LOG("Fail to bind string to query. Error[%s]\n", sqlite3_errmsg(db_handle));
		sqlite3_finalize(state);
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}
	// Enable
	rc = sqlite3_bind_int(state, sql_param_index++, appman->enable);
	if (SQLITE_OK != rc) {
		LOCATION_APPMAN_LOG("Fail to bind string to query. Error[%s]\n", sqlite3_errmsg(db_handle));
		sqlite3_finalize(state);
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}
	// Package
	rc = sqlite3_bind_text(state, sql_param_index++, appman->package, strlen(appman->package), SQLITE_STATIC);
	if (SQLITE_OK != rc) {
		LOCATION_APPMAN_LOG("Fail to bind string to query. Error[%s]\n", sqlite3_errmsg(db_handle));
		sqlite3_finalize(state);
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}
	// step
	rc = sqlite3_step(state);
	if (SQLITE_DONE != rc) {
		LOCATION_APPMAN_LOG("Fail to step. Error[%d]", rc);
		sqlite3_finalize(state);
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}
	sqlite3_finalize(state);
	location_appman_close();

	return LOCATION_APPMAN_ERROR_NONE;
}

int location_appman_get_total_count(int *count)
{
	sqlite3_stmt *state;
	int rc = SQLITE_OK;
	int num_data;
	char query[LOCATION_APPMAN_QUERY_MAX_LEN] = { 0, };
	const char *tail;

	if (location_appman_open() != LOCATION_APPMAN_ERROR_NONE)
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;

	rc = location_appman_query(query, "SELECT count(*) FROM LocationApp");
	if (FALSE == rc) {
		location_appman_close();
		*count = -1;
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	rc = sqlite3_prepare_v2(db_handle, query, strlen(query), &state, &tail);
	if (SQLITE_OK != rc) {
		LOCATION_APPMAN_LOG("sqlite3_prepare_v2 failed : Error[%s]", sqlite3_errmsg(db_handle));
		*count = -1;
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	rc = sqlite3_step(state);
	if (SQLITE_ROW == rc) {
		num_data = sqlite3_column_int(state, 0);
		*count = num_data;
	}

	sqlite3_finalize(state);
	location_appman_close();

	return LOCATION_APPMAN_ERROR_NONE;
}

int location_appman_get_app_list(location_appman_column_e sort_by, int *list_count, location_appman_s ** apps)
{
	sqlite3_stmt *state;
	int rc = SQLITE_OK;
	int column_index = 0;
	int i = 0;
	int count = 0;
	char query[LOCATION_APPMAN_QUERY_MAX_LEN] = { 0, };
	const char *tail;

	if (location_appman_open() != LOCATION_APPMAN_ERROR_NONE)
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;

	if (LOCATION_APPMAN_COLUMN_PACKAGE > sort_by || LOCATION_APPMAN_COLUMN_ENABLE < sort_by) {
		sort_by = LOCATION_APPMAN_COLUMN_INSTALLED_DATE;
		g_snprintf(query, LOCATION_APPMAN_QUERY_MAX_LEN, "SELECT * FROM LocationApp ORDER BY %s",
			   location_column_name[sort_by]);
	} else {
		g_snprintf(query, LOCATION_APPMAN_QUERY_MAX_LEN, "SELECT * FROM LocationApp ORDER BY %s",
			   location_column_name[sort_by]);
	}

	if (LOCATION_APPMAN_COLUMN_RECENTLY_USED == sort_by || LOCATION_APPMAN_COLUMN_ENABLE == sort_by) {

		rc = location_appman_query(query, " DESC");
		if (FALSE == rc) {
			location_appman_close();
			return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
		}
	}

	rc = sqlite3_prepare_v2(db_handle, query, strlen(query), &state, &tail);
	if (SQLITE_OK != rc) {
		LOCATION_APPMAN_LOG("sqlite3_prepare_v2 failed : Error[%s]", sqlite3_errmsg(db_handle));
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	location_appman_get_total_count(&count);
	LOCATION_APPMAN_LOG("count [%d]", count);
	*apps = (location_appman_s *) g_malloc0(sizeof(location_appman_s) * count);

	for (i = 0; i < count; i++) {
		char *name;
		rc = sqlite3_step(state);
		g_strlcpy((*apps + i)->package, (char *)sqlite3_column_text(state, column_index++), 64);
		g_strlcpy((*apps + i)->appicon, (char *)sqlite3_column_text(state, column_index++), 256);

		(*apps + i)->appid = sqlite3_column_int(state, column_index++);
		(*apps + i)->installed_date = sqlite3_column_int(state, column_index++);
		(*apps + i)->recently_used = sqlite3_column_int(state, column_index++);
		(*apps + i)->enable = (gboolean) sqlite3_column_int(state, column_index++);
		column_index = 0;

		location_appman_get_name((*apps + i)->package, &name);
		g_strlcpy((*apps + i)->appname, name, 64);

		if (SQLITE_ROW != rc) {
			break;
		}
	}

	*list_count = i;

	sqlite3_finalize(state);
	location_appman_close();

	return LOCATION_APPMAN_ERROR_NONE;
}

// GET
int location_appman_get_column_text(const char *package, location_appman_column_e type, char **data)
{
	LOCATION_APPMAN_NULL_ARG_CHECK(package);
	sqlite3_stmt *state;
	int rc = LOCATION_APPMAN_ERROR_NONE;
	const char *tail;
	char query[LOCATION_APPMAN_QUERY_MAX_LEN] = { 0, };

	if (location_appman_open() != LOCATION_APPMAN_ERROR_NONE)
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;

	g_snprintf(query, LOCATION_APPMAN_QUERY_MAX_LEN, "SELECT %s FROM LocationApp WHERE package = ?",
		   location_column_name[type]);

	rc = sqlite3_prepare_v2(db_handle, query, strlen(query), &state, &tail);
	if (SQLITE_OK != rc) {
		LOCATION_APPMAN_LOG("sqlite3_prepare_v2 failed : Error[%s]", sqlite3_errmsg(db_handle));
		location_appman_close();
		data = NULL;
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	rc = sqlite3_bind_text(state, 1, package, strlen(package), SQLITE_STATIC);
	if (SQLITE_OK != rc) {
		LOCATION_APPMAN_LOG("Fail to bind string to query. Error[%s]\n", sqlite3_errmsg(db_handle));
		sqlite3_finalize(state);
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	rc = sqlite3_step(state);

	if (SQLITE_ROW == rc) {
		*data = g_strdup((char *)sqlite3_column_text(state, 0));
	}

	sqlite3_finalize(state);
	location_appman_close();

	return LOCATION_APPMAN_ERROR_NONE;
}

int location_appman_get_column_int(const char *package, location_appman_column_e type, unsigned int *data)
{
	LOCATION_APPMAN_NULL_ARG_CHECK(package);
	sqlite3_stmt *state;
	int rc = LOCATION_APPMAN_ERROR_NONE;
	int num_data = 0;
	const char *tail;
	char query[LOCATION_APPMAN_QUERY_MAX_LEN] = { 0, };

	if (location_appman_open() != LOCATION_APPMAN_ERROR_NONE)
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;

	g_snprintf(query, LOCATION_APPMAN_QUERY_MAX_LEN, "SELECT %s FROM LocationApp WHERE package = ?",
		   location_column_name[type]);

	rc = sqlite3_prepare_v2(db_handle, query, strlen(query), &state, &tail);
	if (SQLITE_OK != rc) {
		LOCATION_APPMAN_LOG("sqlite3_prepare_v2 failed : Error[%s]", sqlite3_errmsg(db_handle));
		location_appman_close();
		data = NULL;
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	rc = sqlite3_bind_text(state, 1, package, strlen(package), SQLITE_STATIC);
	if (SQLITE_OK != rc) {
		LOCATION_APPMAN_LOG("Fail to bind string to query. Error[%s]\n", sqlite3_errmsg(db_handle));
		sqlite3_finalize(state);
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	rc = sqlite3_step(state);

	if (SQLITE_ROW == rc) {
		num_data = sqlite3_column_int(state, 0);
		*data = num_data;
	}

	sqlite3_finalize(state);
	location_appman_close();

	return LOCATION_APPMAN_ERROR_NONE;
}

int location_appman_get_name(const char *package, char **appname)
{
	LOCATION_APPMAN_NULL_ARG_CHECK(package);
	app_info_h app_info;
	char *name = NULL;
	int ret = 0;

	ret = app_manager_get_app_info(package, &app_info);
	if (ret != APP_MANAGER_ERROR_NONE) {
		if (package) {
			LOCATION_APPMAN_LOG("Can't get application information: [%s]", package);
			name = strdup (package);
		}
		else {
			LOCATION_APPMAN_LOG("Can't get application information: [%s]", package);
			return LOCATION_APPMAN_ERROR_INTERNAL;
		}
	}
	else {

		app_info_get_name(app_info, &name);
		if (name == NULL) {
			LOCATION_APPMAN_LOG("name of application[%s] is NULL", package);
			return LOCATION_APPMAN_ERROR_INTERNAL;
		}
	}

	*appname = (char *) g_malloc0(sizeof(char)*64);
	g_strlcpy(*appname, name, 64);
	g_free(name);

	return LOCATION_APPMAN_ERROR_NONE;
}

int location_appman_get_icon(const char *package, char **appicon)
{
	LOCATION_APPMAN_NULL_ARG_CHECK(package);
	int rc = LOCATION_APPMAN_ERROR_NONE;

	rc = location_appman_get_column_text(package, LOCATION_APPMAN_COLUMN_APPICON, appicon);
	if (LOCATION_APPMAN_ERROR_NONE != rc) {
		LOCATION_APPMAN_LOG("sqlite3_prepare_v2 failed : Error[%s]", sqlite3_errmsg(db_handle));
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	return LOCATION_APPMAN_ERROR_NONE;
}

int location_appman_get_appid(const char *package, int *appid)
{
	LOCATION_APPMAN_NULL_ARG_CHECK(package);
	int rc = LOCATION_APPMAN_ERROR_NONE;

	rc = location_appman_get_column_int(package, LOCATION_APPMAN_COLUMN_APPID, (unsigned int *)appid);
	if (LOCATION_APPMAN_ERROR_NONE != rc) {
		LOCATION_APPMAN_LOG("sqlite3_prepare_v2 failed : Error[%s]", sqlite3_errmsg(db_handle));
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	return LOCATION_APPMAN_ERROR_NONE;
}

int location_appman_get_installed_date(const char *package, unsigned int *installed_date)
{
	LOCATION_APPMAN_NULL_ARG_CHECK(package);
	int rc = LOCATION_APPMAN_ERROR_NONE;

	rc = location_appman_get_column_int(package, LOCATION_APPMAN_COLUMN_INSTALLED_DATE, installed_date);
	if (LOCATION_APPMAN_ERROR_NONE != rc) {
		LOCATION_APPMAN_LOG("sqlite3_prepare_v2 failed : Error[%s]", sqlite3_errmsg(db_handle));
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	return LOCATION_APPMAN_ERROR_NONE;
}

int location_appman_get_recently_used(const char *package, unsigned int *recently_used)
{
	LOCATION_APPMAN_NULL_ARG_CHECK(package);
	int rc = LOCATION_APPMAN_ERROR_NONE;

	rc = location_appman_get_column_int(package, LOCATION_APPMAN_COLUMN_RECENTLY_USED, recently_used);
	if (LOCATION_APPMAN_ERROR_NONE != rc) {
		LOCATION_APPMAN_LOG("sqlite3_prepare_v2 failed : Error[%s]", sqlite3_errmsg(db_handle));
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	return LOCATION_APPMAN_ERROR_NONE;
}

int location_appman_is_enabled(const char *package, int *enable)
{
	LOCATION_APPMAN_NULL_ARG_CHECK(package);
	int rc = SQLITE_OK;

	rc = location_appman_get_column_int(package, LOCATION_APPMAN_COLUMN_ENABLE, (unsigned int *)enable);
	if (LOCATION_APPMAN_ERROR_NONE != rc) {
		LOCATION_APPMAN_LOG("sqlite3_prepare_v2 failed : Error[%s]", sqlite3_errmsg(db_handle));
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	return LOCATION_APPMAN_ERROR_NONE;
}

// SET
int location_appman_set_column_text(const char *package, location_appman_column_e type, char *data)
{
	LOCATION_APPMAN_NULL_ARG_CHECK(package);
	sqlite3_stmt *state = NULL;
	int rc = SQLITE_OK;
	char query[LOCATION_APPMAN_QUERY_MAX_LEN] = { 0, };

	if (location_appman_open() != LOCATION_APPMAN_ERROR_NONE)
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;

	g_snprintf(query, LOCATION_APPMAN_QUERY_MAX_LEN, "UPDATE LocationApp SET %s = ? WHERE package = ?",
		   location_column_name[type]);

	rc = sqlite3_prepare_v2(db_handle, query, strlen(query), &state, NULL);
	if (SQLITE_OK != rc) {
		LOCATION_APPMAN_LOG("Fail to connect to table. Error[%s]\n", sqlite3_errmsg(db_handle));
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	rc = sqlite3_bind_text(state, 1, data, strlen(data), SQLITE_STATIC);
	if (SQLITE_OK != rc) {
		LOCATION_APPMAN_LOG("Fail to bind string to query. Error[%s]\n", sqlite3_errmsg(db_handle));
		sqlite3_finalize(state);
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	rc = sqlite3_bind_text(state, 2, package, strlen(package), SQLITE_STATIC);
	if (SQLITE_OK != rc) {
		LOCATION_APPMAN_LOG("Fail to bind string to query. Error[%s]\n", sqlite3_errmsg(db_handle));
		sqlite3_finalize(state);
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	rc = sqlite3_step(state);
	if (SQLITE_DONE != rc) {
		LOCATION_APPMAN_LOG("Fail to step. rc[%d]", rc);
		sqlite3_finalize(state);
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	sqlite3_finalize(state);
	location_appman_close();

	return LOCATION_APPMAN_ERROR_NONE;
}

// SET
int location_appman_set_column_int(const char *package, location_appman_column_e type, int data)
{
	LOCATION_APPMAN_NULL_ARG_CHECK(package);
	sqlite3_stmt *state = NULL;
	int rc = SQLITE_OK;
	char query[LOCATION_APPMAN_QUERY_MAX_LEN] = { 0, };

	if (location_appman_open() != LOCATION_APPMAN_ERROR_NONE)
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;

	g_snprintf(query, LOCATION_APPMAN_QUERY_MAX_LEN, "UPDATE LocationApp SET %s = ? WHERE package = ?",
		   location_column_name[type]);

	rc = sqlite3_prepare_v2(db_handle, query, strlen(query), &state, NULL);
	if (SQLITE_OK != rc) {
		LOCATION_APPMAN_LOG("Fail to connect to table. Error[%s]\n", sqlite3_errmsg(db_handle));
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	rc = sqlite3_bind_int(state, 1, data);
	if (SQLITE_OK != rc) {
		LOCATION_APPMAN_LOG("Fail to bind int to query. Error[%s]\n", sqlite3_errmsg(db_handle));
		sqlite3_finalize(state);
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	rc = sqlite3_bind_text(state, 2, package, strlen(package), SQLITE_STATIC);
	if (SQLITE_OK != rc) {
		LOCATION_APPMAN_LOG("Fail to bind string to query. Error[%s]\n", sqlite3_errmsg(db_handle));
		sqlite3_finalize(state);
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	rc = sqlite3_step(state);
	if (SQLITE_DONE != rc) {
		LOCATION_APPMAN_LOG("Fail to step. rc[%d]", rc);
		sqlite3_finalize(state);
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	sqlite3_finalize(state);
	location_appman_close();

	return LOCATION_APPMAN_ERROR_NONE;
}

int location_appman_set_icon(const char *package, char *appicon)
{
	LOCATION_APPMAN_NULL_ARG_CHECK(package);
	LOCATION_APPMAN_NULL_ARG_CHECK(appicon);
	int rc = LOCATION_APPMAN_ERROR_NONE;

	rc = location_appman_set_column_text(package, LOCATION_APPMAN_COLUMN_APPICON, appicon);
	if (LOCATION_APPMAN_ERROR_NONE != rc) {
		LOCATION_APPMAN_LOG("Fail to bind string to query. Error[%s]\n", sqlite3_errmsg(db_handle));
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}
	return LOCATION_APPMAN_ERROR_NONE;
}

int location_appman_set_appid(const char *package, int appid)
{
	LOCATION_APPMAN_NULL_ARG_CHECK(package);
	int rc = LOCATION_APPMAN_ERROR_NONE;

	rc = location_appman_set_column_int(package, LOCATION_APPMAN_COLUMN_APPID, appid);
	if (LOCATION_APPMAN_ERROR_NONE != rc) {
		LOCATION_APPMAN_LOG("Fail to bind int to query. Error[%s]\n", sqlite3_errmsg(db_handle));
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}
	return LOCATION_APPMAN_ERROR_NONE;
}

int location_appman_set_installed_date(const char *package, unsigned int installed_date)
{
	LOCATION_APPMAN_NULL_ARG_CHECK(package);
	int rc = LOCATION_APPMAN_ERROR_NONE;

	rc = location_appman_set_column_int(package, LOCATION_APPMAN_COLUMN_INSTALLED_DATE, installed_date);
	if (LOCATION_APPMAN_ERROR_NONE != rc) {
		LOCATION_APPMAN_LOG("Fail to bind int to query. Error[%s]\n", sqlite3_errmsg(db_handle));
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}
	return LOCATION_APPMAN_ERROR_NONE;
}

int location_appman_set_recently_used(const char *package, unsigned int recently_used)
{
	LOCATION_APPMAN_NULL_ARG_CHECK(package);
	int rc = LOCATION_APPMAN_ERROR_NONE;

	rc = location_appman_set_column_int(package, LOCATION_APPMAN_COLUMN_RECENTLY_USED, recently_used);
	if (LOCATION_APPMAN_ERROR_NONE != rc) {
		LOCATION_APPMAN_LOG("Fail to bind int to query. Error[%s]\n", sqlite3_errmsg(db_handle));
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}
	return LOCATION_APPMAN_ERROR_NONE;
}

int location_appman_set_on(const char *package, int onoff)
{
	LOCATION_APPMAN_NULL_ARG_CHECK(package);
	int rc = LOCATION_APPMAN_ERROR_NONE;

	rc = location_appman_set_column_int(package, LOCATION_APPMAN_COLUMN_ENABLE, onoff);
	if (LOCATION_APPMAN_ERROR_NONE != rc) {
		LOCATION_APPMAN_LOG("Fail to bind int to query. Error[%s]\n", sqlite3_errmsg(db_handle));
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}
	return LOCATION_APPMAN_ERROR_NONE;
}

int location_appamn_parse_manifest(const char *xml, location_appman_s ** appman)
{
	LOCATION_APPMAN_NULL_ARG_CHECK(xml);

	xmlDocPtr doc;
	xmlNodePtr cur;
	xmlChar *key;
	location_appman_s *tmp;

	char *element[4] = { "Location", "Package", "Appid", "Enable" };

	doc = xmlParseDoc(BAD_CAST(xml));

	if (NULL == doc) {
		LOCATION_APPMAN_LOG("Document not parsed successfully.\n");
		return LOCATION_APPMAN_ERROR_INVALID_XML;
	}

	cur = xmlDocGetRootElement(doc);
	if (NULL == cur) {
		LOCATION_APPMAN_LOG("empty document\n");
		xmlFreeDoc(doc);
		return LOCATION_APPMAN_ERROR_INVALID_XML;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *)element[0]) != 0) {
		LOCATION_APPMAN_LOG("document of the wrong type, root node != LocationApp\n");
		xmlFreeDoc(doc);
		return LOCATION_APPMAN_ERROR_INVALID_XML;
	}

	tmp = (location_appman_s *) g_malloc0(sizeof(location_appman_s));
	cur = cur->xmlChildrenNode;
	while (NULL != cur) {
		if (xmlStrcmp(cur->name, (const xmlChar *)element[1]) == 0) {
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);

			g_strlcpy(tmp->package, (char *)key, 64);
			LOCATION_APPMAN_LOG("tmp->package = [%s]", tmp->package);
			xmlFree(key);
		} else if (xmlStrcmp(cur->name, (const xmlChar *)element[2]) == 0) {
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);

			tmp->appid = atoi((char *)key);
			LOCATION_APPMAN_LOG("tmp->appid  = [%d]", tmp->appid);
			xmlFree(key);
		} else if (xmlStrcmp(cur->name, (const xmlChar *)element[3]) == 0) {
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);

			tmp->enable = atoi((char *)key);
			LOCATION_APPMAN_LOG("tmp->enable  = [%d]", tmp->enable);
			xmlFree(key);
		}
		cur = cur->next;
	}

	*appman = tmp;

	xmlFreeDoc(doc);
	return LOCATION_APPMAN_ERROR_NONE;
}

int location_appman_get_package_by_pid(pid_t pid, location_appman_s ** appman)
{
	if (pid <= 0) {
		LOCATION_APPMAN_LOG("Invalid PID : [%ld]", pid);
		return LOCATION_APPMAN_ERROR_INVALID_PARAMETER;
	}

	location_appman_s *tmp = NULL;
	char *package = NULL;
	int ret = 0;

	ret = app_manager_get_package(pid, &package);
	if (ret != APP_MANAGER_ERROR_NONE) {
		const char *pkg_name = g_getenv(LOCATION_APPMAN_PKG_NAME);
		if (!pkg_name) {
			LOCATION_APPMAN_LOG("Can't find package name : [%ld]", pid);
			return LOCATION_APPMAN_ERROR_INVALID_PARAMETER;
		}
		package = g_strdup(pkg_name);
	}

	tmp = (location_appman_s *) g_malloc0(sizeof(location_appman_s));
	g_strlcpy(tmp->package, package, 64);
	*appman = tmp;

	g_free(package);

	return LOCATION_APPMAN_ERROR_NONE;
}

int location_appman_find_package(const char *package, int *findpackage)
{
	LOCATION_APPMAN_NULL_ARG_CHECK(package);
	sqlite3_stmt *state;
	int rc = LOCATION_APPMAN_ERROR_NONE;
	int enable = 0;
	const char *tail;
	char query[LOCATION_APPMAN_QUERY_MAX_LEN] = { 0, };

	if (location_appman_open() != LOCATION_APPMAN_ERROR_NONE)
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;

	g_snprintf(query, LOCATION_APPMAN_QUERY_MAX_LEN, "SELECT enable FROM LocationApp WHERE package = ?");

	rc = sqlite3_prepare_v2(db_handle, query, strlen(query), &state, &tail);
	if (SQLITE_OK != rc) {
		LOCATION_APPMAN_LOG("sqlite3_prepare_v2 failed : Error[%s]", sqlite3_errmsg(db_handle));
		location_appman_close();
		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	rc = sqlite3_bind_text(state, 1, package, strlen(package), SQLITE_STATIC);
	if (SQLITE_OK != rc) {
		LOCATION_APPMAN_LOG("Fail to bind string to query. Error[%s]\n", sqlite3_errmsg(db_handle));
		sqlite3_finalize(state);
		location_appman_close();

		return LOCATION_APPMAN_ERROR_SQLITE_FAIL;
	}

	rc = sqlite3_step(state);
	if (SQLITE_ROW == rc) {
		enable = sqlite3_column_int(state, 0);
		if (enable == 0) {
			*findpackage = LOCATION_APPMAN_ENABLE_OFF;
		} else {
			*findpackage = LOCATION_APPMAN_ENABLE_ON;
		}
	} else if (SQLITE_DONE) {
		*findpackage = LOCATION_APPMAN_PACKAGE_NOTFOUND;
	}

	sqlite3_finalize(state);
	location_appman_close();

	return LOCATION_APPMAN_ERROR_NONE;
}

int location_appman_register_package(location_appman_s * appman)
{
	LOCATION_APPMAN_NULL_ARG_CHECK(appman);
	LOCATION_APPMAN_NULL_ARG_CHECK(appman->package);

	app_info_h app_info;
	time_t timestamp;
	char *name = NULL;
	int ret = 0;

	ret = app_manager_get_app_info(appman->package, &app_info);
	if (ret == APP_MANAGER_ERROR_NONE) {
		app_info_get_name(app_info, &name);
		if (name != NULL) {
			g_strlcpy(appman->appname, name, 64);
			g_free(name);
		}
		app_info_get_icon(app_info, &name);
		if (name != NULL) {
			g_strlcpy(appman->appicon, name, 256);
			g_free(name);
		}
	} else {
		LOCATION_APPMAN_LOG("Can't get application information: [%s]", appman->package);
		return LOCATION_APPMAN_ERROR_INTERNAL;
	}

	// wait for a new api.
	//if(0 == appman->appid)
	//      appman->appid = 0;

	time(&timestamp);
	appman->installed_date = timestamp;
	appman->recently_used = timestamp;

	location_appman_insert(appman);

	g_free(appman);
	app_info_destroy(app_info);

	return LOCATION_APPMAN_ERROR_NONE;
}
