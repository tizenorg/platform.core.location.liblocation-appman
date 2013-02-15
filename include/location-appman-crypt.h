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

#ifndef LOCATION_APPMAN_CRYPTION_H_
#define LOCATION_APPMAN_CRYPTION_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	LOCATION_APPMAN_CRYPT_ERROR_NONE = 0,
	LOCATION_APPMAN_CRYPT_ERROR_GCRYPT = -200,
	LOCATION_APPMAN_CRYPT_ERROR_INTERNAL= LOCATION_APPMAN_CRYPT_ERROR_GCRYPT | 0x01,
	LOCATION_APPMAN_CRYPT_ERROR_UNKNOWN = LOCATION_APPMAN_CRYPT_ERROR_GCRYPT | 0x02,
}Location_appamn_error_last_postion;

int location_appman_encrypt(const char *key, const char *input, char *output);
int location_appman_decrypt(const char *key, const char *input, char *output);
#ifdef __cplusplus
}
#endif

#endif			/* LOCATION_APPMAN_CRYPTION_H_ */
