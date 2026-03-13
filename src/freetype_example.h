#ifndef FRETYPE_EXAMPLE_H
#define FRETYPE_EXAMPLE_H

#ifdef __cplusplus
extern "C" {
#endif
#include "lvgl_sim.h"
    
lv_font_t* get_custom_font(int font_size, int language_index);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* FRETYPE_EXAMPLE_H */