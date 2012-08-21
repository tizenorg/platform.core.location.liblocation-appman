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

#include <glib.h>
#include <gcrypt.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "location-appman.h"
#include "location-appman-log.h"
#include "location-appman-crypt.h"

#define CIPHER_ALGORITHM  GCRY_CIPHER_BLOWFISH
#define CIPHER_MODE       GCRY_CIPHER_MODE_ECB
#define ONE_CIPHER_ALGORITHM_MIN_KEY_LENGTH		5
#define ONE_CIPHER_ALGORITHM_MAX_KEY_LENGTH		32

static const char *key_string = "LocationAppman";

static void *string2hex(const char *string, size_t *buff_len)
{
	LOCATION_APPMAN_NULL_ARG_CHECK(string);
	LOCATION_APPMAN_NULL_ARG_CHECK(buff_len);

	*buff_len = g_utf8_strlen(string, -1)*2+1;
	unsigned char *buffer = g_malloc0_n (*buff_len, sizeof(unsigned char*));
	unsigned int idx = 0;
	for (idx = 0; idx < *buff_len; idx+=2) {
		char temp[3];
		if (!g_ascii_isalnum(string[idx/2])) {
			break;
		}
		g_snprintf (temp, 3, "%x", (unsigned int)string[idx/2]);
		buffer[idx] = temp[0];
		buffer[idx+1] = temp[1];
	}
	buffer[*buff_len-1] = '\0';
	return buffer;
}

static gboolean
location_appman_encrypt_decrypt(gboolean is_encrypt, const void *key_buf, size_t key_buflen, void *inbuf, void *outbuf)
{
	LOCATION_APPMAN_NULL_ARG_CHECK(inbuf);
	LOCATION_APPMAN_NULL_ARG_CHECK(outbuf);

	gpg_error_t err;
	gcry_cipher_hd_t hd;
	int	keylen, len;
	void *outblock = NULL;
	void *inblock = NULL;

	len = strlen((char *)inbuf);

	err = gcry_cipher_open(&hd, CIPHER_ALGORITHM, CIPHER_MODE, 0);
	if(err) {
		LOCATION_APPMAN_LOG("fail to gcry_cipher_open: %s\n", gpg_strerror (err));
		return FALSE;
	}

	keylen = gcry_cipher_get_algo_keylen(CIPHER_ALGORITHM);
	if(!keylen) {
		LOCATION_APPMAN_LOG("fail to gcry_cipher_get_algo_keylen, %d\n", keylen);
		return FALSE;
	}

	if(keylen < ONE_CIPHER_ALGORITHM_MIN_KEY_LENGTH || keylen > ONE_CIPHER_ALGORITHM_MAX_KEY_LENGTH)
	{
		LOCATION_APPMAN_LOG("keylength problem (%d)", keylen);
		return FALSE;
	}

	err = gcry_cipher_setkey(hd, key_buf, key_buflen);
	if(err) {
		//delete
		LOCATION_APPMAN_LOG("fail to gcry_cipher_setkey: %s\n", gpg_strerror (err));
		gcry_cipher_close(hd);
		return FALSE;
	}

	inblock = gcry_xmalloc (keylen);
	outblock = gcry_xmalloc (keylen);
	int idx =0 ;
	while(idx < len) {
		size_t tlen = ((len - idx)<keylen)? len-idx : keylen;
		memset(inblock, 0, keylen);
		memset(outblock, 0, keylen);
		if(!memcpy(inblock, inbuf+idx, tlen)) {
			LOCATION_APPMAN_LOG("location_appman_encrypt_decrypt : inblock MEMCPY ERROR/n");
	       		break;
	    	}

        		if(is_encrypt) err = gcry_cipher_encrypt(hd, outblock, keylen, inblock, keylen);
		else           err = gcry_cipher_decrypt(hd, outblock, keylen, inblock, keylen);

		if(err) {
			LOCATION_APPMAN_LOG("fail to gcry_cipher_encrypt/gcry_cipher_decrypt : %s\n", gpg_strerror (err));
			break;
		}
		if(!memcpy(outbuf+idx, outblock, keylen)) {
			LOCATION_APPMAN_LOG("gcry_cipher_decrypt : outbuf MEMCPY ERROR/n");
		}
	        	break;
	}
	idx += keylen;
	return TRUE;
}

int location_appman_encrypt(const char *key, const char *input, char *output)
{
	LOCATION_APPMAN_NULL_ARG_CHECK(input);
	void *keybuf = NULL;
	char tmp_outbuf[ONE_CIPHER_ALGORITHM_MAX_KEY_LENGTH] = {0, };
	size_t key_buflen;

	if(key == NULL)
		keybuf = string2hex(key_string, &key_buflen);
	else
		keybuf = string2hex(key, &key_buflen);

	if(NULL != keybuf) {
		if(!location_appman_encrypt_decrypt(TRUE, keybuf, key_buflen, (void *)input, (void *)tmp_outbuf)) {
			LOCATION_APPMAN_LOG("location_appman_encrypt :  location_private_encrypt_decrypt FAIL!!!\n");

			return LOCATION_APPMAN_CRYPT_ERROR_GCRYPT;
		}else {
			strcpy(output, tmp_outbuf);
		}
	}else {
		LOCATION_APPMAN_LOG("location_appman_encrypt : keybuf is NULL\n");
		return LOCATION_APPMAN_CRYPT_ERROR_INTERNAL;
	}

	if(keybuf) gcry_free (keybuf);

	return LOCATION_APPMAN_CRYPT_ERROR_NONE;
}


int location_appman_decrypt(const char *key, const char *input, char *output)
{
	LOCATION_APPMAN_NULL_ARG_CHECK(input);
	size_t key_buflen;
	void *keybuf = NULL;
	char tmp_outbuf[ONE_CIPHER_ALGORITHM_MAX_KEY_LENGTH] = {0, };

	if(key == NULL)
		keybuf = string2hex(key_string, &key_buflen);
	else
		keybuf = string2hex(key, &key_buflen);

	if(NULL != keybuf) {
		if(!location_appman_encrypt_decrypt(FALSE, keybuf, key_buflen, (void *)input, (void *)tmp_outbuf)) {
			LOCATION_APPMAN_LOG("location_appman_decrypt : location_private_encrypt_decrypt FAIL!!\n");;
			return LOCATION_APPMAN_CRYPT_ERROR_GCRYPT;
		}else {
			strcpy(output, tmp_outbuf);
		}
	}else {
		LOCATION_APPMAN_LOG("location_appman_decrypt : keybuf is NULL\n");
		return LOCATION_APPMAN_CRYPT_ERROR_INTERNAL;
	}

	if (keybuf) gcry_free (keybuf);

	return LOCATION_APPMAN_CRYPT_ERROR_NONE;
}
