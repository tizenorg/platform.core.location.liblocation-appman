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

#ifndef LOCATION_APPMAN_LOG_H__
#define LOCATION_APPMAN_LOG_H__

/**
 * @file location-appman-log.h
 * @brief This file contains macro functions for logging.
 */

#include <dlog.h>
#include <libgen.h>

#define TAG_LOCATION_APPMAN		"location-appman"

#define LOCATION_APPMAN_LOG(fmt,args...)	SLOG(LOG_DEBUG, TAG_LOCATION_APPMAN, "[%-15s:%-4d]"fmt"\n", basename(__FILE__), __LINE__, ##args)
#define LOCATION_APPMAN_FUNC				LOCATION_APPMAN_LOGD("FUNC: %s", __FUNCTION__)

#define LOCATION_APPMAN_NULL_ARG_CHECK(arg) \
		if(NULL != arg) {} else \
		{LOCATION_APPMAN_LOG("%s(0x%08x)\n", "LOCATION_APPMAN_ERROR_INVALID_PARAMETER",LOCATION_APPMAN_ERROR_INVALID_PARAMETER);}

#endif

