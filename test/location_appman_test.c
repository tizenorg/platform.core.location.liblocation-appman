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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "location-appman.h"
#include "location-appman-crypt.h"

static GMainLoop *g_mainloop = NULL;

int location_appman_insert_test(void)
{
	int rc = LOCATION_APPMAN_ERROR_NONE;
	int i;

	printf("location_appman_insert_test.\n");

	location_appman_s	test_data[5] = {{"test1_package", "test1_name", "test1_icon", 1, 1325882464, 1325893464, TRUE},
						{"test2_package", "test2_name", "test2_icon", 2, 1325893464, 1325900001, TRUE},
						{"test3_package", "test3_name", "test3_icon", 3, 1325898464, 1325900264, FALSE},
						{"test4_package", "test4_name", "test4_icon", 4, 1325866464, 1325901464, FALSE},
						{"test5_package", "test5_name", "test5_icon", 5, 1325888464, 1325900114, TRUE}};


	for(i = 0; i < 5; i++) {
		rc = location_appman_insert(&test_data[i]);
		if(LOCATION_APPMAN_ERROR_NONE != rc) {
			printf( "Fail to location_app_insert. Error[%d], i = %d\n", rc, i );
			return rc;
		}
	}
	return rc;
}

int location_appman_update_test(void)
{
	location_appman_s	test_update;
	int rc = LOCATION_APPMAN_ERROR_NONE;

	printf("location_appman_update_test.\n");

	strcpy(test_update.package, "test2_package");
	strcpy(test_update.appname, "modi2_name");
	strcpy(test_update.appicon, "modi2_icon");
	test_update.appid = 4;
	test_update.installed_date = 1325903464;
	test_update.recently_used = 1325903464;
	test_update.enable = FALSE;

	rc = location_appman_update(&test_update);
	if(LOCATION_APPMAN_ERROR_NONE != rc) {
		printf( "Fail to location_appman_update. Error[%d]\n", rc);
		return rc;
	}
	return rc;
}

int location_appman_delete_test()
{
	int rc = LOCATION_APPMAN_ERROR_NONE;

	printf("location_appman_delete_test.\n");

	rc = location_appman_delete((const char*)"test2_package");
	if(LOCATION_APPMAN_ERROR_NONE != rc) {
		printf( "Fail to location_appman_delete. Error[%d]\n", rc);
		return rc;
	}
	return rc;
}

int location_appman_reset_test()
{
	int rc = LOCATION_APPMAN_ERROR_NONE;

	printf("location_appman_reset_test.\n");

	rc = location_appman_reset();
	if(LOCATION_APPMAN_ERROR_NONE != rc) {
		printf( "Fail to location_appman_delete. Error[%d]\n", rc);
		return rc;
	}
	return rc;
}

int location_appman_get_apps_test_by_installed_date()
{
	location_appman_s *appmantest = NULL;
	int rc = LOCATION_APPMAN_ERROR_NONE, i;
	int list_count = 0;

	rc = location_appman_get_app_list(LOCATION_APPMAN_COLUMN_INSTALLED_DATE, &list_count, &appmantest);

	if(LOCATION_APPMAN_ERROR_NONE != rc) {
		printf( "location_appman_get_apps_test. Error[%d]\n", rc);
		return rc;
	}

	for(i = 0; i < list_count; i++) {
		printf( "%s, %s, %s, %d, installed_date = %d, %d, %d\n", appmantest[i].package,
									appmantest[i].appname,
									appmantest[i].appicon,
									appmantest[i].appid,
									appmantest[i].installed_date,
									appmantest[i].recently_used,
									appmantest[i].enable);
	}

	for(i = 0; i < list_count; i++) {
		g_slice_free(location_appman_s, (appmantest + i));
	}

	return rc;
}

int location_appman_get_apps_test_by_recently_used()
{
	location_appman_s *appmantest = NULL;
	int rc = LOCATION_APPMAN_ERROR_NONE, i;
	int list_count = 0;

	rc = location_appman_get_app_list(LOCATION_APPMAN_COLUMN_RECENTLY_USED, &list_count, &appmantest);

	if(LOCATION_APPMAN_ERROR_NONE != rc) {
		printf( "location_appman_get_apps_test. Error[%d]\n", rc);
		return rc;
	}

	for(i = 0; i < list_count; i++) {
		printf( "%s, %s, %s, %d, %d, recently_used = %d, %d\n", appmantest[i].package,
												appmantest[i].appname,
												appmantest[i].appicon,
												appmantest[i].appid,
												appmantest[i].installed_date,
												appmantest[i].recently_used,
												appmantest[i].enable);
	}

	for(i = 0; i < list_count; i++) {
		g_slice_free(location_appman_s, (appmantest + i));
	}

	return rc;
}

int location_appman_get_apps_test_by_default()
{
	location_appman_s *appmantest = NULL;
	int rc = LOCATION_APPMAN_ERROR_NONE, i;
	int list_count = 0;

	rc = location_appman_get_app_list(-1, &list_count, &appmantest);

	if(LOCATION_APPMAN_ERROR_NONE != rc) {
		printf( "location_appman_get_apps_test. Error[%d]\n", rc);
		return rc;
	}

	for(i = 0; i < list_count; i++) {
		printf( "%s, %s, %s, %d, installed_date = %d, %d, %d\n", appmantest[i].package,
												appmantest[i].appname,
												appmantest[i].appicon,
												appmantest[i].appid,
												appmantest[i].installed_date,
												appmantest[i].recently_used,
												appmantest[i].enable);
	}

	for(i = 0; i < list_count; i++) {
		g_slice_free(location_appman_s, (appmantest + i));
	}

	return rc;
}

/********** GET **********/
int location_appman_get_apps_test()
{
	int rc = LOCATION_APPMAN_ERROR_NONE;
	int	total = 0;

	printf( "location_appman_get_apps_test\n");

	rc = location_appman_get_total_count(&total);
	printf( "return value = %d\n", rc);

	printf( "============== SORTING INSTALLED_DATE ==============\n");
	rc = location_appman_get_apps_test_by_installed_date();
	printf( "location_appman_get_apps_test_by_installed_date : return value = %d\n", rc);

	printf( "============== SORTING RECENTLY_USED ==============\n");
	rc = location_appman_get_apps_test_by_recently_used();
	printf( "location_appman_get_apps_test_by_recently_used : return value = %d\n", rc);

	printf( "============== SORTING DEFAULT ==============\n");
	rc = location_appman_get_apps_test_by_default();
	printf( "location_appman_get_apps_test_by_default : return value = %d\n", rc);

	return rc;
}

int location_appman_get_name_test(const char *package, char **name)
{
	int rc = LOCATION_APPMAN_ERROR_NONE;

	printf( "location_appman_get_name_test\n");
	rc = location_appman_get_name(package, name);
	if(LOCATION_APPMAN_ERROR_NONE != rc) {
		printf( "Fail to location_appman_get_name_test. Error[%d]\n", rc);
		return rc;
	}
	return rc;
}

int location_appman_get_icon_test(const char *package, char **icon)
{
	int rc = LOCATION_APPMAN_ERROR_NONE;

	printf( "location_appman_get_icon_test\n");
	rc = location_appman_get_icon(package, icon);
	if(LOCATION_APPMAN_ERROR_NONE != rc) {
		printf( "Fail to location_appman_get_icon_test. Error[%d]\n", rc);
		return rc;
	}
	return rc;
}

int location_appman_get_appid_test(const char *package, int *appid)
{
	int rc = LOCATION_APPMAN_ERROR_NONE;

	printf( "location_appman_get_appid_test\n");
	rc = location_appman_get_appid(package, appid);
	if(LOCATION_APPMAN_ERROR_NONE != rc) {
		printf( "Fail to location_appman_get_appid_test. Error[%d]\n", rc);
		return rc;
	}
	return rc;
}

int location_appman_get_installed_date_test(const char *package, unsigned int *installed_date)
{
	int rc = LOCATION_APPMAN_ERROR_NONE;

	printf( "location_appman_get_installed_date_test\n");
	rc = location_appman_get_installed_date(package, installed_date);
	if(LOCATION_APPMAN_ERROR_NONE != rc) {
		printf( "Fail to location_appman_get_installed_date_test. Error[%d]\n", rc);
		return rc;
	}
	return rc;
}

int location_appman_get_recently_used_test(const char *package, unsigned int *recently_used)
{
	int rc = LOCATION_APPMAN_ERROR_NONE;

	printf( "location_appman_get_recently_used_test\n");
	rc = location_appman_get_recently_used(package, recently_used);
	if(LOCATION_APPMAN_ERROR_NONE != rc) {
		printf( "Fail to location_appman_get_recently_used_test. Error[%d]\n", rc);
		return rc;
	}
	return rc;
}

int location_appman_is_enabled_test(const char *package, int *enable)
{
	int rc = LOCATION_APPMAN_ERROR_NONE;

	printf( "location_appman_get_enable_test\n");
	rc = location_appman_is_enabled(package, enable);
	if(LOCATION_APPMAN_ERROR_NONE != rc) {
		printf( "Fail to location_appman_get_enable_test. Error[%d]\n", rc);
		return rc;
	}
	return rc;
}

/********** SET **********/
int location_appman_set_icon_test(const char *package, char *icon)
{
	int rc = LOCATION_APPMAN_ERROR_NONE;

	printf( "location_appman_set_icon_test\n");
	rc = location_appman_set_icon(package, icon);
	if(LOCATION_APPMAN_ERROR_NONE != rc) {
		printf( "Fail to location_appman_set_icon_test. Error[%d]\n", rc);
		return rc;
	}
	return rc;
}

int location_appman_set_appid_test(const char *package, int appid)
{
	int rc = LOCATION_APPMAN_ERROR_NONE;

	printf( "location_appman_set_appid_test\n");
	rc = location_appman_set_appid(package, appid);
	if(LOCATION_APPMAN_ERROR_NONE != rc) {
		printf( "Fail to location_appman_set_appid_test. Error[%d]\n", rc);
		return rc;
	}
	return rc;
}

int location_appman_set_installed_date_test(const char *package, unsigned int installed_date)
{
	int rc = LOCATION_APPMAN_ERROR_NONE;

	printf( "location_appman_set_installed_date_test\n");
	rc = location_appman_set_installed_date(package, installed_date);
	if(LOCATION_APPMAN_ERROR_NONE != rc) {
		printf( "Fail to location_appman_set_installed_date_test. Error[%d]\n", rc);
		return rc;
	}
	return rc;
}

int location_appman_set_recently_used_test(const char *package, unsigned int recently_used)
{
	int rc = LOCATION_APPMAN_ERROR_NONE;

	printf( "location_appman_set_recently_used_test\n");
	rc = location_appman_set_recently_used(package, recently_used);
	if(LOCATION_APPMAN_ERROR_NONE != rc) {
		printf( "Fail to location_appman_set_recently_used_test. Error[%d]\n", rc);
		return rc;
	}
	return rc;
}

int location_appman_set_on_test(const char *package, int onoff)
{
	int rc = LOCATION_APPMAN_ERROR_NONE;

	printf( "location_appman_set_enable_test\n");
	rc = location_appman_set_on(package, onoff);
	if(LOCATION_APPMAN_ERROR_NONE != rc) {
		printf( "Fail to location_appman_set_enable_test. Error[%d]\n", rc);
		return rc;
	}
	return rc;
}

int location_appman_set_get_encrypt_test()
{
	int rc = LOCATION_APPMAN_ERROR_NONE;
	//int nbuf = 0;
	unsigned int n2buf = 0;
	unsigned int nRet = 0;
	char *buf = {0, };
	char tmpstr[32] = "efff222";
	char output[32] = {0, };
	char realvalue[32] = {0, };
	char pack[64] = "test3_package";
	char key[32] = "LocationAppman";


	// name
	printf( "location_appman_set_get_encrypt_test\n");
	rc = location_appman_get_name(pack, &buf);
	printf( "location_appman_set_get_encrypt_test : rc = %d, name = %s\n", rc, buf);

	rc = location_appman_encrypt(NULL, (const char *)buf, output);
	printf( "location_appman_set_get_encrypt_test : rc = %d, name's output = %s\n", rc, output);
	rc = location_appman_decrypt(NULL, (const char *)output, realvalue);
	printf( "location_appman_set_get_encrypt_test : rc = %d, name's realvalue = %s\n", rc, realvalue);
	g_free(buf);

	//recently_used
	rc = location_appman_get_recently_used_test(pack, &n2buf);
	printf( "location_appman_set_get_encrypt_test recently_used = %d\n", n2buf);

	sprintf(buf, "%d", n2buf);
	strcpy(tmpstr, buf);
	rc = location_appman_encrypt(key, (const char*)tmpstr, output);
	printf( "location_appman_set_get_encrypt_test recently_used = %s\n", output);

	rc = location_appman_decrypt(key, (const char*)output, realvalue);
	nRet = (unsigned int)atof(realvalue);
	printf( "location_appman_set_get_encrypt_test : rc = %d, recently's nRet = %d\n", rc, nRet);

	//location_appman_decrypt(pack, output);

	if(LOCATION_APPMAN_ERROR_NONE != rc) {
		printf( "Fail to location_appman_set_encrypt_test. Error[%d]\n", rc);
		return rc;
	}
	return rc;
}

int location_appman_get_set_test()
{
	int rc = LOCATION_APPMAN_ERROR_NONE;
	int nbuf = 0;
	unsigned int n2buf = 0;
	char *buf = {0, };
	char *pack = "test3_package";

	printf( "\n\nDB SET/GET ================ location_app_get_set_test\n");

	rc = location_appman_set_icon(pack, (char *)"set_icon");
	rc = location_appman_set_appid(pack, 12);
	rc = location_appman_set_installed_date(pack, 12000);
	rc = location_appman_set_recently_used(pack, 21000);
	rc = location_appman_set_on(pack, TRUE);

	rc = location_appman_get_name(pack, &buf);
	printf( "get name = %s\n", buf);
	g_free(buf);

	rc = location_appman_get_icon(pack, &buf);
	printf( "get icon = %s\n", buf);
	g_free(buf);

	rc = location_appman_get_appid(pack, &nbuf);
	printf( "get appid = %d\n", nbuf);

	rc = location_appman_get_installed_date(pack, &n2buf);
	printf( "get installed_date = %d\n", n2buf);

	rc = location_appman_get_recently_used(pack, &n2buf);
	printf( "get recently_used = %d\n", n2buf);

	rc = location_appman_is_enabled(pack, &nbuf);
	printf( "get enable = %d\n", nbuf);

	return rc;
}

int location_appman_parse_test()
{
	location_appman_s *appmantest = NULL;
	int rc = LOCATION_APPMAN_ERROR_NONE;
	int findpackage = 0;
	int n;

	const char *xmlbuffer = "<Location>\n<Package>package1</Package>\n<Appid>1</Appid>\n<Enable>1</Enable>\n</Location>";

	rc = location_appamn_parse_manifest(xmlbuffer, &appmantest);

	if (rc == LOCATION_APPMAN_ERROR_NONE) {
		location_appman_register_package(appmantest);
	}

	printf( "%s, %s, %s, %d, installed_date = %d, %d, %d\n", appmantest->package,
									appmantest->appname,
									appmantest->appicon,
									appmantest->appid,
									appmantest->installed_date,
									appmantest->recently_used,
									appmantest->enable);

	//location_appman_s *appman_by_pid = NULL;
	//pid_t t = 0;
	//t = getpid();
	//printf("pid : %d\n", t);
	//n = location_appman_get_package_by_pid(t, &appman_by_pid);

	n = location_appman_find_package("pppp", &findpackage);
	printf("location_appman_find_package : n = %d, findpackage = %d\n", n, findpackage);

	g_slice_free(location_appman_s, appmantest);

	return rc;
}

void location_appman_test()
{
	int n;

	n = location_appman_reset_test();
	printf("location_appman_reset_test : %d\n", n);

	n = location_appman_insert_test();
	printf("location_appman_insert_test : %d\n", n);

	n = location_appman_update_test();
	printf("location_appman_test_update_table_all : %d\n", n);

	n = location_appman_delete_test();
	printf("location_appman_delete_test : %d\n", n);

	n = location_appman_get_apps_test();
	printf("location_appman_test_update_table_all : %d\n", n);

	n = location_appman_get_set_test();
	printf("location_appman_get_set_test : %d\n", n);

	n = location_appman_parse_test();
	printf("location_appman_xml_parser_test : %d\n", n);

	/*
	n = location_appman_set_get_encrypt_test();
	printf("location_appman_get_set_test : %d\n", n);
	*/
}

static gboolean exit_program(gpointer data)
{
	g_main_loop_quit(g_mainloop);
	printf("Quit g_main_loop\n");
	return FALSE;
}

int main(int argc, char **argv)
{
	g_setenv("PKG_NAME", "com.samsung.location-appman-test", 1);
	g_mainloop = g_main_loop_new(NULL, 0);
	location_appman_test();
	g_timeout_add_seconds(30, exit_program, NULL);
	g_main_loop_run(g_mainloop);
	return 0;
}

