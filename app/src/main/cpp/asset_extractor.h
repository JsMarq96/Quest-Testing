//
// Created by jsmar on 23/05/2021.
//

#ifndef QUEST_DEMO_ASSET_EXTRACTOR_H
#define QUEST_DEMO_ASSET_EXTRACTOR_H

#include <assert.h>
#include <stdlib.h>
#include <android_native_app_glue.h>
#include <zip.h>
#include <string.h>
#include <android/log.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "common.h"

struct sAssMan {
    const char* apk_dir;
    const char* root_dir;
};

void init_asset_manager(sAssMan *ass_manager,
                        JNIEnv *env,
                        ANativeActivity *activity);

// TODO: comprobar si existen y anadir las carpetas
void init_asset_structure(sAssMan *ass_man);

bool check_asset(sAssMan *ass_manager,
                 const char *asset_name);

void extract_asset(sAssMan *ass_manager,
                   const char* asset_name);

void destroy_asset_manager(sAssMan *ass_manager);

#endif //QUEST_DEMO_ASSET_EXTRACTOR_H
