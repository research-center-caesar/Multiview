#ifndef DATA_H
#define DATA_H

#include <GL/gl.h>
#include <GL/glext.h>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QMatrix4x4>
#include <mutex>
#include <future>
#include "OBJ_Loader.h"
#include "image_util.h"
#include "geometry.h"

enum viewmode_t
{
    EQUIDISTANT, EQUIDISTANT_APPROX, PERSPECTIVE
};

enum viewtype_t
{
    VIEWTYPE_RENDERED = 0, VIEWTYPE_POSITION = 1, VIEWTYPE_DEPTH = 2, VIEWTYPE_FLOW = 3, VIEWTYPE_INDEX = 4
};

enum depthbuffer_size_t{DEPTHBUFFER_16_BIT = 0, DEPTHBUFFER_24_BIT = 1, DEPTHBUFFER_32_BIT = 2};


struct wait_for_rendered_frame
{
    size_t _frame;
    volatile bool _value = false;
    std::condition_variable _cv;

    wait_for_rendered_frame(size_t value_) :_frame(value_) {}

    inline bool operator()() const
    {
        return _value;
    }
};

enum PendingFlag{PENDING_THREAD = 0x1, PENDING_SCENE_EDIT = 0x2, PENDING_FILE_WRITE = 0x4, PENDING_TEXTURE_READ = 0x8, PENDING_FILE_READ = 0x10, PENDING_ALL = 0x1F, PENDING_NONE = 0};

inline PendingFlag operator~   (PendingFlag           a)                { return (PendingFlag)~(int)  a; }
inline PendingFlag operator|   (PendingFlag           a, PendingFlag b) { return (PendingFlag)((int)  a |  (int)b); }
inline PendingFlag operator&   (PendingFlag           a, PendingFlag b) { return (PendingFlag)((int)  a &  (int)b); }
inline PendingFlag operator^   (PendingFlag           a, PendingFlag b) { return (PendingFlag)((int)  a ^  (int)b); }
inline PendingFlag& operator|= (PendingFlag&          a, PendingFlag b) { return (PendingFlag&)((int&)a |= (int)b); }
inline PendingFlag& operator|= (volatile PendingFlag& a, PendingFlag b) { return (PendingFlag&)((int&)a |= (int)b); }
inline PendingFlag& operator&= (PendingFlag&          a, PendingFlag b) { return (PendingFlag&)((int&)a &= (int)b); }
inline PendingFlag& operator&= (volatile PendingFlag& a, PendingFlag b) { return (PendingFlag&)((int&)a &= (int)b); }
inline PendingFlag& operator^= (PendingFlag&          a, PendingFlag b) { return (PendingFlag&)((int&)a ^= (int)b); }

struct pending_task_t
{
    std::future<void> _future;
    PendingFlag volatile _flags;
    std::mutex _mutex;
    std::condition_variable _cond_var;
    
    void set(PendingFlag flag);
    void unset(PendingFlag flag);
    void assign(PendingFlag flag);
    void wait_unset(PendingFlag flag);
    void wait_set(PendingFlag flag);
    pending_task_t(std::future<void> & future_, PendingFlag flags_);
    pending_task_t(PendingFlag flags_);
    pending_task_t(pending_task_t&& other);
    pending_task_t& operator=(pending_task_t&& other);
    bool is_deletable() const;
};

struct exec_env
{
    std::mutex _mtx;
    std::vector<pending_task_t*> _pending_tasks;
    std::string _script_dir;
    
    exec_env(const exec_env&) = delete;
 
    exec_env(std::string const & script_dir_) :_script_dir(script_dir_) {}
    
    void clean();
    
    void clean_impl();
    
    pending_task_t & emitPendingTask();
    
    void emplace_back(pending_task_t &task);
    
    void join(pending_task_t const * self, PendingFlag flag);

    void join_impl(pending_task_t const * self, PendingFlag flag);
    
    ~exec_env();
};

enum screenshot_state{screenshot_state_inited = 0, screenshot_state_queued = 1, screenshot_state_rendered = 2, screenshot_state_copied = 3, screenshot_state_saved = 4, screenshot_state_error = 5};

struct screenshot_handle_t
{
    std::string _camera;
    size_t _prerendering;
    viewtype_t _type;
    size_t _width;
    size_t _height;
    size_t _channels;
    size_t _datatype;
    bool _ignore_nan;
    void *_data = nullptr;
    volatile screenshot_state _state;
    std::mutex _mtx;
    std::condition_variable _cv;
    GLuint _bufferAddress;
    
    size_t num_elements() const;
    
    size_t size() const;
    
    void set_state(screenshot_state state);

    bool operator()() const;
};

struct arrow_t
{
    float _x0, _y0, _x1, _y1;
};
struct view_t
{
    std::string const & _camera;
    GLuint *_cubemap_texture;
    size_t _x, _y, _width, _height;
    viewtype_t _viewtype;
};

struct named_image
{
    advanced_image_base_t *img;
};

struct framelist_t
{
    std::string _name;
    std::vector<size_t> _frames;

    framelist_t(std::string const & name_, std::vector<size_t> const & framelist_);
};
struct object_t
{
    std::string _name;
    size_t _id;
    std::map<size_t, vec3f_t> _key_pos;
    std::map<size_t, rotation_t> _key_rot;
    QMatrix4x4 _transformation;
    bool _visible;

    object_t(std::string const & name_);
};

struct mesh_object_t: object_t
{
    QOpenGLTexture *_tex_Ka = nullptr;
    // Diffuse Texture Map
    QOpenGLTexture *_tex_Kd = nullptr;
    // Specular Texture Map
    QOpenGLTexture *_tex_Ks;
    // Specular Hightlight Map
    QOpenGLTexture *_tex_Ns;
    // Alpha Texture Map
    QOpenGLTexture *_tex_d;
    // Bump Map
    QOpenGLTexture *_tex_bump;
    objl::Loader _loader;
    std::vector<GLuint> _vbo;
    std::vector<GLuint> _vbi;
    bool _flow;

    mesh_object_t(std::string const & name_, std::string const & objfile);
};

struct camera_t : object_t
{
    viewmode_t _viewmode;
    camera_t(std::string const & name_) : object_t(name_), _viewmode(PERSPECTIVE) {}
};

struct scene_t
{
    std::vector<camera_t> _cameras;
    std::vector<mesh_object_t> _objects;
    std::vector<framelist_t> _framelists;
    std::vector<screenshot_handle_t *> _screenshot_handles;
    std::mutex _mtx;
    
    size_t get_camera_index(std::string const & name);
    
    camera_t * get_camera(std::string const & name);

    object_t * get_object(std::string const & name);

    object_t & get_object(size_t index);

    size_t num_objects() const;
};



#endif
