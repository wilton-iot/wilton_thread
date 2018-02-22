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
 * File:   wilton_thread.cpp
 * Author: alex
 *
 * Created on December 9, 2016, 10:19 PM
 */

#include "wilton/wilton_thread.h"

#include <cstdint>
#include <chrono>
#include <thread>

#include "staticlib/config.hpp"
#include "staticlib/support.hpp"
#include "staticlib/utils.hpp"

#include "wilton/support/alloc.hpp"
#include "wilton/support/exception.hpp"

char* wilton_thread_run(void* cb_ctx, void (*cb)(void* cb_ctx)) /* noexcept */ {
    if (nullptr == cb) return wilton::support::alloc_copy(TRACEMSG("Null 'cb' parameter specified"));
    try {
        auto th = std::thread([cb, cb_ctx]() {
            auto cleaner = sl::support::defer([]() STATICLIB_NOEXCEPT {
                auto tid = sl::support::to_string_any(std::this_thread::get_id());
                wilton_clean_tls(tid.c_str(), static_cast<int>(tid.length()));
            });
            try {
                cb(cb_ctx);
            } catch (...) {
                // silently prevent termination
            }
        });
        th.detach();
        return nullptr;
    } catch (const std::exception& e) {
        return wilton::support::alloc_copy(TRACEMSG(e.what() + "\nException raised"));
    }
}

char* wilton_thread_sleep_millis(int millis) /* noexcept */ {
    if (!sl::support::is_uint32_positive(millis)) return wilton::support::alloc_copy(TRACEMSG(
            "Invalid 'millis' parameter specified: [" + sl::support::to_string(millis) + "]"));
    try {
        uint32_t millis_u32 = static_cast<uint32_t> (millis);
        std::this_thread::sleep_for(std::chrono::milliseconds{millis_u32});
        return nullptr;
    } catch (const std::exception& e) {
        return wilton::support::alloc_copy(TRACEMSG(e.what() + "\nException raised"));
    }
}
