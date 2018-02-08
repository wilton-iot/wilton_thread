/*
 * Copyright 2017, alex at staticlibs.net
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

/* 
 * File:   wilton_thread.h
 * Author: alex
 *
 * Created on September 22, 2017, 8:25 PM
 */

#ifndef WILTON_THREAD_H
#define WILTON_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

char* wilton_thread_run(
        void* cb_ctx,
        void (*cb)(
                void* cb_ctx));

char* wilton_thread_sleep_millis(
        int millis);

#ifdef __cplusplus
}
#endif

#endif /* WILTON_THREAD_H */

