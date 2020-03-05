#pragma once

#include <qopenglfunctions_3_3_core.h>
#include <qopenglcontext.h>

#define _OPENGL_API_VERSION_ QOpenGLFunctions_3_3_Core

#define _CURRENT_CONTEXT_ QOpenGLContext::currentContext()->

#define DEFAULT_FRAMEBUFFER _CURRENT_CONTEXT_ defaultFramebufferObject()

#define _OPENGL_API_PREFIX_ _CURRENT_CONTEXT_ versionFunctions<_OPENGL_API_VERSION_>()->

#ifndef glActiveTexture
#define glActiveTexture _OPENGL_API_PREFIX_ glActiveTexture
#endif

#ifndef glAttachShader
#define glAttachShader _OPENGL_API_PREFIX_ glAttachShader
#endif

#ifndef glBeginConditionalRender
#define glBeginConditionalRender _OPENGL_API_PREFIX_ glBeginConditionalRender
#endif

#ifndef glBeginQuery
#define glBeginQuery _OPENGL_API_PREFIX_ glBeginQuery
#endif

#ifndef glBeginTransformFeedback
#define glBeginTransformFeedback _OPENGL_API_PREFIX_ glBeginTransformFeedback
#endif

#ifndef glBindAttribLocation
#define glBindAttribLocation _OPENGL_API_PREFIX_ glBindAttribLocation
#endif

#ifndef glBindBuffer
#define glBindBuffer _OPENGL_API_PREFIX_ glBindBuffer
#endif

#ifndef glBindBufferBase
#define glBindBufferBase _OPENGL_API_PREFIX_ glBindBufferBase
#endif

#ifndef glBindBufferRange
#define glBindBufferRange _OPENGL_API_PREFIX_ glBindBufferRange
#endif

#ifndef glBindFragDataLocation
#define glBindFragDataLocation _OPENGL_API_PREFIX_ glBindFragDataLocation
#endif

#ifndef glBindFragDataLocationIndexed
#define glBindFragDataLocationIndexed _OPENGL_API_PREFIX_ glBindFragDataLocationIndexed
#endif

#ifndef glBindFramebuffer
#define glBindFramebuffer _OPENGL_API_PREFIX_ glBindFramebuffer
#endif

#ifndef glBindRenderbuffer
#define glBindRenderbuffer _OPENGL_API_PREFIX_ glBindRenderbuffer
#endif

#ifndef glBindSampler
#define glBindSampler _OPENGL_API_PREFIX_ glBindSampler
#endif

#ifndef glBindTexture
#define glBindTexture _OPENGL_API_PREFIX_ glBindTexture
#endif

#ifndef glBindVertexArray
#define glBindVertexArray _OPENGL_API_PREFIX_ glBindVertexArray
#endif

#ifndef glBlendColor
#define glBlendColor _OPENGL_API_PREFIX_ glBlendColor
#endif

#ifndef glBlendEquation
#define glBlendEquation _OPENGL_API_PREFIX_ glBlendEquation
#endif

#ifndef glBlendEquationSeparate
#define glBlendEquationSeparate _OPENGL_API_PREFIX_ glBlendEquationSeparate
#endif

#ifndef glBlendFunc
#define glBlendFunc _OPENGL_API_PREFIX_ glBlendFunc
#endif

#ifndef glBlendFuncSeparate
#define glBlendFuncSeparate _OPENGL_API_PREFIX_ glBlendFuncSeparate
#endif

#ifndef glBlitFramebuffer
#define glBlitFramebuffer _OPENGL_API_PREFIX_ glBlitFramebuffer
#endif

#ifndef glBufferData
#define glBufferData _OPENGL_API_PREFIX_ glBufferData
#endif

#ifndef glBufferSubData
#define glBufferSubData _OPENGL_API_PREFIX_ glBufferSubData
#endif

#ifndef glCheckFramebufferStatus
#define glCheckFramebufferStatus _OPENGL_API_PREFIX_ glCheckFramebufferStatus
#endif

#ifndef glClampColor
#define glClampColor _OPENGL_API_PREFIX_ glClampColor
#endif

#ifndef glClear
#define glClear _OPENGL_API_PREFIX_ glClear
#endif

#ifndef glClearBufferfi
#define glClearBufferfi _OPENGL_API_PREFIX_ glClearBufferfi
#endif

#ifndef glClearBufferfv
#define glClearBufferfv _OPENGL_API_PREFIX_ glClearBufferfv
#endif

#ifndef glClearBufferiv
#define glClearBufferiv _OPENGL_API_PREFIX_ glClearBufferiv
#endif

#ifndef glClearBufferuiv
#define glClearBufferuiv _OPENGL_API_PREFIX_ glClearBufferuiv
#endif

#ifndef glClearColor
#define glClearColor _OPENGL_API_PREFIX_ glClearColor
#endif

#ifndef glClearDepth
#define glClearDepth _OPENGL_API_PREFIX_ glClearDepth
#endif

#ifndef glClearStencil
#define glClearStencil _OPENGL_API_PREFIX_ glClearStencil
#endif

#ifndef glClientWaitSync
#define glClientWaitSync _OPENGL_API_PREFIX_ glClientWaitSync
#endif

#ifndef glColorMask
#define glColorMask _OPENGL_API_PREFIX_ glColorMask
#endif

#ifndef glColorMaski
#define glColorMaski _OPENGL_API_PREFIX_ glColorMaski
#endif

#ifndef glColorP3ui
#define glColorP3ui _OPENGL_API_PREFIX_ glColorP3ui
#endif

#ifndef glColorP3uiv
#define glColorP3uiv _OPENGL_API_PREFIX_ glColorP3uiv
#endif

#ifndef glColorP4ui
#define glColorP4ui _OPENGL_API_PREFIX_ glColorP4ui
#endif

#ifndef glColorP4uiv
#define glColorP4uiv _OPENGL_API_PREFIX_ glColorP4uiv
#endif

#ifndef glCompileShader
#define glCompileShader _OPENGL_API_PREFIX_ glCompileShader
#endif

#ifndef glCompressedTexImage1D
#define glCompressedTexImage1D _OPENGL_API_PREFIX_ glCompressedTexImage1D
#endif

#ifndef glCompressedTexImage2D
#define glCompressedTexImage2D _OPENGL_API_PREFIX_ glCompressedTexImage2D
#endif

#ifndef glCompressedTexImage3D
#define glCompressedTexImage3D _OPENGL_API_PREFIX_ glCompressedTexImage3D
#endif

#ifndef glCompressedTexSubImage1D
#define glCompressedTexSubImage1D _OPENGL_API_PREFIX_ glCompressedTexSubImage1D
#endif

#ifndef glCompressedTexSubImage2D
#define glCompressedTexSubImage2D _OPENGL_API_PREFIX_ glCompressedTexSubImage2D
#endif

#ifndef glCompressedTexSubImage3D
#define glCompressedTexSubImage3D _OPENGL_API_PREFIX_ glCompressedTexSubImage3D
#endif

#ifndef glCopyBufferSubData
#define glCopyBufferSubData _OPENGL_API_PREFIX_ glCopyBufferSubData
#endif

#ifndef glCopyTexImage1D
#define glCopyTexImage1D _OPENGL_API_PREFIX_ glCopyTexImage1D
#endif

#ifndef glCopyTexImage2D
#define glCopyTexImage2D _OPENGL_API_PREFIX_ glCopyTexImage2D
#endif

#ifndef glCopyTexSubImage1D
#define glCopyTexSubImage1D _OPENGL_API_PREFIX_ glCopyTexSubImage1D
#endif

#ifndef glCopyTexSubImage2D
#define glCopyTexSubImage2D _OPENGL_API_PREFIX_ glCopyTexSubImage2D
#endif

#ifndef glCopyTexSubImage3D
#define glCopyTexSubImage3D _OPENGL_API_PREFIX_ glCopyTexSubImage3D
#endif

#ifndef glCreateProgram
#define glCreateProgram _OPENGL_API_PREFIX_ glCreateProgram
#endif

#ifndef glCreateShader
#define glCreateShader _OPENGL_API_PREFIX_ glCreateShader
#endif

#ifndef glCullFace
#define glCullFace _OPENGL_API_PREFIX_ glCullFace
#endif

#ifndef glDeleteBuffers
#define glDeleteBuffers _OPENGL_API_PREFIX_ glDeleteBuffers
#endif

#ifndef glDeleteFramebuffers
#define glDeleteFramebuffers _OPENGL_API_PREFIX_ glDeleteFramebuffers
#endif

#ifndef glDeleteProgram
#define glDeleteProgram _OPENGL_API_PREFIX_ glDeleteProgram
#endif

#ifndef glDeleteQueries
#define glDeleteQueries _OPENGL_API_PREFIX_ glDeleteQueries
#endif

#ifndef glDeleteRenderbuffers
#define glDeleteRenderbuffers _OPENGL_API_PREFIX_ glDeleteRenderbuffers
#endif

#ifndef glDeleteSamplers
#define glDeleteSamplers _OPENGL_API_PREFIX_ glDeleteSamplers
#endif

#ifndef glDeleteShader
#define glDeleteShader _OPENGL_API_PREFIX_ glDeleteShader
#endif

#ifndef glDeleteSync
#define glDeleteSync _OPENGL_API_PREFIX_ glDeleteSync
#endif

#ifndef glDeleteTextures
#define glDeleteTextures _OPENGL_API_PREFIX_ glDeleteTextures
#endif

#ifndef glDeleteVertexArrays
#define glDeleteVertexArrays _OPENGL_API_PREFIX_ glDeleteVertexArrays
#endif

#ifndef glDepthFunc
#define glDepthFunc _OPENGL_API_PREFIX_ glDepthFunc
#endif

#ifndef glDepthMask
#define glDepthMask _OPENGL_API_PREFIX_ glDepthMask
#endif

#ifndef glDepthRange
#define glDepthRange _OPENGL_API_PREFIX_ glDepthRange
#endif

#ifndef glDetachShader
#define glDetachShader _OPENGL_API_PREFIX_ glDetachShader
#endif

#ifndef glDisable
#define glDisable _OPENGL_API_PREFIX_ glDisable
#endif

#ifndef glDisableVertexAttribArray
#define glDisableVertexAttribArray _OPENGL_API_PREFIX_ glDisableVertexAttribArray
#endif

#ifndef glDisablei
#define glDisablei _OPENGL_API_PREFIX_ glDisablei
#endif

#ifndef glDrawArrays
#define glDrawArrays _OPENGL_API_PREFIX_ glDrawArrays
#endif

#ifndef glDrawArraysInstanced
#define glDrawArraysInstanced _OPENGL_API_PREFIX_ glDrawArraysInstanced
#endif

#ifndef glDrawBuffer
#define glDrawBuffer _OPENGL_API_PREFIX_ glDrawBuffer
#endif

#ifndef glDrawBuffers
#define glDrawBuffers _OPENGL_API_PREFIX_ glDrawBuffers
#endif

#ifndef glDrawElements
#define glDrawElements _OPENGL_API_PREFIX_ glDrawElements
#endif

#ifndef glDrawElementsBaseVertex
#define glDrawElementsBaseVertex _OPENGL_API_PREFIX_ glDrawElementsBaseVertex
#endif

#ifndef glDrawElementsInstanced
#define glDrawElementsInstanced _OPENGL_API_PREFIX_ glDrawElementsInstanced
#endif

#ifndef glDrawElementsInstancedBaseVertex
#define glDrawElementsInstancedBaseVertex _OPENGL_API_PREFIX_ glDrawElementsInstancedBaseVertex
#endif

#ifndef glDrawRangeElements
#define glDrawRangeElements _OPENGL_API_PREFIX_ glDrawRangeElements
#endif

#ifndef glDrawRangeElementsBaseVertex
#define glDrawRangeElementsBaseVertex _OPENGL_API_PREFIX_ glDrawRangeElementsBaseVertex
#endif

#ifndef glEnable
#define glEnable _OPENGL_API_PREFIX_ glEnable
#endif

#ifndef glEnableVertexAttribArray
#define glEnableVertexAttribArray _OPENGL_API_PREFIX_ glEnableVertexAttribArray
#endif

#ifndef glEnablei
#define glEnablei _OPENGL_API_PREFIX_ glEnablei
#endif

#ifndef glEndConditionalRender
#define glEndConditionalRender _OPENGL_API_PREFIX_ glEndConditionalRender
#endif

#ifndef glEndQuery
#define glEndQuery _OPENGL_API_PREFIX_ glEndQuery
#endif

#ifndef glEndTransformFeedback
#define glEndTransformFeedback _OPENGL_API_PREFIX_ glEndTransformFeedback
#endif

#ifndef glFenceSync
#define glFenceSync _OPENGL_API_PREFIX_ glFenceSync
#endif

#ifndef glFinish
#define glFinish _OPENGL_API_PREFIX_ glFinish
#endif

#ifndef glFlush
#define glFlush _OPENGL_API_PREFIX_ glFlush
#endif

#ifndef glFlushMappedBufferRange
#define glFlushMappedBufferRange _OPENGL_API_PREFIX_ glFlushMappedBufferRange
#endif

#ifndef glFramebufferRenderbuffer
#define glFramebufferRenderbuffer _OPENGL_API_PREFIX_ glFramebufferRenderbuffer
#endif

#ifndef glFramebufferTexture
#define glFramebufferTexture _OPENGL_API_PREFIX_ glFramebufferTexture
#endif

#ifndef glFramebufferTexture1D
#define glFramebufferTexture1D _OPENGL_API_PREFIX_ glFramebufferTexture1D
#endif

#ifndef glFramebufferTexture2D
#define glFramebufferTexture2D _OPENGL_API_PREFIX_ glFramebufferTexture2D
#endif

#ifndef glFramebufferTexture3D
#define glFramebufferTexture3D _OPENGL_API_PREFIX_ glFramebufferTexture3D
#endif

#ifndef glFramebufferTextureLayer
#define glFramebufferTextureLayer _OPENGL_API_PREFIX_ glFramebufferTextureLayer
#endif

#ifndef glFrontFace
#define glFrontFace _OPENGL_API_PREFIX_ glFrontFace
#endif

#ifndef glGenBuffers
#define glGenBuffers _OPENGL_API_PREFIX_ glGenBuffers
#endif

#ifndef glGenFramebuffers
#define glGenFramebuffers _OPENGL_API_PREFIX_ glGenFramebuffers
#endif

#ifndef glGenQueries
#define glGenQueries _OPENGL_API_PREFIX_ glGenQueries
#endif

#ifndef glGenRenderbuffers
#define glGenRenderbuffers _OPENGL_API_PREFIX_ glGenRenderbuffers
#endif

#ifndef glGenSamplers
#define glGenSamplers _OPENGL_API_PREFIX_ glGenSamplers
#endif

#ifndef glGenTextures
#define glGenTextures _OPENGL_API_PREFIX_ glGenTextures
#endif

#ifndef glGenVertexArrays
#define glGenVertexArrays _OPENGL_API_PREFIX_ glGenVertexArrays
#endif

#ifndef glGenerateMipmap
#define glGenerateMipmap _OPENGL_API_PREFIX_ glGenerateMipmap
#endif

#ifndef glGetActiveAttrib
#define glGetActiveAttrib _OPENGL_API_PREFIX_ glGetActiveAttrib
#endif

#ifndef glGetActiveUniform
#define glGetActiveUniform _OPENGL_API_PREFIX_ glGetActiveUniform
#endif

#ifndef glGetActiveUniformBlockName
#define glGetActiveUniformBlockName _OPENGL_API_PREFIX_ glGetActiveUniformBlockName
#endif

#ifndef glGetActiveUniformBlockiv
#define glGetActiveUniformBlockiv _OPENGL_API_PREFIX_ glGetActiveUniformBlockiv
#endif

#ifndef glGetActiveUniformName
#define glGetActiveUniformName _OPENGL_API_PREFIX_ glGetActiveUniformName
#endif

#ifndef glGetActiveUniformsiv
#define glGetActiveUniformsiv _OPENGL_API_PREFIX_ glGetActiveUniformsiv
#endif

#ifndef glGetAttachedShaders
#define glGetAttachedShaders _OPENGL_API_PREFIX_ glGetAttachedShaders
#endif

#ifndef glGetAttribLocation
#define glGetAttribLocation _OPENGL_API_PREFIX_ glGetAttribLocation
#endif

#ifndef glGetBooleani_v
#define glGetBooleani_v _OPENGL_API_PREFIX_ glGetBooleani_v
#endif

#ifndef glGetBooleanv
#define glGetBooleanv _OPENGL_API_PREFIX_ glGetBooleanv
#endif

#ifndef glGetBufferParameteri64v
#define glGetBufferParameteri64v _OPENGL_API_PREFIX_ glGetBufferParameteri64v
#endif

#ifndef glGetBufferParameteriv
#define glGetBufferParameteriv _OPENGL_API_PREFIX_ glGetBufferParameteriv
#endif

#ifndef glGetBufferPointerv
#define glGetBufferPointerv _OPENGL_API_PREFIX_ glGetBufferPointerv
#endif

#ifndef glGetBufferSubData
#define glGetBufferSubData _OPENGL_API_PREFIX_ glGetBufferSubData
#endif

#ifndef glGetCompressedTexImage
#define glGetCompressedTexImage _OPENGL_API_PREFIX_ glGetCompressedTexImage
#endif

#ifndef glGetDoublev
#define glGetDoublev _OPENGL_API_PREFIX_ glGetDoublev
#endif

#ifndef glGetError
#define glGetError _OPENGL_API_PREFIX_ glGetError
#endif

#ifndef glGetFloatv
#define glGetFloatv _OPENGL_API_PREFIX_ glGetFloatv
#endif

#ifndef glGetFragDataIndex
#define glGetFragDataIndex _OPENGL_API_PREFIX_ glGetFragDataIndex
#endif

#ifndef glGetFragDataLocation
#define glGetFragDataLocation _OPENGL_API_PREFIX_ glGetFragDataLocation
#endif

#ifndef glGetFramebufferAttachmentParameteriv
#define glGetFramebufferAttachmentParameteriv _OPENGL_API_PREFIX_ glGetFramebufferAttachmentParameteriv
#endif

#ifndef glGetInteger64i_v
#define glGetInteger64i_v _OPENGL_API_PREFIX_ glGetInteger64i_v
#endif

#ifndef glGetInteger64v
#define glGetInteger64v _OPENGL_API_PREFIX_ glGetInteger64v
#endif

#ifndef glGetIntegeri_v
#define glGetIntegeri_v _OPENGL_API_PREFIX_ glGetIntegeri_v
#endif

#ifndef glGetIntegerv
#define glGetIntegerv _OPENGL_API_PREFIX_ glGetIntegerv
#endif

#ifndef glGetMultisamplefv
#define glGetMultisamplefv _OPENGL_API_PREFIX_ glGetMultisamplefv
#endif

#ifndef glGetPointerv
#define glGetPointerv _OPENGL_API_PREFIX_ glGetPointerv
#endif

#ifndef glGetProgramInfoLog
#define glGetProgramInfoLog _OPENGL_API_PREFIX_ glGetProgramInfoLog
#endif

#ifndef glGetProgramiv
#define glGetProgramiv _OPENGL_API_PREFIX_ glGetProgramiv
#endif

#ifndef glGetQueryObjecti64v
#define glGetQueryObjecti64v _OPENGL_API_PREFIX_ glGetQueryObjecti64v
#endif

#ifndef glGetQueryObjectiv
#define glGetQueryObjectiv _OPENGL_API_PREFIX_ glGetQueryObjectiv
#endif

#ifndef glGetQueryObjectui64v
#define glGetQueryObjectui64v _OPENGL_API_PREFIX_ glGetQueryObjectui64v
#endif

#ifndef glGetQueryObjectuiv
#define glGetQueryObjectuiv _OPENGL_API_PREFIX_ glGetQueryObjectuiv
#endif

#ifndef glGetQueryiv
#define glGetQueryiv _OPENGL_API_PREFIX_ glGetQueryiv
#endif

#ifndef glGetRenderbufferParameteriv
#define glGetRenderbufferParameteriv _OPENGL_API_PREFIX_ glGetRenderbufferParameteriv
#endif

#ifndef glGetSamplerParameterIiv
#define glGetSamplerParameterIiv _OPENGL_API_PREFIX_ glGetSamplerParameterIiv
#endif

#ifndef glGetSamplerParameterIuiv
#define glGetSamplerParameterIuiv _OPENGL_API_PREFIX_ glGetSamplerParameterIuiv
#endif

#ifndef glGetSamplerParameterfv
#define glGetSamplerParameterfv _OPENGL_API_PREFIX_ glGetSamplerParameterfv
#endif

#ifndef glGetSamplerParameteriv
#define glGetSamplerParameteriv _OPENGL_API_PREFIX_ glGetSamplerParameteriv
#endif

#ifndef glGetShaderInfoLog
#define glGetShaderInfoLog _OPENGL_API_PREFIX_ glGetShaderInfoLog
#endif

#ifndef glGetShaderSource
#define glGetShaderSource _OPENGL_API_PREFIX_ glGetShaderSource
#endif

#ifndef glGetShaderiv
#define glGetShaderiv _OPENGL_API_PREFIX_ glGetShaderiv
#endif

#ifndef glGetString
#define glGetString _OPENGL_API_PREFIX_ glGetString
#endif

#ifndef glGetStringi
#define glGetStringi _OPENGL_API_PREFIX_ glGetStringi
#endif

#ifndef glGetSynciv
#define glGetSynciv _OPENGL_API_PREFIX_ glGetSynciv
#endif

#ifndef glGetTexImage
#define glGetTexImage _OPENGL_API_PREFIX_ glGetTexImage
#endif

#ifndef glGetTexLevelParameterfv
#define glGetTexLevelParameterfv _OPENGL_API_PREFIX_ glGetTexLevelParameterfv
#endif

#ifndef glGetTexLevelParameteriv
#define glGetTexLevelParameteriv _OPENGL_API_PREFIX_ glGetTexLevelParameteriv
#endif

#ifndef glGetTexParameterIiv
#define glGetTexParameterIiv _OPENGL_API_PREFIX_ glGetTexParameterIiv
#endif

#ifndef glGetTexParameterIuiv
#define glGetTexParameterIuiv _OPENGL_API_PREFIX_ glGetTexParameterIuiv
#endif

#ifndef glGetTexParameterfv
#define glGetTexParameterfv _OPENGL_API_PREFIX_ glGetTexParameterfv
#endif

#ifndef glGetTexParameteriv
#define glGetTexParameteriv _OPENGL_API_PREFIX_ glGetTexParameteriv
#endif

#ifndef glGetTransformFeedbackVarying
#define glGetTransformFeedbackVarying _OPENGL_API_PREFIX_ glGetTransformFeedbackVarying
#endif

#ifndef glGetUniformBlockIndex
#define glGetUniformBlockIndex _OPENGL_API_PREFIX_ glGetUniformBlockIndex
#endif

#ifndef glGetUniformIndices
#define glGetUniformIndices _OPENGL_API_PREFIX_ glGetUniformIndices
#endif

#ifndef glGetUniformLocation
#define glGetUniformLocation _OPENGL_API_PREFIX_ glGetUniformLocation
#endif

#ifndef glGetUniformfv
#define glGetUniformfv _OPENGL_API_PREFIX_ glGetUniformfv
#endif

#ifndef glGetUniformiv
#define glGetUniformiv _OPENGL_API_PREFIX_ glGetUniformiv
#endif

#ifndef glGetUniformuiv
#define glGetUniformuiv _OPENGL_API_PREFIX_ glGetUniformuiv
#endif

#ifndef glGetVertexAttribIiv
#define glGetVertexAttribIiv _OPENGL_API_PREFIX_ glGetVertexAttribIiv
#endif

#ifndef glGetVertexAttribIuiv
#define glGetVertexAttribIuiv _OPENGL_API_PREFIX_ glGetVertexAttribIuiv
#endif

#ifndef glGetVertexAttribPointerv
#define glGetVertexAttribPointerv _OPENGL_API_PREFIX_ glGetVertexAttribPointerv
#endif

#ifndef glGetVertexAttribdv
#define glGetVertexAttribdv _OPENGL_API_PREFIX_ glGetVertexAttribdv
#endif

#ifndef glGetVertexAttribfv
#define glGetVertexAttribfv _OPENGL_API_PREFIX_ glGetVertexAttribfv
#endif

#ifndef glGetVertexAttribiv
#define glGetVertexAttribiv _OPENGL_API_PREFIX_ glGetVertexAttribiv
#endif

#ifndef glHint
#define glHint _OPENGL_API_PREFIX_ glHint
#endif

#ifndef glIndexub
#define glIndexub _OPENGL_API_PREFIX_ glIndexub
#endif

#ifndef glIndexubv
#define glIndexubv _OPENGL_API_PREFIX_ glIndexubv
#endif

#ifndef glIsBuffer
#define glIsBuffer _OPENGL_API_PREFIX_ glIsBuffer
#endif

#ifndef glIsEnabled
#define glIsEnabled _OPENGL_API_PREFIX_ glIsEnabled
#endif

#ifndef glIsEnabledi
#define glIsEnabledi _OPENGL_API_PREFIX_ glIsEnabledi
#endif

#ifndef glIsFramebuffer
#define glIsFramebuffer _OPENGL_API_PREFIX_ glIsFramebuffer
#endif

#ifndef glIsProgram
#define glIsProgram _OPENGL_API_PREFIX_ glIsProgram
#endif

#ifndef glIsQuery
#define glIsQuery _OPENGL_API_PREFIX_ glIsQuery
#endif

#ifndef glIsRenderbuffer
#define glIsRenderbuffer _OPENGL_API_PREFIX_ glIsRenderbuffer
#endif

#ifndef glIsSampler
#define glIsSampler _OPENGL_API_PREFIX_ glIsSampler
#endif

#ifndef glIsShader
#define glIsShader _OPENGL_API_PREFIX_ glIsShader
#endif

#ifndef glIsSync
#define glIsSync _OPENGL_API_PREFIX_ glIsSync
#endif

#ifndef glIsTexture
#define glIsTexture _OPENGL_API_PREFIX_ glIsTexture
#endif

#ifndef glIsVertexArray
#define glIsVertexArray _OPENGL_API_PREFIX_ glIsVertexArray
#endif

#ifndef glLineWidth
#define glLineWidth _OPENGL_API_PREFIX_ glLineWidth
#endif

#ifndef glLinkProgram
#define glLinkProgram _OPENGL_API_PREFIX_ glLinkProgram
#endif

#ifndef glLogicOp
#define glLogicOp _OPENGL_API_PREFIX_ glLogicOp
#endif

#ifndef glMapBuffer
#define glMapBuffer _OPENGL_API_PREFIX_ glMapBuffer
#endif

#ifndef glMapBufferRange
#define glMapBufferRange _OPENGL_API_PREFIX_ glMapBufferRange
#endif

#ifndef glMultiDrawArrays
#define glMultiDrawArrays _OPENGL_API_PREFIX_ glMultiDrawArrays
#endif

#ifndef glMultiDrawElements
#define glMultiDrawElements _OPENGL_API_PREFIX_ glMultiDrawElements
#endif

#ifndef glMultiDrawElementsBaseVertex
#define glMultiDrawElementsBaseVertex _OPENGL_API_PREFIX_ glMultiDrawElementsBaseVertex
#endif

#ifndef glMultiTexCoordP1ui
#define glMultiTexCoordP1ui _OPENGL_API_PREFIX_ glMultiTexCoordP1ui
#endif

#ifndef glMultiTexCoordP1uiv
#define glMultiTexCoordP1uiv _OPENGL_API_PREFIX_ glMultiTexCoordP1uiv
#endif

#ifndef glMultiTexCoordP2ui
#define glMultiTexCoordP2ui _OPENGL_API_PREFIX_ glMultiTexCoordP2ui
#endif

#ifndef glMultiTexCoordP2uiv
#define glMultiTexCoordP2uiv _OPENGL_API_PREFIX_ glMultiTexCoordP2uiv
#endif

#ifndef glMultiTexCoordP3ui
#define glMultiTexCoordP3ui _OPENGL_API_PREFIX_ glMultiTexCoordP3ui
#endif

#ifndef glMultiTexCoordP3uiv
#define glMultiTexCoordP3uiv _OPENGL_API_PREFIX_ glMultiTexCoordP3uiv
#endif

#ifndef glMultiTexCoordP4ui
#define glMultiTexCoordP4ui _OPENGL_API_PREFIX_ glMultiTexCoordP4ui
#endif

#ifndef glMultiTexCoordP4uiv
#define glMultiTexCoordP4uiv _OPENGL_API_PREFIX_ glMultiTexCoordP4uiv
#endif

#ifndef glNormalP3ui
#define glNormalP3ui _OPENGL_API_PREFIX_ glNormalP3ui
#endif

#ifndef glNormalP3uiv
#define glNormalP3uiv _OPENGL_API_PREFIX_ glNormalP3uiv
#endif

#ifndef glPixelStoref
#define glPixelStoref _OPENGL_API_PREFIX_ glPixelStoref
#endif

#ifndef glPixelStorei
#define glPixelStorei _OPENGL_API_PREFIX_ glPixelStorei
#endif

#ifndef glPointParameterf
#define glPointParameterf _OPENGL_API_PREFIX_ glPointParameterf
#endif

#ifndef glPointParameterfv
#define glPointParameterfv _OPENGL_API_PREFIX_ glPointParameterfv
#endif

#ifndef glPointParameteri
#define glPointParameteri _OPENGL_API_PREFIX_ glPointParameteri
#endif

#ifndef glPointParameteriv
#define glPointParameteriv _OPENGL_API_PREFIX_ glPointParameteriv
#endif

#ifndef glPointSize
#define glPointSize _OPENGL_API_PREFIX_ glPointSize
#endif

#ifndef glPolygonMode
#define glPolygonMode _OPENGL_API_PREFIX_ glPolygonMode
#endif

#ifndef glPolygonOffset
#define glPolygonOffset _OPENGL_API_PREFIX_ glPolygonOffset
#endif

#ifndef glPrimitiveRestartIndex
#define glPrimitiveRestartIndex _OPENGL_API_PREFIX_ glPrimitiveRestartIndex
#endif

#ifndef glProvokingVertex
#define glProvokingVertex _OPENGL_API_PREFIX_ glProvokingVertex
#endif

#ifndef glQueryCounter
#define glQueryCounter _OPENGL_API_PREFIX_ glQueryCounter
#endif

#ifndef glReadBuffer
#define glReadBuffer _OPENGL_API_PREFIX_ glReadBuffer
#endif

#ifndef glReadPixels
#define glReadPixels _OPENGL_API_PREFIX_ glReadPixels
#endif

#ifndef glRenderbufferStorage
#define glRenderbufferStorage _OPENGL_API_PREFIX_ glRenderbufferStorage
#endif

#ifndef glRenderbufferStorageMultisample
#define glRenderbufferStorageMultisample _OPENGL_API_PREFIX_ glRenderbufferStorageMultisample
#endif

#ifndef glSampleCoverage
#define glSampleCoverage _OPENGL_API_PREFIX_ glSampleCoverage
#endif

#ifndef glSampleMaski
#define glSampleMaski _OPENGL_API_PREFIX_ glSampleMaski
#endif

#ifndef glSamplerParameterIiv
#define glSamplerParameterIiv _OPENGL_API_PREFIX_ glSamplerParameterIiv
#endif

#ifndef glSamplerParameterIuiv
#define glSamplerParameterIuiv _OPENGL_API_PREFIX_ glSamplerParameterIuiv
#endif

#ifndef glSamplerParameterf
#define glSamplerParameterf _OPENGL_API_PREFIX_ glSamplerParameterf
#endif

#ifndef glSamplerParameterfv
#define glSamplerParameterfv _OPENGL_API_PREFIX_ glSamplerParameterfv
#endif

#ifndef glSamplerParameteri
#define glSamplerParameteri _OPENGL_API_PREFIX_ glSamplerParameteri
#endif

#ifndef glSamplerParameteriv
#define glSamplerParameteriv _OPENGL_API_PREFIX_ glSamplerParameteriv
#endif

#ifndef glScissor
#define glScissor _OPENGL_API_PREFIX_ glScissor
#endif

#ifndef glSecondaryColorP3ui
#define glSecondaryColorP3ui _OPENGL_API_PREFIX_ glSecondaryColorP3ui
#endif

#ifndef glSecondaryColorP3uiv
#define glSecondaryColorP3uiv _OPENGL_API_PREFIX_ glSecondaryColorP3uiv
#endif

#ifndef glShaderSource
#define glShaderSource _OPENGL_API_PREFIX_ glShaderSource
#endif

#ifndef glStencilFunc
#define glStencilFunc _OPENGL_API_PREFIX_ glStencilFunc
#endif

#ifndef glStencilFuncSeparate
#define glStencilFuncSeparate _OPENGL_API_PREFIX_ glStencilFuncSeparate
#endif

#ifndef glStencilMask
#define glStencilMask _OPENGL_API_PREFIX_ glStencilMask
#endif

#ifndef glStencilMaskSeparate
#define glStencilMaskSeparate _OPENGL_API_PREFIX_ glStencilMaskSeparate
#endif

#ifndef glStencilOp
#define glStencilOp _OPENGL_API_PREFIX_ glStencilOp
#endif

#ifndef glStencilOpSeparate
#define glStencilOpSeparate _OPENGL_API_PREFIX_ glStencilOpSeparate
#endif

#ifndef glTexBuffer
#define glTexBuffer _OPENGL_API_PREFIX_ glTexBuffer
#endif

#ifndef glTexCoordP1ui
#define glTexCoordP1ui _OPENGL_API_PREFIX_ glTexCoordP1ui
#endif

#ifndef glTexCoordP1uiv
#define glTexCoordP1uiv _OPENGL_API_PREFIX_ glTexCoordP1uiv
#endif

#ifndef glTexCoordP2ui
#define glTexCoordP2ui _OPENGL_API_PREFIX_ glTexCoordP2ui
#endif

#ifndef glTexCoordP2uiv
#define glTexCoordP2uiv _OPENGL_API_PREFIX_ glTexCoordP2uiv
#endif

#ifndef glTexCoordP3ui
#define glTexCoordP3ui _OPENGL_API_PREFIX_ glTexCoordP3ui
#endif

#ifndef glTexCoordP3uiv
#define glTexCoordP3uiv _OPENGL_API_PREFIX_ glTexCoordP3uiv
#endif

#ifndef glTexCoordP4ui
#define glTexCoordP4ui _OPENGL_API_PREFIX_ glTexCoordP4ui
#endif

#ifndef glTexCoordP4uiv
#define glTexCoordP4uiv _OPENGL_API_PREFIX_ glTexCoordP4uiv
#endif

#ifndef glTexImage1D
#define glTexImage1D _OPENGL_API_PREFIX_ glTexImage1D
#endif

#ifndef glTexImage2D
#define glTexImage2D _OPENGL_API_PREFIX_ glTexImage2D
#endif

#ifndef glTexImage2DMultisample
#define glTexImage2DMultisample _OPENGL_API_PREFIX_ glTexImage2DMultisample
#endif

#ifndef glTexImage3D
#define glTexImage3D _OPENGL_API_PREFIX_ glTexImage3D
#endif

#ifndef glTexImage3DMultisample
#define glTexImage3DMultisample _OPENGL_API_PREFIX_ glTexImage3DMultisample
#endif

#ifndef glTexParameterIiv
#define glTexParameterIiv _OPENGL_API_PREFIX_ glTexParameterIiv
#endif

#ifndef glTexParameterIuiv
#define glTexParameterIuiv _OPENGL_API_PREFIX_ glTexParameterIuiv
#endif

#ifndef glTexParameterf
#define glTexParameterf _OPENGL_API_PREFIX_ glTexParameterf
#endif

#ifndef glTexParameterfv
#define glTexParameterfv _OPENGL_API_PREFIX_ glTexParameterfv
#endif

#ifndef glTexParameteri
#define glTexParameteri _OPENGL_API_PREFIX_ glTexParameteri
#endif

#ifndef glTexParameteriv
#define glTexParameteriv _OPENGL_API_PREFIX_ glTexParameteriv
#endif

#ifndef glTexSubImage1D
#define glTexSubImage1D _OPENGL_API_PREFIX_ glTexSubImage1D
#endif

#ifndef glTexSubImage2D
#define glTexSubImage2D _OPENGL_API_PREFIX_ glTexSubImage2D
#endif

#ifndef glTexSubImage3D
#define glTexSubImage3D _OPENGL_API_PREFIX_ glTexSubImage3D
#endif

#ifndef glTransformFeedbackVaryings
#define glTransformFeedbackVaryings _OPENGL_API_PREFIX_ glTransformFeedbackVaryings
#endif

#ifndef glUniform1f
#define glUniform1f _OPENGL_API_PREFIX_ glUniform1f
#endif

#ifndef glUniform1fv
#define glUniform1fv _OPENGL_API_PREFIX_ glUniform1fv
#endif

#ifndef glUniform1i
#define glUniform1i _OPENGL_API_PREFIX_ glUniform1i
#endif

#ifndef glUniform1iv
#define glUniform1iv _OPENGL_API_PREFIX_ glUniform1iv
#endif

#ifndef glUniform1ui
#define glUniform1ui _OPENGL_API_PREFIX_ glUniform1ui
#endif

#ifndef glUniform1uiv
#define glUniform1uiv _OPENGL_API_PREFIX_ glUniform1uiv
#endif

#ifndef glUniform2f
#define glUniform2f _OPENGL_API_PREFIX_ glUniform2f
#endif

#ifndef glUniform2fv
#define glUniform2fv _OPENGL_API_PREFIX_ glUniform2fv
#endif

#ifndef glUniform2i
#define glUniform2i _OPENGL_API_PREFIX_ glUniform2i
#endif

#ifndef glUniform2iv
#define glUniform2iv _OPENGL_API_PREFIX_ glUniform2iv
#endif

#ifndef glUniform2ui
#define glUniform2ui _OPENGL_API_PREFIX_ glUniform2ui
#endif

#ifndef glUniform2uiv
#define glUniform2uiv _OPENGL_API_PREFIX_ glUniform2uiv
#endif

#ifndef glUniform3f
#define glUniform3f _OPENGL_API_PREFIX_ glUniform3f
#endif

#ifndef glUniform3fv
#define glUniform3fv _OPENGL_API_PREFIX_ glUniform3fv
#endif

#ifndef glUniform3i
#define glUniform3i _OPENGL_API_PREFIX_ glUniform3i
#endif

#ifndef glUniform3iv
#define glUniform3iv _OPENGL_API_PREFIX_ glUniform3iv
#endif

#ifndef glUniform3ui
#define glUniform3ui _OPENGL_API_PREFIX_ glUniform3ui
#endif

#ifndef glUniform3uiv
#define glUniform3uiv _OPENGL_API_PREFIX_ glUniform3uiv
#endif

#ifndef glUniform4f
#define glUniform4f _OPENGL_API_PREFIX_ glUniform4f
#endif

#ifndef glUniform4fv
#define glUniform4fv _OPENGL_API_PREFIX_ glUniform4fv
#endif

#ifndef glUniform4i
#define glUniform4i _OPENGL_API_PREFIX_ glUniform4i
#endif

#ifndef glUniform4iv
#define glUniform4iv _OPENGL_API_PREFIX_ glUniform4iv
#endif

#ifndef glUniform4ui
#define glUniform4ui _OPENGL_API_PREFIX_ glUniform4ui
#endif

#ifndef glUniform4uiv
#define glUniform4uiv _OPENGL_API_PREFIX_ glUniform4uiv
#endif

#ifndef glUniformBlockBinding
#define glUniformBlockBinding _OPENGL_API_PREFIX_ glUniformBlockBinding
#endif

#ifndef glUniformMatrix2fv
#define glUniformMatrix2fv _OPENGL_API_PREFIX_ glUniformMatrix2fv
#endif

#ifndef glUniformMatrix2x3fv
#define glUniformMatrix2x3fv _OPENGL_API_PREFIX_ glUniformMatrix2x3fv
#endif

#ifndef glUniformMatrix2x4fv
#define glUniformMatrix2x4fv _OPENGL_API_PREFIX_ glUniformMatrix2x4fv
#endif

#ifndef glUniformMatrix3fv
#define glUniformMatrix3fv _OPENGL_API_PREFIX_ glUniformMatrix3fv
#endif

#ifndef glUniformMatrix3x2fv
#define glUniformMatrix3x2fv _OPENGL_API_PREFIX_ glUniformMatrix3x2fv
#endif

#ifndef glUniformMatrix3x4fv
#define glUniformMatrix3x4fv _OPENGL_API_PREFIX_ glUniformMatrix3x4fv
#endif

#ifndef glUniformMatrix4fv
#define glUniformMatrix4fv _OPENGL_API_PREFIX_ glUniformMatrix4fv
#endif

#ifndef glUniformMatrix4x2fv
#define glUniformMatrix4x2fv _OPENGL_API_PREFIX_ glUniformMatrix4x2fv
#endif

#ifndef glUniformMatrix4x3fv
#define glUniformMatrix4x3fv _OPENGL_API_PREFIX_ glUniformMatrix4x3fv
#endif

#ifndef glUnmapBuffer
#define glUnmapBuffer _OPENGL_API_PREFIX_ glUnmapBuffer
#endif

#ifndef glUseProgram
#define glUseProgram _OPENGL_API_PREFIX_ glUseProgram
#endif

#ifndef glValidateProgram
#define glValidateProgram _OPENGL_API_PREFIX_ glValidateProgram
#endif

#ifndef glVertexAttribDivisor
#define glVertexAttribDivisor _OPENGL_API_PREFIX_ glVertexAttribDivisor
#endif

#ifndef glVertexAttribIPointer
#define glVertexAttribIPointer _OPENGL_API_PREFIX_ glVertexAttribIPointer
#endif

#ifndef glVertexAttribP1ui
#define glVertexAttribP1ui _OPENGL_API_PREFIX_ glVertexAttribP1ui
#endif

#ifndef glVertexAttribP1uiv
#define glVertexAttribP1uiv _OPENGL_API_PREFIX_ glVertexAttribP1uiv
#endif

#ifndef glVertexAttribP2ui
#define glVertexAttribP2ui _OPENGL_API_PREFIX_ glVertexAttribP2ui
#endif

#ifndef glVertexAttribP2uiv
#define glVertexAttribP2uiv _OPENGL_API_PREFIX_ glVertexAttribP2uiv
#endif

#ifndef glVertexAttribP3ui
#define glVertexAttribP3ui _OPENGL_API_PREFIX_ glVertexAttribP3ui
#endif

#ifndef glVertexAttribP3uiv
#define glVertexAttribP3uiv _OPENGL_API_PREFIX_ glVertexAttribP3uiv
#endif

#ifndef glVertexAttribP4ui
#define glVertexAttribP4ui _OPENGL_API_PREFIX_ glVertexAttribP4ui
#endif

#ifndef glVertexAttribP4uiv
#define glVertexAttribP4uiv _OPENGL_API_PREFIX_ glVertexAttribP4uiv
#endif

#ifndef glVertexAttribPointer
#define glVertexAttribPointer _OPENGL_API_PREFIX_ glVertexAttribPointer
#endif

#ifndef glVertexP2ui
#define glVertexP2ui _OPENGL_API_PREFIX_ glVertexP2ui
#endif

#ifndef glVertexP2uiv
#define glVertexP2uiv _OPENGL_API_PREFIX_ glVertexP2uiv
#endif

#ifndef glVertexP3ui
#define glVertexP3ui _OPENGL_API_PREFIX_ glVertexP3ui
#endif

#ifndef glVertexP3uiv
#define glVertexP3uiv _OPENGL_API_PREFIX_ glVertexP3uiv
#endif

#ifndef glVertexP4ui
#define glVertexP4ui _OPENGL_API_PREFIX_ glVertexP4ui
#endif

#ifndef glVertexP4uiv
#define glVertexP4uiv _OPENGL_API_PREFIX_ glVertexP4uiv
#endif

#ifndef glViewport
#define glViewport _OPENGL_API_PREFIX_ glViewport
#endif

#ifndef glWaitSync
#endif
#define glWaitSync _OPENGL_API_PREFIX_ glWaitSync
