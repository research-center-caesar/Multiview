#ifndef ENUMS_H
#define ENUMS_H

#include <GL/gl.h>

enum RedrawScedule{REDRAW_ALWAYS, REDRAW_AUTOMATIC, REDRAW_MANUAL, REDRAW_END};

enum viewmode_t
{
    EQUIDISTANT, EQUIDISTANT_APPROX, PERSPECTIVE
};

enum viewtype_t
{
    VIEWTYPE_RENDERED = 0, VIEWTYPE_POSITION = 1, VIEWTYPE_DEPTH = 2, VIEWTYPE_FLOW = 3, VIEWTYPE_INDEX = 4, VIEWTYPE_VISIBILITY = 5, VIEWTYPE_END = 6
};

enum coordinate_system_t
{
    COORDINATE_SPHERICAL_APPROXIMATED, COORDINATE_SPHERICAL_CUBEMAP_SINGLEPASS, COORDINATE_SPHERICAL_CUBEMAP_MULTIPASS, COORDINATE_EQUIRECTANGULAR, COORDINATE_END
};

enum depthbuffer_size_t{DEPTHBUFFER_16_BIT = 0, DEPTHBUFFER_24_BIT = 1, DEPTHBUFFER_32_BIT = 2, DEPTHBUFFER_END = 3};

enum motion_blur_curve_t{MOTION_BLUR_CONSTANT,MOTION_BLUR_LINEAR,MOTION_BLUR_QUADRATIC,MOTION_BLUR_CUBIC,MOTION_BLUR_CUSTOM,MOTION_BLUR_END};

#endif
