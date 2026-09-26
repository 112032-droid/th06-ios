#pragma once

#ifdef __APPLE__

#include <TargetConditionals.h>

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR

// ============================================================
// iOS OpenGL ES 1.1
// Used by FixedFunctionGL
// ============================================================

#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>

// ============================================================
// Basic compatibility types
// ============================================================

#ifndef GLclampf
typedef GLfloat GLclampf;
#endif

#ifndef GLclampd
typedef GLfloat GLclampd;
#endif

#ifndef GLAPIENTRY
#define GLAPIENTRY
#endif

#ifndef GLbitfield
typedef unsigned int GLbitfield;
#endif

// ============================================================
// OpenGL ES 2.0 shader constants
//
// WebGL.cpp uses these constants, while FixedFunctionGL
// uses the OpenGL ES 1.1 fixed-function pipeline.
//
// We only need the constants here. The actual ES2 functions
// are resolved dynamically through GLFuncTable.
// ============================================================

#ifndef GL_COMPILE_STATUS
#define GL_COMPILE_STATUS 0x8B81
#endif

#ifndef GL_INFO_LOG_LENGTH
#define GL_INFO_LOG_LENGTH 0x8B84
#endif

#ifndef GL_LINK_STATUS
#define GL_LINK_STATUS 0x8B82
#endif

#ifndef GL_VERTEX_SHADER
#define GL_VERTEX_SHADER 0x8B31
#endif

#ifndef GL_FRAGMENT_SHADER
#define GL_FRAGMENT_SHADER 0x8B30
#endif

// ============================================================
// OpenGL ES 2.0 shader function pointer types
// ============================================================

#ifndef PFNGLATTACHSHADERPROC
typedef void (*PFNGLATTACHSHADERPROC)(
    GLuint program,
    GLuint shader
);
#endif

#ifndef PFNGLBINDATTRIBLOCATIONPROC
typedef void (*PFNGLBINDATTRIBLOCATIONPROC)(
    GLuint program,
    GLuint index,
    const GLchar *name
);
#endif

#ifndef PFNGLCOMPILESHADERPROC
typedef void (*PFNGLCOMPILESHADERPROC)(
    GLuint shader
);
#endif

#ifndef PFNGLCREATEPROGRAMPROC
typedef GLuint (*PFNGLCREATEPROGRAMPROC)(
    void
);
#endif

#ifndef PFNGLCREATESHADERPROC
typedef GLuint (*PFNGLCREATESHADERPROC)(
    GLenum type
);
#endif

#ifndef PFNGLDELETEPROGRAMPROC
typedef void (*PFNGLDELETEPROGRAMPROC)(
    GLuint program
);
#endif

#ifndef PFNGLDELETESHADERPROC
typedef void (*PFNGLDELETESHADERPROC)(
    GLuint shader
);
#endif

#ifndef PFNGLDISABLEVERTEXATTRIBARRAYPROC
typedef void (*PFNGLDISABLEVERTEXATTRIBARRAYPROC)(
    GLuint index
);
#endif

#ifndef PFNGLENABLEVERTEXATTRIBARRAYPROC
typedef void (*PFNGLENABLEVERTEXATTRIBARRAYPROC)(
    GLuint index
);
#endif

#ifndef PFNGLGETPROGRAMINFOLOGPROC
typedef void (*PFNGLGETPROGRAMINFOLOGPROC)(
    GLuint program,
    GLsizei bufSize,
    GLsizei *length,
    GLchar *infoLog
);
#endif

#ifndef PFNGLGETPROGRAMIVPROC
typedef void (*PFNGLGETPROGRAMIVPROC)(
    GLuint program,
    GLenum pname,
    GLint *params
);
#endif

#ifndef PFNGLGETSHADERINFOLOGPROC
typedef void (*PFNGLGETSHADERINFOLOGPROC)(
    GLuint shader,
    GLsizei bufSize,
    GLsizei *length,
    GLchar *infoLog
);
#endif

#ifndef PFNGLGETSHADERIVPROC
typedef void (*PFNGLGETSHADERIVPROC)(
    GLuint shader,
    GLenum pname,
    GLint *params
);
#endif

#ifndef PFNGLGETUNIFORMLOCATIONPROC
typedef GLint (*PFNGLGETUNIFORMLOCATIONPROC)(
    GLuint program,
    const GLchar *name
);
#endif

#ifndef PFNGLLINKPROGRAMPROC
typedef void (*PFNGLLINKPROGRAMPROC)(
    GLuint program
);
#endif

#ifndef PFNGLSHADERSOURCEPROC
typedef void (*PFNGLSHADERSOURCEPROC)(
    GLuint shader,
    GLsizei count,
    const GLchar *const *string,
    const GLint *length
);
#endif

#ifndef PFNGLUNIFORM1FPROC
typedef void (*PFNGLUNIFORM1FPROC)(
    GLint location,
    GLfloat v0
);
#endif

#ifndef PFNGLUNIFORM1IPROC
typedef void (*PFNGLUNIFORM1IPROC)(
    GLint location,
    GLint v0
);
#endif

#ifndef PFNGLUNIFORM4FPROC
typedef void (*PFNGLUNIFORM4FPROC)(
    GLint location,
    GLfloat v0,
    GLfloat v1,
    GLfloat v2,
    GLfloat v3
);
#endif

#ifndef PFNGLUNIFORMMATRIX4FVPROC
typedef void (*PFNGLUNIFORMMATRIX4FVPROC)(
    GLint location,
    GLsizei count,
    GLboolean transpose,
    const GLfloat *value
);
#endif

#ifndef PFNGLUSEPROGRAMPROC
typedef void (*PFNGLUSEPROGRAMPROC)(
    GLuint program
);
#endif

#ifndef PFNGLVERTEXATTRIBPOINTERPROC
typedef void (*PFNGLVERTEXATTRIBPOINTERPROC)(
    GLuint index,
    GLint size,
    GLenum type,
    GLboolean normalized,
    GLsizei stride,
    const GLvoid *pointer
);
#endif

#else

// ============================================================
// Desktop OpenGL
// ============================================================

#include <SDL2/SDL_opengl.h>

#endif

#else

// ============================================================
// Non-Apple platforms
// ============================================================

#include <SDL2/SDL_opengl.h>

#endif


// ================================================================
// GL Function Table
// ================================================================

struct GLFuncTable
{
    void ResolveFunctions(bool glesContext);

    // ============================================================
    // Depth functions
    // ============================================================

    void glClearDepthf(GLclampf depth);
    void glDepthRangef(GLclampf near_val, GLclampf far_val);

    // ============================================================
    // OpenGL / OpenGL ES fixed-function API
    // ============================================================

    void(GLAPIENTRY *glAlphaFunc)(GLenum func, GLclampf ref);

    void(GLAPIENTRY *glBindTexture)(
        GLenum target,
        GLuint texture
    );

    void(GLAPIENTRY *glBlendFunc)(
        GLenum sfactor,
        GLenum dfactor
    );

    void(GLAPIENTRY *glClear)(
        GLbitfield mask
    );

    void(GLAPIENTRY *glClearColor)(
        GLclampf red,
        GLclampf green,
        GLclampf blue,
        GLclampf alpha
    );

    void(GLAPIENTRY *glColorPointer)(
        GLint size,
        GLenum type,
        GLsizei stride,
        const GLvoid *ptr
    );

    void(GLAPIENTRY *glDeleteTextures)(
        GLsizei n,
        const GLuint *textures
    );

    void(GLAPIENTRY *glDepthFunc)(
        GLenum func
    );

    void(GLAPIENTRY *glDepthMask)(
        GLboolean flag
    );

    void(GLAPIENTRY *glDisableClientState)(
        GLenum cap
    );

    void(GLAPIENTRY *glDrawArrays)(
        GLenum mode,
        GLint first,
        GLsizei count
    );

    void(GLAPIENTRY *glEnable)(
        GLenum cap
    );

    void(GLAPIENTRY *glEnableClientState)(
        GLenum cap
    );

    void(GLAPIENTRY *glFogf)(
        GLenum pname,
        GLfloat param
    );

    void(GLAPIENTRY *glFogfv)(
        GLenum pname,
        const GLfloat *params
    );

    void(GLAPIENTRY *glGenTextures)(
        GLsizei n,
        GLuint *textures
    );

    GLenum(GLAPIENTRY *glGetError)(
        void
    );

    void(GLAPIENTRY *glGetFloatv)(
        GLenum pname,
        GLfloat *params
    );

    void(GLAPIENTRY *glGetIntegerv)(
        GLenum pname,
        GLint *params
    );

    void(GLAPIENTRY *glLoadIdentity)(
        void
    );

    void(GLAPIENTRY *glLoadMatrixf)(
        const GLfloat *m
    );

    void(GLAPIENTRY *glMatrixMode)(
        GLenum mode
    );

    void(GLAPIENTRY *glMultMatrixf)(
        const GLfloat *m
    );

    void(GLAPIENTRY *glPopMatrix)(
        void
    );

    void(GLAPIENTRY *glPushMatrix)(
        void
    );

    void(GLAPIENTRY *glReadPixels)(
        GLint x,
        GLint y,
        GLsizei width,
        GLsizei height,
        GLenum format,
        GLenum type,
        GLvoid *pixels
    );

    void(GLAPIENTRY *glShadeModel)(
        GLenum mode
    );

    void(GLAPIENTRY *glTexCoordPointer)(
        GLint size,
        GLenum type,
        GLsizei stride,
        const GLvoid *ptr
    );

    void(GLAPIENTRY *glTexEnvfv)(
        GLenum target,
        GLenum pname,
        const GLfloat *params
    );

    void(GLAPIENTRY *glTexEnvi)(
        GLenum target,
        GLenum pname,
        GLint param
    );

    void(GLAPIENTRY *glTexImage2D)(
        GLenum target,
        GLint level,
        GLint internalFormat,
        GLsizei width,
        GLsizei height,
        GLint border,
        GLenum format,
        GLenum type,
        const GLvoid *pixels
    );

    void(GLAPIENTRY *glTexParameteri)(
        GLenum target,
        GLenum pname,
        GLint param
    );

    void(GLAPIENTRY *glTexSubImage2D)(
        GLenum target,
        GLint level,
        GLint xoffset,
        GLint yoffset,
        GLsizei width,
        GLsizei height,
        GLenum format,
        GLenum type,
        const GLvoid *pixels
    );

    void(GLAPIENTRY *glVertexPointer)(
        GLint size,
        GLenum type,
        GLsizei stride,
        const GLvoid *ptr
    );

    void(GLAPIENTRY *glViewport)(
        GLint x,
        GLint y,
        GLsizei width,
        GLsizei height
    );


    // ============================================================
    // OpenGL ES 2.0 / WebGL shader functions
    // ============================================================

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

    // ============================================================
    // GLES depth functions
    // ============================================================

    void(GLAPIENTRY *glClearDepthf_ptr)(
        GLclampf depth
    );

    void(GLAPIENTRY *glDepthRangef_ptr)(
        GLclampf near_val,
        GLclampf far_val
    );


    // ============================================================
    // Desktop OpenGL depth functions
    // ============================================================

    void(GLAPIENTRY *glClearDepth)(
        GLclampd depth
    );

    void(GLAPIENTRY *glDepthRange)(
        GLclampd near_val,
        GLclampd far_val
    );


    // ============================================================
    // Current context type
    // ============================================================

    bool isGlesContext;
};


// ================================================================
// Global function table
// ================================================================

extern GLFuncTable g_glFuncTable;
