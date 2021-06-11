//
// Created by Juan S. Marquerie on 29/05/2021.
//

#include "scene.h"
#include "../utils/hashmap.h"

void scene_init(sScene        *new_scene) {
    hashmap_create(10, &new_scene->resource_index_relation);

    CC_init(&new_scene->collision_controller);

    for (int i = 0; i < MAX_INSTANCE_SIZE; i++) {
        new_scene->color[i] =  { 1.0f, 1.0f, 1.0f };
    }
}

void scene_destroy(sScene  *to_destroy){
    hashmap_destroy(&to_destroy->resource_index_relation);
}

int scene_resource_add_mesh(sScene       *scene,
                            const char*  texture,
                            const bool   is_static) {
    char *mesh_dir;
    AM_fetch_asset(&ass_man_instance,
                   texture,
                   &mesh_dir);
    return BMR_add_mesh(&scene->mesh_renderer, mesh_dir, is_static);
}

int scene_resource_add_material(sScene         *scene,
                                const char*    color_texture,
                                const char*    normal_texture,
                                const char*    specular_texture,
                                const char     *vertex_shader,
                                const char     *fragment_shader) {
    sAssMan *ass_man = &ass_man_instance;

    char* color_texture_dir = NULL;
    if (color_texture != NULL) {
        AM_fetch_asset(ass_man,
                       color_texture,
                       &color_texture_dir);

    }

    char* normal_texture_dir = NULL;
    if (normal_texture != NULL) {
        AM_fetch_asset(ass_man,
                       normal_texture,
                       &normal_texture_dir);

    }

    char* specular_texture_dir = NULL;
    if (specular_texture != NULL) {
        AM_fetch_asset(ass_man,
                       specular_texture,
                       &specular_texture_dir);

    }

    return BMR_add_material(&scene->mesh_renderer,
                            color_texture_dir,
                            normal_texture_dir,
                            specular_texture_dir,
                            vertex_shader,
                            fragment_shader);
}

void scene_set_skybox(sScene      *scene,
                      const char  *skyboy_dir) {
    char *cubemap_dir;
    AM_fetch_cubemap_textures(&ass_man_instance,
                              skyboy_dir,
                              &cubemap_dir);

    skybox_renderer_init(&scene->skybox_renderer, cubemap_dir);
}

int scene_add_object(sScene          *scene,
                     const char      *obj_tag,
                     const int       mesh_id,
                     const int       material_id,
                     const sVector3  position) {
    int index = -1;

    for (int i = 0; i < MAX_INSTANCE_SIZE; i++) {
        if (!scene->initialized[i]) {
            index = i;
            break;
        }
    }

    // Error
    if (index == -1) {
        return index;
    }

    scene->render_instances[index].mesh_index = mesh_id;
    scene->render_instances[index].material_index = material_id;
    scene->position[index] = position;
    scene->enabled[index] = true;
    scene->initialized[index] = true;

    hashmap_put(&scene->resource_index_relation,
                obj_tag,
                strlen(obj_tag)+1,
                &index);

    return index;
}

int scene_add_collider(sScene          *scene,
                       const sVector3  position,
                       const sVector3  dimensions) {

    return CC_add_AABB_collider(&scene->collision_controller,
                                position,
                                dimensions.x,
                                dimensions.y,
                                dimensions.z);
}

void scene_update(sScene *scene,
                  const double elapsed_time) {
    sCollision collision_result[MAX_SCENE_COLLIDERS];
    int collision_count = 0;
    CC_update(&scene->collision_controller,
              scene->position,
              scene->rotation,
              collision_result,
              &collision_count);

    if (collision_count > 0) {
        scene->color[collision_result[0].collider1_index] = sVector3{0.5f, 0.f, 0.f};
        scene->color[collision_result[0].collider2_index] = sVector3{0.5f, 0.f, 0.f};
    } else {
        scene->color[collision_result[0].collider1_index] = sVector3{1.f, 1.f, 1.f};
        scene->color[collision_result[0].collider2_index] = sVector3{1.f, 1.f, 1.f};
    }

    scene->scene_update(elapsed_time);
}

void scene_render(const sScene           *scene,
                  const ovrTracking2     *tracking,
                  const unsigned int     eye_index) {
    int obj_to_render_count = 0;

    for(int i = 0; i < MAX_INSTANCE_SIZE; i++) {
        if (scene->enabled[i]) {
            obj_to_render_count++;
        }
    }

    sMat44 *render_models = (sMat44*) malloc(sizeof(sMat44) * obj_to_render_count);
    sVector3 *render_colors = (sVector3*) malloc(sizeof(sVector3) * obj_to_render_count);
    sRenderInstance *render_instances = (sRenderInstance*) malloc(sizeof(sRenderInstance) * obj_to_render_count);

    int index = 0;
    for(int i = 0; i < MAX_INSTANCE_SIZE; i++) {
        if (scene->enabled[i]) {
            render_models[index].set_identity();
            render_models[index].rotate(&scene->rotation[i]);
            render_models[index].set_position(scene->position[i]);
            render_instances[index].material_index = scene->render_instances[i].material_index;
            render_instances[index].mesh_index = scene->render_instances[i].mesh_index;
            render_colors[index] = scene->color[i];
            index++;
        }
    }

    info("Rendering scene's meshes");
    BMR_render(&scene->mesh_renderer,
               render_models,
               render_instances,
               render_colors,
               obj_to_render_count,
               tracking,
               eye_index);

    info("DEBUG: Rendering the scene's colliders");
    CC_render(&scene->collision_controller,
              tracking,
              eye_index);

    info("Rendering scene's skybox");
    skybox_render(&scene->skybox_renderer,
                  tracking,
                  eye_index);
}

void scene_attach_collider_to_object(sScene           *scene,
                                     const int        colder_index,
                                     const int        obj_index,
                                     const sVector3   relative_position) {
    sColliderController *controller = &scene->collision_controller;

    controller->collider_object_entanglement[colder_index] = obj_index;
    controller->entangled_position_delta[colder_index] = relative_position;
}