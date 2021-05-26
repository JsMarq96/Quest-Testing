//
// Created by jsmar on 23/05/2021.
//

#include <errno.h>
#include "asset_extractor.h"

void init_asset_manager(sAssMan *ass_manager,
                        JNIEnv *env,
                        ANativeActivity *activity) {
    const char* str;
    jboolean isCopy;

    jclass clazz = env->GetObjectClass(activity->clazz);
    jmethodID methodID = env->GetMethodID(clazz, "getPackageCodePath", "()Ljava/lang/String;");
    jobject result_str = env->CallObjectMethod(activity->clazz, methodID);
    ass_manager->apk_dir = env->GetStringUTFChars( (jstring)result_str, &isCopy);

    // TODO: Dynamically fetch this folder or wherever you are supposed to store it
    ass_manager->root_dir = "/data/data/app.upstairs.quest_sample_project"; //(const char*) malloc(strlen(activity->internalDataPath) + 2);

    // Test if teh folder where we are going to store the assets exists, and if not, create them
    char *temp_dir = (char*) malloc(strlen(ass_manager->root_dir) + 8 + 1);
    strcpy(temp_dir, ass_manager->root_dir);

    char *file_struct[2] = { "/res", "/raw"};
    for(int  i = 0; i < 2; i++) {
        strcat(temp_dir, file_struct[i]);

        if (access(temp_dir, F_OK)) {
            mkdir(temp_dir, 0777);
        }
    }
    free(temp_dir);
};

bool check_asset(sAssMan *ass_manager,
                 const char *asset_name) {
    int asset_name_len = strlen(asset_name);

    char* check_name = (char*) malloc(strlen(ass_manager->root_dir) + asset_name_len + 1);
    strcpy(check_name, ass_manager->root_dir);
    strcat(check_name, "/");
    strcat(check_name, asset_name);
    check_name[strlen(ass_manager->root_dir) + asset_name_len] = '\0';

    FILE* n = fopen(check_name, "r");
    free(check_name);
    return n != NULL;
}

// Assets should be stored on the res/raw
void extract_asset(sAssMan *ass_manager,
                   const char* asset_name) {
    int err_code;
    struct zip_stat apk_zip_st;
    FILE *dump_file;

    info("APK_dir: %s", ass_manager->apk_dir);
    zip *apk = zip_open(ass_manager->apk_dir, 0, &err_code);

    // TODO:Check errcode

    zip_stat_init(&apk_zip_st);
    zip_stat(apk, asset_name, 0, &apk_zip_st);

    char* raw_file = (char*) malloc(apk_zip_st.size);

    zip_file *file = zip_fopen(apk, asset_name, 0);
    zip_fread(file, raw_file, apk_zip_st.size);

    int asset_name_len = strlen(asset_name);

    char* asset_dir = (char*) malloc(strlen(ass_manager->root_dir) + asset_name_len + 1);
    strcpy(asset_dir, ass_manager->root_dir);
    strcat(asset_dir, "/");
    strcat(asset_dir, asset_name);
    //asset_dir[strlen(ass_manager->root_dir) + asset_name_len] = '\0';

    dump_file = fopen(asset_dir, "w");

    info("Error: %d (%s)", errno, strerror(errno));

    assert(dump_file != NULL && "Cannot open file to store asset");

    info("size escrito: %i", fputs(raw_file, dump_file));
    info("size escr: %s", raw_file);

    zip_fclose(file);
    fclose(dump_file);
    zip_close(apk);
    free(raw_file);

    info("apk_file size: %s", asset_dir);
}

void destroy_asset_manager(sAssMan *ass_manager) {
    free((void*) ass_manager->apk_dir);
    free((void*) ass_manager->root_dir);
}