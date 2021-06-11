//
// Created by jsmar on 20/05/2021.
//

#ifndef QUEST_DEMO_COMMON_H
#define QUEST_DEMO_COMMON_H

#include "../../../../../../../../AppData/Local/Android/Sdk/ndk/22.1.7171670/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/android/log.h"
#include "../../../../../../../../AppData/Local/Android/Sdk/ndk/22.1.7171670/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/android/window.h"
#include "../../../../../../../../AppData/Local/Android/Sdk/ndk/22.1.7171670/sources/android/native_app_glue/android_native_app_glue.h"

static const char* TAG = "Test_App";

#define error(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

#ifndef NDEBUG
#define info(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#endif // NDEBUG

#endif //QUEST_DEMO_COMMON_H
