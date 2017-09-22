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

