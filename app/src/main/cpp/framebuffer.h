//
// Created by jsmar on 05/06/2021.
//

#ifndef QUEST_DEMO_FRAMEBUFFER_H
#define QUEST_DEMO_FRAMEBUFFER_H

#include <GLES3/gl3.h>
#include <VrApi.h>

#define SWAPCHAIN_LEN 3

struct sFramebuffers {
    unsigned int         framebuffers        [SWAPCHAIN_LEN]     = {0};
    unsigned int         depth_renderbuffers [SWAPCHAIN_LEN]     = {0};

    ovrTextureSwapChain   *ovr_color_swapchain   = NULL;

    int  swapchain_index  =  0;
    int  width            = -1;
    int  height           = -1;
};

//// LIFECYCLE FUNCTIONS
void framebuffers_init(sFramebuffers  *to_init,
                       const int      width,
                       const int      height);

void framebuffers_destroy(sFramebuffers *to_init);

#endif //QUEST_DEMO_FRAMEBUFFER_H
