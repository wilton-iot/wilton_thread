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
 * File:   wiltoncall_thread.cpp
 * Author: alex
 *
 * Created on January 11, 2017, 8:43 AM
 */

#include <functional>
#include <string>

#include "staticlib/support.hpp"
#include "staticlib/json.hpp"

#include "wilton/wiltoncall.h"
#include "wilton/wilton_thread.h"

#include "wilton/support/buffer.hpp"
#include "wilton/support/misc.hpp"
#include "wilton/support/logging.hpp"
#include "wilton/support/registrar.hpp"

namespace wilton {
namespace thread {

namespace { // anonymous

const std::string logger = std::string("wilton.thread");

} // namespace

support::buffer run(sl::io::span<const char> data) {
    // json parse
    auto json = sl::json::load(data);
    auto rcallback = std::ref(sl::json::null_value_ref());
    for (const sl::json::field& fi : json.as_object()) {
        auto& name = fi.name();
        if ("callbackScript" == name) {
            support::check_json_callback_script(fi);
            rcallback = fi.val();
        } else {
            throw support::exception(TRACEMSG("Unknown data field: [" + name + "]"));
        }
    }
    if (sl::json::type::nullt == rcallback.get().json_type()) throw support::exception(TRACEMSG(
            "Required parameter 'callbackScript' not specified"));
    const sl::json::value& callback = rcallback.get();
    std::string* callback_str_ptr = new std::string();
    *callback_str_ptr = callback.dumps();
    support::log_debug(logger, "Spawning thread, callback script: [" + *callback_str_ptr + "] ...");
    // call wilton
    char* err = wilton_thread_run(callback_str_ptr,
            [](void* passed) {
                std::string* sptr = static_cast<std::string*>(passed);
                sl::json::value cb_json = sl::json::loads(*sptr);
                std::string engine = cb_json["engine"].as_string();
                // output will be ignored
                char* out = nullptr;
                int out_len = 0;
                auto err = wiltoncall_runscript(engine.c_str(), static_cast<int> (engine.length()),
                        sptr->c_str(), static_cast<int>(sptr->length()),
                        std::addressof(out), std::addressof(out_len));
                delete sptr;
                if (nullptr != err) {
                    support::log_error(logger, TRACEMSG(err));
                    wilton_free(err);
                }
                if (nullptr != out) {
                    wilton_free(out);
                }
            });
    support::log_debug(logger, "Thread spawn complete, result: [" + sl::support::to_string_bool(nullptr == err) + "] ...");
    if (nullptr != err) {
        support::throw_wilton_error(err, TRACEMSG(err));
    }
    return support::make_null_buffer();
}

support::buffer sleep_millis(sl::io::span<const char> data) {
    // json parse
    auto json = sl::json::load(data);
    int64_t millis = -1;
    for (const sl::json::field& fi : json.as_object()) {
        auto& name = fi.name();
        if ("millis" == name) {
            millis = fi.as_int64_or_throw(name);
        } else {
            throw support::exception(TRACEMSG("Unknown data field: [" + name + "]"));
        }
    }
    if (-1 == millis) throw support::exception(TRACEMSG(
            "Required parameter 'millis' not specified"));
    // call wilton
    char* err = wilton_thread_sleep_millis(static_cast<int> (millis));
    if (nullptr != err) {
        support::throw_wilton_error(err, TRACEMSG(err));
    }
    return support::make_null_buffer();
}

} // namespace
}

extern "C" char* wilton_module_init() {
    try {
        wilton::support::register_wiltoncall("thread_run", wilton::thread::run);
        wilton::support::register_wiltoncall("thread_sleep_millis", wilton::thread::sleep_millis);
        return nullptr;
    } catch (const std::exception& e) {
        return wilton::support::alloc_copy(TRACEMSG(e.what() + "\nException raised"));
    }
}
