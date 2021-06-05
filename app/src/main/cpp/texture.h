//
// Created by Juan S. Marquerie on 29/05/2021.
//

#ifndef QUEST_DEMO_TEXTURE_H
#define QUEST_DEMO_TEXTURE_H

#include <GLES3/gl3.h>
#include <string.h>

#define DEFAULT_TEXT_FIDELITY 0

struct sTexture {
    bool             store_on_RAM = false;
    bool             is_cube_map  = false;

    // Raw data
    int             width     = 0;
    int             height    = 0;
    int             layers    = 0;
    unsigned char   *raw_data = NULL;

    // OpenGL id
    unsigned int     texture_id;
};

void load_texture(sTexture  *text,
                  const bool is_cube_map,
                  const bool store_on_RAM,
                  const char *texture_name);

void delete_texture(sTexture  *text);

#endif //QUEST_DEMO_TEXTURE_H
