#pragma once

#ifdef __APPLE__

#include <TargetConditionals.h>

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR

// ============================================================
// iOS
// ============================================================
//
// FixedFunctionGL uses the OpenGL ES 1.1 fixed-function pipeline.
// Keep the ES2 shader function pointer typedefs below because
// other parts of the project may still use them.
//
// ============================================================

#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>

// ------------------------------------------------------------
// Compatibility types
// ------------------------------------------------------------

typedef GLfloat GLclampf;
typedef GLfloat GLclampd;

#ifndef GLAPIENTRY
#define GLAPIENTRY
#endif

#ifndef GLbitfield
typedef unsigned int GLbitfield;
#endif

// ------------------------------------------------------------
// OpenGL ES 2.x function pointer types
//
// ES1 headers do not provide these types, but the project still
// has shader-related entries in GLFuncTable.
// ------------------------------------------------------------

#ifndef PFNGLATTACHSHADERPROC
typedef void (*PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
#endif

#ifndef PFNGLBINDATTRIBLOCATIONPROC
typedef void (*PFNGLBINDATTRIBLOCATIONPROC)(
    GLuint program,
    GLuint index,
    const GLchar *name);
#endif

#ifndef PFNGLCOMPILESHADERPROC
typedef void (*PFNGLCOMPILESHADERPROC)(GLuint shader);
#endif

#ifndef PFNGLCREATEPROGRAMPROC
typedef GLuint (*PFNGLCREATEPROGRAMPROC)(void);
#endif

#ifndef PFNGLCREATESHADERPROC
typedef GLuint (*PFNGLCREATESHADERPROC)(GLenum type);
#endif

#ifndef PFNGLDELETEPROGRAMPROC
typedef void (*PFNGLDELETEPROGRAMPROC)(GLuint program);
#endif

#ifndef PFNGLDELETESHADERPROC
typedef void (*PFNGLDELETESHADERPROC)(GLuint shader);
#endif

#ifndef PFNGLDISABLEVERTEXATTRIBARRAYPROC
typedef void (*PFNGLDISABLEVERTEXATTRIBARRAYPROC)(GLuint index);
#endif

#ifndef PFNGLENABLEVERTEXATTRIBARRAYPROC
typedef void (*PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
#endif

#ifndef PFNGLGETPROGRAMINFOLOGPROC
typedef void (*PFNGLGETPROGRAMINFOLOGPROC)(
    GLuint program,
    GLsizei bufSize,
    GLsizei *length,
    GLchar *infoLog);
#endif

#ifndef PFNGLGETPROGRAMIVPROC
typedef void (*PFNGLGETPROGRAMIVPROC)(
    GLuint program,
    GLenum pname,
    GLint *params);
#endif

#ifndef PFNGLGETSHADERINFOLOGPROC
typedef void (*PFNGLGETSHADERINFOLOGPROC)(
    GLuint shader,
    GLsizei bufSize,
    GLsizei *length,
    GLchar *infoLog);
#endif

#ifndef PFNGLGETSHADERIVPROC
typedef void (*PFNGLGETSHADERIVPROC)(
    GLuint shader,
    GLenum pname,
    GLint *params);
#endif

#ifndef PFNGLGETUNIFORMLOCATIONPROC
typedef GLint (*PFNGLGETUNIFORMLOCATIONPROC)(
    GLuint program,
    const GLchar *name);
#endif

#ifndef PFNGLLINKPROGRAMPROC
typedef void (*PFNGLLINKPROGRAMPROC)(GLuint program);
#endif

#ifndef PFNGLSHADERSOURCEPROC
typedef void (*PFNGLSHADERSOURCEPROC)(
    GLuint shader,
    GLsizei count,
    const GLchar *const *string,
    const GLint *length);
#endif

#ifndef PFNGLUNIFORM1FPROC
typedef void (*PFNGLUNIFORM1FPROC)(
    GLint location,
    GLfloat v0);
#endif

#ifndef PFNGLUNIFORM1IPROC
typedef void (*PFNGLUNIFORM1IPROC)(
    GLint location,
    GLint v0);
#endif

#ifndef PFNGLUNIFORM4FPROC
typedef void (*PFNGLUNIFORM4FPROC)(
    GLint location,
    GLfloat v0,
    GLfloat v1,
    GLfloat v2,
    GLfloat v3);
#endif

#ifndef PFNGLUNIFORMMATRIX4FVPROC
typedef void (*PFNGLUNIFORMMATRIX4FVPROC)(
    GLint location,
    GLsizei count,
    GLboolean transpose,
    const GLfloat *value);
#endif

#ifndef PFNGLUSEPROGRAMPROC
typedef void (*PFNGLUSEPROGRAMPROC)(GLuint program);
#endif

#ifndef PFNGLVERTEXATTRIBPOINTERPROC
typedef void (*PFNGLVERTEXATTRIBPOINTERPROC)(
    GLuint index,
    GLint size,
    GLenum type,
    GLboolean normalized,
    GLsizei stride,
    const GLvoid *pointer);
#endif

#else

// macOS
#include <SDL2/SDL_opengl.h>

#endif

#else

// Non-Apple platforms
#include <SDL2/SDL_opengl.h>

#endif


// ============================================================
// GLFuncTable
// ============================================================
//
// Function pointers for OpenGL functions used in EoSD.
//
// Windows opengl32 only exposes OpenGL 1.1 directly, while the
// project uses some newer OpenGL functionality. SDL_GL_GetProcAddress
// is therefore used to resolve functions at runtime.
//
// On iOS, ResolveFunctions(true) resolves the OpenGL ES path.
//
// ============================================================

struct GLFuncTable
{
    void ResolveFunctions(bool glesContext);

    // --------------------------------------------------------
    // Functions whose GL/GLES signatures differ
    // --------------------------------------------------------

    void glClearDepthf(GLclampf depth);

    void glDepthRangef(
        GLclampf near_val,
        GLclampf far_val);

    // --------------------------------------------------------
    // OpenGL / OpenGL ES fixed-function functions
    // --------------------------------------------------------

    void(GLAPIENTRY *glAlphaFunc)(
        GLenum func,
        GLclampf ref);

    void(GLAPIENTRY *glBindTexture)(
        GLenum target,
        GLuint texture);

    void(GLAPIENTRY *glBlendFunc)(
        GLenum sfactor,
        GLenum dfactor);

    void(GLAPIENTRY *glClear)(
        GLbitfield mask);

    void(GLAPIENTRY *glClearColor)(
        GLclampf red,
        GLclampf green,
        GLclampf blue,
        GLclampf alpha);

    void(GLAPIENTRY *glColorPointer)(
        GLint size,
        GLenum type,
        GLsizei stride,
        const GLvoid *ptr);

    void(GLAPIENTRY *glDeleteTextures)(
        GLsizei n,
        const GLuint *textures);

    void(GLAPIENTRY *glDepthFunc)(
        GLenum func);

    void(GLAPIENTRY *glDepthMask)(
        GLboolean flag);

    void(GLAPIENTRY *glDisableClientState)(
        GLenum cap);

    void(GLAPIENTRY *glDrawArrays)(
        GLenum mode,
        GLint first,
        GLsizei count);

    void(GLAPIENTRY *glEnable)(
        GLenum cap);

    void(GLAPIENTRY *glEnableClientState)(
        GLenum cap);

    void(GLAPIENTRY *glFogf)(
        GLenum pname,
        GLfloat param);

    void(GLAPIENTRY *glFogfv)(
        GLenum pname,
        const GLfloat *params);

    void(GLAPIENTRY *glGenTextures)(
        GLsizei n,
        GLuint *textures);

    GLenum(GLAPIENTRY *glGetError)(void);

    void(GLAPIENTRY *glGetFloatv)(
        GLenum pname,
        GLfloat *params);

    void(GLAPIENTRY *glGetIntegerv)(
        GLenum pname,
        GLint *params);

    void(GLAPIENTRY *glLoadIdentity)(void);

    void(GLAPIENTRY *glLoadMatrixf)(
        const GLfloat *m);

    void(GLAPIENTRY *glMatrixMode)(
        GLenum mode);

    void(GLAPIENTRY *glMultMatrixf)(
        const GLfloat *m);

    void(GLAPIENTRY *glPopMatrix)(void);

    void(GLAPIENTRY *glPushMatrix)(void);

    void(GLAPIENTRY *glReadPixels)(
        GLint x,
        GLint y,
        GLsizei width,
        GLsizei height,
        GLenum format,
        GLenum type,
        GLvoid *pixels);

    void(GLAPIENTRY *glShadeModel)(
        GLenum mode);

    void(GLAPIENTRY *glTexCoordPointer)(
        GLint size,
        GLenum type,
        GLsizei stride,
        const GLvoid *ptr);

    void(GLAPIENTRY *glTexEnvfv)(
        GLenum target,
        GLenum pname,
        const GLfloat *params);

    void(GLAPIENTRY *glTexEnvi)(
        GLenum target,
        GLenum pname,
        GLint param);

    void(GLAPIENTRY *glTexImage2D)(
        GLenum target,
        GLint level,
        GLint internalFormat,
        GLsizei width,
        GLsizei height,
        GLint border,
        GLenum format,
        GLenum type,
        const GLvoid *pixels);

    void(GLAPIENTRY *glTexParameteri)(
        GLenum target,
        GLenum pname,
        GLint param);

    void(GLAPIENTRY *glTexSubImage2D)(
        GLenum target,
        GLint level,
        GLint xoffset,
        GLint yoffset,
        GLsizei width,
        GLsizei height,
        GLenum format,
        GLenum type,
        const GLvoid *pixels);

    void(GLAPIENTRY *glVertexPointer)(
        GLint size,
        GLenum type,
        GLsizei stride,
        const GLvoid *ptr);

    void(GLAPIENTRY *glViewport)(
        GLint x,
        GLint y,
        GLsizei width,
        GLsizei height);


    // ========================================================
    // GL / GLES 2.x shader functions
    // ========================================================
    //
    // These are kept because other parts of the project may
    // still use the shader-based renderer.
    //
    // They are not used by the OpenGL ES 1.1 fixed-function
    // renderer itself.
    //
    // ========================================================

    PFNGLATTACHSHADERPROC glAttachShader;

    PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;

    PFNGLCOMPILESHADERPROC glCompileShader;

    PFNGLCREATEPROGRAMPROC glCreateProgram;

    PFNGLCREATESHADERPROC glCreateShader;

    PFNGLDELETEPROGRAMPROC glDeleteProgram;

    PFNGLDELETESHADERPROC glDeleteShader;

    PFNGLDISABLEVERTEXATTRIBARRAYPROC
        glDisableVertexAttribArray;

    PFNGLENABLEVERTEXATTRIBARRAYPROC
        glEnableVertexAttribArray;

    PFNGLGETPROGRAMINFOLOGPROC
        glGetProgramInfoLog;

    PFNGLGETPROGRAMIVPROC
        glGetProgramiv;

    PFNGLGETSHADERINFOLOGPROC
        glGetShaderInfoLog;

    PFNGLGETSHADERIVPROC
        glGetShaderiv;

    PFNGLGETUNIFORMLOCATIONPROC
        glGetUniformLocation;

    PFNGLLINKPROGRAMPROC
        glLinkProgram;

    PFNGLSHADERSOURCEPROC
        glShaderSource;

    PFNGLUNIFORM1FPROC
        glUniform1f;

    PFNGLUNIFORM1IPROC
        glUniform1i;

    PFNGLUNIFORM4FPROC
        glUniform4f;

    PFNGLUNIFORMMATRIX4FVPROC
        glUniformMatrix4fv;

    PFNGLUSEPROGRAMPROC
        glUseProgram;

    PFNGLVERTEXATTRIBPOINTERPROC
        glVertexAttribPointer;


private:

    // --------------------------------------------------------
    // GLES versions for functions whose signatures differ
    // --------------------------------------------------------

    void(GLAPIENTRY *glClearDepthf_ptr)(
        GLclampf depth);

    void(GLAPIENTRY *glDepthRangef_ptr)(
        GLclampf near_val,
        GLclampf far_val);


    // --------------------------------------------------------
    // Desktop OpenGL versions
    // --------------------------------------------------------

    void(GLAPIENTRY *glClearDepth)(
        GLclampd depth);

    void(GLAPIENTRY *glDepthRange)(
        GLclampd near_val,
        GLclampd far_val);


    // --------------------------------------------------------
    // Current rendering API
    // --------------------------------------------------------

    bool isGlesContext;
};

extern GLFuncTable g_glFuncTable;
