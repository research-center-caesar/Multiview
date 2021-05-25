/*
Copyright (c) 2020 Paul Stahr

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "rendering_view.h"
#include "qt_gl_util.h"
#include <sstream>

void print_models(objl::Loader & Loader, std::ostream & file)
{
    for (size_t i = 0; i < Loader.LoadedMeshes.size(); i++)
    {
        // Copy one of the loaded meshes to be our current mesh
        objl::Mesh curMesh = Loader.LoadedMeshes[i];

        // Print Mesh Name
        file << "Mesh " << i << ": " << curMesh.MeshName << "\n";

        /*// Print Vertices
        file << "Vertices:\n";

        // Go through each vertex and print its number,
        //  position, normal, and texture coordinate
        for (size_t j = 0; j < curMesh.Vertices.size(); j++)
        {
            file << "V" << j << ": " <<
                "P(" << curMesh.Vertices[j].Position.X << ", " << curMesh.Vertices[j].Position.Y << ", " << curMesh.Vertices[j].Position.Z << ") " <<
                "N(" << curMesh.Vertices[j].Normal.X << ", " << curMesh.Vertices[j].Normal.Y << ", " << curMesh.Vertices[j].Normal.Z << ") " <<
                "TC(" << curMesh.Vertices[j].TextureCoordinate.X << ", " << curMesh.Vertices[j].TextureCoordinate.Y << ")\n";
        }

        // Print Indices
        file << "Indices:\n";

        // Go through every 3rd index and print the
        //	triangle that these indices represent
        for (size_t j = 0; j < curMesh.Indices.size(); j += 3)
        {
            file << "T" << j / 3 << ": " << curMesh.Indices[j] << ", " << curMesh.Indices[j + 1] << ", " << curMesh.Indices[j + 2] << "\n";
        }*/

        // Print Material
        file << "Material: " << curMesh.MeshMaterial.name << "\n";
        file << "Ambient Color: "  << curMesh.MeshMaterial.Ka.x() << ", " << curMesh.MeshMaterial.Ka.y() << ", " << curMesh.MeshMaterial.Ka.z() << "\n";
        file << "Diffuse Color: "  << curMesh.MeshMaterial.Kd.x() << ", " << curMesh.MeshMaterial.Kd.y() << ", " << curMesh.MeshMaterial.Kd.z() << "\n";
        file << "Specular Color: " << curMesh.MeshMaterial.Ks.x() << ", " << curMesh.MeshMaterial.Ks.y() << ", " << curMesh.MeshMaterial.Ks.z() << "\n";
        file << "Specular Exponent: " << curMesh.MeshMaterial.Ns << "\n";
        file << "Optical Density: " << curMesh.MeshMaterial.Ni << "\n";
        file << "Dissolve: " << curMesh.MeshMaterial.d << "\n";
        file << "Illumination: " << curMesh.MeshMaterial.illum << "\n";
        file << "Ambient Texture Map: " << curMesh.MeshMaterial.map_Ka << "\n";
        file << "Diffuse Texture Map: " << curMesh.MeshMaterial.map_Kd << "\n";
        file << "Specular Texture Map: " << curMesh.MeshMaterial.map_Ks << "\n";
        file << "Alpha Texture Map: " << curMesh.MeshMaterial.map_d << "\n";
        file << "Bump Map: " << curMesh.MeshMaterial.map_bump << "\n";

        // Leave a space to separate from the next mesh
        file << "\n";
    }
}

void load_meshes(mesh_object_t & mesh)
{
    if (mesh._vbo.size() == 0)
    {
        std::cout << "load meshes " << mesh._name << std::endl;
        mesh._vbo.resize(mesh._loader.LoadedMeshes.size());
        glGenBuffers(mesh._vbo.size(), mesh._vbo.data());
        mesh._vbi.resize(mesh._loader.LoadedMeshes.size());
        glGenBuffers(mesh._vbi.size(), mesh._vbi.data());
        for (size_t i = 0; i < mesh._vbo.size(); ++i)
        {
            std::cout << "load mesh " << i << std::endl;
            objl::Mesh const & curMesh = mesh._loader.LoadedMeshes[i];
            glBindBuffer(GL_ARRAY_BUFFER, mesh._vbo[i]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(objl::Vertex) * curMesh.Vertices.size(), curMesh.Vertices.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh._vbi[i]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * curMesh.Indices.size(), curMesh.Indices.data(), GL_STATIC_DRAW);
        }
    }
}

void load_textures(mesh_object_t & mesh)
{
    for (size_t i = 0; i < mesh._loader.LoadedMeshes.size(); ++i)
    {
        std::string map_Ka = mesh._loader.LoadedMeshes[i].MeshMaterial.map_Ka;
        if (map_Ka != "" && mesh._textures[map_Ka] == nullptr)
        {
            QImage img;
            if (!img.load(map_Ka.c_str()))
            {
                std::cout << "error, can't load image " << map_Ka.c_str() << std::endl;
            }
            std::cout << img.width() << ' ' << img.height() << std::endl;
            mesh._textures[map_Ka] =  new QOpenGLTexture(img.mirrored());
        }
        std::string map_Kd = mesh._loader.LoadedMeshes[i].MeshMaterial.map_Kd;
        if (map_Kd != "" && mesh._textures[map_Kd] == nullptr)
        {
            QImage img;
            if (!img.load(map_Kd.c_str()))
            {
                std::cout << "error, can't load image " << map_Kd.c_str() << std::endl;
            }
            std::cout << img.width() << ' ' << img.height() << std::endl;
            mesh._textures[map_Kd] =  new QOpenGLTexture(img.mirrored());
        }
    }
}

void destroy(mesh_object_t & mesh)
{
    glDeleteBuffers(mesh._vbo.size(), mesh._vbo.data());
    mesh._vbo.clear();
    glDeleteBuffers(mesh._vbi.size(), mesh._vbi.data());
    mesh._vbi.clear();
    for (auto iter = mesh._textures.begin(); iter != mesh._textures.end(); ++iter)
    {
        iter->second -> destroy();
        delete iter->second;
        iter->second = nullptr;
    }
}

std::ostream & print_gl_errors(std::ostream & out, std::string const & message, bool endl)
{
    GLenum error = glGetError();
    if (error == 0)
    {
        return out;
    }
    out << message << error;// << gluErrorString(error);
    while (true)
    {
        error = glGetError();
        if (!error)
        {
            return endl ? (out << std::endl) : out;
        }
        out << ' '<< error;// << gluErrorString(error);
    }
    return out;
}

std::string getGlErrorString()
{
    std::stringstream ss;
    while (true)
    {
        GLenum error = glGetError();
        if (error == 0)
        {
            return "";
        }
        ss << ' ' << error;
    }
    return ss.str();
}

void setupTexture(GLenum target, std::shared_ptr<gl_texture_id> texture, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type)
{
    glBindTexture(target, *texture.get());
    if (target == GL_TEXTURE_CUBE_MAP)
    {
        for (uint8_t f = 0; f < 6; ++f)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + f, 0,internalFormat, width, height, 0,format, type, 0);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0,internalFormat, width, height, 0,format, type, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
}


void transform_matrix(object_t const & obj, QMatrix4x4 & matrix, size_t mt_frame, size_t t_smooth, size_t mr_frame, size_t r_smooth)
{
    if (obj._key_pos.size() != 0)
    {
        //vec3f_t pos = smoothed(obj._key_pos, mt_frame, t_smooth);
        vec3f_t pos = smoothed(obj._key_pos, 1, mt_frame - t_smooth, mt_frame + t_smooth);
        matrix.translate(pos.x(), pos.y(), pos.z());
        //std::cout << pos ;
    }
    if (obj._key_rot.size() != 0)
    {
        //std::cout << "found" << std::endl;
        matrix.rotate(to_qquat(smoothed(obj._key_rot, 1, mr_frame - r_smooth, mr_frame + r_smooth)));
        //std::cout << obj._key_rot.lower_bound(m_frame)->second << ' '<< std::endl;
    }
    matrix *= obj._transformation;
}

void render_map(std::shared_ptr<gl_texture_id> cubemap, remapping_shader_t & remapping_shader, bool flipped)
{
    glActiveTexture(GL_TEXTURE0);
    
    glBindTexture(dynamic_cast<remapping_spherical_shader_t*>(&remapping_shader) ?  GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, *cubemap.get());
    glUniform1i(remapping_shader._texAttr, 0);
    
    glVertexAttribPointer(remapping_shader._posAttr, 2, GL_FLOAT, GL_FALSE, 0, g_quad_vertex_buffer_data);
    glVertexAttribPointer(remapping_shader._corAttr, 2, GL_FLOAT, GL_FALSE, 0, flipped ? g_quad_texture_coords_flipped : g_quad_texture_coords);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
}

void activate_render_settings(remapping_shader_t & remapping_shader, render_setting_t const & render_setting)
{
    if (render_setting._viewtype == VIEWTYPE_DEPTH)
    {
        remapping_shader._program->setUniformValue(remapping_shader._transformUniform, render_setting._camera_transformation);            
    }
    else if (render_setting._viewtype == VIEWTYPE_POSITION)
    {
        remapping_shader._program->setUniformValue(remapping_shader._transformUniform, render_setting._position_transformation);
    }
    remapping_shader._program->setUniformValue(remapping_shader._transformColorUniform, render_setting._color_transformation);
    setShaderInt(*remapping_shader._program, remapping_shader._viewtypeUniform, "viewtype", static_cast<GLint>(render_setting._viewtype));
}

void render_view(remapping_shader_t & remapping_shader, render_setting_t const & render_setting)
{
    activate_render_settings(remapping_shader, render_setting);
    glActiveTexture(GL_TEXTURE1);
    GLenum target = dynamic_cast<remapping_spherical_shader_t*>(&remapping_shader) ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
    glBindTexture(target, *render_setting._selfPositionTexture.get());
    glUniform1i(remapping_shader._positionMap, 1);
    for (size_t i = 0; i < render_setting._other_views.size(); ++i)
    {
        other_view_information_t const & other = render_setting._other_views[i];
        remapping_shader._program->setUniformValue(remapping_shader._transformCam[i], other._camera_transformation);
        glActiveTexture(GL_TEXTURE2 + i);
        glBindTexture(target, *other._position_texture.get());
        glUniform1i(remapping_shader._positionMaps[i], 2 + i);
    }
    setShaderInt(*remapping_shader._program, remapping_shader._numOverlays, "numOverlays", static_cast<GLint>(render_setting._other_views.size()));
    render_map(render_setting._rendered_texture, remapping_shader, render_setting._flipped);
}

size_t get_channels(viewtype_t viewtype)
{
    switch(viewtype)
    {
        case VIEWTYPE_RENDERED: return 3;
        case VIEWTYPE_POSITION: return 3;
        case VIEWTYPE_DEPTH:    return 1;
        case VIEWTYPE_FLOW:     return 3;
        case VIEWTYPE_INDEX:    return 1;
        default: throw std::runtime_error("Unsupported viewtype");
    }
}

GLenum get_format(size_t channels)
{
    switch(channels)
    {
        case 1: return GL_RED;
        case 2: return GL_RG;
        case 3: return GL_RGB;
        case 4: return GL_RGBA;
        default: throw std::runtime_error("Unsupported number of channels"); 
    }
}

void dmaTextureCopy(screenshot_handle_t & current, bool debug)
{
    if (debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
    assert(current._state == screenshot_state_rendered_texture);
    size_t textureType = GL_TEXTURE_2D;
    if (current._prerendering != std::numeric_limits<size_t>::max())
    {
        textureType = GL_TEXTURE_CUBE_MAP_POSITIVE_X + current._prerendering;
        glBindTexture(GL_TEXTURE_CUBE_MAP, *current._textureId.get());
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, *current._textureId.get());
    }
    if (current._channels == 0) {current._channels = get_channels(current._type);}
    GLuint pbo_userImage;
    glGenBuffers(1, &pbo_userImage);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_userImage);
    glBufferData(GL_PIXEL_PACK_BUFFER, current.size(), 0, GL_STREAM_READ);
    if (debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}

    glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
    glGetTexImage(textureType, 0, get_format(current._channels), current._datatype, 0);
    current._bufferAddress = pbo_userImage;
    current.set_state(screenshot_state_rendered_buffer);
    if (debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
}

void TriangleWindow::delete_texture(GLuint tex){_to_remove_textures.emplace_back(tex);}

std::shared_ptr<gl_texture_id> TriangleWindow::create_texture(size_t swidth, size_t sheight, viewtype_t vtype)
{
    std::shared_ptr<gl_texture_id> screenshotTexture;
    gen_textures(1, &screenshotTexture);
    
    GLuint internalFormat;
    GLuint type;
    GLuint format;
    switch(vtype)
    {
        case VIEWTYPE_RENDERED: internalFormat = GL_RGBA;    format = GL_RGBA;  type = GL_UNSIGNED_BYTE; break;
        case VIEWTYPE_POSITION: internalFormat = GL_RGBA32F; format = GL_RGBA;  type = GL_FLOAT;         break;
        case VIEWTYPE_DEPTH:    internalFormat = GL_R32F;    format = GL_RED;   type = GL_FLOAT;         break;
        case VIEWTYPE_FLOW:     internalFormat = GL_RGBA32F; format = GL_RGBA;  type = GL_FLOAT;         break;
        case VIEWTYPE_INDEX:    internalFormat = GL_R32F;    format = GL_RED;   type = GL_FLOAT;         break;
        default: throw std::runtime_error("Unknown type");
    }
    setupTexture(GL_TEXTURE_2D, screenshotTexture, internalFormat, swidth, sheight, format, type);
    return screenshotTexture;
}

void TriangleWindow::render_to_texture(screenshot_handle_t & current, render_setting_t const & render_setting, size_t loglevel, bool debug, remapping_shader_t & remapping_shader)
{
    if (debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
    assert(current._state == screenshot_state_queued);
    if (loglevel > 2){std::cout << "take screenshot " << current._camera << std::endl;}
    activate_render_settings(remapping_shader, render_setting);
    GLuint screenshotFramebuffer = 0;
    glGenFramebuffers(1, &screenshotFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, screenshotFramebuffer);
    if (current._task == TAKE_SCREENSHOT){current._textureId = create_texture(current._width, current._height, current._type);}
    GLuint depthrenderbuffer;
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, current._width, current._height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *current._textureId.get(), 0);

    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);
    if (debug){
        GLuint framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if(framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("Error, no framebuffer(" + std::to_string(__LINE__) + "):" + std::to_string(framebufferStatus));
    }
    glViewport(0,0,current._width, current._height);
    glDepthFunc(GL_LESS);
    glDisable(GL_DEPTH_TEST);
    if (current._task == RENDER_TO_TEXTURE)
    {
        glBlendFunc(GL_ONE, GL_ONE);
        glEnable(GL_BLEND);
    }
    else
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    if (debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}    
    render_view(remapping_shader, render_setting);
    if (debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
    if (current._task == RENDER_TO_TEXTURE)
    {
        glDisable(GL_BLEND);
    }
    current.set_state(screenshot_state_rendered_texture);
    glDeleteRenderbuffers(1, &depthrenderbuffer);
    glDeleteFramebuffers(1, &screenshotFramebuffer);
    if (debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
    return;
}

TriangleWindow::TriangleWindow()
{
    QObject::connect(this, SIGNAL(renderLaterSignal()), this, SLOT(renderLater()));
    //QObject::connect(this, SIGNAL(renderNowSignal()), this, SLOT(renderNow()));
    session._m_frame = 100000;
    _updating = false;
    _update_handler = [this](SessionUpdateType sut){
        setAnimating(this->session._animating == REDRAW_ALWAYS || (this->session._animating == REDRAW_AUTOMATIC && this->session._play != 0));
        if (_updating)
        {
            return;
        }
        switch(session._animating)
        {
            case REDRAW_ALWAYS:break;
            case REDRAW_AUTOMATIC:
                if (sut & (UPDATE_REDRAW | UPDATE_SESSION | UPDATE_FRAME | UPDATE_SCENE)){renderLater();}break;
            case REDRAW_MANUAL:
                if (sut & UPDATE_REDRAW){renderLater();}break;
        }
    };
    session._updateListener.emplace_back(&_update_handler);
}

TriangleWindow::~TriangleWindow()
{
    session._updateListener.erase(std::remove(session._updateListener.begin(), session._updateListener.end(), &_update_handler), session._updateListener.end());
    //size_t address = UTIL::get_address(_update_handler);
    //session._updateListener.erase(std::remove_if(session._updateListener.begin(), session._updateListener.end(), [address](std::function<void(SessionUpdateType)> const & f){return UTIL::get_address(f) == address;}), session._updateListener.end());
}

void TriangleWindow::initialize()
{
    perspective_shader.init(*this);
    remapping_spherical_shader.init(*this);
    approximation_shader.init(*this);
    remapping_identity_shader.init(*this);
    GLint maxColorAttachememts = 0;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachememts);
    std::cout << "max attachments:" << maxColorAttachememts << std::endl;
    //GLubyte const* msg = glGetString(GL_EXTENSIONS);
    //std::cout << "extensions:" << msg << std::endl;//TODO
    image_io_init();
    
    for (size_t i = 0; i <6; ++i)
    {
        cubemap_camera_to_view[i].setToIdentity();
        cubemap_camera_to_view[i].perspective(90.0f, 1.0f/1.0f, 0.1f, 1000.0f);
    }
    //settransform left_eye rot 0.7 0.7 0 0
    cubemap_camera_to_view[0].rotate(90, 0, 0, 1);
    cubemap_camera_to_view[0].scale(1,-1,1);
    cubemap_camera_to_view[1].rotate(180, 1, 0, 0);
    cubemap_camera_to_view[1].rotate(270, 0, 0, 1);
    cubemap_camera_to_view[1].scale(1,-1,1);
    cubemap_camera_to_view[2].rotate(270, 0, 1, 0);
    cubemap_camera_to_view[2].scale(1,-1,1);
    cubemap_camera_to_view[3].rotate(90, 0, 1, 0);
    cubemap_camera_to_view[3].scale(1, 1, -1);
    cubemap_camera_to_view[4].rotate(270, 0, 0, 1);
    cubemap_camera_to_view[4].rotate(90, 1, 0, 0);
    cubemap_camera_to_view[4].scale(-1, 1, 1);
    cubemap_camera_to_view[5].rotate(270, 0, 0, 1);
    cubemap_camera_to_view[5].rotate(270, 1, 0, 0);
    cubemap_camera_to_view[5].scale(-1,1,1);
    
    for (size_t i = 0; i < 6; ++i)
    {
        cubemap_camera_to_view[i].rotate(-90,1,0,0);
        cubemap_camera_to_view[i].rotate(90,0,0,1);
    }
}

std::ostream & operator << (std::ostream & out, arrow_t const & arrow)
{
    return out << '('<< arrow._x0 << ' ' << arrow._y0 << ' ' << arrow._x1 << ' ' << arrow._y1 << ')';
}

template <typename T>
void copy_pixel_buffer_to_screenshot_impl(screenshot_handle_t & current, bool)
{
    T *pixels = new T[current.num_elements()];
    T* ptr = static_cast<T*>(glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY));
    if (!ptr){throw std::runtime_error("map buffer returned null" + getGlErrorString());}
    std::copy(ptr, ptr + current.num_elements(), pixels);
    current._data = pixels;
}

void copy_pixel_buffer_to_screenshot(screenshot_handle_t & current, bool debug)
{
    if (debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
    assert(current._state == screenshot_state_rendered_buffer);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, current._bufferAddress);
    if      (current._datatype == GL_FLOAT)         {copy_pixel_buffer_to_screenshot_impl<float>   (current, debug);}
    else if (current._datatype == GL_UNSIGNED_BYTE) {copy_pixel_buffer_to_screenshot_impl<uint8_t> (current, debug);}
    else if (current._datatype == GL_UNSIGNED_SHORT){copy_pixel_buffer_to_screenshot_impl<uint16_t>(current, debug);}
    else                                            {throw std::runtime_error("Unsupported image-type");}
    current.set_state(screenshot_state_copied);
    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    glDeleteBuffers(1, &current._bufferAddress);
    current._bufferAddress = 0;
}

void render_objects(
    std::vector<mesh_object_t> & meshes,
    rendering_shader_t & shader,
    int m_frame,
    bool diffobj,
    int32_t diffbackward,
    int32_t diffforward,
    bool diffnormalize,
    bool difffallback,
    size_t smoothing,
    QMatrix4x4 const & world_to_view,
    QMatrix4x4 const & world_to_camera_pre,
    QMatrix4x4 const & world_to_camera_cur,
    QMatrix4x4 const & world_to_camera_post,
    bool debug)
{
    for (mesh_object_t & mesh : meshes)
    {
        if (!mesh._visible){continue;}
        if (debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
        int32_t currentdiffbackward = diffbackward;
        int32_t currentdiffforward = diffforward;
        QMatrix4x4 current_world_to_camera_pre = world_to_camera_pre;
        QMatrix4x4 current_world_to_camera_post = world_to_camera_post;
        load_meshes(mesh);
        QMatrix4x4 object_to_world_pre;
        QMatrix4x4 object_to_world_cur;
        QMatrix4x4 object_to_world_post;
        bool difftranscurrent = diffobj && mesh._difftrans;
        bool diffrotcurrent   = diffobj && mesh._diffrot;
        transform_matrix(mesh, object_to_world_pre, m_frame + (difftranscurrent ? diffbackward : 0), smoothing, m_frame + (diffrotcurrent ? diffbackward : 0), smoothing);
        transform_matrix(mesh, object_to_world_cur, m_frame, smoothing, m_frame, smoothing);
        transform_matrix(mesh, object_to_world_post, m_frame + (difftranscurrent ? diffforward : 0), smoothing, m_frame + (diffrotcurrent ? diffforward : 0), smoothing);
        if (contains_nan(object_to_world_cur)){continue;}
        setShaderInt(*shader._program, shader._objidUniform, "objid", static_cast<GLint>(mesh._id));
        shader._program->setUniformValue(shader._preMatrixUniform, world_to_camera_pre * object_to_world_pre);
        shader._program->setUniformValue(shader._curMatrixUniform, world_to_camera_cur * object_to_world_cur);
        if (difffallback)
        {
            if (contains_nan(world_to_camera_pre))
            {
                currentdiffbackward = 0;
                current_world_to_camera_pre = world_to_camera_cur;
                object_to_world_pre = object_to_world_cur;
            }
            if (contains_nan(world_to_camera_post))
            {
                currentdiffforward = 0;
                current_world_to_camera_post = world_to_camera_cur;
                object_to_world_post = object_to_world_cur;
            }
        }
        QMatrix4x4 flowMatrix = current_world_to_camera_pre * object_to_world_pre - current_world_to_camera_post * object_to_world_post;
        if (diffnormalize){flowMatrix *= 1. / (currentdiffforward - currentdiffbackward);}
        shader._program->setUniformValue(shader._flowMatrixUniform, flowMatrix);
        shader._program->setUniformValue(shader._matrixUniform, world_to_view * object_to_world_cur);
        shader._program->setUniformValue(shader._objMatrixUniform, object_to_world_cur);

        objl::Loader & Loader = mesh._loader;

        if (debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
        for (size_t i = 0; i < Loader.LoadedMeshes.size(); ++i)
        {
            objl::Mesh const & curMesh = Loader.LoadedMeshes[i];
            load_textures(mesh);
            QOpenGLTexture *tex = mesh._textures[curMesh.MeshMaterial.map_Kd];
            if (tex != nullptr)
            {
                glActiveTexture(GL_TEXTURE0);
                tex->bind();
                glUniform1i(shader._texKd, 0);
            }
            
            glBindBuffer(GL_ARRAY_BUFFER, mesh._vbo[i]);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(shader._posAttr, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), BUFFER_OFFSET(0));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(shader._colAttr, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), BUFFER_OFFSET(3 * sizeof(float)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(shader._corAttr, 2, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), BUFFER_OFFSET(6 * sizeof(float)));
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh._vbi[i]);

            glDrawElements( GL_TRIANGLES, curMesh.Indices.size(), GL_UNSIGNED_INT, (void*)0);

            glDisableVertexAttribArray(2);
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            if (tex!= nullptr){glBindTexture(GL_TEXTURE_2D, 0);}
        }
        if (debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
    }
}

GLint depth_component(depthbuffer_size_t depthbuffer_size)
{
    switch(depthbuffer_size)
    {
        case DEPTHBUFFER_16_BIT: return GL_DEPTH_COMPONENT16;
        case DEPTHBUFFER_24_BIT: return GL_DEPTH_COMPONENT24;
        case DEPTHBUFFER_32_BIT: return GL_DEPTH_COMPONENT32;
        default: throw std::runtime_error("Illegal depthbuffer_size " + std::to_string(depthbuffer_size));
    }
}

void setup_framebuffer(GLuint target, size_t resolution, session_t const & session, rendered_framebuffer_t const & framebuffer)
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, *framebuffer._rendered.get(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, target, *framebuffer._flow.get(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, target, *framebuffer._position.get(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, target, *framebuffer._index.get(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  target, *framebuffer._depth.get(), 0 );
    GLenum drawBuffers[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
    glDrawBuffers(4, drawBuffers);
    if (session._debug){
        GLenum frameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if(frameBufferStatus != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("Error, no framebuffer(" + std::to_string(__LINE__) + "):" + std::to_string(frameBufferStatus));
    }
    glViewport(0,0,resolution,resolution);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LESS);
    if (session._depth_testing){glEnable(GL_DEPTH_TEST);}else{glDisable(GL_DEPTH_TEST);}
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void TriangleWindow::clean()
{
    glDeleteTextures(_to_remove_textures.size(), _to_remove_textures.data());
    _to_remove_textures.clear();
}

void TriangleWindow::render()
{
    if (destroyed){return;}
    uint8_t overlay = 1;
    bool show_arrows = session._show_arrows;
    bool show_curser = session._show_curser;
    bool show_flow = session._show_flow;
    bool approximated = session._approximated;
    float fov = session._fov;
    size_t loglevel = session._loglevel;
    size_t smoothing = session._smoothing;
    int32_t m_frame = session._m_frame;
    std::string const & show_only = session._show_only;
    QPoint curser_pos = mapFromGlobal(QCursor::pos());
    //std::cout << p.x() << ' ' << p.y()  << std::endl;
    const high_res_clock current_time = std::chrono::high_resolution_clock::now();
    //std::cout << "fps " << CLOCKS_PER_SEC / float( current_time - last_rendertime ) << std::endl;

    if (session._loglevel > 5){std::cout << "start render" << std::endl;}
    if (session._reload_shader)
    {
        initialize();
        session._reload_shader = false;
    }
    scene_t & scene = session._scene;
    for (texture_t & tex : scene._textures)
    {
        if (*tex._tex.get() == GL_INVALID_VALUE)
        {
            std::cout << "create" << tex._width << ' ' << tex._height << std::endl;
            tex._tex = create_texture(tex._width, tex._height, tex._type);
        }
    }
    size_t resolution = session._preresolution;
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale , height() * retinaScale);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    size_t num_cams = 0;
    for (camera_t const & cam : scene._cameras)
    {
        if (cam._visible)
        {
            _active_cameras.push_back(&cam);
            ++num_cams;
        }
    }
    size_t num_views = static_cast<size_t>(session._show_raytraced) + static_cast<size_t>(session._show_position) + static_cast<size_t>(session._show_index) + static_cast<size_t>(session._show_flow) + static_cast<size_t>(session._show_depth);
    views.clear();

    marker.clear();
    QPointF curserViewPos;
    if (num_cams != 0 && num_views != 0)
    {
        curserViewPos.setX((curser_pos.x() % (width() / num_cams))/static_cast<float>(width() / num_cams));
        curserViewPos.setY((curser_pos.y() % (height() / num_views))/static_cast<float>(height() / num_views));
        if (loglevel > 5){std::cout << curserViewPos.x() << ' ' << curserViewPos.y() << '\t';}
    }
    
    switch(session._culling)
    {
        case 0: glDisable(GL_CULL_FACE);        break;
        case 1: glCullFace(GL_FRONT);           break;
        case 2: glCullFace(GL_BACK);            break;
        case 3: glCullFace(GL_FRONT_AND_BACK);  break;
        default:    throw std::runtime_error("Illegal face-culling value");
    }
    if (session._culling != 0)
    {
        glEnable(GL_CULL_FACE);
    }
    {
        std::lock_guard<std::mutex> lockGuard(scene._mtx);
        if (session._loglevel > 5){std::cout << "locked scene" << std::endl;}
        size_t num_textures = num_cams;
        bool diffnormalize = session._diffnormalize;
        bool difffallback = session._difffallback;
        std::vector<rendered_framebuffer_t> framebuffer_cubemaps(num_textures);
        gen_textures(num_textures * 5, reinterpret_cast<std::shared_ptr<gl_texture_id>* >(framebuffer_cubemaps.data()));
        if (num_views != 0)
        {
            size_t c = 0;
            for (camera_t const * cam : _active_cameras)
            {
                if (cam->_visible)
                {
                    size_t x = c * width() / num_cams;
                    size_t w = width() / num_cams;
                    size_t h = height()/num_views;
                    size_t i = 0;
                    if (session._show_raytraced)
                    {
                        size_t y = (i++) * h;
                        views.push_back(view_t({cam->_name, framebuffer_cubemaps[c]._rendered, x, y, w, h, VIEWTYPE_RENDERED}));
                    }
                    if (session._show_position)
                    {
                        size_t y = (i++) * h;
                        views.push_back(view_t({cam->_name, framebuffer_cubemaps[c]._position, x, y, w, h, VIEWTYPE_POSITION}));
                    }
                    if (session._show_index)
                    {
                        size_t y = (i++) * h;
                        views.push_back(view_t({cam->_name, framebuffer_cubemaps[c]._index, x, y, w, h, VIEWTYPE_INDEX}));
                    }
                    if (session._show_flow)
                    {
                        size_t y = (i++) * h;
                        views.push_back(view_t({cam->_name, framebuffer_cubemaps[c]._flow, x, y, w, h, VIEWTYPE_FLOW}));
                    }
                    if (session._show_depth)
                    {
                        size_t y = (i++) * h;
                        views.push_back(view_t({cam->_name, framebuffer_cubemaps[c]._position, x, y, w, h, VIEWTYPE_DEPTH}));
                    }
                    ++c;
                }
            }
        }

        world_to_camera.clear();
        for (camera_t const * cam : _active_cameras)
        {
            world_to_camera.emplace_back();
            QMatrix4x4 &world_to_camera_cur = world_to_camera.back();
            transform_matrix(*cam, world_to_camera_cur, m_frame, smoothing, m_frame, smoothing);
            world_to_camera_cur = world_to_camera_cur.inverted();
        }
        bool difftrans = session._difftrans;
        bool diffrot = session._diffrot;
        bool diffobj = session._diffobjects;
        int32_t diffbackward = session._diffbackward;
        int32_t diffforward = session._diffforward;
        
        GLuint FramebufferName = 0;
        glGenFramebuffers(1, &FramebufferName);
        glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
        if (session._debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
        for (size_t c = 0; c < _active_cameras.size(); ++c)
        {
            camera_t const & cam = *_active_cameras[c];
            if (cam._visible)
            {
                QMatrix4x4 &world_to_camera_cur = world_to_camera[c];
                QMatrix4x4 world_to_camera_pre;
                QMatrix4x4 world_to_camera_post;
                transform_matrix(cam, world_to_camera_pre,  m_frame + (difftrans && cam._difftrans ? diffbackward : 0), smoothing, m_frame + (diffrot && cam._diffrot ? diffbackward : 0), smoothing);
                transform_matrix(cam, world_to_camera_post, m_frame + (difftrans && cam._difftrans ? diffforward  : 0), smoothing, m_frame + (diffrot && cam._diffrot ? diffforward  : 0), smoothing);
                world_to_camera_pre = world_to_camera_pre.inverted();
                world_to_camera_post = world_to_camera_post.inverted();
                int32_t currentdiffbackward = diffbackward;
                int32_t currentdiffforward = diffforward;
                if (difffallback)
                {
                    if (contains_nan(world_to_camera_pre))
                    {
                        currentdiffbackward = 0;
                        world_to_camera_pre = world_to_camera_cur;
                    }
                    if (contains_nan(world_to_camera_post))
                    {
                        currentdiffforward = 0;
                        world_to_camera_post = world_to_camera_cur;
                    }
                }
                GLuint target = approximated ? GL_TEXTURE_2D : GL_TEXTURE_CUBE_MAP;
                rendered_framebuffer_t & framebuffer = framebuffer_cubemaps[c];
                setupTexture(target, framebuffer._rendered,GL_RGBA,    resolution, resolution, GL_BGRA,        GL_UNSIGNED_BYTE);
                setupTexture(target, framebuffer._flow,    GL_RGB16F,  resolution, resolution, GL_BGR,         GL_FLOAT);
                setupTexture(target, framebuffer._position,GL_RGBA32F, resolution, resolution, GL_BGRA,        GL_FLOAT);//TODO: BGR should be sufficient
                setupTexture(target, framebuffer._index,   GL_R32UI,   resolution, resolution, GL_RED_INTEGER, GL_UNSIGNED_INT);
                if (session._debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
                setupTexture(target, framebuffer._depth, depth_component(session._depthbuffer_size), resolution, resolution, GL_DEPTH_COMPONENT, GL_FLOAT);
                if (session._debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
                glPolygonMode( GL_FRONT_AND_BACK, cam._wireframe ? GL_LINE : GL_FILL);
                if (approximated)
                {
                    QMatrix4x4 world_to_view = world_to_camera_cur;
                    if (contains_nan(world_to_view)){continue;}
                    approximation_shader._program->bind();
                    setup_framebuffer(GL_TEXTURE_2D, resolution, session, framebuffer);
                    float fova = fov * (M_PI / 180);
                    setShaderFloat(*approximation_shader._program, approximation_shader._fovUniform, "fovUnif", static_cast<GLfloat>(fova));
                    setShaderFloat(*approximation_shader._program, approximation_shader._fovCapUniform, "fovCapUnif", static_cast<GLfloat>(1/tan(fova)));
                    setShaderBoolean(*approximation_shader._program, approximation_shader._cropUniform, "cropUnif", session._crop);
                    if (session._debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
                    render_objects(scene._objects,
                                   approximation_shader,
                                   m_frame,
                                   diffobj,
                                   currentdiffbackward,
                                   currentdiffforward,
                                   diffnormalize,
                                   difffallback,
                                   smoothing,
                                   world_to_view,
                                   world_to_camera_pre,
                                   world_to_camera_cur,
                                   world_to_camera_post,
                                   session._debug);
                    approximation_shader._program->release();
                }
                else
                {
                    perspective_shader._program->bind();
                    for (size_t f = 0; f < 6; ++f)
                    {
                        if ((f == 5 && fov < 120) || (f != 4 && fov <= 45)){continue;}
                        QMatrix4x4 world_to_view = cubemap_camera_to_view[f] * world_to_camera_cur;
                        if (contains_nan(world_to_view)){continue;}
                        setup_framebuffer(GL_TEXTURE_CUBE_MAP_POSITIVE_X + f, resolution, session, framebuffer);
                        render_objects(
                            scene._objects,
                            perspective_shader,
                            m_frame, diffobj,
                            currentdiffbackward,
                            currentdiffforward,
                            diffnormalize,
                            difffallback,
                            smoothing,
                            world_to_view,
                            world_to_camera_pre,
                            world_to_camera_cur,
                            world_to_camera_post,
                            session._debug);
                    }
                    perspective_shader._program->release();
                }
                if (session._debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
            }
        }
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDeleteFramebuffers(1, &FramebufferName);
        glDisable(GL_CULL_FACE);
        remapping_shader_t &remapping_shader = approximated ? static_cast<remapping_shader_t&>(remapping_identity_shader) : static_cast<remapping_shader_t&>(remapping_spherical_shader);
        remapping_shader._program->bind();
        setShaderFloat(*remapping_shader._program, remapping_shader._fovUniform, "fovUnif", fov * (M_PI / 180));
        setShaderBoolean(*remapping_shader._program, remapping_shader._cropUniform, "cropUnif", session._crop);
        if (session._debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
        
        QVector4D curser_3d;
        
        size_t arrow_lines = 16;
        if (show_arrows)
        {
            _arrow_handles.reserve(num_cams);
            if (session._debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
            for (size_t icam = 0; icam < _active_cameras.size(); ++icam)
            {
                screenshot_handle_t & current = *(new screenshot_handle_t);
                current._task = RENDER_TO_TEXTURE;
                current._width = arrow_lines;
                current._height = arrow_lines;
                current._channels = 2;
                current._type = VIEWTYPE_FLOW;
                current._ignore_nan = true;
                current._datatype = GL_FLOAT;
                current._data = nullptr;
                current._state = screenshot_state_queued;
                current._camera = _active_cameras[icam]->_name;
                current._prerendering = std::numeric_limits<size_t>::max();
                _arrow_handles.emplace_back(&current);
                
                render_setting_t render_setting;
                render_setting._viewtype = current._type;
                render_setting._camera_transformation = world_to_camera[icam];
                render_setting._position_transformation = world_to_camera.size() == 2 ? world_to_camera[current._camera == _active_cameras[0]->_name] : QMatrix4x4();
                render_setting._selfPositionTexture = framebuffer_cubemaps[icam]._position;
                render_setting._rendered_texture = framebuffer_cubemaps[icam]._flow;
                render_setting._color_transformation.scale(1, 1, 1);
                render_setting._flipped = false;
                render_to_texture(current, render_setting, loglevel, session._debug, remapping_shader);
                dmaTextureCopy(current, session._debug);
                clean();

                if (session._debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
            }
        }
        
        scene._screenshot_handles.erase(std::remove_if(scene._screenshot_handles.begin(), scene._screenshot_handles.end(),
            [&scene, this, &framebuffer_cubemaps, resolution, loglevel, &remapping_shader](screenshot_handle_t *current)
            {
                if (current->_task == SAVE_TEXTURE)
                {
                    return false;
                }
                if (current->_task == RENDER_TO_TEXTURE)
                {
                    texture_t *tex = scene.get_texture(current->_texture);
                    if (!tex)
                    {
                        std::cout << "error, texture " + current->_texture + " doesn't exist" << std::endl;
                        current->set_state(screenshot_state_error);
                    }
                    else
                    {
                        current->_textureId = tex->_tex;
                        current->_width = tex->_width;
                        current->_height = tex->_height;
                    }
                }
                camera_t *cam = scene.get_camera(current->_camera);
                if (!cam)
                {
                    std::cout << "error, camera " + current->_camera + " doesn't exist" << std::endl;
                    current->set_state(screenshot_state_error);
                    return true;
                }
                size_t icam = std::distance(_active_cameras.begin(), std::find(_active_cameras.begin(), _active_cameras.end(), cam));
                if (!current->_ignore_nan && contains_nan(world_to_camera[icam]))
                {
                    std::cout << "camera-transformation invalid " << current->_id << std::endl;
                    current->set_state(screenshot_state_error);
                    return true;
                }
                std::cout << "rendering_screenshot " << current->_id << std::endl;
                if (current->_prerendering == std::numeric_limits<size_t>::max())
                {
                    render_setting_t render_setting;
                    render_setting._viewtype = current->_type;
                    render_setting._camera_transformation = world_to_camera[icam];
                    render_setting._position_transformation = world_to_camera.size() == 2 ? world_to_camera[current->_camera == scene._cameras[0]._name] : QMatrix4x4();
                    render_setting._selfPositionTexture = framebuffer_cubemaps[icam]._position;
                    render_setting._flipped = current->_flip;
                    current->_flip = false;
                    switch(current->_type)
                    {
                        case VIEWTYPE_RENDERED  :render_setting._rendered_texture = framebuffer_cubemaps[icam]._rendered;  break;
                        case VIEWTYPE_POSITION  :render_setting._rendered_texture = framebuffer_cubemaps[icam]._position;  break;
                        case VIEWTYPE_DEPTH     :render_setting._rendered_texture = framebuffer_cubemaps[icam]._position;  break;//TODO should be changed to distance in the future?
                        case VIEWTYPE_FLOW      :render_setting._rendered_texture = framebuffer_cubemaps[icam]._flow; render_setting._color_transformation.scale(-1, 1, 1);break;
                        case VIEWTYPE_INDEX     :render_setting._rendered_texture = framebuffer_cubemaps[icam]._index;     break;
                        default: throw std::runtime_error("Unknown rendertype");
                    }
                    for (size_t i = 0; i < current->_vcam.size(); ++i)
                    {
                        size_t index = std::distance(scene._cameras.data(), scene.get_camera(current->_vcam[i]));
                        if (index == scene._cameras.size())
                        {
                            std::cerr << "Could not find camera " << current->_vcam[i] << std::endl;
                            continue;
                        }
                        render_setting._other_views.emplace_back(world_to_camera[index], framebuffer_cubemaps[index]._position);
                    }
                    render_to_texture(*current, render_setting, loglevel, session._debug, remapping_shader);
                    if (current->_task == TAKE_SCREENSHOT)
                    {
                        dmaTextureCopy(*current, session._debug);
                        clean();
                    }
                    else if (current->_task == RENDER_TO_TEXTURE)
                    {
                        current->set_state(screenshot_state_saved);
                        return true;
                    }
                }
                else
                {
                    if (session._debug){print_gl_errors(std::cout, "gl error (" + std::to_string(__LINE__) + "):", true);}
                    if (current->_task == TAKE_SCREENSHOT)
                    {
                        current->_width = resolution;
                        current->_height = resolution;
                        current->_textureId = reinterpret_cast<std::shared_ptr<gl_texture_id> * >(&framebuffer_cubemaps[icam])[current->_type];
                        current->_state =  screenshot_state_rendered_texture;
                        dmaTextureCopy(*current, session._debug);
                    }
                }
                std::cout << "rendered_screenshot" << current->_id << std::endl;
                return false;
            }),scene._screenshot_handles.end());
        
        scene._screenshot_handles.erase(std::remove_if(scene._screenshot_handles.begin(), scene._screenshot_handles.end(),
            [&scene, this](screenshot_handle_t *current)
            {
                std::cout << current->_id << "task: " << current->_task << std::endl;
                if (current->_task != SAVE_TEXTURE){return false;}
                texture_t const * texture = scene.get_texture(current->_texture);
                if (!texture){ 
                    std::cout << "don't rendering_screenshot " << current->_id << std::endl;
                    current->set_state(screenshot_state_error);
                    return true;}
                current->_textureId = texture->_tex;
                current->_width = texture->_width;
                current->_height = texture->_height;
                current->set_state(screenshot_state_rendered_texture);
                current->_channels = texture->_channels;
                current->_type = texture->_type;
                current->_prerendering = std::numeric_limits<size_t>::max();
                dmaTextureCopy(*current,this->session._debug);
                return false;
        }),scene._screenshot_handles.end());

        screenshot_handle_t curser_handle;
        if (show_curser && num_cams != 0)
        {
            size_t icam = clamp(curser_pos.x() * num_cams / width(), size_t(0), num_cams-1);
            curser_handle._task = RENDER_TO_TEXTURE;
            curser_handle._width = 1024;
            curser_handle._height = 1024;
            curser_handle._channels = 3;
            curser_handle._type = VIEWTYPE_POSITION;
            curser_handle._ignore_nan = true;
            curser_handle._datatype = GL_FLOAT;
            curser_handle._data = nullptr;
            curser_handle._state = screenshot_state_inited;
            curser_handle._camera = scene._cameras[icam]._name;
            
            render_setting_t render_setting;
            render_setting._viewtype = curser_handle._type;
            render_setting._camera_transformation = world_to_camera[icam];
            render_setting._position_transformation = QMatrix4x4();
            render_setting._selfPositionTexture = framebuffer_cubemaps[icam]._position;
            render_setting._rendered_texture = reinterpret_cast<std::shared_ptr<gl_texture_id>*>(framebuffer_cubemaps.data() + icam)[curser_handle._type];
            render_setting._flipped = false;
            render_to_texture(curser_handle, render_setting, loglevel, session._debug, remapping_shader);
            dmaTextureCopy(curser_handle, session._debug);
            clean();
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDepthFunc(GL_LESS);
        glDisable(GL_DEPTH_TEST);  
        
        for (view_t & view : views)
        {
            size_t icam = std::distance(_active_cameras.begin(), std::find(_active_cameras.begin(), _active_cameras.end(), scene.get_camera(view._camera)));
            render_setting_t render_setting;
            render_setting._viewtype = view._viewtype;
            render_setting._camera_transformation = world_to_camera[icam];
            render_setting._position_transformation = world_to_camera.size() == 2 ? world_to_camera[view._camera == _active_cameras[0]->_name] : QMatrix4x4();
            render_setting._selfPositionTexture = framebuffer_cubemaps[icam]._position;
            render_setting._rendered_texture = view._cubemap_texture;
            render_setting._flipped = false;
            if (session._show_rendered_visibility)
            {
                for (size_t i = 0; i < scene._cameras.size() && i < 3; ++i)
                {
                    render_setting._other_views.emplace_back(world_to_camera[i], framebuffer_cubemaps[i]._position);
                }
            }
            if (view._viewtype == VIEWTYPE_INDEX)
            {
               // render_setting._color_transformation.scale(1./255,1./255,1./255);
            }
            else if (view._viewtype == VIEWTYPE_DEPTH)
            {
                float depthscale = session._depth_scale;
                render_setting._color_transformation.scale(depthscale, depthscale, depthscale);                
            }
            else if (view._viewtype == VIEWTYPE_FLOW)
            {
                render_setting._color_transformation.scale(-157, 157, 157);//TODO add flowscale
            }
            glViewport(view._x, view._y, view._width, view._height);
            render_view(remapping_shader, render_setting);
        }
        if (show_arrows)
        {
            for (size_t icam = 0; icam < num_cams; ++icam)
            {
                screenshot_handle_t & current = *_arrow_handles[icam];
                copy_pixel_buffer_to_screenshot(current, session._debug);                
                float *data = reinterpret_cast<float*>(current.get_data());
                
                size_t sy = current._height - static_cast<size_t>(curserViewPos.y() * current._height);
                size_t sx = static_cast<size_t>(curserViewPos.x() * current._width);
                if (sx < current._width && sy < current._height)
                {
                    size_t index = 2 * (sy * current._width + sx);
                    _curser_flow.emplace_back(data[index],data[index + 1]);
                }
                       
                for (size_t y = 0; y < current._height; ++y)
                {
                    for (size_t x = 0; x < current._width; ++x)
                    {
                        float xf = (static_cast<float>(x) + 0.5) / current._width;
                        float yf = (static_cast<float>(y) + 0.5) / current._height;
                        
                        size_t index = 2 * (y * current._width + x);
                        float xdiff = -data[index];
                        float ydiff = data[index + 1];
                        if (!std::isnan(xdiff) && !std::isnan(ydiff))
                        {
                            for (view_t & view : views)
                            {
                                if (view._camera == _active_cameras[icam]->_name && view._cubemap_texture == (show_flow ? framebuffer_cubemaps[icam]._flow : framebuffer_cubemaps[icam]._rendered))
                                {
                                    arrows.emplace_back(arrow_t({xf * view._width + view._x, height() - yf * view._height - view._y, xdiff * view._width, ydiff * view._height}));
                                    //std::cout << arrows.back() << std::endl;
                                }
                            }
                        }
                    }
                }
                delete[] data;
            }
        }

        if (show_curser && num_cams != 0)
        {
            copy_pixel_buffer_to_screenshot(curser_handle, session._debug);
            size_t sy = 1024 - static_cast<size_t>(curserViewPos.y() * 1024);
            size_t sx = static_cast<size_t>(curserViewPos.x() * 1024);
            if (sx < 1024 && sy < 1024)
            {
                size_t index = 3 * (sy * 1024 + sx);
                if (loglevel > 5)
                {
                    std::cout << index << '\t';
                }
                float *data = reinterpret_cast<float*>(curser_handle.get_data());
                curser_3d = QVector4D(data[index], data[index + 1], data[index + 2], 1);
                if (loglevel > 5)
                {
                    std::cout << curser_3d.x() << ' ' << curser_3d.y() << ' ' << curser_3d.z() << '\t';
                }
                delete[] data;
                if (curser_3d.x() != 0 || curser_3d.y() != 0 || curser_3d.z() != 0)
                {
                    for (size_t icam = 0; icam < num_cams; ++icam)
                    {
                        QVector4D test = world_to_camera[icam] * curser_3d;
                        if (loglevel > 5)
                        {
                            std::cout << "test " << test.x() << ' ' << test.y() << '\t';
                        }
                        
                        std::array<float, 2> tmp = kart_to_equidistant(std::array<float, 3>({-test.x(), -test.y(), -test.z()}));
                        tmp[0] = -tmp[0];
                        
                        //std::array<float, 2> tmp = kart_to_equidistant(std::array<float, 3>({test.x(), test.y(), test.z()}));
                        for (size_t icam = 0; icam < 2; ++icam)
                        {
                            tmp[icam] = tmp[icam] * fov / 45.;
                        }
                        if (tmp[0] * tmp[0] + tmp[1] * tmp[1] > 0.25)
                        {
                            continue;
                        }
                        tmp[0] += 0.5;
                        tmp[1] += 0.5;
                        
                        for (view_t & view : views)
                        {
                            if (view._camera == _active_cameras[icam]->_name)
                            {
                                marker.push_back(QPointF(tmp[0] * view._width + view._x, tmp[1] * view._height + view._y));
                            }
                        }
                        
                        //marker.push_back(QPointF(test.x(), test.y()));
                        if (loglevel > 5)
                        {
                            std::cout << "marker " << tmp[0] << ' ' << tmp[1] << std::endl;
                        }
                    }
                }
            }  
        }
        for (screenshot_handle_t * current : scene._screenshot_handles)
        {
            if (current->_task == TAKE_SCREENSHOT || current->_task == SAVE_TEXTURE)
            {
                copy_pixel_buffer_to_screenshot(*current, session._debug);
                last_screenshottimes.emplace_back(std::chrono::high_resolution_clock::now());
            }
        }
        scene._screenshot_handles.clear();
        remapping_shader._program->release();
        glDeleteTextures(num_textures * 5, reinterpret_cast<GLuint*>(framebuffer_cubemaps.data()));

        //screenshot = "movie/" + std::to_string(m_frame) + ".tga";
        glViewport(0,0,width(), height());
        glDisable(GL_DEPTH_TEST);
        //GLdouble glColor[4];
        //glGetDoublev(GL_CURRENT_COLOR, glColor);
        //QColor fontColor = QColor(glColor[0], glColor[1], glColor[2], glColor[3]);

        if (qogpd == nullptr)
        {
            qogpd = new QOpenGLPaintDevice;
        }
        int ratio = devicePixelRatio();
        qogpd->setSize(QSize(width() * ratio, height() * ratio));
        qogpd->setDevicePixelRatio(ratio);
        QPainter painter(qogpd);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        painter.setFont(QFont("Times", 24));

        std::string framestr = std::to_string(m_frame);
        //painter.setPen(QColor(clamp(static_cast<int>(curser_3d.x() * 255), 0, 0xFF), clamp(static_cast<int>(curser_3d.y() * 255), 0, 0xFF), clamp(static_cast<int>(curser_3d.z() * 255), 0, 0xFF), 255));
        //painter.setPen(QColor(255, 255, 255, 255));
        //painter.drawEllipse(QPointF(100,100), 10, 10);
        painter.setPen(QColor(255,255,255,255));
        if (overlay != 0)
        {
            for(view_t view : views)
            {
                double x0 = view._x, y0 = view._y;
                double x1 = x0 + view._width, y1 = y0 + view._height;
                double cx = x0 + 0.5 * view._width, cy = y0 + 0.5 * view._height;
                painter.drawEllipse(QPointF(view._width * 0.5 + view._x,0.5 * view._height + view._y), view._width/2, view._height/2);
                painter.drawLine(cx, y0, cx, y1);
                painter.drawLine(x0, cy, x1, cy);
            }
        }
        for (QPointF const & m : marker)
        {
            painter.drawEllipse(QPointF(m.x(),m.y()), 10, 10);
        }
        for (arrow_t const & arrow : arrows)
        {
            float headx = arrow._x0+ arrow._x1, heady = arrow._y0 + arrow._y1;
            float centerx = arrow._x0+ arrow._x1 * 0.5, centery = arrow._y0 + arrow._y1 * 0.5;
            painter.drawLine(arrow._x0, arrow._y0, headx, heady);
            painter.drawLine(centerx + arrow._y1 * 0.5, centery - arrow._x1 * 0.5, headx, heady);
            painter.drawLine(centerx - arrow._y1 * 0.5, centery + arrow._x1 * 0.5, headx, heady);
        }
        arrows.clear();
        painter.drawText(30, 30, QString(framestr.c_str()));
        last_rendertimes.push_back(current_time);
        //std::cout << last_rendertimes.front() << '+' << CLOCKS_PER_SEC <<'<' << current_time << std::endl;
        while (std::chrono::duration_cast<std::chrono::microseconds>(current_time - last_rendertimes.front()).count() > 1000000)
        {
            last_rendertimes.pop_front();
        }
        while (last_screenshottimes.size() > 0 && std::chrono::duration_cast<std::chrono::microseconds>(current_time - last_screenshottimes.front()).count() > 1000000)
        {
            last_screenshottimes.pop_front();
        }
        //std::string tmp = "fps " + std::to_string(last_rendertimes.size());
        
        double duration = (static_cast<std::chrono::duration<double> >( current_time - last_rendertime )).count();
        std::string tmp = "fps "  + std::to_string(last_rendertimes.size()/* + static_cast<float>(current_time - last_rendertimes.front()) / CLOCKS_PER_SEC*/) + " " + std::to_string(last_screenshottimes.size()) + " " + std::to_string(1 / duration);
        painter.drawText(150,30,QString(tmp.c_str()));
        size_t row = 0;
        for (vec2f_t const & cf : _curser_flow)
        {
            tmp = "("+std::to_string(cf.x()) + " " + std::to_string(cf.y()) + ") " + std::to_string(sqrt(cf.dot()));
            painter.drawText(400,30 + row * 30,QString(tmp.c_str()));
            ++row;
        }
        last_rendertime = current_time;
        if (session._show_framelists)
        {
            for (size_t i = 0; i < scene._framelists.size(); ++i)
            {
                bool found = std::binary_search(scene._framelists[i]._frames.begin(), scene._framelists[i]._frames.end(), m_frame);
                painter.setPen(QColor((!found) * 255,found * 255,0,255));
                painter.drawText(30, i*30 + 60, QString(scene._framelists[i]._name.c_str()));
                for (int32_t frame = -50; frame < 50; ++frame)
                {
                    if (std::binary_search(scene._framelists[i]._frames.begin(), scene._framelists[i]._frames.end(), m_frame + frame))
                    {
                        painter.drawEllipse(600 + frame * 10, i * 30 + 45, 5, 5);
                    }
                }
            }
        }
        painter.end();
        if (session._screenshot != "")
        {
            //awx_ScreenShot(session._screenshot);
            session._screenshot = "";
        }
        
        if (session._play != 0)
        {
            if (session._realtime)
            {
                session._m_frame += session._play * session._frames_per_second * duration;
            }
            else
            {
                session._m_frame += session._play * session._frames_per_step;
            }
            if (show_only != "")
            {
                for (size_t i = 0; i < scene._framelists.size(); ++i)
                {
                    if (scene._framelists[i]._name == show_only)
                    {
                        auto iter = std::lower_bound(scene._framelists[i]._frames.begin(), scene._framelists[i]._frames.end(), m_frame);
                        if (iter != scene._framelists[i]._frames.end())
                        {
                            session._m_frame = session._play >= 0 ? *(iter) : *(iter-1);
                        }
                    }
                }
            }
            if (m_frame != session._m_frame)
            {
                _updating = true;
                session.scene_update(UPDATE_FRAME);
                _updating = false;
            }
        }
        ++session._rendered_frames;
        std::vector<wait_for_rendered_frame_t*> & wait_for_rendered_frame_handles = session._wait_for_rendered_frame_handles;
        wait_for_rendered_frame_handles.erase(std::remove_if(wait_for_rendered_frame_handles.begin(), wait_for_rendered_frame_handles.end(),[this, loglevel](wait_for_rendered_frame_t *wait){
            if (wait->_frame < this->session._rendered_frames)
            {
                wait->_value = true;
                wait->_cv.notify_all();
                if (loglevel > 5)
                {
                    std::cout << "notify " << std::endl;
                }
                return true;
            }
            return false;
        }), wait_for_rendered_frame_handles.end());
        _curser_flow.clear();
        std::for_each(_arrow_handles.begin(), _arrow_handles.end(), UTIL::delete_functor);
        _arrow_handles.clear();
        _active_cameras.clear();
    }//End of lock
    if (session._exit_program)
    {
        perspective_shader.destroy();
        remapping_spherical_shader.destroy();
        remapping_identity_shader.destroy();
        approximation_shader.destroy();
        std::vector<mesh_object_t>().swap(scene._objects);
        //scene._objects.clear();
        deleteLater();
        _exit = true;
        destroyed = true;
    }
    if (loglevel > 5)
    {
        std::cout << "end render" << std::endl;
    }
}

void TriangleWindow::mouseMoveEvent(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton)
    {
        ++session._perm;
    }
}

