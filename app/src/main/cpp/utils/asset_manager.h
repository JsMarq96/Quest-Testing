//
// Created by jsmar on 23/05/2021.
//

#ifndef QUEST_DEMO_ASSET_MANAGER_H
#define QUEST_DEMO_ASSET_MANAGER_H

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
    const char* root_asset_dir;
};

extern sAssMan ass_man_instance;

void AM_init(sAssMan *ass_manager,
             JNIEnv *env,
             ANativeActivity *activity);

// TODO: comprobar si existen y anadir las carpetas
void AM_init_asset_structure(sAssMan *ass_man);

bool AM_check_asset(sAssMan *ass_manager,
                    const char *asset_name);

void AM_extract_asset(const sAssMan *ass_manager,
                      const char* asset_name);

void destroy_asset_manager(sAssMan *ass_manager);

void AM_fetch_asset(const sAssMan *asset_manager,
                    const char *asset_dir,
                    char **result_dir);

void AM_fetch_asset(const sAssMan *asset_manager,
                    const char *asset_dir,
                    char **result_dir);

void AM_fetch_cubemap_textures(const sAssMan *asset_manager,
                               const char *asset_dir,
                               char **result);

#endif //QUEST_DEMO_ASSET_MANAGER_H
