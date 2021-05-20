//
// Created by jsmar on 20/05/2021.
//

#ifndef QUEST_DEMO_COMMON_H
#define QUEST_DEMO_COMMON_H

#include <android/log.h>
#include <android/window.h>
#include <android_native_app_glue.h>

static const char* TAG = "Test_App";

#define error(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

#ifndef NDEBUG
#define info(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#endif // NDEBUG

#endif //QUEST_DEMO_COMMON_H
