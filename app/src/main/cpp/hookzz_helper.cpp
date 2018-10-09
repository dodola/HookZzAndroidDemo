//
// Created by z on 2018/7/7.
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "hookzz.h"
#include <android/log.h>

#ifndef ZREG_MACRO
#define ZREG_MACRO
#if defined(__LP64__)
#define ZREG(n) general.regs.x##n
#else
#define ZREG(n) general.regs.r##n
#endif
#endif
#define  LOG_TAG    "HOOOOOOOOK"
#define  ALOG(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
/* demo for replace */

size_t (*origin_fread)(void *ptr, size_t size, size_t nitems, FILE *stream);

size_t (fake_fread)(void *ptr, size_t size, size_t nitems, FILE *stream) {
    printf("[FileMonitor|fread|model|%p] >>> %ld, %ld\n", ptr, size, nitems);
    return origin_fread(ptr, size, nitems, stream);
}

void hook_fread() { ZzReplace((void *) fread, (void *) fake_fread, (void **) &origin_fread); }


/* demo for wrap */

void open_pre_call(RegisterContext *reg_ctx, const HookEntryInfo *info) {
    char *path = (char *) reg_ctx->ZREG(0);
    int oflag = (int) reg_ctx->ZREG(1);
    switch (oflag) {
        case O_RDONLY:
            ALOG("[FileMonitor|open|R] >>> %s\n", path);
            break;
        case O_WRONLY:
            ALOG("[FileMonitor|open|W] >>> %s\n", path);
            break;
        case O_RDWR:
            ALOG("[FileMonitor|open|RW] >>> %s\n", path);
            break;
        default:
            ALOG("[FileMonitor|open|-] >>> %s\n", path);
            break;
    }
}

void open_post_call(RegisterContext *reg_ctx, const HookEntryInfo *info) {
}

void hook_open() { ZzWrap((void *) open, open_pre_call, open_post_call); }


__attribute__((constructor)) void hookzz_initialize() {
    ALOG("============initialize===============");
    hook_open();
    hook_fread();
}