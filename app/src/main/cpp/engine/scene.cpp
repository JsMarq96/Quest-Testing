//
// Created by Juan S. Marquerie on 29/05/2021.
//

#include "scene.h"
#include "../utils/kv_storage.h"


////
//// LIFECYCLE FUNCTIONS
////

void scene_init(sScene        *new_scene) {
    KVS_init(&new_scene->obj_index_storage);
    KVS_init(&new_scene->mesh_index_storage);
    KVS_init(&new_scene->material_index_storage);

    CC_init(&new_scene->collision_controller,
            new_scene->enabled,
            new_scene->position,
            new_scene->rotation);

    PHYS_init(&new_scene->physics_controller,
              new_scene->enabled,
              new_scene->position,
              new_scene->rotation);


    for (int i = 0; i < MAX_INSTANCE_SIZE; i++) {
        new_scene->obj_highlight_color[i] =  { 1.0f, 1.0f, 1.0f };
    }
}

void scene_destroy(sScene  *to_destroy){
    //hashmap_destroy(&to_destroy->obj_index_storage);
}

void scene_update(sScene *scene,
                  const sControllerInput  *input,
                  const double display_time,
                  const double elapsed_time) {
    /// PHYSICS UPDATE
    PHYS_update(&scene->physics_controller, elapsed_time);

    /// COLLISION DETECTION
    sCollisionManifold collision_result[MAX_SCENE_COLLIDERS];
    int collision_count = 0;
    CC_update(&scene->collision_controller,
              collision_result,
              &collision_count);

    // Debugging
    if (collision_count > 0) {
        scene->obj_highlight_color[0] = sVector3{0.5f, 0.f, 0.f};
        scene->obj_highlight_color[0] = sVector3{0.5f, 0.f, 0.f};
    } else {
        scene->obj_highlight_color[0] = sVector3{1.f, 1.f, 1.f};
        scene->obj_highlight_color[0] = sVector3{1.f, 1.f, 1.f};
    }
    info("COLLISION COUNT: %d", collision_count);

    /// COLLISION RESOLUTION
    PHYS_resolve_collisions(&scene->physics_controller,
                            collision_result,
                            collision_count);

    /// GAMEPLAY
    scene->scene_update(scene, input, collision_result, collision_count, elapsed_time);
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

    info("Rendering %i meshes", obj_to_render_count);

    int index = 0;
    for(int i = 0; i < MAX_INSTANCE_SIZE; i++) {
        if (scene->enabled[i]) {
            render_models[index].set_identity();
            render_models[index].rotate(&scene->rotation[i]);
            render_models[index].set_position(scene->position[i]);
            render_instances[index].material_index = scene->render_instances[i].material_index;
            render_instances[index].mesh_index = scene->render_instances[i].mesh_index;
            render_colors[index] = scene->obj_highlight_color[i];
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


////
////  SCENE RESOURCE FUNCTIONS
////

int scene_resource_add_mesh(sScene       *scene,
                            const char*  texture,
                            const bool   is_static) {
    char *mesh_dir;
    AM_fetch_asset(&ass_man_instance,
                   texture,
                   &mesh_dir);
    return BMR_add_mesh(&scene->mesh_renderer, mesh_dir, is_static);
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
                     const sVector3  position,
                     const sQuaternion4  rotation) {
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
    scene->rotation[index] = rotation;
    scene->enabled[index] = true;
    scene->initialized[index] = true;

    KVS_add(&scene->obj_index_storage,
            obj_tag,
            strlen(obj_tag),
            index);

    return index;
}

int scene_add_object(sScene              *scene,
                     const char          *obj_tag,
                     const int           mesh_id,
                     const int           material_id,
                     const sVector3      position,
                     const sQuaternion4  rotation,
                     const sVector3      bounding_box_size) {
    int obj_id = scene_add_object(scene,
                                  obj_tag,
                                  mesh_id,
                                  material_id,
                                  position,
                                  rotation);

    int collider_id = scene_add_collider(scene,
                                         sVector3{0.f, 0.f, 0.f},
                                         bounding_box_size,
                                         sQuaternion4{1.0f, 0.f, 0.f, 0.f});
    scene_attach_collider_to_object(scene,
                                    collider_id,
                                    obj_id,
                                    sVector3{0.0f});

    return obj_id;
}

int scene_add_object(sScene              *scene,
                     const char          *obj_tag,
                     const int           mesh_id,
                     const int           material_id,
                     const sVector3      position,
                     const sQuaternion4  rotation,
                     const sVector3      bounding_box_size,
                     const sVector3      bounding_box_pos) {
    int obj_id = scene_add_object(scene, obj_tag, mesh_id, material_id, position, rotation);

    int collider_id = scene_add_collider(scene,
                                         sVector3{0.f, 0.f, 0.f},
                                         bounding_box_size,
                                         sQuaternion4{1.0f, 0.f, 0.f, 0.f});
    scene_attach_collider_to_object(scene,
                                    collider_id,
                                    obj_id,
                                    bounding_box_pos);

    return obj_id;
}

int scene_add_object(sScene              *scene,
                     const char          *obj_tag,
                     const int           mesh_id,
                     const int           material_id,
                     const sVector3      position,
                     const sQuaternion4  rotation,
                     const sVector3      bounding_box_size,
                     const sVector3      bounding_box_pos,
                     const bool          is_static,
                     const float         mass) {
    int obj_id = scene_add_object(scene,
                                  obj_tag,
                                  mesh_id,
                                  material_id,
                                  position,
                                  rotation,
                                  bounding_box_size,
                                  bounding_box_pos);

    if (is_static) {
        scene->physics_controller.attributes[obj_id] |= IS_UNMOVABLE;
        return obj_id;
    }

    PHYS_set_mass(&scene->physics_controller,
                  obj_id,
                  mass);

    return obj_id;
}

int scene_add_collider(sScene          *scene,
                       const sVector3  position,
                       const sVector3 dimensions,
                       const sQuaternion4 rotation) {

    return CC_add_OBB_collider(&scene->collision_controller,
                                position,
                                rotation,
                                dimensions.x,
                                dimensions.y,
                                dimensions.z);
}

void scene_attach_collider_to_object(sScene           *scene,
                                     const int        colder_index,
                                     const int        obj_index,
                                     const sVector3   relative_position) {
    sColliderController *controller = &scene->collision_controller;

    controller->object_entanglement[colder_index] = obj_index;
    controller->origin_points[colder_index] = relative_position;
}
