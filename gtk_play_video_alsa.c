#define _GNU_SOURCE

#include <bcm_host.h>

#include <gtk/gtk.h>
//#include <gdk/gdkx.h>

#include <libavcodec/avcodec.h>
#include <libavutil/pixfmt.h>
#include <libavutil/avutil.h>
#include <libavutil/samplefmt.h>
#include <libavutil/opt.h>
#include <libavformat/avformat.h>
#include <libavresample/avresample.h>
#include <libswscale/swscale.h>
#include "libswresample/swresample.h"
#include <alsa/asoundlib.h>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <assert.h>
#include <math.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

GtkWidget *window;
GdkPixbuf *pixbuf;

/* FFMpeg vbls */
AVFormatContext *pFormatCtx = NULL;
AVCodecContext *pCodecCtx = NULL;
int videoStream;
AVCodecContext *pCodecCtxA = NULL;
int audioStream;
struct SwsContext *sws_ctx = NULL;
AVCodec *pCodec = NULL;
AVCodec *pCodecA = NULL;

SwrContext *swr;

#define idle 0
#define playing 1
#define paused 2
#define draining 3

int playerstatus = idle;

pthread_t tid[3];
int retval_video, retval_audio, retval_readframes;
cpu_set_t cpu[4];

int64_t videoduration;
long long usecs; // microseconds
long long usecs2; // microseconds

int frametime;
//long diff1, diff2, diff3, diff4, diff5, diff6;

char *now_playing = NULL;

typedef struct
{
    // Handle to a program object
   GLuint programObject;

   // Attribute locations
   GLint  positionLoc;
   GLint  texCoordLoc;

   // Sampler location
   GLint samplerLoc;
   
   // Image size vector location vec2(WIDTH, HEIGHT)
   GLint sizeLoc;

   // Input texture
   GLuint tex;

   // YUV->RGB conversion matrix
   GLuint cmatrixLoc;

	char *outrgb;
} UserData;

typedef struct
{
    uint32_t screen_width;
    uint32_t screen_height;

    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;

	UserData *user_data;

} CUBE_STATE_T;

CUBE_STATE_T state, *p_state = &state;

GLfloat vVertices[] = { -1.0f,  1.0f, 0.0f, // Position 0
                        -1.0f, -1.0f, 0.0f, // Position 1
                         1.0f, -1.0f, 0.0f, // Position 2
                         1.0f,  1.0f, 0.0f  // Position 3
                      };

GLfloat tVertices[] = {  0.0f,  0.0f,        // TexCoord 0 
                         0.0f,  1.0f,        // TexCoord 1
                         1.0f,  1.0f,        // TexCoord 2
                         1.0f,  0.0f         // TexCoord 3
                      };

GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

void print_current_time_with_ms (void)
{
    long            ms; // Milliseconds
    time_t          s;  // Seconds
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    s  = spec.tv_sec;
    ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds

    printf("Current time: %"PRIdMAX".%03ld seconds since the Epoch\n",
           (intmax_t)s, ms);
}

long get_first_time_microseconds()
{
	long long micros;
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

	micros = spec.tv_sec * 1.0e6 + round(spec.tv_nsec / 1000); // Convert nanoseconds to microseconds
	usecs = micros;
	return(0L);
}

long get_next_time_microseconds()
{
    long delta;
    long long micros;
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    micros = spec.tv_sec * 1.0e6 + round(spec.tv_nsec / 1000); // Convert nanoseconds to microseconds
    delta = micros - usecs;
    usecs = micros;
    return(delta);
}

long get_first_time_microseconds_2()
{
	long long micros;
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

	micros = spec.tv_sec * 1.0e6 + round(spec.tv_nsec / 1000); // Convert nanoseconds to microseconds
	usecs2 = micros;
	return(0L);
}

long get_next_time_microseconds_2()
{
    long delta;
    long long micros;
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    micros = spec.tv_sec * 1.0e6 + round(spec.tv_nsec / 1000); // Convert nanoseconds to microseconds
    delta = micros - usecs2;
    usecs2 = micros;
    return(delta);
}

void checkNoGLES2Error() 
{
    int error;
    error = glGetError();
    if (error != GL_NO_ERROR)
    {
		printf("GLES20 error: %d", error);
	}
}
  
void init_ogl(CUBE_STATE_T *state)
{
    int32_t success = 0;
    EGLBoolean result;
    EGLint num_config;

    static EGL_DISPMANX_WINDOW_T nativewindow;

    DISPMANX_ELEMENT_HANDLE_T dispman_element;
    DISPMANX_DISPLAY_HANDLE_T dispman_display;
    DISPMANX_UPDATE_HANDLE_T dispman_update;
    VC_RECT_T dst_rect;
    VC_RECT_T src_rect;

    static const EGLint attribute_list[] =
	{
	    EGL_RED_SIZE, 8,
	    EGL_GREEN_SIZE, 8,
	    EGL_BLUE_SIZE, 8,
	    EGL_ALPHA_SIZE, 8,
	    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
	    EGL_NONE
	};

    static const EGLint context_attributes[] =
	{
	    EGL_CONTEXT_CLIENT_VERSION, 2,
	    EGL_NONE
	};

    EGLConfig config;

    // get an EGL display connection
    p_state->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    // initialize the EGL display connection
    result = eglInitialize(p_state->display, NULL, NULL);

    // get an appropriate EGL frame buffer configuration
    result = eglChooseConfig(p_state->display, attribute_list, &config, 1, &num_config);
    assert(EGL_FALSE != result);

    // get an appropriate EGL frame buffer configuration
    result = eglBindAPI(EGL_OPENGL_ES_API);
    assert(EGL_FALSE != result);

    // create an EGL rendering context
    p_state->context = eglCreateContext(p_state->display, config, EGL_NO_CONTEXT, context_attributes);
    assert(p_state->context!=EGL_NO_CONTEXT);

    // create an EGL window surface
    success = graphics_get_display_size(0 /* LCD */, &p_state->screen_width, &p_state->screen_height);
    assert( success >= 0 );

    //printf("Screen size = %d * %d\n", p_state->screen_width, p_state->screen_height);

    //p_state->screen_width = pCodecCtx->width;
    //p_state->screen_height = pCodecCtx->height;
    p_state->screen_width = 1280;
    p_state->screen_height = 720;

	printf("Screen size = %d * %d\n", pCodecCtx->width, pCodecCtx->height);

    dst_rect.x = 0;
    dst_rect.y = 0;

    dst_rect.width = p_state->screen_width;
    dst_rect.height = p_state->screen_height;

    src_rect.x = 0;
    src_rect.y = 0;

    src_rect.width = p_state->screen_width << 16;
    src_rect.height = p_state->screen_height << 16;


    dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
    dispman_update = vc_dispmanx_update_start( 0 );

    dispman_element = 
	vc_dispmanx_element_add(dispman_update, dispman_display,
				0/*layer*/, &dst_rect, 0/*src*/,
				&src_rect, DISPMANX_PROTECTION_NONE, 
				0 /*alpha*/, 0/*clamp*/, 0/*transform*/);

    nativewindow.element = dispman_element;
    nativewindow.width = p_state->screen_width;
    nativewindow.height = p_state->screen_height;
    vc_dispmanx_update_submit_sync( dispman_update );

    state->surface = eglCreateWindowSurface( p_state->display, config, &nativewindow, NULL );
    assert(p_state->surface != EGL_NO_SURFACE);

    // connect the context to the surface
    result = eglMakeCurrent(p_state->display, p_state->surface, p_state->surface, p_state->context);
    assert(EGL_FALSE != result);
}

void exit_func(void)
// Function to be passed to atexit().
{
	UserData *userData = p_state->user_data;

   // clear screen
   glClear( GL_COLOR_BUFFER_BIT );
   eglSwapBuffers(p_state->display, p_state->surface);

	// Delete allocated objects
	glDeleteProgram(userData->programObject);
	glDeleteTextures(1, &userData->tex);

   // Release OpenGL resources
   eglMakeCurrent( p_state->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
   eglDestroySurface( p_state->display, p_state->surface );
   eglDestroyContext( p_state->display, p_state->context );
   eglTerminate( p_state->display );

   // release texture buffers
	//free(p_state->user_data->rgba);

   //printf("\nclosed\n");
}

// Create a shader object, load the shader source, and
// compile the shader.
//
GLuint LoadShader(GLenum type, const char *shaderSrc)
{
    GLuint shader;
    GLint compiled;
    // Create the shader object
    shader = glCreateShader(type);
    if(shader == 0)
    return 0;
    // Load the shader source
    glShaderSource(shader, 1, &shaderSrc, NULL);
    // Compile the shader
    glCompileShader(shader);
    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if(!compiled)
    {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if(infoLen > 1)
        {
            char* infoLog = malloc(sizeof(char) * infoLen);
            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            fprintf(stderr, "Error compiling shader:\n%s\n", infoLog);
            free(infoLog);
        }
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint LoadProgram ( const char *vertShaderSrc, const char *fragShaderSrc )
{
   GLuint vertexShader;
   GLuint fragmentShader;
   GLuint programObject;
   GLint linked;

   // Load the vertex/fragment shaders
   vertexShader = LoadShader ( GL_VERTEX_SHADER, vertShaderSrc );
   if ( vertexShader == 0 )
      return 0;

   fragmentShader = LoadShader ( GL_FRAGMENT_SHADER, fragShaderSrc );
   if ( fragmentShader == 0 )
   {
      glDeleteShader( vertexShader );
      return 0;
   }

   // Create the program object
   programObject = glCreateProgram ( );

   if ( programObject == 0 )
      return 0;

   glAttachShader ( programObject, vertexShader );
   glAttachShader ( programObject, fragmentShader );

   // Link the program
   glLinkProgram ( programObject );

   // Check the link status
   glGetProgramiv ( programObject, GL_LINK_STATUS, &linked );

   if ( !linked ) 
   {
      GLint infoLen = 0;
      glGetProgramiv ( programObject, GL_INFO_LOG_LENGTH, &infoLen );

      if ( infoLen > 1 )
      {
         char* infoLog = malloc (sizeof(char) * infoLen );

         glGetProgramInfoLog ( programObject, infoLen, NULL, infoLog );
         fprintf (stderr, "Error linking program:\n%s\n", infoLog );            

         free ( infoLog );
      }

      glDeleteProgram ( programObject );
      return 0;
   }

   // Free up no longer needed shader resources
   glDeleteShader ( vertexShader );
   glDeleteShader ( fragmentShader );

   return programObject;
}

//
// Initialize the shader and program object
//
int Init(CUBE_STATE_T *p_state)
{
   p_state->user_data = malloc(sizeof(UserData));      
   UserData *userData = p_state->user_data;

   GLbyte vShaderStr[] =  
      "attribute vec4 a_position;   \n"
      "attribute vec2 a_texCoord;   \n"
      "varying vec2 v_texCoord;     \n"
      "void main()                  \n"
      "{                            \n"
      "   gl_Position = a_position; \n"
      "   v_texCoord = a_texCoord;  \n"
      "}                            \n";

/*
   GLbyte fShaderStr_stpq[] =  
      "precision mediump float;                              \n"
      "varying vec2 v_texCoord;                              \n"
      "uniform vec2 texSize;                                 \n"
      "uniform sampler2D texture;                            \n"
      "void main()                                           \n"
      "{                                                     \n"
      "  float y, u, v, r, g, b;                             \n"
      "  float yx, yy, ux, uy, vx, vy;                       \n"

      "  vec2 texCoord=vec2(v_texCoord.x*texSize.x, v_texCoord.y*texSize.y);\n"
      "  float oe=floor(mod(texCoord.y/2.0, 2.0));           \n"

      "  yx=v_texCoord.x;                                    \n"
      "  yy=v_texCoord.y*2.0/3.0;                            \n"
      "  ux=oe*0.5+v_texCoord.x/2.0;                         \n"
      "  uy=4.0/6.0+v_texCoord.y/6.0;                        \n"
      "  vx=ux;                                              \n"
      "  vy=5.0/6.0+v_texCoord.y/6.0;                        \n"

      "  int x=int(mod(texCoord.x, 8.0));                    \n"

      "  vec4 y4 = vec4(float((x==0)||(x==4)), float((x==1)||(x==5)), float((x==2)||(x==6)), float((x==3)||(x==7))); \n"
      "  vec4 uv4 = vec4(float((x==0)||(x==1)), float((x==2)||(x==3)), float((x==4)||(x==5)), float((x==6)||(x==7))); \n"
      "  y=dot(y4,  texture2D(texture, vec2(yx, yy))); \n"
      "  u=dot(uv4, texture2D(texture, vec2(ux, uy))); \n"
      "  v=dot(uv4, texture2D(texture, vec2(vx, vy))); \n"

      "  y=1.1643*(y-0.0625);                                \n"
      "  u=u-0.5;                                            \n"
      "  v=v-0.5;                                            \n"
      "  r=y+1.5958*v;                                       \n"
      "  g=y-0.3917*u-0.8129*v;                              \n"
      "  b=y+2.017*u;                                        \n"

      "  gl_FragColor=vec4(r, g, b, 1.0);                    \n"
      "}                                                     \n";
*/

/*
   GLbyte fShaderStr_stpq[] =  
      "precision mediump float;                              \n"
      "varying vec2 v_texCoord;                              \n"
      "uniform vec2 texSize;                                 \n"
      "uniform sampler2D texture;                            \n"
      "void main()                                           \n"
      "{                                                     \n"
      "  float y, u, v, r, g, b;                             \n"
      "  float yx, yy, ux, uy, vx, vy;                       \n"

      "  vec2 texCoord=vec2(v_texCoord.x*texSize.x, v_texCoord.y*texSize.y);\n"
      "  float oe=floor(mod(texCoord.y/2.0, 2.0));           \n"

      "  yx=v_texCoord.x;                                    \n"
      "  yy=v_texCoord.y*2.0/3.0;                            \n"
      "  ux=oe*0.5+v_texCoord.x/2.0;                         \n"
      "  uy=4.0/6.0+v_texCoord.y/6.0;                        \n"
      "  vx=ux;                                              \n"
      "  vy=5.0/6.0+v_texCoord.y/6.0;                        \n"

      "  int x=int(mod(texCoord.x, 8.0));                    \n"

      "  vec4 y4 = vec4(float((x==0)||(x==4)), float((x==1)||(x==5)), float((x==2)||(x==6)), float((x==3)||(x==7))); \n"
      "  vec4 uv4 = vec4(float((x==0)||(x==1)), float((x==2)||(x==3)), float((x==4)||(x==5)), float((x==6)||(x==7))); \n"
      "  y=dot(y4,  texture2D(texture, vec2(yx, yy))); \n"
      "  u=dot(uv4, texture2D(texture, vec2(ux, uy))); \n"
      "  v=dot(uv4, texture2D(texture, vec2(vx, vy))); \n"

      "  vec3 yuv=vec3(1.1643*(y-0.0625), u-0.5, v-0.5);     \n"
      "  vec3 rgb=yuv*mat3(vec3(1.0,  0.0,     1.5958),      \n"
      "                    vec3(1.0, -0.3917, -0.8129),      \n"
      "                    vec3(1.0,  2.017,   0.0   ));     \n"

      "  gl_FragColor=vec4(rgb, 1.0);                        \n"
      "}                                                     \n";
*/

   GLbyte fShaderStr_stpq[] =  
      "precision mediump float;                              \n"
      "varying vec2 v_texCoord;                              \n"
      "uniform vec2 texSize;                                 \n"
      "uniform mat3 yuv2rgb;                                 \n"
      "uniform sampler2D texture;                            \n"
      "void main()                                           \n"
      "{                                                     \n"
      "  float y, u, v, r, g, b;                             \n"
      "  float yx, yy, ux, uy, vx, vy;                       \n"

      "  vec2 texCoord=vec2(v_texCoord.x*texSize.x, v_texCoord.y*texSize.y);\n"
      "  float oe=floor(mod(texCoord.y/2.0, 2.0));           \n"

      "  yx=v_texCoord.x;                                    \n"
      "  yy=v_texCoord.y*2.0/3.0;                            \n"
      "  ux=oe*0.5+v_texCoord.x/2.0;                         \n"
      "  uy=4.0/6.0+v_texCoord.y/6.0;                        \n"
      "  vx=ux;                                              \n"
      "  vy=5.0/6.0+v_texCoord.y/6.0;                        \n"

      "  int x=int(mod(texCoord.x, 8.0));                    \n"

      "  vec4 y4 = vec4(float((x==0)||(x==4)), float((x==1)||(x==5)), float((x==2)||(x==6)), float((x==3)||(x==7))); \n"
      "  vec4 uv4 = vec4(float((x==0)||(x==1)), float((x==2)||(x==3)), float((x==4)||(x==5)), float((x==6)||(x==7))); \n"
      "  y=dot(y4,  texture2D(texture, vec2(yx, yy))); \n"
      "  u=dot(uv4, texture2D(texture, vec2(ux, uy))); \n"
      "  v=dot(uv4, texture2D(texture, vec2(vx, vy))); \n"

      "  vec3 yuv=vec3(1.1643*(y-0.0625), u-0.5, v-0.5);     \n"
      "  vec3 rgb=yuv*yuv2rgb;                               \n"

      "  gl_FragColor=vec4(rgb, 1.0);                        \n"
      "}                                                     \n";

   // Load the shaders and get a linked program object
   userData->programObject = LoadProgram((char *)vShaderStr, (char *)fShaderStr_stpq );
   if (!userData->programObject)
   {
     printf("Load Program %d\n",userData->programObject);
     return GL_FALSE;
   }

   userData->samplerLoc = glGetUniformLocation ( userData->programObject, "texture" );

   // Get the attribute locations
   userData->positionLoc = glGetAttribLocation ( userData->programObject, "a_position" ); // Query only
   userData->texCoordLoc = glGetAttribLocation ( userData->programObject, "a_texCoord" );
   
   userData->sizeLoc = glGetUniformLocation ( userData->programObject, "texSize" );
   userData->cmatrixLoc = glGetUniformLocation ( userData->programObject, "yuv2rgb" );
   
   // Use the program object
   glUseProgram ( userData->programObject ); 

   // Load the vertex position
   glVertexAttribPointer ( userData->positionLoc, 3, GL_FLOAT, 
                           GL_FALSE, 3 * sizeof(GLfloat), vVertices );
                           
   // Load the texture coordinate
   glVertexAttribPointer ( userData->texCoordLoc, 2, GL_FLOAT,
                           GL_FALSE, 2 * sizeof(GLfloat), tVertices );

   glEnableVertexAttribArray ( userData->positionLoc );
   glEnableVertexAttribArray ( userData->texCoordLoc );
   
   glUniform1i ( userData->samplerLoc, 0 );

   glClearColor ( 0.0f, 0.0f, 0.0f, 1.0f );
   return GL_TRUE;
}

void setSize(int width, int height)
{
	UserData *userData = p_state->user_data;
	int w, h;

	w = width;
	h = height;
	glGenTextures(1, &userData->tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, userData->tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLfloat)GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLfloat)GL_CLAMP_TO_EDGE);
}

void texImage2D(char* buf, int width, int height)
{
   UserData *userData = p_state->user_data;

   int w, h;

   w = width;
   h = height;
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, userData->tex);
   //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
   glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, buf);
   
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLfloat)GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLfloat)GL_CLAMP_TO_EDGE);
   glUniform1i ( userData->samplerLoc, 0 );
    //checkNoGLES2Error();
}

void redraw_scene(CUBE_STATE_T *state)
{
   // Set the viewport
   glViewport ( 0, 0, state->screen_width, state->screen_height);
   // Clear the color buffer
   glClear ( GL_COLOR_BUFFER_BIT );
   glDrawElements ( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );
   eglSwapBuffers(p_state->display, p_state->surface);
}

// Audio
struct audioqueue
{
	struct audioqueue *prev;
	struct audioqueue *next;
	AVPacket *packet;
	uint8_t **dst_data;
	int dst_bufsize;
	int64_t label;
};
struct audioqueue *aq;
int aqLength;
const int aqMaxLength = 20;
pthread_mutex_t aqmutex; // = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t aqlowcond; // = PTHREAD_COND_INITIALIZER;
pthread_cond_t aqhighcond; // = PTHREAD_COND_INITIALIZER;

void aq_init(struct audioqueue **q, pthread_mutex_t *m, pthread_cond_t *cl, pthread_cond_t *ch)
{
	int ret;

	*q = NULL;
	aqLength = 0;

	if((ret=pthread_mutex_init(m, NULL))!=0 )
		printf("mutex init failed, %d\n", ret);

	if((ret=pthread_cond_init(ch, NULL))!=0 )
		printf("cond init failed, %d\n", ret);

	if((ret=pthread_cond_init(cl, NULL))!=0 )
		printf("cond init failed, %d\n", ret);
}

void aq_add(struct audioqueue **q,  AVPacket *packet, int64_t label)
{
	struct audioqueue *p;

	pthread_mutex_lock(&aqmutex);
	while (aqLength>=aqMaxLength)
	{
		//printf("Audio queue sleeping, overrun\n");
		pthread_cond_wait(&aqhighcond, &aqmutex);
	}

	p = malloc(sizeof(struct audioqueue));
//printf("malloc aq %d\n", sizeof(struct audioqueue));
	if (*q == NULL)
	{
		p->next = p;
		p->prev = p;
		*q = p;
	}
	else
	{
		p->next = *q;
		p->prev = (*q)->prev;
		(*q)->prev = p;
		p->prev->next = p;
	}
	p->packet = packet;
	p->label = label;
	p->dst_bufsize = 0;
	p->dst_data = NULL;
	aqLength++;

	int frameFinished = 0;
	AVFrame *pFrame = NULL;
	int line_size;
	int ret;

	pFrame=av_frame_alloc();
//printf("av_frame_alloc\n");
	if ((ret = avcodec_decode_audio4(pCodecCtxA, pFrame, &frameFinished, packet)) < 0)
		printf("Error decoding audio frame\n");
	if (frameFinished)
	{
		ret = av_samples_alloc_array_and_samples(&(p->dst_data), &line_size, pCodecCtxA->channels, pFrame->nb_samples, AV_SAMPLE_FMT_S16, 0);
//printf("av_samples_alloc_array_and_samples, %d\n", ret);
		ret = swr_convert(swr, p->dst_data, pFrame->nb_samples, (const uint8_t **)pFrame->extended_data, pFrame->nb_samples);
		if (ret<0) printf("swr_convert error %d\n", ret);
		p->dst_bufsize = av_samples_get_buffer_size(&line_size, pCodecCtxA->channels, ret, AV_SAMPLE_FMT_S16, 1);
	}
	av_frame_free(&pFrame);
//printf("av_frame_free");

	//condition = true;
	pthread_cond_signal(&aqlowcond); // Should wake up *one* thread
	pthread_mutex_unlock(&aqmutex);
}

struct audioqueue* aq_remove_element(struct audioqueue **q)
{
	struct audioqueue *p;

	if ((*q)->next == (*q))
	{
		p=*q;
		*q = NULL;
	}
	else
	{
		p = (*q);
		(*q) = (*q)->next;
		(*q)->prev = p->prev;
		(*q)->prev->next = (*q);
	}
	return p;
}

struct audioqueue* aq_remove(struct audioqueue **q)
{
	struct audioqueue *p;

	pthread_mutex_lock(&aqmutex);
	while((*q)==NULL) // queue empty
	{
		if (playerstatus==playing)
		{
		//printf("Audio queue sleeping, underrun\n");
		pthread_cond_wait(&aqlowcond, &aqmutex);
		}
		else
			break;
	}
	switch (playerstatus)
	{
		case playing:
			p = aq_remove_element(q);
			aqLength--;
			break;
		case draining:
			if (aqLength>0)
			{
				p = aq_remove_element(q);
				aqLength--;
			}
			else
				p=NULL;
			break;
		default:
			p = NULL;
			break;
	}

	//condition = true;
	pthread_cond_signal(&aqhighcond); // Should wake up *one* thread
	pthread_mutex_unlock(&aqmutex);

	return p;
}

/*
void aq_close(struct audioqueue **q, pthread_mutex_t *m, pthread_cond_t *cl, pthread_cond_t *ch)
{
	struct audioqueue *p;

	pthread_mutex_lock(&aqmutex);
	while(!q)
	{
		p = aq_remove_element(q);
		aqLength--;
		av_free_packet(p->packet);
		free(p);
	}
	pthread_mutex_destroy(m);
	pthread_cond_destroy(cl);
	pthread_cond_destroy(ch);
}
*/

void aq_drain(struct audioqueue **q)
{
	pthread_mutex_lock(&aqmutex);
	while (aqLength)
	{
		pthread_mutex_unlock(&aqmutex);
		usleep(100000); // 0.1s
//printf("aqLength=%d\n", aqLength);
		pthread_mutex_lock(&aqmutex);
	}
//printf("aqLength=0\n");
	pthread_cond_signal(&aqlowcond); // Should wake up *one* thread
	pthread_mutex_unlock(&aqmutex);
}

void aq_destroy(pthread_mutex_t *m, pthread_cond_t *cl, pthread_cond_t *ch)
{
	pthread_mutex_destroy(m);
	pthread_cond_destroy(cl);
	pthread_cond_destroy(ch);
}

snd_pcm_t *handle;
signed short *samples;
snd_pcm_channel_area_t *areas;
snd_output_t *output = NULL;
//char *device = "default";	// playback device
char *device = "plughw:0,0";	// playback device
unsigned int rate = 44100;		// stream rate
snd_pcm_format_t format = SND_PCM_FORMAT_S16;	// sample format
unsigned int channels = 2;		// count of channels
int resample = 1;	// enable alsa-lib resampling
static int period_event = 0;	// produce poll event after each period
snd_pcm_sframes_t buffer_size;
static snd_pcm_sframes_t period_size;
snd_pcm_hw_params_t *hwparams;
snd_pcm_sw_params_t *swparams;

const int persize = 1024;	// 
const int bufsize = 10240; // persize * 10; // 10 periods

void write_status(snd_pcm_state_t stat)
{
	switch(stat)
	{
		case SND_PCM_STATE_OPEN:
			printf("Open");
			break;
		case SND_PCM_STATE_SETUP:
			printf("Setup installed");
			break;
		case SND_PCM_STATE_PREPARED:
			printf("Ready to start");
			break;
		case SND_PCM_STATE_RUNNING:
			printf("Running");
			break;
		case SND_PCM_STATE_XRUN:
			printf("Stopped: underrun (playback) or overrun (capture) detected");
			break;
		case SND_PCM_STATE_DRAINING:
			printf("Draining: running (playback) or stopped (capture)");
			break;
		case SND_PCM_STATE_PAUSED:
			printf("Paused");
			break;
		case SND_PCM_STATE_SUSPENDED:
			printf("Hardware is suspended");
			break;
		case SND_PCM_STATE_DISCONNECTED:
			printf("Hardware is disconnected");
			break;
	}
	printf("\n");
}

/* Underrun and suspend recovery */
static int xrun_recovery(snd_pcm_t *handle, int err)
{
        if (err == -EPIPE) {    // under-run
                err = snd_pcm_prepare(handle);
                if (err < 0)
                        printf("Can't recovery from underrun, prepare failed: %s\n", snd_strerror(err));
                return 0;
        } else if (err == -ESTRPIPE) {
                while ((err = snd_pcm_resume(handle)) == -EAGAIN)
                        sleep(1);       /* wait until the suspend flag is released */
                if (err < 0) {
                        err = snd_pcm_prepare(handle);
                        if (err < 0)
                                printf("Can't recovery from suspend, prepare failed: %s\n", snd_strerror(err));
                }
                return 0;
        }
        return err;
}

int play_period(snd_pcm_t *handle, struct audioqueue *p)
{
	snd_pcm_sframes_t avail;
	int err;

	//write_status(snd_pcm_state(handle));
	avail = snd_pcm_avail_update(handle);
	if (avail);
	//printf("avail: %d\n", avail);
	//printf("audio playing %d\n", p->label);

	if (p->dst_data)
	{
	//printf("writing %d\n", dst_bufsize);
		//err = snd_pcm_writei(handle, p->dst_data[0], p->dst_bufsize/4); // snd_pcm_format_width(format)/8*channels
		err = snd_pcm_writei(handle, p->dst_data[0], p->dst_bufsize/(snd_pcm_format_width(format)/8*channels));
	//printf("snd_pcm_writei err:%d\n", err);
		av_freep(&(p->dst_data[0]));
//printf("av_freep dst_data0\n");

		av_freep(&(p->dst_data));
//printf("av_freep\n");
	}

	//av_free_packet(p->packet);
	av_packet_unref(p->packet);
//printf("av_packet_unref\n");
	free(p);

	if (err == -EAGAIN)
	{
		//continue;
		printf("EAGAIN\n");
		return err;
	}
	if (err < 0)
	{
		if (xrun_recovery(handle, err) < 0)
		{
			printf("Write error: %s\n", snd_strerror(err));
			//exit(EXIT_FAILURE);
		}
		//break;  // skip one period
	}
	return err;
}

static int write_loop(snd_pcm_t *handle, signed short *samples, snd_pcm_channel_area_t *areas)
{
	struct audioqueue *p;
	int err;

/*
	int frameFinished;
	AVFrame *pFrame = NULL;
	uint8_t **dst_data = NULL;
	int line_size;
	int ret;
*/

	while(1)
	{
		//pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
		//pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		if ((p = aq_remove(&aq)) == NULL)
		{
			break;
		}

// play 1 period
        err=play_period(handle, p);
		if (err);
/*
		if ((ret = avcodec_decode_audio4(pCodecCtxA, pFrame, &frameFinished, p->packet)) < 0)
		{
			fprintf(stderr, "Error decoding audio frame\n");
		}
		//printf("avcodec_decode_audio4\n");

		if(frameFinished)
		{
			ret = av_samples_alloc_array_and_samples(&dst_data, &line_size, pCodecCtxA->channels, pFrame->nb_samples, AV_SAMPLE_FMT_S16, 0);
			//printf("av_samples_alloc_array_and_samples, %d\n", ret);
			ret = swr_convert(swr, dst_data, pFrame->nb_samples, pFrame->extended_data, pFrame->nb_samples);
			if (ret<0) printf("swr_convert error %d\n", ret);
			//printf("swr_convert, %d", ret);
			int dst_bufsize = av_samples_get_buffer_size(&line_size, pCodecCtxA->channels, ret, AV_SAMPLE_FMT_S16, 1);


			//printf("writing %d\n", dst_bufsize);
			err = snd_pcm_writei(handle, dst_data[0], dst_bufsize);
			//printf("snd_pcm_writei err:%d\n", err);
			if (dst_data)
				av_freep(&dst_data[0]);
			av_freep(&dst_data);

			if (err == -EAGAIN)
			{
				av_free_packet(p->packet);
				free(p);
				continue;
			}
			if (err < 0)
			{
				if (xrun_recovery(handle, err) < 0)
				{
					printf("Write error: %s\n", snd_strerror(err));
					exit(EXIT_FAILURE);
				}
				//break;  // skip one period
			}
		}
*/
// play
	}
	return(0);
}

struct transfer_method {
        const char *name;
        snd_pcm_access_t access;
        int (*transfer_loop)(snd_pcm_t *handle, signed short *samples, snd_pcm_channel_area_t *areas);
};
static struct transfer_method transfer_methods[] = {
        { "write", SND_PCM_ACCESS_RW_INTERLEAVED, write_loop },
/*
        { "write_and_poll", SND_PCM_ACCESS_RW_INTERLEAVED, write_and_poll_loop },
        { "async", SND_PCM_ACCESS_RW_INTERLEAVED, async_loop },
        { "async_direct", SND_PCM_ACCESS_MMAP_INTERLEAVED, async_direct_loop },
        { "direct_interleaved", SND_PCM_ACCESS_MMAP_INTERLEAVED, direct_loop },
        { "direct_noninterleaved", SND_PCM_ACCESS_MMAP_NONINTERLEAVED, direct_loop },
        { "direct_write", SND_PCM_ACCESS_MMAP_INTERLEAVED, direct_write_loop },
*/
        { NULL, SND_PCM_ACCESS_RW_INTERLEAVED, NULL }
};

int method = 0;

static int set_hwparams(snd_pcm_t *handle, snd_pcm_hw_params_t *params, snd_pcm_access_t access)
{
        unsigned int rrate;
        snd_pcm_uframes_t size;
        int err, dir;
        /* choose all parameters */
        err = snd_pcm_hw_params_any(handle, params);
        if (err < 0) {
                printf("Broken configuration for playback: no configurations available: %s\n", snd_strerror(err));
                return err;
        }
        /* set hardware resampling */
        err = snd_pcm_hw_params_set_rate_resample(handle, params, resample);
        if (err < 0) {
                printf("Resampling setup failed for playback: %s\n", snd_strerror(err));
                return err;
        }
        /* set the interleaved read/write format */
        err = snd_pcm_hw_params_set_access(handle, params, access);
        if (err < 0) {
                printf("Access type not available for playback: %s\n", snd_strerror(err));
                return err;
        }
        /* set the sample format */
        err = snd_pcm_hw_params_set_format(handle, params, format);
        if (err < 0) {
                printf("Sample format not available for playback: %s\n", snd_strerror(err));
                return err;
        }
        /* set the count of channels */
        err = snd_pcm_hw_params_set_channels(handle, params, channels);
        if (err < 0) {
                printf("Channels count (%i) not available for playbacks: %s\n", channels, snd_strerror(err));
                return err;
        }
        /* set the stream rate */
        rrate = rate;
        err = snd_pcm_hw_params_set_rate_near(handle, params, &rrate, 0);
        if (err < 0) {
                printf("Rate %iHz not available for playback: %s\n", rate, snd_strerror(err));
                return err;
        }
        if (rrate != rate) {
                printf("Rate doesn't match (requested %iHz, get %iHz)\n", rate, err);
                return -EINVAL;
        }
        
        /* set the buffer time */
/*
        err = snd_pcm_hw_params_set_buffer_time_near(handle, params, &buffer_time, &dir);
        if (err < 0) {
                printf("Unable to set buffer time %i for playback: %s\n", buffer_time, snd_strerror(err));
                return err;
        }
*/
        err = snd_pcm_hw_params_set_buffer_size(handle, params, bufsize);
        if (err < 0) {
                printf("Unable to set buffer size %i for playback: %s\n", (int)buffer_size, snd_strerror(err));
                return err;
        }

        err = snd_pcm_hw_params_get_buffer_size(params, &size);
        if (err < 0) {
                printf("Unable to get buffer size for playback: %s\n", snd_strerror(err));
                return err;
        }
        buffer_size = size;

        /* set the period time */
/*
        err = snd_pcm_hw_params_set_period_time_near(handle, params, &period_time, &dir);
        if (err < 0) {
                printf("Unable to set period time %i for playback: %s\n", period_time, snd_strerror(err));
                return err;
        }
*/
        err = snd_pcm_hw_params_set_period_size(handle, params, persize, 0);
        if (err < 0) {
                printf("Unable to set period size %i for playback: %s\n", persize, snd_strerror(err));
                return err;
        }

        err = snd_pcm_hw_params_get_period_size(params, &size, &dir);
        if (err < 0) {
                printf("Unable to get period size for playback: %s\n", snd_strerror(err));
                return err;
        }
        period_size = size;

        /* write the parameters to device */
        err = snd_pcm_hw_params(handle, params);
        if (err < 0) {
                printf("Unable to set hw params for playback: %s\n", snd_strerror(err));
                return err;
        }
        return 0;
}

static int set_swparams(snd_pcm_t *handle, snd_pcm_sw_params_t *swparams)
{
        int err;
        /* get the current swparams */
        if ((err = snd_pcm_sw_params_current(handle, swparams)) < 0)
        {
                printf("Unable to determine current swparams for playback: %s\n", snd_strerror(err));
                return err;
        }
        /* start the transfer when the buffer is almost full: */
        /* (buffer_size / avail_min) * avail_min */
        //err = snd_pcm_sw_params_set_start_threshold(handle, swparams, (buffer_size / period_size) * period_size);

		// start transfer when first period arrives
        err = snd_pcm_sw_params_set_start_threshold(handle, swparams, period_size);
        if (err < 0)
        {
                printf("Unable to set start threshold mode for playback: %s\n", snd_strerror(err));
                return err;
        }
        /* allow the transfer when at least period_size samples can be processed */
        /* or disable this mechanism when period event is enabled (aka interrupt like style processing) */
        err = snd_pcm_sw_params_set_avail_min(handle, swparams, period_event ? buffer_size : period_size);
        if (err < 0)
        {
                printf("Unable to set avail min for playback: %s\n", snd_strerror(err));
                return err;
        }
        /* enable period events when requested */
        if (period_event) {
                err = snd_pcm_sw_params_set_period_event(handle, swparams, 1);
                if (err < 0) {
                        printf("Unable to set period event: %s\n", snd_strerror(err));
                        return err;
                }
        }
        /* write the parameters to the playback device */
        err = snd_pcm_sw_params(handle, swparams);
        if (err < 0) {
                printf("Unable to set sw params for playback: %s\n", snd_strerror(err));
                return err;
        }
        return 0;
}

int init_sound(AVCodecContext *ctx)
{
	int err;
	unsigned int chn;

	rate = ctx->sample_rate;
	channels = ctx->channels;

	snd_pcm_hw_params_alloca(&hwparams);
	snd_pcm_sw_params_alloca(&swparams);

	if ((err = snd_output_stdio_attach(&output, stdout, 0)) < 0)
	{
		printf("Output failed: %s\n", snd_strerror(err));
		return err;
	}
	printf("Playback device is %s\n", device);
	printf("Stream parameters are %iHz, %s, %i channels\n", rate, snd_pcm_format_name(format), channels);
	printf("Using transfer method: %s\n", transfer_methods[method].name);
	if ((err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0)
	{
		printf("Playback open error: %s\n", snd_strerror(err));
		return 0;
	}
	if ((err = set_hwparams(handle, hwparams, transfer_methods[method].access)) < 0)
	{
		printf("Setting of hwparams failed: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}
	if ((err = set_swparams(handle, swparams)) < 0)
	{
		printf("Setting of swparams failed: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	samples = malloc((period_size * channels * snd_pcm_format_physical_width(format)) / 8);
	if (samples == NULL)
	{
		printf("No enough memory\n");
		exit(EXIT_FAILURE);
	}

	areas = calloc(channels, sizeof(snd_pcm_channel_area_t));
	if (areas == NULL)
	{
		printf("No enough memory\n");
		exit(EXIT_FAILURE);
	}
	for (chn = 0; chn < channels; chn++)
	{
		areas[chn].addr = samples;
		areas[chn].first = chn * snd_pcm_format_physical_width(format);
		areas[chn].step = channels * snd_pcm_format_physical_width(format);
	}

	return 0;
}

int close_sound()
{
	free(areas);
	free(samples);
	snd_pcm_close(handle);
	return 0;
}


// Video
struct videoqueue
{
	struct videoqueue *prev;
	struct videoqueue *next;
	AVPacket *packet;
	char *rgba; // y,u,v byte arrays concatenated
	int label;
};
struct videoqueue *vq;
int vqLength;
const int vqMaxLength = 15;
pthread_mutex_t vqmutex; // = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t vqlowcond; // = PTHREAD_COND_INITIALIZER;
pthread_cond_t vqhighcond; // = PTHREAD_COND_INITIALIZER;

void vq_init(struct videoqueue **q, pthread_mutex_t *m, pthread_cond_t *cl, pthread_cond_t *ch)
{
	int ret;

	*q = NULL;
	vqLength = 0;

	if((ret=pthread_mutex_init(m, NULL))!=0 )
		printf("mutex init failed, %d\n", ret);

	if((ret=pthread_cond_init(ch, NULL))!=0 )
		printf("cond init failed, %d\n", ret);

	if((ret=pthread_cond_init(cl, NULL))!=0 )
		printf("cond init failed, %d\n", ret);
}

void vq_add(struct videoqueue **q,  AVPacket *packet, char *rgba, int label)
{
	struct videoqueue *p;

	pthread_mutex_lock(&vqmutex);
	while (vqLength>=vqMaxLength)
	{
		//printf("Video queue sleeping, overrun\n");
		pthread_cond_wait(&vqhighcond, &vqmutex);
	}

	p = malloc(sizeof(struct videoqueue));
//printf("malloc vq %d\n", sizeof(struct videoqueue));
	if (*q == NULL)
	{
		p->next = p;
		p->prev = p;
		*q = p;
	}
	else
	{
		p->next = *q;
		p->prev = (*q)->prev;
		(*q)->prev = p;
		p->prev->next = p;
	}
	p->packet = packet;
	p->rgba = rgba;
	p->label = label;

	vqLength++;

	//condition = true;
	pthread_cond_signal(&vqlowcond); // Should wake up *one* thread
	pthread_mutex_unlock(&vqmutex);
}

struct videoqueue* vq_remove_element(struct videoqueue **q)
{
	struct videoqueue *p;

	if ((*q)->next == (*q))
	{
		p=*q;
		*q = NULL;
	}
	else
	{
		p = (*q);
		(*q) = (*q)->next;
		(*q)->prev = p->prev;
		(*q)->prev->next = (*q);
	}
	return p;
}

struct videoqueue* vq_remove(struct videoqueue **q)
{
	struct videoqueue *p;

	pthread_mutex_lock(&vqmutex);
	while((*q)==NULL) // queue empty
	{
		if (playerstatus==playing)
		{
			//printf("Video queue sleeping, underrun\n");
			pthread_cond_wait(&vqlowcond, &vqmutex);
		}
		else
			break;
	}
	switch (playerstatus)
	{
		case playing:
			p = vq_remove_element(q);
			vqLength--;
			break;
		case draining:
			if (vqLength>0)
			{
				p = vq_remove_element(q);
				vqLength--;
			}
			else
				p=NULL;
			break;
		default:
			p = NULL;
			break;
	}

	//condition = true;
	pthread_cond_signal(&vqhighcond); // Should wake up *one* thread
	pthread_mutex_unlock(&vqmutex);

	return p;
}

/*
void vq_close(struct videoqueue **q, pthread_mutex_t *m, pthread_cond_t *cl, pthread_cond_t *ch)
{
	struct videoqueue *p;

	pthread_mutex_lock(&vqmutex);
	while(!q)
	{
		p = vq_remove_element(q);
		vqLength--;
		av_free_packet(p->packet);
		free(p);
	}
	pthread_mutex_destroy(m);
	pthread_cond_destroy(cl);
	pthread_cond_destroy(ch);
}
*/

void vq_drain(struct videoqueue **q)
{
	pthread_mutex_lock(&vqmutex);
	while (vqLength)
	{
		pthread_mutex_unlock(&vqmutex);
		usleep(100000); // 0.1s
//printf("vqLength=%d\n", vqLength);
		pthread_mutex_lock(&vqmutex);
	}
	pthread_cond_signal(&vqlowcond); // Should wake up *one* thread
	pthread_mutex_unlock(&vqmutex);
//printf("vq_drain exit\n");
}

void vq_destroy(pthread_mutex_t *m, pthread_cond_t *cl, pthread_cond_t *ch)
{
	pthread_mutex_destroy(m);
	pthread_cond_destroy(cl);
	pthread_cond_destroy(ch);
}

int open_file(char * filename)
{
    int i;

    /* FFMpeg stuff */
    AVDictionary *optionsDict = NULL;
    AVDictionary *optionsDictA = NULL;

    av_register_all();
    avformat_network_init();

	if(avformat_open_input(&pFormatCtx, filename, NULL, NULL)!=0)
		return -1; // Couldn't open file
  
    // Retrieve stream information
	if(avformat_find_stream_info(pFormatCtx, NULL)<0)
		return -1; // Couldn't find stream information
  
    // Dump information about file onto standard error
	//av_dump_format(pFormatCtx, 0, filename, 0);
  
    // Find the first video stream
    videoStream=-1;
	for(i=0; i<pFormatCtx->nb_streams; i++)
	{
		if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
		{
			videoStream=i;
			break;
		}
	}
	if(videoStream==-1)
		return -1; // Didn't find a video stream

	audioStream = -1;
	for(i=0; i<pFormatCtx->nb_streams; i++)
	{
		if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO)
		{
			audioStream = i;
			break;
		}
	}
//	if(audioStream==-1)
//		return -1; // Didn't find an audio stream

	// Get a pointer to the codec context for the video stream
	pCodecCtx=pFormatCtx->streams[videoStream]->codec;

	// Find the decoder for the video stream
	pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
	if(pCodec==NULL)
	{
		fprintf(stderr, "Unsupported video codec!\n");
		return -1; // Codec not found
	}
  
    pCodecCtx->thread_count = 4;
    // Open codec
	if(avcodec_open2(pCodecCtx, pCodec, &optionsDict)<0)
		return -1; // Could not open video codec

	//sws_ctx=sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BILINEAR, NULL, NULL, NULL);
/*
    sws_context = sws_getCachedContext(sws_context, frame->width, frame->height, AV_PIX_FMT_YUV420P, frame2->width, frame2->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
    sws_scale(sws_context, (const uint8_t * const *)frame->data, frame->linesize, 0, frame->height, frame2->data, frame2->linesize);
*/

	AVStream *st = pFormatCtx->streams[videoStream];
    double frame_rate = st->avg_frame_rate.num / (double)st->avg_frame_rate.den;
//    printf("Frame rate = %2.2f\n", frame_rate);
	frametime = 1000000 / frame_rate; // usec
//	printf("frametime-2000 = %d usec\n", frametime);

//	printf("Width : %d, Height : %d\n", pCodecCtx->width, pCodecCtx->height);

	videoduration = ( pFormatCtx->duration / AV_TIME_BASE ) * frame_rate;

	// Get a pointer to the codec context for the audio stream
    pCodecCtxA=pFormatCtx->streams[audioStream]->codec;

	// Find the decoder for the audio stream
    pCodecA=avcodec_find_decoder(pCodecCtxA->codec_id);

	if(pCodecA==NULL)
	{
		fprintf(stderr, "Unsupported audio codec!\n");
		return -1; // Codec not found
	}

    // Open codec
    if(avcodec_open2(pCodecCtxA, pCodecA, &optionsDictA)<0){
        return -1; // Could not open audio codec
    }

	// Set up SWR context once you've got codec information
	swr = swr_alloc();
	av_opt_set_int(swr, "in_channel_layout",  pCodecCtxA->channel_layout, 0);
	av_opt_set_int(swr, "out_channel_layout", pCodecCtxA->channel_layout,  0);
	av_opt_set_int(swr, "in_sample_rate",     pCodecCtxA->sample_rate, 0);
	av_opt_set_int(swr, "out_sample_rate",    pCodecCtxA->sample_rate, 0);
	//av_opt_set_sample_fmt(swr, "in_sample_fmt",  AV_SAMPLE_FMT_FLTP, 0);
	av_opt_set_sample_fmt(swr, "in_sample_fmt",  pCodecCtxA->sample_fmt, 0);
	
	av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_S16,  0);
	swr_init(swr);

	//initialize ALSA lib
	init_sound(pCodecCtxA);

	return 0;
}

gboolean main_quit(gpointer data)
{
	gtk_main_quit();
	return FALSE;
}

static gpointer read_frames(gpointer args)
{
	int ctype = PTHREAD_CANCEL_ASYNCHRONOUS;
	int ctype_old;
	pthread_setcanceltype(ctype, &ctype_old);

    int frameFinished;
    AVFrame *pFrame = NULL;
    pFrame=av_frame_alloc();

    /* initialize packet, set data to NULL, let the demuxer fill it */
    /* http://ffmpeg.org/doxygen/trunk/doc_2examples_2demuxing_8c-example.html#a80 */
    AVPacket *packet;

    packet=av_malloc(sizeof(AVPacket));
//printf("AVPacket av_malloc %d\n", sizeof(AVPacket));
    av_init_packet(packet);
    packet->data = NULL;
    packet->size = 0;

    int i=0, j=0;

	char *rgba;

//get_first_time_microseconds_2();
    while (av_read_frame(pFormatCtx, packet)>=0)
    {
//long diff1=get_next_time_microseconds_2();
//printf("%lu usec av_read_frame\n", diff1);
		if (packet->stream_index==videoStream) 
		{
//printf("videoStream %d\n", j); printf("vqlength %d\n", vqLength);
//get_first_time_microseconds_2();
			avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, packet); // Decode video frame
//long diff2=get_next_time_microseconds_2();
//printf("%lu usec avcodec_decode_video2\n", diff2);

			if (frameFinished)
			{
				int width = pCodecCtx->width;
				int height = pCodecCtx->height;
				rgba = malloc(width * height*3/2);
				memcpy(&rgba[0], pFrame->data[0], width*height); //y
				memcpy(&rgba[width*height], pFrame->data[1], width*height/4); //u
				memcpy(&rgba[width*height*5/4], pFrame->data[2], width*height/4); //v

				vq_add(&vq, packet, rgba, j++);
			}
		}
		else if (packet->stream_index==audioStream)
		{
//printf("audioStream %d\n", i); printf("aqlength %d\n", aqLength);
			aq_add(&aq, packet, i++);
		}
		packet=av_malloc(sizeof(AVPacket));
//printf("AVPacket av_malloc %d\n", sizeof(AVPacket));
		av_init_packet(packet);
		packet->data = NULL;
		packet->size = 0;
//get_first_time_microseconds_2();
	}
	//avcodec_close(pCodec);
	//avcodec_close(pCodecA);

	av_frame_free(&pFrame);
//printf("av_frame_free\n");

	av_packet_unref(packet);
//printf("av_packet_unref\n");

	avformat_close_input(&pFormatCtx);
	avformat_network_deinit();

	pthread_mutex_lock(&vqmutex);
	pthread_mutex_lock(&aqmutex);
	playerstatus = draining;
	pthread_mutex_unlock(&vqmutex);
	pthread_mutex_unlock(&aqmutex);
//printf("vq\n");
	vq_drain(&vq);

//printf("aq\n");
	aq_drain(&aq);

	i=pthread_join(tid[0], NULL);
//printf("join 0 %d\n", i);
	i=pthread_join(tid[1], NULL);
//printf("join 1 %d\n", i);

	aq_destroy(&aqmutex, &aqlowcond, &aqhighcond);
	vq_destroy(&vqmutex, &vqlowcond, &vqhighcond);

	close_sound();

	free(swr);

/*
	g_mutex_lock(&pixbufmutex);
	begindrawcallback = 0;
	g_mutex_unlock(&pixbufmutex);
*/

	playerstatus = idle;

	gdk_threads_add_idle(main_quit, NULL);

//printf("exiting read_frames\n");
	retval_readframes = 0;
	pthread_exit(&retval_readframes);
}

void* audioPlayFromQueue(void *arg)
{
	int err;
	int ctype = PTHREAD_CANCEL_ASYNCHRONOUS;
	int ctype_old;
	pthread_setcanceltype(ctype, &ctype_old);

	if((err = transfer_methods[method].transfer_loop(handle, samples, areas)) < 0)
		printf("Transfer failed: %s\n", snd_strerror(err));

	//printf("exiting audioplayfromqueue\n");
	retval_audio = 0;
	pthread_exit((void*)&retval_audio);
}

void* videoPlayFromQueue(void *arg)
{
	struct videoqueue *p;
	UserData *userData;
	long diff;

	int ctype = PTHREAD_CANCEL_ASYNCHRONOUS;
	int ctype_old;
	pthread_setcanceltype(ctype, &ctype_old);

    bcm_host_init();

    init_ogl(p_state);
    if (Init(p_state) == GL_FALSE)
    {
		printf("Init\n");
		exit(0);
	}

	int width = pCodecCtx->width;
	int height = pCodecCtx->height;
	userData = p_state->user_data;
    setSize(width/4, height*3/2);

	GLfloat picSize[2] = { (GLfloat)width, (GLfloat)height*3/2 };
	glUniform2fv(userData->sizeLoc, 1, picSize);
	GLfloat yuv2rgbmatrix[9] = { 1.0, 0.0, 1.5958, 1.0, -0.3917, -0.8129, 1.0, 2.017, 0.0 };
	glUniformMatrix3fv(userData->cmatrixLoc, 1, FALSE, yuv2rgbmatrix);

	diff = 0;
	while (1)
	{
		if ((p = vq_remove(&vq)) == NULL)
			break;

		if (diff > 0)
		{
			diff-=frametime;
			if (diff<0)
			{
				//usleep(0-diff);
				diff = 0;
			}
//printf("skip %d\n", p->label);

			free(p->rgba);
//printf("free rgba\n");
			av_packet_unref(p->packet);
//printf("av_packet_unref\n");
			free(p);
//printf("free vq\n");

			continue;
		}

//get_first_time_microseconds();
		texImage2D(p->rgba, width/4, height*3/2);
//long diff=get_next_time_microseconds();
//printf("%lu usec texImage2D\n", diff);

//get_first_time_microseconds();
		redraw_scene(p_state);
		glFinish();
//diff=get_next_time_microseconds();
//printf("%lu usec redraw\n", diff);

		diff -= frametime - 1000;
		if (diff<0)
		{
			//usleep(0-diff);
			diff = 0;
		}

		free(p->rgba);
//printf("free rgba\n");
		av_packet_unref(p->packet);
//printf("av_packet_unref\n");
		free(p);
//printf("free vq\n");
	}

	exit_func();
//printf("exiting videoPlayFromQueue\n");
	retval_video = 0;
	pthread_exit((void*)&retval_video);
}

void create_threads_av()
{
	int err;

    err = pthread_create(&(tid[0]), NULL, &audioPlayFromQueue, NULL);
    if (err)
    {}
    CPU_ZERO(&(cpu[1]));
    CPU_SET(1, &(cpu[1]));
    err = pthread_setaffinity_np(tid[0], sizeof(cpu_set_t), &(cpu[1]));
    if (err)
    {}

    err = pthread_create(&(tid[1]), NULL, &videoPlayFromQueue, NULL);
    if (err)
    {}
    CPU_ZERO(&(cpu[2]));
    CPU_SET(2, &(cpu[2]));
    err = pthread_setaffinity_np(tid[1], sizeof(cpu_set_t), &(cpu[2]));
    if (err)
    {}
}

void create_thread_framereader()
{
	int err;

    err = pthread_create(&(tid[2]), NULL, &read_frames, NULL);
    if (err)
    {}
    CPU_ZERO(&(cpu[3]));
    CPU_SET(3, &(cpu[3]));
    err = pthread_setaffinity_np(tid[2], sizeof(cpu_set_t), &(cpu[3]));
    if (err)
    {}
}

/* Called when the windows are realized
 */
static void realize_cb (GtkWidget *widget, gpointer data)
{
}

static gboolean delete_event( GtkWidget *widget,
                              GdkEvent  *event,
                              gpointer   data )
{
    /* If you return FALSE in the "delete-event" signal handler,
     * GTK will emit the "destroy" signal. Returning TRUE means
     * you don't want the window to be destroyed.
     * This is useful for popping up 'are you sure you want to quit?'
     * type dialogs. */

	//g_print("delete event occurred\n");

    /* Change TRUE to FALSE and the main window will be destroyed with
     * a "delete-event". */
    return FALSE;
}

/* Another callback */
static void destroy(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}

int main(int argc, char** argv)
{
	// Init Video Queue
	vq_init(&vq, &vqmutex, &vqlowcond, &vqhighcond);
//printf("vq_init\n");
	// Init Audio Queue
	aq_init(&aq, &aqmutex, &aqlowcond, &aqhighcond);
//printf("aq_init\n");

	now_playing = argv[1];
	int ret = open_file(now_playing);
	if (ret == 0)
	{
//printf("opened %s\n", now_playing);
	}
	else
	{
		printf("Error opening file %s\n", now_playing);
		return -1;
	}

	playerstatus = playing;
	create_thread_framereader();
//printf("create_thread_framereader\n");
	// Audio and video player threads
	create_threads_av();
//printf("create_threads_av\n");


    /* GTK stuff now */

     /* This is called in all GTK applications. Arguments are parsed
     * from the command line and are returned to the application. */
    gtk_init (&argc, &argv);
    
    /* create a new window */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    
    /* When the window is given the "delete-event" signal (this is given
     * by the window manager, usually by the "close" option, or on the
     * titlebar), we ask it to call the delete_event () function
     * as defined above. The data passed to the callback
     * function is NULL and is ignored in the callback function. */
    g_signal_connect (window, "delete-event",
		      G_CALLBACK (delete_event), NULL);
    
    /* Here we connect the "destroy" event to a signal handler.  
     * This event occurs when we call gtk_widget_destroy() on the window,
     * or if we return FALSE in the "delete-event" callback. */
    g_signal_connect (window, "destroy",
		      G_CALLBACK (destroy), NULL);

    g_signal_connect (window, "realize", G_CALLBACK (realize_cb), NULL);
    
    /* Sets the border width of the window. */
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
       
    /* and the window */
    gtk_widget_show (window);
    
    /* All GTK applications must have a gtk_main(). Control ends here
     * and waits for an event to occur (like a key press or
     * mouse event). */
    gtk_main();
    
    return 0;
}
