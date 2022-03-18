#ifndef LANG_H
#define LANG_H

#include "data.h"
#include <tuple>

namespace lang        
{                                                              
const std::pair<depthbuffer_size_t, const char*>    depthbuffer_values[]    = {{DEPTHBUFFER_16_BIT, "16 bit"},{DEPTHBUFFER_24_BIT, "24 bit"},{DEPTHBUFFER_32_BIT, "32 bit"},{DEPTHBUFFER_END, nullptr}};

    const char *get_culling_string(size_t value);

    size_t get_culling_value(const char* value);

    const char *get_redraw_scedule_string(RedrawScedule value);

    RedrawScedule get_redraw_scedule_value(const char* value);
    
    const char *get_depthbuffer_string(size_t value);
    
    depthbuffer_size_t get_depthbuffer_value(const char* value);
    
    const char *get_motion_blur_curve_string(motion_blur_curve_t mpc);

    motion_blur_curve_t get_motion_blur_curve_value(const char* mpc);

    viewtype_t get_viewtype_type(const char* value);
    
    const char *get_viewtype_string(viewtype_t value);
    
    const std::tuple<coordinate_system_t, const char*, const char*> & get_coordinate_system_by_name(const char* mpc);

    const std::tuple<coordinate_system_t, const char*, const char*> & get_coordinate_system_by_enum(coordinate_system_t cs);
    
    GLuint get_gl_type_value(const char* value);
    
    const char*get_gl_type_string(GLuint value);
}

#endif
