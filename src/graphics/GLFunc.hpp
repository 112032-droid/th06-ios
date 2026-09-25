#pragma once

#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
// 針對 iOS / OpenGLES 補上缺少的型別與巨集
typedef GLfloat GLclampf;
typedef GLfloat GLclampd;
#ifndef GLAPIENTRY
#define GLAPIENTRY
#endif
#ifndef GLbitfield
typedef unsigned int GLbitfield;
#endif

// 補上 iOS 缺少的 PFNGL 函式指標型別定義
#ifndef PFNGLATTACHSHADERPROC
typedef void (*PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
#endif
#ifndef PFNGLBINDATTRIBLOCATIONPROC
typedef void (*PFNGLBINDATTRIBLOCATIONPROC) (GLuint program, GLuint index, const GLchar* name);
#endif
#ifndef PFNGLCOMPILESHADERPROC
typedef void (*PFNGLCOMPILESHADERPROC) (GLuint shader);
#endif
#ifndef PFNGLCREATEPROGRAMPROC
typedef GLuint (*PFNGLCREATEPROGRAMPROC) (void);
#endif
#ifndef PFNGLCREATESHADERPROC
typedef GLuint (*PFNGLCREATESHADERPROC) (GLenum type);
#endif
#ifndef PFNGLDELETEPROGRAMPROC
typedef void (*PFNGLDELETEPROGRAMPROC) (GLuint program);
#endif
#ifndef PFNGLDELETESHADERPROC
typedef void (*PFNGLDELETESHADERPROC) (GLuint shader);
#endif
#ifndef PFNGLDISABLEVERTEXATTRIBARRAYPROC
typedef void (*PFNGLDISABLEVERTEXATTRIBARRAYPROC) (GLuint index);
#endif
#ifndef PFNGLENABLEVERTEXATTRIBARRAYPROC
typedef void (*PFNGLENABLEVERTEXATTRIBARRAYPROC) (GLuint index);
#endif
#ifndef PFNGLGETPROGRAMINFOLOGPROC
typedef void (*PFNGLGETPROGRAMINFOLOGPROC) (GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
#endif
#ifndef PFNGLGETPROGRAMIVPROC
typedef void (*PFNGLGETPROGRAMIVPROC) (GLuint program, GLenum pname, GLint* params);
#endif
#ifndef PFNGLGETSHADERINFOLOGPROC
typedef void (*PFNGLGETSHADERINFOLOGPROC) (GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
#endif
#ifndef PFNGLGETSHADERIVPROC
typedef void (*PFNGLGETSHADERIVPROC) (GLuint shader, GLenum pname, GLint* params);
#endif
#ifndef PFNGLGETUNIFORMLOCATIONPROC
typedef GLint (*PFNGLGETUNIFORMLOCATIONPROC) (GLuint program, const GLchar* name);
#endif
#ifndef PFNGLLINKPROGRAMPROC
typedef void (*PFNGLLINKPROGRAMPROC) (GLuint program);
#endif
#ifndef PFNGLSHADERSOURCEPROC
typedef void (*PFNGLSHADERSOURCEPROC) (GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
#endif
#ifndef PFNGLUNIFORM1FPROC
typedef void (*PFNGLUNIFORM1FPROC) (GLint location, GLfloat v0);
#endif
#ifndef PFNGLUNIFORM1IPROC
typedef void (*PFNGLUNIFORM1IPROC) (GLint location, GLint v0);
#endif
#ifndef PFNGLUNIFORM4FPROC
typedef void (*PFNGLUNIFORM4FPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
#endif
#ifndef PFNGLUNIFORMMATRIX4FVPROC
typedef void (*PFNGLUNIFORMMATRIX4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
#endif
#ifndef PFNGLUSEPROGRAMPROC
typedef void (*PFNGLUSEPROGRAMPROC) (GLuint program);
#endif
#ifndef PFNGLVERTEXATTRIBPOINTERPROC
typedef void (*PFNGLVERTEXATTRIBPOINTERPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer);
#endif

#else
#include <SDL2/SDL_opengl.h>
#endif
#else
#include <SDL2/SDL_opengl.h>
#endif

// Function pointers for OpenGL functions used in EoSD. This is necessary because Windows
//   opengl32 only goes up to OpenGL 1.1 and some of the blending parameters we need are
//   from 1.3. Resolving function addresses at runtime using SDL_GL_GetProcAddress gets
//   around that restriction. Plus not directly linking the GL library is good for flexibility
//   in general, even on UNIX
struct GLFuncTable
{
    void ResolveFunctions(bool glesContext);

    // Functions where arguments use doubles in OpenGL and floats in GLES and therefore need manual dispatch
    void glClearDepthf(GLclampf depth);
    void glDepthRangef(GLclampf near_val, GLclampf far_val);

    // Function pointers for functions shared between GL and GLES
    void(GLAPIENTRY *glAlphaFunc)(GLenum func, GLclampf ref);
    void(GLAPIENTRY *glBindTexture)(GLenum target, GLuint texture);
    void(GLAPIENTRY *glBlendFunc)(GLenum sfactor, GLenum dfactor);
    void(GLAPIENTRY *glClear)(GLbitfield mask);
    void(GLAPIENTRY *glClearColor)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
    void(GLAPIENTRY *glColorPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr);
    void(GLAPIENTRY *glDeleteTextures)(GLsizei n, const GLuint *textures);
    void(GLAPIENTRY *glDepthFunc)(GLenum func);
    void(GLAPIENTRY *glDepthMask)(GLboolean flag);
    void(GLAPIENTRY *glDisableClientState)(GLenum cap);
    void(GLAPIENTRY *glDrawArrays)(GLenum mode, GLint first, GLsizei count);
    void(GLAPIENTRY *glEnable)(GLenum cap);
    void(GLAPIENTRY *glEnableClientState)(GLenum cap);
    void(GLAPIENTRY *glFogf)(GLenum pname, GLfloat param);
    void(GLAPIENTRY *glFogfv)(GLenum pname, const GLfloat *params);
    void(GLAPIENTRY *glGenTextures)(GLsizei n, GLuint *textures);
    GLenum(GLAPIENTRY *glGetError)(void);
    void(GLAPIENTRY *glGetFloatv)(GLenum pname, GLfloat *params);
    void(GLAPIENTRY *glGetIntegerv)(GLenum pname, GLint *params);
    void(GLAPIENTRY *glLoadIdentity)(void);
    void(GLAPIENTRY *glLoadMatrixf)(const GLfloat *m);
    void(GLAPIENTRY *glMatrixMode)(GLenum mode);
    void(GLAPIENTRY *glMultMatrixf)(const GLfloat *m);
    void(GLAPIENTRY *glPopMatrix)(void);
    void(GLAPIENTRY *glPushMatrix)(void);
    void(GLAPIENTRY *glReadPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type,
                                   GLvoid *pixels);
    void(GLAPIENTRY *glShadeModel)(GLenum mode);
    void(GLAPIENTRY *glTexCoordPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr);
    void(GLAPIENTRY *glTexEnvfv)(GLenum target, GLenum pname, const GLfloat *params);
    void(GLAPIENTRY *glTexEnvi)(GLenum target, GLenum pname, GLint param);
    void(GLAPIENTRY *glTexImage2D)(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height,
                                   GLint border, GLenum format, GLenum type, const GLvoid *pixels);
    void(GLAPIENTRY *glTexParameteri)(GLenum target, GLenum pname, GLint param);
    void(GLAPIENTRY *glTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width,
                                      GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
    void(GLAPIENTRY *glVertexPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr);
    void(GLAPIENTRY *glViewport)(GLint x, GLint y, GLsizei width, GLsizei height);

    // GL(ES) 2.X / WebGL
    PFNGLATTACHSHADERPROC glAttachShader;
    PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
    PFNGLCOMPILESHADERPROC glCompileShader;
    PFNGLCREATEPROGRAMPROC glCreateProgram;
    PFNGLCREATESHADERPROC glCreateShader;
    PFNGLDELETEPROGRAMPROC glDeleteProgram;
    PFNGLDELETESHADERPROC glDeleteShader;
    PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
    PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
    PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
    PFNGLGETPROGRAMIVPROC glGetProgramiv;
    PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
    PFNGLGETSHADERIVPROC glGetShaderiv;
    PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
    PFNGLLINKPROGRAMPROC glLinkProgram;
    PFNGLSHADERSOURCEPROC glShaderSource;
    PFNGLUNIFORM1FPROC glUniform1f;
    PFNGLUNIFORM1IPROC glUniform1i;
    PFNGLUNIFORM4FPROC glUniform4f;
    PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
    PFNGLUSEPROGRAMPROC glUseProgram;
    PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;

  private:
    // GLES forms for cases where they're different
    void(GLAPIENTRY *glClearDepthf_ptr)(GLclampf depth);
    void(GLAPIENTRY *glDepthRangef_ptr)(GLclampf near_val, GLclampf far_val);

    // GL forms for cases where they're different
    void(GLAPIENTRY *glClearDepth)(GLclampd depth);
    void(GLAPIENTRY *glDepthRange)(GLclampd near_val, GLclampd far_val);

    bool isGlesContext;
};

extern GLFuncTable g_glFuncTable;
