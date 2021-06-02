//
// Created by Juan S. Marquerie on 31/05/2021.
//

#ifndef QUEST_DEMO_MATERIAL_H
#define QUEST_DEMO_MATERIAL_H

//#include <stdlib.h>

#include "texture.h"
#include "shader.h"

struct sMaterial {
    unsigned int    texture_count  = 0;
   // sTexture        **textures     = NULL;

    sShader         shader;
};

//void init_material(sMaterial *mat, , )

#endif //QUEST_DEMO_MATERIAL_H
