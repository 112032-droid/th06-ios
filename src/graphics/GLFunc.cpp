#include "GLFunc.hpp"

#include <SDL2/SDL_video.h>

GLFuncTable g_glFuncTable;

#define TRY_RESOLVE_FUNCTION(func) \
    this->func = reinterpret_cast<decltype(this->func)>(SDL_GL_GetProcAddress(#func));

#define TRY_RESOLVE_FUNCTION_GLES(func) \
    this->func##_ptr = reinterpret_cast<decltype(this->func##_ptr)>(SDL_GL_GetProcAddress(#func));


void GLFuncTable::ResolveFunctions(bool glesContext)
{
    // ============================================================
    // Common OpenGL / OpenGL ES fixed-function functions
    // ============================================================

    TRY_RESOLVE_FUNCTION(glAlphaFunc)
    TRY_RESOLVE_FUNCTION(glBindTexture)
    TRY_RESOLVE_FUNCTION(glBlendFunc)
    TRY_RESOLVE_FUNCTION(glClear)
    TRY_RESOLVE_FUNCTION(glClearColor)
    TRY_RESOLVE_FUNCTION(glColorPointer)
    TRY_RESOLVE_FUNCTION(glDeleteTextures)
    TRY_RESOLVE_FUNCTION(glDepthFunc)
    TRY_RESOLVE_FUNCTION(glDepthMask)
    TRY_RESOLVE_FUNCTION(glDisableClientState)
    TRY_RESOLVE_FUNCTION(glDrawArrays)
    TRY_RESOLVE_FUNCTION(glEnable)
    TRY_RESOLVE_FUNCTION(glEnableClientState)
    TRY_RESOLVE_FUNCTION(glFogf)
    TRY_RESOLVE_FUNCTION(glFogfv)
    TRY_RESOLVE_FUNCTION(glGenTextures)
    TRY_RESOLVE_FUNCTION(glGetError)
    TRY_RESOLVE_FUNCTION(glGetFloatv)
    TRY_RESOLVE_FUNCTION(glGetIntegerv)
    TRY_RESOLVE_FUNCTION(glLoadIdentity)
    TRY_RESOLVE_FUNCTION(glLoadMatrixf)
    TRY_RESOLVE_FUNCTION(glMatrixMode)
    TRY_RESOLVE_FUNCTION(glMultMatrixf)
    TRY_RESOLVE_FUNCTION(glPopMatrix)
    TRY_RESOLVE_FUNCTION(glPushMatrix)
    TRY_RESOLVE_FUNCTION(glReadPixels)
    TRY_RESOLVE_FUNCTION(glShadeModel)
    TRY_RESOLVE_FUNCTION(glTexCoordPointer)
    TRY_RESOLVE_FUNCTION(glTexEnvfv)
    TRY_RESOLVE_FUNCTION(glTexEnvi)
    TRY_RESOLVE_FUNCTION(glTexImage2D)
    TRY_RESOLVE_FUNCTION(glTexParameteri)
    TRY_RESOLVE_FUNCTION(glTexSubImage2D)
    TRY_RESOLVE_FUNCTION(glVertexPointer)
    TRY_RESOLVE_FUNCTION(glViewport)

    // ============================================================
    // glClearDepth / glDepthRange
    // ============================================================

    if (glesContext)
    {
        TRY_RESOLVE_FUNCTION_GLES(glClearDepthf)
        TRY_RESOLVE_FUNCTION_GLES(glDepthRangef)
    }
    else
    {
        TRY_RESOLVE_FUNCTION(glClearDepth)
        TRY_RESOLVE_FUNCTION(glDepthRange)
    }

    // ============================================================
    // OpenGL ES 2 / shader functions
    // ============================================================

    TRY_RESOLVE_FUNCTION(glAttachShader)
    TRY_RESOLVE_FUNCTION(glBindAttribLocation)
    TRY_RESOLVE_FUNCTION(glCompileShader)
    TRY_RESOLVE_FUNCTION(glCreateProgram)
    TRY_RESOLVE_FUNCTION(glCreateShader)
    TRY_RESOLVE_FUNCTION(glDeleteProgram)
    TRY_RESOLVE_FUNCTION(glDeleteShader)
    TRY_RESOLVE_FUNCTION(glDisableVertexAttribArray)
    TRY_RESOLVE_FUNCTION(glEnableVertexAttribArray)
    TRY_RESOLVE_FUNCTION(glGetProgramInfoLog)
    TRY_RESOLVE_FUNCTION(glGetProgramiv)
    TRY_RESOLVE_FUNCTION(glGetShaderInfoLog)
    TRY_RESOLVE_FUNCTION(glGetShaderiv)
    TRY_RESOLVE_FUNCTION(glGetUniformLocation)
    TRY_RESOLVE_FUNCTION(glLinkProgram)
    TRY_RESOLVE_FUNCTION(glShaderSource)
    TRY_RESOLVE_FUNCTION(glUniform1f)
    TRY_RESOLVE_FUNCTION(glUniform1i)
    TRY_RESOLVE_FUNCTION(glUniform4f)
    TRY_RESOLVE_FUNCTION(glUniformMatrix4fv)
    TRY_RESOLVE_FUNCTION(glUseProgram)
    TRY_RESOLVE_FUNCTION(glVertexAttribPointer)

    // ============================================================
    // Save context type
    // ============================================================

    this->isGlesContext = glesContext;
}


// ================================================================
// glClearDepthf
// ================================================================

void GLFuncTable::glClearDepthf(GLclampf depth)
{
    if (this->isGlesContext)
    {
        if (this->glClearDepthf_ptr)
        {
            this->glClearDepthf_ptr(depth);
        }
    }
    else
    {
        if (this->glClearDepth)
        {
            this->glClearDepth(depth);
        }
    }
}


// ================================================================
// glDepthRangef
// ================================================================

void GLFuncTable::glDepthRangef(GLclampf near_val, GLclampf far_val)
{
    if (this->isGlesContext)
    {
        if (this->glDepthRangef_ptr)
        {
            this->glDepthRangef_ptr(near_val, far_val);
        }
    }
    else
    {
        if (this->glDepthRange)
        {
            this->glDepthRange(near_val, far_val);
        }
    }
}
