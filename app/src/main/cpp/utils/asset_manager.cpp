//
// Created by jsmar on 23/05/2021.
//

#include <errno.h>
#include "asset_manager.h"

void AM_init(sAssMan *ass_manager,
             JNIEnv *env,
             ANativeActivity *activity) {
    const char* str;
    jboolean isCopy;

    jclass clazz = env->GetObjectClass(activity->clazz);
    jmethodID methodID = env->GetMethodID(clazz, "getPackageCodePath", "()Ljava/lang/String;");
    jobject result_str = env->CallObjectMethod(activity->clazz, methodID);
    ass_manager->apk_dir = env->GetStringUTFChars( (jstring)result_str, &isCopy);

    // TODO: Dynamically fetch this folder or wherever you are supposed to store it
    ass_manager->root_asset_dir = "/data/data/app.upstairs.quest_sample_project"; //(const char*) malloc(strlen(activity->internalDataPath) + 2);
    ///data/data/app.upstairs.quest_sample_project/res/raw/player_2.obj
    // Test if teh folder where we are going to store the assets exists, and if not, create them
    char *temp_dir = (char*) malloc(strlen(ass_manager->root_asset_dir) + 8 + 8 + 1);
    strcpy(temp_dir, ass_manager->root_asset_dir);

    char *file_struct[3] = { "/res", "/raw", "/skybox"};
    for(int  i = 0; i < 3; i++) {
        strcat(temp_dir, file_struct[i]);

        if (access(temp_dir, F_OK)) {
            mkdir(temp_dir, 0777);
        }
    }
    free(temp_dir);
};

bool AM_check_asset(sAssMan *ass_manager,
                    const char *asset_name) {
    int asset_name_len = strlen(asset_name);

    char* check_name = (char*) malloc(strlen(ass_manager->root_asset_dir) + asset_name_len + 1);
    strcpy(check_name, ass_manager->root_asset_dir);
    strcat(check_name, "/");
    strcat(check_name, asset_name);
    check_name[strlen(ass_manager->root_asset_dir) + asset_name_len] = '\0';

    FILE* n = fopen(check_name, "r");
    free(check_name);
    return n != NULL;
}

// Assets should be stored on the res/raw
void AM_extract_asset(const sAssMan *ass_manager,
                      const char* asset_name) {
    int err_code;
    struct zip_stat apk_zip_st;
    FILE *dump_file;

    zip *apk = zip_open(ass_manager->apk_dir, 0, &err_code);

    // TODO:Check errcode

    zip_stat_init(&apk_zip_st);
    zip_stat(apk, asset_name, 0, &apk_zip_st);

    char* raw_file = (char*) malloc(apk_zip_st.size);

    zip_file *file = zip_fopen(apk, asset_name, 0);
    zip_fread(file, raw_file, apk_zip_st.size);

    int asset_name_len = strlen(asset_name);

    char* asset_dir = (char*) malloc(strlen(ass_manager->root_asset_dir) + asset_name_len + 1);
    strcpy(asset_dir, ass_manager->root_asset_dir);
    strcat(asset_dir, "/");
    strcat(asset_dir, asset_name);
    //asset_dir[strlen(ass_manager->root_asset_dir) + asset_name_len] = '\0';

    dump_file = fopen(asset_dir, "wb");

    info("Error: %d (%s)", errno, strerror(errno));

    assert(dump_file != NULL && "Cannot open file to store asset");

    info("Asset name: %s", asset_name);
    info("Dump File: %s", asset_dir);
    //
    info("Size archivo: %i size escrito: %i", apk_zip_st.size, fwrite(raw_file, apk_zip_st.size, 1, dump_file));
    //info("size escr: %s", raw_file);

    zip_fclose(file);
    fclose(dump_file);
    zip_close(apk);
    free(raw_file);
}

void destroy_asset_manager(sAssMan *ass_manager) {
    free((void*) ass_manager->apk_dir);
    free((void*) ass_manager->root_asset_dir);
}

void AM_get_asset_dir(const sAssMan *ass_manager,
                      const char *project_dir,
                      char **result_dir) {
    *result_dir = (char*) malloc(strlen(ass_manager->root_asset_dir) + strlen(project_dir) + 1);
    strcpy(*result_dir, ass_manager->root_asset_dir);
    strcat(*result_dir, "/");
    strcat(*result_dir, project_dir);
}

void AM_fetch_asset(const sAssMan *asset_manager,
                    const char *asset_dir,
                    char **result_dir) {
    char *real_dir;
    AM_get_asset_dir(asset_manager, asset_dir, &real_dir);

    FILE* n = fopen(real_dir, "r");

    if (n == NULL) {
        AM_extract_asset(asset_manager, asset_dir);
    }

    *result_dir = real_dir;
}

void AM_fetch_cubemap_textures(const sAssMan *asset_manager,
                               const char *asset_dir,
                               char **result) {
    char *cubemap_terminations[] = {"right.jpg",
                                    "left.jpg",
                                    "top.jpg",
                                    "bottom.jpg",
                                    "front.jpg",
                                    "back.jpg" };

    char *name_buffer = (char*) malloc(strlen(asset_dir) + sizeof("bottom.png") + 1);

    for (int i = 0; i < 6; i++) {
        memset(name_buffer, '\0', strlen(name_buffer));
        strcat(name_buffer, asset_dir);
        strcat(name_buffer, cubemap_terminations[i]);

        char *tmp;
        AM_fetch_asset(asset_manager, name_buffer, &tmp);
        free(tmp);
        info("Fetched cubemap texture %s", name_buffer);
    }

    AM_get_asset_dir(asset_manager, asset_dir, result);
}