/* 
 * Copyright 2017 Ilker Cakir
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

/*
Using Geany Editor
Compile with gcc -Wall -c "%f" -DUSE_OPENGL -DUSE_EGL -DIS_RPI -DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -g -ftree-vectorize -pipe -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi $(pkg-config --cflags gtk+-3.0) -I/opt/vc/include/ -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux -Wno-deprecated-declarations
Link with gcc -Wall -o "%e" "%f" -D_POSIX_C_SOURCE=199309L $(pkg-config --cflags gtk+-3.0) -Wl,--whole-archive -I/opt/vc/include -L/opt/vc/lib/ -lGLESv2 -lEGL -lbcm_host -lvchiq_arm -lpthread -lrt -ldl -lm -Wl,--no-whole-archive -rdynamic $(pkg-config --libs gtk+-3.0) $(pkg-config --libs libavcodec libavformat libavutil libswscale) -lasound $(pkg-config --libs sqlite3)
*/
/*
To Do
1) gdk_cairo_draw_from_gl() // since 3.16
*/

#define _GNU_SOURCE

#include <bcm_host.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include <libavcodec/avcodec.h>
#include <libavutil/pixfmt.h>
#include <libavutil/avutil.h>
#include <libavutil/samplefmt.h>
#include <libavutil/opt.h>
#include <libavformat/avformat.h>
#include <libavresample/avresample.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <alsa/asoundlib.h>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <assert.h>
#include <math.h>
#include <sched.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <ctype.h>

#include <sqlite3.h> 

#define EGL_EGLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#define M_LN2 0.69314718055994530942

GtkWidget *image;
GtkWidget *window;
GdkPixbuf *pixbuf;
GtkWidget *box1;
GtkWidget *horibox;
GtkWidget *button_box;
GtkWidget *button1;
GtkWidget *button2;
GtkWidget *button8;
GtkWidget *button9;
GtkWidget *button10;
GtkWidget *buttonParameters;
GtkWidget *dwgarea;
GtkAdjustment *hadjustment;
GtkWidget *hscale;
GtkWidget *stackswitcher;
GtkWidget *stack;
GtkWidget *playerbox;
GtkWidget *box2;
GtkWidget *playlistbox;
GtkWidget *button_box2;
GtkWidget *button3;
GtkWidget *button4;
GtkWidget *button6;
GtkWidget *button7;
GtkWidget *listview;
GtkListStore *store;
GtkTreeIter iter;
GtkWidget *scrolled_window;
GtkWidget *levelbar1;
GtkWidget *levelbar2;
GtkWidget *levelbar3;
GtkWidget *levelbar4;
GtkWidget *levelbar5;
GtkWidget *levelbar6;
GtkWidget *levelbar7;
GtkWidget *label1;
GtkWidget *label2;
GtkWidget *label3;
GtkWidget *label4;
GtkWidget *label5;
GtkWidget *label6;
GtkWidget *label7;
GtkWidget *statusbar;
gint context_id;
GtkWidget *windoweq;
GtkWidget *eqvbox;
GtkWidget *eqbox;
GtkWidget *eqbox2;
GtkAdjustment *vadj0;
GtkAdjustment *vadj1;
GtkAdjustment *vadj2;
GtkAdjustment *vadj3;
GtkAdjustment *vadj4;
GtkAdjustment *vadj5;
GtkAdjustment *vadj6;
GtkAdjustment *vadj7;
GtkAdjustment *vadj8;
GtkAdjustment *vadj9;
GtkAdjustment *vadjA;
GtkWidget *vscaleeq0;
GtkWidget *vscaleeq1;
GtkWidget *vscaleeq2;
GtkWidget *vscaleeq3;
GtkWidget *vscaleeq4;
GtkWidget *vscaleeq5;
GtkWidget *vscaleeq6;
GtkWidget *vscaleeq7;
GtkWidget *vscaleeq8;
GtkWidget *vscaleeq9;
GtkWidget *vscaleeqA;
GtkWidget *eqbox0;
GtkWidget *eqbox1;
GtkWidget *eqbox2;
GtkWidget *eqbox3;
GtkWidget *eqbox4;
GtkWidget *eqbox5;
GtkWidget *eqbox6;
GtkWidget *eqbox7;
GtkWidget *eqbox8;
GtkWidget *eqbox9;
GtkWidget *eqboxA;
GtkWidget *eqlabel0;
GtkWidget *eqlabel1;
GtkWidget *eqlabel2;
GtkWidget *eqlabel3;
GtkWidget *eqlabel4;
GtkWidget *eqlabel5;
GtkWidget *eqlabel6;
GtkWidget *eqlabel7;
GtkWidget *eqlabel8;
GtkWidget *eqlabel9;
GtkWidget *eqlabelA;
GtkWidget *eqenable;
GtkWidget *eqautolevel;
GtkWidget *combopreset;
GtkWidget *windowparm;
GtkWidget *parmvbox;
GtkWidget *parmhbox1;
GtkWidget *parmlabel11;
GtkWidget *parmlabel12;
GtkWidget *spinbutton1;
GtkWidget *parmhbox2;
GtkWidget *parmlabel21;
GtkWidget *parmlabel22;
GtkWidget *spinbutton2;
GtkWidget *parmhbox3;
GtkWidget *parmlabel3;
GtkWidget *spinbutton3;
GtkWidget *parmhbox4;
GtkWidget *parmlabel4;
GtkWidget *spinbutton4;
GtkWidget *parmhlevel1;
GtkWidget *levellabel1;
GtkWidget *parmhlevel2;
GtkWidget *levellabel2;
GtkWidget *parmhlevel3;
GtkWidget *levellabel3;
GtkWidget *parmhlevel4;
GtkWidget *levellabel4;
GtkWidget *parmhlevel5;
GtkWidget *levellabel5;
GtkWidget *parmhlevel6;
GtkWidget *levellabel6;
GtkWidget *parmhlevel7;
GtkWidget *levellabel7;

GtkWidget *dlyvbox;
GtkWidget *framedelay1;
GtkWidget *dlybox1;
GtkWidget *dlyenable;
GtkWidget *delaylabel1;
GtkWidget *spinbutton5;
GtkWidget *delaytypelabel;
GtkWidget *combodelaytype;
GtkWidget *feedbacklabel1;
GtkWidget *spinbutton6;

GtkWidget *framervrb1;
GtkWidget *rvrbboxv;
GtkWidget *rvrbbox1;
GtkWidget *rvrbenable;
GtkWidget *rvrblabel1;
GtkWidget *spinbutton7;
GtkWidget *rvrblabel2;
GtkWidget *spinbutton8;
GtkWidget *rvrbbox2;
GtkWidget *rvrbdelays;
GtkWidget *comborvrbdelays;
GtkWidget *rvrblabel3;
GtkWidget *spinbutton11;
GtkWidget *rvrblabel4;
GtkWidget *spinbutton18;

GtkWidget *frametremolo1;
GtkWidget *tremolobox1;
GtkWidget *tremoloenable;
GtkWidget *tremololabel1;
GtkWidget *spinbutton9;
GtkWidget *tremololabel2;
GtkWidget *spinbutton10;

GtkWidget *framefd1;
GtkWidget *fdbox1;
GtkWidget *fdenable;
GtkWidget *fdlabel1;
GtkWidget *spinbutton12;
GtkWidget *fdlabel2;
GtkWidget *spinbutton13;

GtkWidget *framemod1;
GtkWidget *modbox1;
GtkWidget *modenable;
GtkWidget *modlabel1;
GtkWidget *spinbutton14;
GtkWidget *modlabel2;
GtkWidget *spinbutton15;

GtkWidget *framecho1;
GtkWidget *chobox1;
GtkWidget *choenable;
GtkWidget *cholabel1;
GtkWidget *spinbutton16;

GtkWidget *framehaas1;
GtkWidget *haasbox1;
GtkWidget *mnenable1;
GtkWidget *haasenable;
GtkWidget *haaslabel1;
GtkWidget *spinbutton17;

GtkWidget *mnenable2;

char leveltext1[10];
char leveltext2[10];
char leveltext3[10];
char leveltext4[10];
char leveltext5[10];
char leveltext6[10];
char leveltext7[10];
char leveltext8[10];
char leveltext9[10];

GMutex pixbufmutex;

/* FFMpeg vbls */
AVFormatContext *pFormatCtx = NULL;
AVCodecContext *pCodecCtx = NULL;
int videoStream;
AVCodecContext *pCodecCtxA = NULL;
int audioStream;
struct SwsContext *sws_ctx = NULL;
AVCodec *pCodec = NULL;
AVCodec *pCodecA = NULL;
int thread_count = 4;

SwrContext *swr;

#define idle 0
#define playing 1
#define paused 2
#define draining 3

int playerstatus = idle;
int stoprequested = 0;
char *now_playing = NULL;
char *catalog_folder = NULL;

pthread_t tid[3];
int retval_video, retval_audio, retval_readframes;
cpu_set_t cpu[4];

int playerWidth, playerHeight;
int frametime;
double frame_rate, sample_rate;
int64_t now_playing_frame;
int64_t now_decoding_frame;
int hscaleupd;
int64_t videoduration;
int64_t audioduration;

long long usecs; // microseconds
long long usecs1; // microseconds
long long usecs2; // microseconds

long diff1, diff2, diff3, diff4, diff5, diff6;
float diff7;
// read, decode, tex2d, draw, glread, cairo

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

	// Frame & Render buffers
	GLuint canvasFrameBuffer;
	GLuint canvasRenderBuffer;

	char *outrgb;

	// Output texture
	GLuint outtex;
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


/*// Upside down with GTK
GLfloat tVertices[] = {  0.0f,  0.0f,        // TexCoord 0 
                         0.0f,  1.0f,        // TexCoord 1
                         1.0f,  1.0f,        // TexCoord 2
                         1.0f,  0.0f         // TexCoord 3
                      };
*/
GLfloat tVertices[] = {  0.0f,  1.0f,        // TexCoord 0 
                         0.0f,  0.0f,        // TexCoord 1
                         1.0f,  0.0f,        // TexCoord 2
                         1.0f,  1.0f         // TexCoord 3
                      };
                      
GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

pid_t gettid( void )
{
	return syscall( __NR_gettid );
}

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

long get_first_time_microseconds_1()
{
	long long micros;
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

	micros = spec.tv_sec * 1.0e6 + round(spec.tv_nsec / 1000); // Convert nanoseconds to microseconds
	usecs1 = micros;
	return(0L);
}

long get_next_time_microseconds_1()
{
    long delta;
    long long micros;
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    micros = spec.tv_sec * 1.0e6 + round(spec.tv_nsec / 1000); // Convert nanoseconds to microseconds
    delta = micros - usecs1;
    usecs1 = micros;
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

    printf("Screen size = %d * %d\n", p_state->screen_width, p_state->screen_height);

    p_state->screen_width = pCodecCtx->width;
    p_state->screen_height = pCodecCtx->height;

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

void init_ogl2(CUBE_STATE_T *state)
{
    EGLBoolean result;
    EGLint num_config;

/*
    static const EGLint attribute_list[] =
	{
	    EGL_RED_SIZE, 8,
	    EGL_GREEN_SIZE, 8,
	    EGL_BLUE_SIZE, 8,
	    EGL_ALPHA_SIZE, 0,
	    EGL_SURFACE_TYPE, EGL_WINDOW_BIT | EGL_PBUFFER_BIT,
	    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT | EGL_OPENVG_BIT,
	    EGL_NONE
	};
*/
    static const EGLint attribute_list[] =
	{
	    EGL_RED_SIZE, 8,
	    EGL_GREEN_SIZE, 8,
	    EGL_BLUE_SIZE, 8,
	    EGL_ALPHA_SIZE, 0,
	    EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
	    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
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
    p_state->screen_width = playerWidth;
    p_state->screen_height = playerHeight;

/*
    state->surface = eglCreateWindowSurface( p_state->display, config, NULL, NULL);
    assert(p_state->surface != EGL_NO_SURFACE);
*/
    EGLint pbuffer_attributes[] = 
    {
		EGL_WIDTH, p_state->screen_width,
		EGL_HEIGHT, p_state->screen_height,
		EGL_NONE
    };
    p_state->surface = eglCreatePbufferSurface(p_state->display, config, pbuffer_attributes);
    assert (p_state->surface != EGL_NO_SURFACE);

    // connect the context to the surface
    result = eglMakeCurrent(p_state->display, p_state->surface, p_state->surface, p_state->context);
    assert(EGL_FALSE != result);
}

void exit_func(void) // Function to be passed to atexit().
{
	UserData *userData = p_state->user_data;

/*
// unbind frame buffer
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteRenderbuffers(1, &(userData->canvasRenderBuffer));
	glDeleteFramebuffers(1, &(userData->canvasFrameBuffer));
*/

	// Delete allocated objects
	glDeleteProgram(userData->programObject);
	glDeleteTextures(1, &userData->tex);

	// Release OpenGL resources
	eglMakeCurrent(p_state->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroySurface(p_state->display, p_state->surface);
	eglDestroyContext(p_state->display, p_state->context);
	eglTerminate(p_state->display);
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

         free(infoLog);
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
   userData->programObject = LoadProgram ((char *)vShaderStr, (char *)fShaderStr_stpq );
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

   glEnableVertexAttribArray(userData->positionLoc);
   glEnableVertexAttribArray(userData->texCoordLoc);
   
   glUniform1i(userData->samplerLoc, 0);

/*
	// Create framebuffer
	glGenFramebuffers(1, &(userData->canvasFrameBuffer));
	glBindFramebuffer(GL_RENDERBUFFER, userData->canvasFrameBuffer);

	// Attach renderbuffer
	glGenRenderbuffers(1, &(userData->canvasRenderBuffer));
	glBindRenderbuffer(GL_RENDERBUFFER, userData->canvasRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, pCodecCtx->width, pCodecCtx->height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, userData->canvasRenderBuffer);

	glGenTextures(1, &(userData->outtex));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, userData->outtex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLfloat)GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLfloat)GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  pCodecCtx->width, pCodecCtx->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, userData->outtex, 0);
*/
   glClearColor ( 0.0f, 0.0f, 0.0f, 1.0f );
   glClear(GL_COLOR_BUFFER_BIT);
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

    //checkNoGLES2Error();
}

void redraw_scene(CUBE_STATE_T *state)
{
	UserData *userData = p_state->user_data;

   glUniform1i(userData->samplerLoc, 0);
   // Set the viewport
   glViewport(0, 0, state->screen_width, state->screen_height);
   // Clear the color buffer
   glClear(GL_COLOR_BUFFER_BIT);
   glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
   glFinish();
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
struct audioqueue *aq = NULL;
int aqLength;
int aqMaxLength = 20;
pthread_mutex_t aqmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t aqlowcond = PTHREAD_COND_INITIALIZER;
pthread_cond_t aqhighcond = PTHREAD_COND_INITIALIZER;

void aq_init(struct audioqueue **q, pthread_mutex_t *m, pthread_cond_t *cl, pthread_cond_t *ch)
{
	int ret;

	*q = NULL;
	aqLength = 0;

	pthread_mutex_destroy(m);
	pthread_cond_destroy(cl);
	pthread_cond_destroy(ch);

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
	av_frame_unref(pFrame);
	if ((ret = avcodec_decode_audio4(pCodecCtxA, pFrame, &frameFinished, packet)) < 0)
		printf("Error decoding audio frame\n");
	if (frameFinished)
	{
		ret = av_samples_alloc_array_and_samples(&(p->dst_data), &line_size, pCodecCtxA->channels, pFrame->nb_samples, AV_SAMPLE_FMT_S16, 0);
//printf("av_samples_alloc_array_and_samples, %d\n", ret);
		ret = swr_convert(swr, p->dst_data, pFrame->nb_samples, (const uint8_t **)pFrame->extended_data, pFrame->nb_samples);
		if (ret<0) printf("swr_convert error %d\n", ret);
		p->dst_bufsize = av_samples_get_buffer_size(NULL, pCodecCtxA->channels, ret, AV_SAMPLE_FMT_S16, 0);
//printf("bufsize=%d\n", p->dst_bufsize);
	}
	av_frame_free(&pFrame);
//printf("av_frame_free\n");

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
		if ((playerstatus==playing) || (playerstatus==paused))
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
		case paused:
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
	int ret;

	pthread_mutex_destroy(m);
	pthread_cond_destroy(cl);
	pthread_cond_destroy(ch);

	if((ret=pthread_mutex_init(m, NULL))!=0 )
		printf("destroy, mutex init failed, %d\n", ret);

	if((ret=pthread_cond_init(ch, NULL))!=0 )
		printf("destroy, cond init failed, %d\n", ret);

	if((ret=pthread_cond_init(cl, NULL))!=0 )
		printf("destroy, cond init failed, %d\n", ret);
}

pthread_mutex_t seekmutex; // = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t framemutex; // = PTHREAD_MUTEX_INITIALIZER;

snd_pcm_t *handle;
signed short *samples;
snd_pcm_channel_area_t *areas;
snd_output_t *output = NULL;
char *device = "plughw:0,0";	// playback device
//char *device = "default";	// playback device
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

// Biquad Audio Equalizer
/* filter types */
enum {
   LPF, /* low pass filter */
   HPF, /* High pass filter */
   BPF, /* band pass filter */
   NOTCH, /* Notch Filter */
   PEQ, /* Peaking band EQ filter */
   LSH, /* Low shelf filter */
   HSH /* High shelf filter */
};

typedef struct biquad
{
   float a0, a1, a2, a3, a4;
   float x1, x2, y1, y2;
}biquadtype;

#define EQBANDS 10
const float eqfreqs[EQBANDS] = {31.0, 62.0, 125.0, 250.0, 500.0, 1000.0, 2000.0, 4000.0, 8000.0, 16000.0};
const char* eqlabels[EQBANDS] = {"31", "62", "125", "250", "500", "1K", "2K", "4K", "8K", "16K"};
struct biquad bqLeft[EQBANDS], bqRight[EQBANDS];
float eqoctave = 1.0;
pthread_mutex_t eqmutex; // = PTHREAD_MUTEX_INITIALIZER;
int eqenabled = 1;
int eqautoleveling = 1;
float effgain;

void effectivegain()
{
	float value;
	effgain = 0.0;

	value = 0.0 - gtk_range_get_value(GTK_RANGE(vscaleeq0));
	effgain += pow(10, value/5.0);
	value = 0.0 - gtk_range_get_value(GTK_RANGE(vscaleeq1));
	effgain += pow(10, value/5.0);
	value = 0.0 - gtk_range_get_value(GTK_RANGE(vscaleeq2));
	effgain += pow(10, value/5.0);
	value = 0.0 - gtk_range_get_value(GTK_RANGE(vscaleeq3));
	effgain += pow(10, value/5.0);
	value = 0.0 - gtk_range_get_value(GTK_RANGE(vscaleeq4));
	effgain += pow(10, value/5.0);
	value = 0.0 - gtk_range_get_value(GTK_RANGE(vscaleeq5));
	effgain += pow(10, value/5.0);
	value = 0.0 - gtk_range_get_value(GTK_RANGE(vscaleeq6));
	effgain += pow(10, value/5.0);
	value = 0.0 - gtk_range_get_value(GTK_RANGE(vscaleeq7));
	effgain += pow(10, value/5.0);
	value = 0.0 - gtk_range_get_value(GTK_RANGE(vscaleeq8));
	effgain += pow(10, value/5.0);
	value = 0.0 - gtk_range_get_value(GTK_RANGE(vscaleeq9));
	effgain += pow(10, value/5.0);

	effgain = 1.0 / sqrt(effgain/EQBANDS);
	effgain *= 0.9; // 10% margin
	//printf("EQ %5.2f\n", effgain);

	if (eqautoleveling)
		gtk_range_set_value(GTK_RANGE(vscaleeqA), 16.0-effgain);
}

/* Computes a BiQuad filter on a sample */
inline float BiQuad(float sample, struct biquad *b)
{
   float result;

   /* compute result */
   result = b->a0 * sample + b->a1 * b->x1 + b->a2 * b->x2 - b->a3 * b->y1 - b->a4 * b->y2;

   /* shift x1 to x2, sample to x1 */
   b->x2 = b->x1;
   b->x1 = sample;

   /* shift y1 to y2, result to y1 */
   b->y2 = b->y1;
   b->y1 = result;

   return result;
}

/* sets up a BiQuad Filter */
void BiQuad_init(int type, float dbGain, float freq, float srate, float bandwidth, struct biquad *b)
{
   float A, omega, sn, cs, alpha, beta;
   float a0, a1, a2, b0, b1, b2;

   /* setup variables */
   A = pow(10, dbGain /40);
   omega = 2 * M_PI * freq /srate;
   sn = sin(omega);
   cs = cos(omega);
   alpha = sn * sinh(M_LN2 /2 * bandwidth * omega /sn);
   beta = sqrt(A + A);

   switch (type) {
   case LPF:
       b0 = (1 - cs) /2;
       b1 = 1 - cs;
       b2 = (1 - cs) /2;
       a0 = 1 + alpha;
       a1 = -2 * cs;
       a2 = 1 - alpha;
       break;
   case HPF:
       b0 = (1 + cs) /2;
       b1 = -(1 + cs);
       b2 = (1 + cs) /2;
       a0 = 1 + alpha;
       a1 = -2 * cs;
       a2 = 1 - alpha;
       break;
   case BPF:
       b0 = alpha;
       b1 = 0;
       b2 = -alpha;
       a0 = 1 + alpha;
       a1 = -2 * cs;
       a2 = 1 - alpha;
       break;
   case NOTCH:
       b0 = 1;
       b1 = -2 * cs;
       b2 = 1;
       a0 = 1 + alpha;
       a1 = -2 * cs;
       a2 = 1 - alpha;
       break;
   case PEQ:
       b0 = 1 + (alpha * A);
       b1 = -2 * cs;
       b2 = 1 - (alpha * A);
       a0 = 1 + (alpha /A);
       a1 = -2 * cs;
       a2 = 1 - (alpha /A);
       break;
   case LSH:
       b0 = A * ((A + 1) - (A - 1) * cs + beta * sn);
       b1 = 2 * A * ((A - 1) - (A + 1) * cs);
       b2 = A * ((A + 1) - (A - 1) * cs - beta * sn);
       a0 = (A + 1) + (A - 1) * cs + beta * sn;
       a1 = -2 * ((A - 1) + (A + 1) * cs);
       a2 = (A + 1) + (A - 1) * cs - beta * sn;
       break;
   case HSH:
       b0 = A * ((A + 1) + (A - 1) * cs + beta * sn);
       b1 = -2 * A * ((A - 1) + (A + 1) * cs);
       b2 = A * ((A + 1) + (A - 1) * cs - beta * sn);
       a0 = (A + 1) - (A - 1) * cs + beta * sn;
       a1 = 2 * ((A - 1) - (A + 1) * cs);
       a2 = (A + 1) - (A - 1) * cs - beta * sn;
       break;
   }

   pthread_mutex_lock(&eqmutex);

   /* precompute the coefficients */
   b->a0 = b0 /a0;
   b->a1 = b1 /a0;
   b->a2 = b2 /a0;
   b->a3 = a1 /a0;
   b->a4 = a2 /a0;

   /* zero initial samples */
   b->x1 = b->x2 = 0;
   b->y1 = b->y2 = 0;

   pthread_mutex_unlock(&eqmutex);
}

static void vscale0(GtkWidget *widget, gpointer data)
{
    float value = 0.0 - gtk_range_get_value(GTK_RANGE(widget));
//    printf("Adjustment value: %f\n", value);
	BiQuad_init(LSH, value, eqfreqs[0], (float)rate, eqoctave, &(bqLeft[0]));
	BiQuad_init(LSH, value, eqfreqs[0], (float)rate, eqoctave, &(bqRight[0]));
	effectivegain();
}

static void vscale1(GtkWidget *widget, gpointer data)
{
    float value = 0.0 - gtk_range_get_value(GTK_RANGE(widget));
//    printf("Adjustment value: %f\n", value);
	BiQuad_init(PEQ, value, eqfreqs[1], (float)rate, eqoctave, &(bqLeft[1]));
	BiQuad_init(PEQ, value, eqfreqs[1], (float)rate, eqoctave, &(bqRight[1]));
	effectivegain();
}

static void vscale2(GtkWidget *widget, gpointer data)
{
    float value = 0.0 - gtk_range_get_value(GTK_RANGE(widget));
//    printf("Adjustment value: %f\n", value);
	BiQuad_init(PEQ, value, eqfreqs[2], (float)rate, eqoctave, &(bqLeft[2]));
	BiQuad_init(PEQ, value, eqfreqs[2], (float)rate, eqoctave, &(bqRight[2]));
	effectivegain();
}

static void vscale3(GtkWidget *widget, gpointer data)
{
    float value = 0.0 - gtk_range_get_value(GTK_RANGE(widget));
//    printf("Adjustment value: %f\n", value);
	BiQuad_init(PEQ, value, eqfreqs[3], (float)rate, eqoctave, &(bqLeft[3]));
	BiQuad_init(PEQ, value, eqfreqs[3], (float)rate, eqoctave, &(bqRight[3]));
	effectivegain();
}

static void vscale4(GtkWidget *widget, gpointer data)
{
    float value = 0.0 - gtk_range_get_value(GTK_RANGE(widget));
//    printf("Adjustment value: %f\n", value);
	BiQuad_init(PEQ, value, eqfreqs[4], (float)rate, eqoctave, &(bqLeft[4]));
	BiQuad_init(PEQ, value, eqfreqs[4], (float)rate, eqoctave, &(bqRight[4]));
	effectivegain();
}

static void vscale5(GtkWidget *widget, gpointer data)
{
    float value = 0.0 - gtk_range_get_value(GTK_RANGE(widget));
//    printf("Adjustment value: %f\n", value);
	BiQuad_init(PEQ, value, eqfreqs[5], (float)rate, eqoctave, &(bqLeft[5]));
	BiQuad_init(PEQ, value, eqfreqs[5], (float)rate, eqoctave, &(bqRight[5]));
	effectivegain();
}

static void vscale6(GtkWidget *widget, gpointer data)
{
    float value = 0.0 - gtk_range_get_value(GTK_RANGE(widget));
//    printf("Adjustment value: %f\n", value);
	BiQuad_init(PEQ, value, eqfreqs[6], (float)rate, eqoctave, &(bqLeft[6]));
	BiQuad_init(PEQ, value, eqfreqs[6], (float)rate, eqoctave, &(bqRight[6]));
	effectivegain();
}

static void vscale7(GtkWidget *widget, gpointer data)
{
    float value = 0.0 - gtk_range_get_value(GTK_RANGE(widget));
//    printf("Adjustment value: %f\n", value);
	BiQuad_init(PEQ, value, eqfreqs[7], (float)rate, eqoctave, &(bqLeft[7]));
	BiQuad_init(PEQ, value, eqfreqs[7], (float)rate, eqoctave, &(bqRight[7]));
	effectivegain();
}

static void vscale8(GtkWidget *widget, gpointer data)
{
    float value = 0.0 - gtk_range_get_value(GTK_RANGE(widget));
//    printf("Adjustment value: %f\n", value);
	BiQuad_init(PEQ, value, eqfreqs[8], (float)rate, eqoctave, &(bqLeft[8]));
	BiQuad_init(PEQ, value, eqfreqs[8], (float)rate, eqoctave, &(bqRight[8]));
	effectivegain();
}

static void vscale9(GtkWidget *widget, gpointer data)
{
    float value = 0.0 - gtk_range_get_value(GTK_RANGE(widget));
//    printf("Adjustment value: %f\n", value);
	BiQuad_init(HSH, value, eqfreqs[9], (float)rate, eqoctave, &(bqLeft[9]));
	BiQuad_init(HSH, value, eqfreqs[9], (float)rate, eqoctave, &(bqRight[9]));
	effectivegain();
}

static void vscaleA(GtkWidget *widget, gpointer data)
{
//    float value = 1.0 - gtk_range_get_value(GTK_RANGE(widget));
//    printf("Adjustment value: %f\n", value);
}

gboolean update_hscale(gpointer data)
{
	if (hscaleupd)
		gtk_adjustment_set_value(hadjustment, now_playing_frame);
	return FALSE;
}

void BiQuad_initAll()
{
	vscale0(vscaleeq0, NULL);
	vscale1(vscaleeq1, NULL);
	vscale2(vscaleeq2, NULL);
	vscale3(vscaleeq3, NULL);
	vscale4(vscaleeq4, NULL);
	vscale5(vscaleeq5, NULL);
	vscale6(vscaleeq6, NULL);
	vscale7(vscaleeq7, NULL);
	vscale8(vscaleeq8, NULL);
	vscale9(vscaleeq9, NULL);
}

void BiQuad_process(uint8_t *buf, int bufsize, int bytesinsample)
{
	int a, b;
	signed short *intp;
	intp=(signed short *)buf;
	float preampfactor;

	pthread_mutex_lock(&eqmutex);
	if (eqenabled)
	{
		preampfactor = 16.0 - gtk_range_get_value((GtkRange*)vscaleeqA);
		for (a=0,b=0;a<bufsize;a+=bytesinsample, b+=2) // process first 2 channels only
		{
			if (preampfactor < 1.0)
			{
				intp[b] *= preampfactor;
				intp[b+1] *= preampfactor;
			}

			intp[b] = BiQuad(intp[b], &(bqLeft[0]));
			intp[b+1] = BiQuad(intp[b+1], &(bqRight[0]));

			intp[b] = BiQuad(intp[b], &(bqLeft[1]));
			intp[b+1] = BiQuad(intp[b+1], &(bqRight[1]));

			intp[b] = BiQuad(intp[b], &(bqLeft[2]));
			intp[b+1] = BiQuad(intp[b+1], &(bqRight[2]));

			intp[b] = BiQuad(intp[b], &(bqLeft[3]));
			intp[b+1] = BiQuad(intp[b+1], &(bqRight[3]));

			intp[b] = BiQuad(intp[b], &(bqLeft[4]));
			intp[b+1] = BiQuad(intp[b+1], &(bqRight[4]));

			intp[b] = BiQuad(intp[b], &(bqLeft[5]));
			intp[b+1] = BiQuad(intp[b+1], &(bqRight[5]));

			intp[b] = BiQuad(intp[b], &(bqLeft[6]));
			intp[b+1] = BiQuad(intp[b+1], &(bqRight[6]));

			intp[b] = BiQuad(intp[b], &(bqLeft[7]));
			intp[b+1] = BiQuad(intp[b+1], &(bqRight[7]));

			intp[b] = BiQuad(intp[b], &(bqLeft[8]));
			intp[b+1] = BiQuad(intp[b+1], &(bqRight[8]));

			intp[b] = BiQuad(intp[b], &(bqLeft[9]));
			intp[b+1] = BiQuad(intp[b+1], &(bqRight[9]));
			
			if (preampfactor > 1.0)
			{
				intp[b] *= preampfactor;
				intp[b+1] *= preampfactor;
			}
		}
	}
	pthread_mutex_unlock(&eqmutex);
}

// Delay Processor

enum dly_type /* Delay types */
{
	DLY_ECHO,
	DLY_DELAY,
	DLY_REVERB,
	DLY_LATE,
};

struct sounddelay
{
	snd_pcm_format_t format; // SND_PCM_FORMAT_S16
	unsigned int rate; // sampling rate
	unsigned int channels; // channels
	float feedback; // feedback level 0.0 .. 1.0
	float millisec; // delay in milliseconds
	int N; // parallel delays
	int enabled;
	pthread_mutex_t delaymutex; // = PTHREAD_MUTEX_INITIALIZER;

	int physicalwidth; // bits per sample
	char *fbuffer;
	int fbuffersize;
	int fbuffersamples;
	int delaysamples;
	int delaybytes;
	int insamples;

	int front, rear, readfront;
	signed short *fshort;

	enum dly_type delaytype;
};

struct sounddelay snddly; // Delay

void sounddelay_init(int N, enum dly_type delaytype, float millisec, float feedback, snd_pcm_format_t format, unsigned int rate, unsigned int channels, struct sounddelay *s)
{
	s->N = N;
	s->delaytype = delaytype;
	s->format = format;
	s->rate = rate;
	s->channels = channels;
	s->feedback = feedback;
	s->millisec = millisec;
	s->fbuffer = NULL;
	//printf("Delay initialized, type %d, %5.2f ms, %5.2f feedback, %d rate, %d channels\n", s->delaytype, s->millisec, s->feedback, s->rate, s->channels);
}

void sounddelay_add(char* inbuffer, int inbuffersize, struct sounddelay *s)
{
	int i;
	float prescale;
	signed short *inshort;

	pthread_mutex_lock(&(s->delaymutex));
	if (s->enabled)
	{
		if (!s->fbuffer)
		{
			s->physicalwidth = snd_pcm_format_width(s->format);
			s->insamples = inbuffersize * 8 / s->physicalwidth;
			s->delaysamples = ceil((s->millisec / 1000.0) * (float)s->rate) * s->channels;
			s->delaybytes = s->delaysamples * s->physicalwidth / 8;

			s->fbuffersize = s->delaybytes + inbuffersize;
			s->fbuffer = malloc(s->fbuffersize);
			memset(s->fbuffer, 0, s->fbuffersize);
			s->fbuffersamples = s->insamples + s->delaysamples;
			s->fshort = (signed short *)s->fbuffer;

			s->rear = s->delaysamples;
			s->front = 0;
			s->readfront = 0;
		}
		inshort = (signed short *)inbuffer;

		switch (s->delaytype)
		{
			case DLY_ECHO: // Repeating echo added to original
				prescale = sqrt(1 - s->feedback*s->feedback); // prescale=sqrt(sum(r^2n)), n=0..infinite
				for(i=0; i<s->insamples; i++)
				{
					inshort[i]*=prescale;
					s->fshort[s->rear++] = inshort[i] += s->fshort[s->front++]*s->feedback;
					s->front%=s->fbuffersamples;
					s->rear%=s->fbuffersamples;
				}
				break;
			case DLY_DELAY: // Single delayed signal added to original
				prescale = 1 / sqrt(1 + s->feedback*s->feedback); // prescale = 1/sqrt(1 + r^2)
				for(i=0;i<s->insamples; i++)
				{
					inshort[i]*=prescale;
					s->fshort[s->rear++] = inshort[i];
					inshort[i] += s->fshort[s->front++]*s->feedback;
					s->front%=s->fbuffersamples;
					s->rear%=s->fbuffersamples;
				}
				break;
			case DLY_REVERB: // Only repeating echo, no original
				//prescale = sqrt(1 - s->feedback*s->feedback); // prescale=sqrt(sum(r^2n)), n=0..infinite
				prescale = sqrt((1.0-s->feedback*s->feedback)/((s->N-1)*s->feedback*s->feedback+1.0)); // prescale=sqrt(sum(r^2n)-1), for all channels, n=0..infinite
				for(i=0; i<s->insamples; i++)
				{
					//s->fshort[s->rear++] = inshort[i]*prescale + s->fshort[s->front++]*s->feedback;
					s->fshort[s->rear++] = (inshort[i]*prescale + s->fshort[s->front++])*s->feedback;
					s->front%=s->fbuffersamples;
					s->rear%=s->fbuffersamples;
				}
				break;
			case DLY_LATE: // Single delayed signal, no original
				for(i=0;i<s->insamples; i++)
				{
					s->fshort[s->rear++] = inshort[i]*s->feedback;
					s->front++;
					s->front%=s->fbuffersamples;
					s->rear%=s->fbuffersamples;
				}
				break;
			default:
				break;
		}
	}
	pthread_mutex_unlock(&(s->delaymutex));
}

void sounddelay_close(struct sounddelay *s)
{
	if (s->fbuffer)
	{
		free(s->fbuffer);
		s->fbuffer = NULL;
	}
}

void Delay_initAll(snd_pcm_format_t format, float rate, unsigned int channels, struct sounddelay *s)
{
	gchar *strval;

	pthread_mutex_lock(&(s->delaymutex));
	g_object_get((gpointer)combodelaytype, "active-id", &strval, NULL);
	//printf("Selected id %s\n", strval);
	s->delaytype = atoi((const char *)strval);
	sounddelay_init(1, atoi((const char *)strval), (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton5)), 
					(float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton6)), format, rate, channels, s);
	g_free(strval);
	pthread_mutex_unlock(&(s->delaymutex));
}

void Delay_closeAll(struct sounddelay *s)
{
	pthread_mutex_lock(&(s->delaymutex));
	sounddelay_close(s);
	pthread_mutex_unlock(&(s->delaymutex));
}

// Reverb

#define REVERBDLINES 24
//float reverbprimes[REVERBDLINES] = {290.0, 387.0, 433.0, 470.0, 523.0, 559.0, 643.0};
//float reverbprimes[REVERBDLINES] = {307.0, 331.0, 353.0, 379.0, 401.0, 431.0, 449.0, 467.0};
//float reverbprimes[REVERBDLINES] = {293.0, 313.0, 347.0, 367.0, 383.0, 409.0, 431.0, 449.0, 463.0, 491.0, 509.0, 547.0, 569.0, 593.0, 613.0, 631.0};
float reverbprimes[REVERBDLINES] = {293.0, 307.0, 313.0, 331.0, 347.0, 367.0, 373.0, 383.0, 409.0, 419.0, 431.0, 449.0, 457.0, 463.0, 491.0, 509.0, 547.0, 557.0, 569.0, 577.0, 593.0, 613.0, 619.0, 631.0};

/*
           293    307    311    313    317    331    337    347    349 
    353    359    367    373    379    383    389    397    401    409 
    419    421    431    433    439    443    449    457    461    463 
    467    479    487    491    499    503    509    521    523    541 
    547    557    563    569    571    577    587    593    599    601 
    607    613    617    619    631 
*/

struct soundreverb
{
	int reverbdelaylines;
	struct sounddelay snddlyrev[REVERBDLINES];
	char* bbuf;

	snd_pcm_format_t format; // SND_PCM_FORMAT_S16
	unsigned int rate; // sampling rate
	unsigned int channels; // channels
	float feedback; // feedback level 0.0 .. 1.0
	float presence; // reverbation presence
	int enabled;
	pthread_mutex_t reverbmutex; // = PTHREAD_MUTEX_INITIALIZER;

	float LPFfreq;
	float LSHfreq;
	struct biquad bl[2], br[2];
};

struct soundreverb sndreverb;

void RevBiQuad_process(uint8_t *buf, int bufsize, int bytesinsample, struct soundreverb *r)
{
	int a, b;
	signed short *intp;
	intp=(signed short *)buf;
	float preampfactor;

	preampfactor = 1.0;
	for (a=0,b=0;a<bufsize;a+=bytesinsample, b+=2) // process first 2 channels only
	{
		intp[b] *= preampfactor;
		intp[b+1] *= preampfactor;

		intp[b] = BiQuad(intp[b], &(r->bl[0]));
		intp[b+1] = BiQuad(intp[b+1], &(r->br[0]));

		intp[b] = BiQuad(intp[b], &(r->bl[1]));
		intp[b+1] = BiQuad(intp[b+1], &(r->br[1]));
	}
}

void soundreverb_init(int delaylines, float feedback, float presence, float LPFf, float LSHf, snd_pcm_format_t format, unsigned int rate, unsigned int channels, struct soundreverb *r)
{
	int i;
	r->reverbdelaylines = delaylines;
	r->format = format;
	r->rate = rate;
	r->channels = channels;
	r->feedback = feedback;
	r->presence = presence;
	for(i=0; i<r->reverbdelaylines; i++)
	{
		r->snddlyrev[i].enabled = TRUE;
		sounddelay_init(r->reverbdelaylines, DLY_REVERB, reverbprimes[i], feedback, format, rate, channels, &(r->snddlyrev[i]));
	}

	r->LPFfreq = LPFf;
	r->LSHfreq = LSHf;
	r->bbuf = NULL;

	BiQuad_init(LPF, 12.0, r->LPFfreq, r->rate, eqoctave, &(r->bl[0]));
	BiQuad_init(LPF, 12.0, r->LPFfreq, r->rate, eqoctave, &(r->br[0]));

	BiQuad_init(LSH, -12.0, r->LSHfreq, r->rate, eqoctave, &(r->bl[1]));
	BiQuad_init(LSH, -12.0, r->LSHfreq, r->rate, eqoctave, &(r->br[1]));
}

void soundreverb_add(char* inbuffer, int inbuffersize, struct soundreverb *r)
{
	int i, j, *readfront, *fbsamples;
	signed short *dstbuf, *srcbuf;

	pthread_mutex_lock(&(r->reverbmutex));
	if (r->enabled)
	{
		for(i=0; i<r->reverbdelaylines; i++)
		{
			sounddelay_add(inbuffer, inbuffersize, &(r->snddlyrev[i]));
		}

		if (!r->bbuf)
		{
			r->bbuf = malloc(inbuffersize);
		}
		memset(r->bbuf, 0, inbuffersize);
		dstbuf = (signed short*)r->bbuf;
		for(i=0; i<r->reverbdelaylines; i++)
		{
			srcbuf = (signed short*)r->snddlyrev[i].fbuffer;
			readfront = &(r->snddlyrev[i].readfront);
			fbsamples = &(r->snddlyrev[i].fbuffersamples);
			for(j=0; j<r->snddlyrev[i].insamples; j++)
			{
				dstbuf[j] += srcbuf[(*readfront)++] * r->presence;
				(*readfront)%=(*fbsamples);
			}
		}

		RevBiQuad_process((uint8_t*)r->bbuf, inbuffersize, snd_pcm_format_width(r->format)/8*r->channels, r);

		dstbuf = (signed short*)inbuffer;
		srcbuf = (signed short*)r->bbuf;
		for(j=0; j<r->snddlyrev[0].insamples; j++)
		{
			dstbuf[j] += srcbuf[j];
		}
	}
	pthread_mutex_unlock(&(r->reverbmutex));
}

void soundreverb_close(struct soundreverb *r)
{
	int i;

	for(i=0; i<r->reverbdelaylines; i++)
	{
		if (r->snddlyrev[i].fbuffer)
		{
			free(r->snddlyrev[i].fbuffer);
			r->snddlyrev[i].fbuffer = NULL;
		}
	}
	if (r->bbuf)
	{
		free(r->bbuf);
		r->bbuf = NULL;
	}
}

void Reverb_initAll(snd_pcm_format_t format, float rate, unsigned int channels, struct soundreverb *r)
{
	int count;
	gchar *strval;

	pthread_mutex_lock(&(r->reverbmutex));
	g_object_get((gpointer)comborvrbdelays, "active-id", &strval, NULL);
	//printf("Selected id %s\n", strval);
	count = atoi((const char *)strval);
	soundreverb_init(count, (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton7)), (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton8)), 
					(float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton11)), (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton11)),
					format, rate, channels, r);
	g_free(strval);
	pthread_mutex_unlock(&(r->reverbmutex));
}

void Reverb_closeAll(struct soundreverb *r)
{
	pthread_mutex_lock(&(r->reverbmutex));
	soundreverb_close(r);
	pthread_mutex_unlock(&(r->reverbmutex));
}

// Tremolo

struct soundtremolo
{
	snd_pcm_format_t format; // SND_PCM_FORMAT_S16
	unsigned int rate; // sampling rate
	unsigned int channels; // channels
	float depth; // depth 0.0 .. 1.0
	float tremolorate; // tremolo rate  0.1 .. 10
	unsigned int framecount; // time=framecount/rate % rate; instantaneous volume=1-depth/2*(1-sin(2*pi*tremolorate*time))

	int enabled;
	pthread_mutex_t tremolomutex; // = PTHREAD_MUTEX_INITIALIZER;
};

struct soundtremolo sndtremolo;

void soundtremolo_init(float depth, float tremolorate, snd_pcm_format_t format, unsigned int rate, unsigned int channels, struct soundtremolo *t)
{
	t->format = format;
	t->rate = rate;
	t->channels = channels;
	t->depth = depth;
	t->tremolorate = tremolorate;
	t->framecount = 0;
}

void soundtremolo_add(char* inbuffer, int inbuffersize, struct soundtremolo *t)
{
	pthread_mutex_lock(&(t->tremolomutex));
	if (t->enabled)
	{
		int physicalwidth = snd_pcm_format_width(t->format); // bits per sample
		int insamples = inbuffersize / physicalwidth * 8;
		int frames = insamples / t->channels;
		int framesinT = (int)((float)t->rate / (float)t->tremolorate);
		signed short *inshort = (signed short *)inbuffer;
		int i,j,k;
		for(i=0,j=0;i<=frames;i++,t->framecount++)
		{
			float time = ((float)t->framecount / (float)t->rate);
			float theta = 2.0*M_PI*t->tremolorate*time; // w*t = 2*pi*f*t
			for(k=0;k<t->channels;k++)
				inshort[j++] *= 1.0-((t->depth/2.0)*(1.0-sin(theta)));
		}
		t->framecount %= framesinT;
	}
	pthread_mutex_unlock(&(t->tremolomutex));
}

void soundtremolo_close(struct soundtremolo *t)
{
}

void Tremolo_initAll(snd_pcm_format_t format, float rate, unsigned int channels, struct soundtremolo *t)
{
	pthread_mutex_lock(&(t->tremolomutex));
	soundtremolo_init((float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton9)), 
					(float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton10)), format, rate, channels, t);
	pthread_mutex_unlock(&(t->tremolomutex));
}

void Tremolo_closeAll(struct soundtremolo *t)
{
	pthread_mutex_lock(&(t->tremolomutex));
	soundtremolo_close(t);
	pthread_mutex_unlock(&(t->tremolomutex));
}

// Folding Distortion

struct soundfoldingdistortion
{
	snd_pcm_format_t format; // SND_PCM_FORMAT_S16
	unsigned int rate; // sampling rate
	unsigned int channels; // channels
	float threshold;
	float gain;
	int enabled;
	pthread_mutex_t fdmutex; // = PTHREAD_MUTEX_INITIALIZER;

	int initialized;
	int physicalwidth;
	int insamples;
};

struct soundfoldingdistortion sndflddistort;

void soundfoldingdistort_init(float threshold, float gain, snd_pcm_format_t format, unsigned int rate, unsigned int channels, struct soundfoldingdistortion *d)
{
	d->format = format;
	d->rate = rate;
	d->channels = channels;
	d->threshold = threshold;
	d->gain = gain;

	d->initialized = FALSE;
}

void soundfoldingdistort_add(char* inbuffer, int inbuffersize, struct soundfoldingdistortion *d)
{
	pthread_mutex_lock(&(d->fdmutex));
	if (d->enabled)
	{
		if (!d->initialized)
		{
			d->physicalwidth = snd_pcm_format_width(d->format); // bits per sample
			d->insamples = inbuffersize / d->physicalwidth * 8;
			d->initialized = TRUE;
		}
		signed short *inshort = (signed short *)inbuffer;
		int i;
		for(i=0;i<d->insamples;i++)
			inshort[i]= (inshort[i]>d->threshold?2*d->threshold-inshort[i]:(inshort[i]<-d->threshold?-2*d->threshold-inshort[i]:inshort[i]))*d->gain;
			//inshort[i] = (inshort[i]>d->threshold?d->threshold:(inshort[i]<-d->threshold?-d->threshold:inshort[i]))*d->gain;
	}
	pthread_mutex_unlock(&(d->fdmutex));
}

void soundfoldingdistort_close(struct soundfoldingdistortion *d)
{
	d->initialized = TRUE;
}

void FoldingDistortion_initAll(snd_pcm_format_t format, float rate, unsigned int channels, struct soundfoldingdistortion *d)
{
	pthread_mutex_lock(&(d->fdmutex));
	soundfoldingdistort_init((float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton12)), (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton13)),
							format, rate, channels, d);
	pthread_mutex_unlock(&(d->fdmutex));
}

void FoldingDistortion_closeAll(struct soundfoldingdistortion *d)
{
	pthread_mutex_lock(&(d->fdmutex));
	soundfoldingdistort_close(d);
	pthread_mutex_unlock(&(d->fdmutex));
}

// VFO

struct soundvfo
{
	snd_pcm_format_t format; // SND_PCM_FORMAT_S16
	unsigned int rate; // sampling rate
	unsigned int channels; // channels
	float vfofreq; // modulation frequency
	float vfodepth; // modulation depth in percent 0..1.0
	int invertphase; // invert phase of modulation
	int enabled;

	int N; // extra frames
	char *vfobuf;
	int physicalwidth;
	int vfobufframes;
	int vfobufsamples;
	int framebytes;
	int inbuffersamples;
	int inbufferframes;
	int front, rear;
	int *framepos;
	int frameindex;
	int framei;
	int framesinT;

	int readfront;
};

struct soundvfo sndvfo;

void soundvfo_init(float vfofreq, float vfodepth, int invertphase, snd_pcm_format_t format, unsigned int rate, unsigned int channels, struct soundvfo *v)
{
	v->format = format;
	v->rate = rate;
	v->channels = channels;
	v->vfofreq = vfofreq;
	v->vfodepth = vfodepth;
	v->invertphase = invertphase;

	v->vfobuf = NULL;
	v->rear = v->front = 0;
	v->framepos = NULL;
	v->framei = 0;
	v->frameindex = 0;

	v->readfront = 0;
}

void soundvfo_add(char* inbuffer, int inbuffersize, struct soundvfo *v)
{
	float thetapos, thetaneg, tpos, tneg;

	if (v->enabled)
	{
		if (!v->vfobuf)
		{
			v->physicalwidth = snd_pcm_format_width(v->format);
			v->framebytes = v->physicalwidth / 8 * v->channels;
			v->inbufferframes = inbuffersize / v->framebytes;
			//v->N = ceil(v->inbufferframes * v->vfodepth);
			v->N = ceil((float)v->rate/v->vfofreq*v->vfodepth);
			v->inbuffersamples = v->inbufferframes * v->channels;
			v->vfobufframes = v->inbufferframes + v->N;
			v->vfobuf = malloc(v->vfobufframes * v->framebytes);
			v->vfobufsamples = v->vfobufframes * v->channels;
			memset(v->vfobuf, 0, v->vfobufframes * v->framebytes);
			v->framepos = malloc(2*v->N*sizeof(int));
			int k;
			for(k=1;k<=v->N;k++)
			{
				thetapos = acos(1.0-2.0*k/v->N);
				tpos = thetapos / (2 * M_PI * v->vfofreq);
				v->framepos[k-1] = round(tpos * v->rate);
			}
			for(k=1;k<v->N;k++)
			{
				thetaneg = 2 * M_PI - acos(1.0-2.0*k/v->N);
				tneg = thetaneg / (2 * M_PI * v->vfofreq);
				v->framepos[2*v->N-1-k] = round(tneg * v->rate);
			}
			tneg = 1.0 / v->vfofreq;
			v->framesinT = v->framepos[2*v->N-1] = round(tneg * v->rate);

			v->framepos[2*v->N-1] = v->framesinT - 1;

			if (v->invertphase)
				v->frameindex = v->N;
		}

		int i, j, k, frameposition;
		frameposition = v->framepos[v->frameindex];
		signed short *inshort, *vfshort;
		inshort = (signed short *)inbuffer;
		vfshort = (signed short *)v->vfobuf;
		for(i=0;i<v->inbufferframes;i++)
		{
			if (v->framei >= frameposition) // +- frame at this position
			{
				if (((v->frameindex < v->N) && (!v->invertphase))
				 || ((v->frameindex >= v->N) && (v->invertphase))) // +
				{
					//vfshort[v->rear++] = inshort[i*v->channels]; // L
					//vfshort[v->rear++] = inshort[i*v->channels+1]; // R
					for(j=0,k=i*v->channels;j<v->channels;j++)
						vfshort[v->rear++] = inshort[k++];
					v->rear %= v->vfobufsamples;

					//vfshort[v->rear++] = inshort[i*v->channels]; // L
					//vfshort[v->rear++] = inshort[i*v->channels+1]; // R
					for(j=0,k=i*v->channels;j<v->channels;j++)
						vfshort[v->rear++] = inshort[k++];
					v->rear %= v->vfobufsamples;
				}
				else // -
				{
				}
				v->frameindex++;
				v->frameindex%=2*v->N;
				frameposition = v->framepos[v->frameindex];
			}
			else
			{
				//vfshort[v->rear++] = inshort[i*v->channels]; // L
				//vfshort[v->rear++] = inshort[i*v->channels+1]; // R
				for(j=0,k=i*v->channels;j<v->channels;j++)
					vfshort[v->rear++] = inshort[k++];
				v->rear %= v->vfobufsamples;
			}
			v->framei++;
			v->framei %= v->framesinT;
		}
	}
}

void soundvfo_close(struct soundvfo *v)
{
	if (!v->vfobuf)
	{
		free(v->vfobuf);
		v->vfobuf = NULL;
		free(v->framepos);
		v->framepos = NULL;
	}
}

// Modulator

struct soundmod
{
	snd_pcm_format_t format; // SND_PCM_FORMAT_S16
	unsigned int rate; // sampling rate
	unsigned int channels; // channels
	float modfreq; // modulation frequency
	float moddepth; // modulation depth in percent 0..1.0
	int enabled;
	pthread_mutex_t modmutex; // = PTHREAD_MUTEX_INITIALIZER;

	struct soundvfo v;
};

struct soundmod sndmod;

void soundmod_init(float modfreq, float moddepth, snd_pcm_format_t format, unsigned int rate, unsigned int channels, struct soundmod *m)
{
	m->format = format;
	m->rate = rate;
	m->channels = channels;
	m->modfreq = modfreq;
	m->moddepth = moddepth;
	m->v.enabled = TRUE;
	soundvfo_init(modfreq, moddepth, FALSE, format, rate, channels, &(m->v));
}

void soundmod_add(char* inbuffer, int inbuffersize, struct soundmod *m)
{
	int i, j;
	signed short *inshort, *vfshort;

	pthread_mutex_lock(&(m->modmutex));
	if (m->enabled)
	{
		soundvfo_add(inbuffer, inbuffersize, &(m->v));
		inshort = (signed short *)inbuffer;
		vfshort = (signed short *)m->v.vfobuf;
		for(i=0;i<m->v.inbuffersamples;)
		{
			for(j=0;j<m->channels;j++)
				inshort[i++] = vfshort[m->v.readfront++];
			m->v.readfront %= m->v.vfobufsamples;
		}
	}
	pthread_mutex_unlock(&(m->modmutex));
}

void soundmod_close(struct soundmod *m)
{
	soundvfo_close(&(m->v));
}

void Modulator_initAll(snd_pcm_format_t format, float rate, unsigned int channels, struct soundmod *m)
{
	pthread_mutex_lock(&(m->modmutex));
	soundmod_init((float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton14)), (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton15)), format, rate, channels, m);
	pthread_mutex_unlock(&(m->modmutex));
}

void Modulator_closeAll(struct soundmod *m)
{
	soundmod_close(m);
}

// Chorus
#define MAXCHORUS 3

float chofreq[MAXCHORUS] = {0.307, 0.409, 0.509}; // modulation frequencies
float chodepth[MAXCHORUS] = {0.003, 0.003, 0.003}; // modulation depths in percent 0 .. 1.0

struct soundcho
{
	snd_pcm_format_t format; // SND_PCM_FORMAT_S16
	unsigned int rate; // sampling rate
	unsigned int channels; // channels
	int enabled;
	pthread_mutex_t chomutex; // = PTHREAD_MUTEX_INITIALIZER;

	int maxcho;
	struct soundvfo v[MAXCHORUS];
};

struct soundcho sndchorus;

void soundcho_init(int maxcho, snd_pcm_format_t format, unsigned int rate, unsigned int channels, struct soundcho *c)
{
	int i;

	c->format = format;
	c->rate = rate;
	c->channels = channels;
	c->maxcho = maxcho;
	for(i=0;i<maxcho;i++)
	{
		c->v[i].enabled = TRUE;
		soundvfo_init(chofreq[i], chodepth[i], FALSE, format, rate, channels, &(c->v[i]));
	}
}

void soundcho_add(char* inbuffer, int inbuffersize, struct soundcho *c)
{
	int i, j, k;
	signed short *inshort, *vfshort;
	float prescale = 1.0 / sqrt(c->maxcho+1); // 1/sqrt(N+1);

	pthread_mutex_lock(&(c->chomutex));
	if (c->enabled)
	{
		for(j=0;j<c->maxcho;j++)
		{
			soundvfo_add(inbuffer, inbuffersize, &(c->v[j]));
		}
		inshort = (signed short *)inbuffer;
		for(i=0;i<c->v[0].inbuffersamples;)
			inshort[i++] *= prescale;
		for(j=0;j<c->maxcho;j++)
		{
			vfshort = (signed short *)c->v[j].vfobuf;
			for(i=0;i<c->v[j].inbuffersamples;)
			{
				for(k=0;k<c->channels;k++)
					inshort[i++] += vfshort[c->v[j].readfront++]*prescale;
				c->v[j].readfront %= c->v[j].vfobufsamples;
			}
		}
	}
	pthread_mutex_unlock(&(c->chomutex));
}

void soundcho_close(struct soundcho *c)
{
	int j;
	for(j=0;j<c->maxcho;j++)
	{
		soundvfo_close(&(c->v[j]));
	}
}

void Chorus_initAll(snd_pcm_format_t format, float rate, unsigned int channels, struct soundcho *c)
{
	pthread_mutex_lock(&(c->chomutex));
	soundcho_init((int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton16)), format, rate, channels, c);
	pthread_mutex_unlock(&(c->chomutex));
}

void Chorus_closeAll(struct soundcho *c)
{
	pthread_mutex_lock(&(c->chomutex));
	soundcho_close(c);
	pthread_mutex_unlock(&(c->chomutex));
}

// Mono

struct soundmono
{
	snd_pcm_format_t format; // SND_PCM_FORMAT_S16
	unsigned int rate; // sampling rate
	unsigned int channels; // channels
	float threshold;
	float gain;

	pthread_mutex_t monomutex; // = PTHREAD_MUTEX_INITIALIZER;
	int initialized;
	int enabled;
	int physicalwidth;
	int insamples;
	float prescale;
};

struct soundmono sndmono1, sndmono2;

void soundmono_init(snd_pcm_format_t format, unsigned int rate, unsigned int channels, struct soundmono *m)
{
	m->format = format;
	m->rate = rate;
	m->channels = channels;

	m->initialized = FALSE;
	m->prescale = 1.0/sqrt(m->channels);
}

void soundmono_add(char* inbuffer, int inbuffersize, struct soundmono *m)
{
	pthread_mutex_lock(&(m->monomutex));
	if (m->enabled)
	{
		if (!m->initialized)
		{
			m->physicalwidth = snd_pcm_format_width(m->format); // bits per sample
			m->insamples = inbuffersize / m->physicalwidth * 8;
			m->initialized = TRUE;
			//printf("init %d %d %d %d\n", inbuffersize, m->format, m->physicalwidth, m->insamples);
		}
		signed short *inshort = (signed short *)inbuffer;
		int i,j;
		for(i=0;i<m->insamples;)
		{
			signed short value = 0;
			for(j=0;j<m->channels;j++) value+=inshort[i+j]*m->prescale;
			for(j=0;j<m->channels;j++) inshort[i++]=value;
		}
	}
	pthread_mutex_unlock(&(m->monomutex));
}

void soundmono_close(struct soundmono *m)
{
}

void Mono_initAll(snd_pcm_format_t format, float rate, unsigned int channels, struct soundmono *m)
{
	pthread_mutex_lock(&(m->monomutex));
	soundmono_init(format, rate, channels, m);
	pthread_mutex_unlock(&(m->monomutex));
}

void Mono_closeAll(struct soundmono *m)
{
	pthread_mutex_lock(&(m->monomutex));
	soundmono_close(m);
	pthread_mutex_unlock(&(m->monomutex));
}

// Haas

struct soundhaas
{
	snd_pcm_format_t format; // SND_PCM_FORMAT_S16
	unsigned int rate; // sampling rate
	unsigned int channels; // channels
	float threshold;
	float gain;

	pthread_mutex_t haasmutex; // = PTHREAD_MUTEX_INITIALIZER;
	int initialized;
	int enabled;
	int physicalwidth;
	int insamples;

	struct sounddelay haasdly;
};

struct soundhaas sndhaas;

void soundhaas_init(float millisec, snd_pcm_format_t format, unsigned int rate, unsigned int channels, struct soundhaas *h)
{
	h->format = format;
	h->rate = rate;
	h->channels = channels;

	h->initialized = FALSE;
	h->haasdly.enabled = TRUE;
	sounddelay_init(1, DLY_LATE, millisec, 1.0, format, rate, channels, &(h->haasdly));
}

void soundhaas_add(char* inbuffer, int inbuffersize, struct soundhaas *h)
{
	pthread_mutex_lock(&(h->haasmutex));
	if (h->enabled)
	{
		if (!h->initialized)
		{
			h->physicalwidth = snd_pcm_format_width(h->format); // bits per sample
			h->insamples = inbuffersize / h->physicalwidth * 8;
			h->initialized = TRUE;
		}
		sounddelay_add(inbuffer, inbuffersize, &(h->haasdly));
		signed short *inshort = (signed short *)inbuffer;
		signed short *fbuffer = h->haasdly.fshort;
		int i;
		for(i=0;i<h->insamples;)
		{
			inshort[i++] *= 0.7; h->haasdly.readfront++; // rescale left channel
			inshort[i++] = fbuffer[h->haasdly.readfront++]; // Haas effect on right channel
			h->haasdly.readfront%=h->haasdly.fbuffersamples;
		}
	}
	pthread_mutex_unlock(&(h->haasmutex));
}

void soundhaas_close(struct soundhaas *h)
{
	sounddelay_close(&(h->haasdly));
}

void Haas_initAll(snd_pcm_format_t format, float rate, unsigned int channels, struct soundhaas *h)
{
	pthread_mutex_lock(&(h->haasmutex));
	soundhaas_init((float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton17)), format, rate, channels, h);
	pthread_mutex_unlock(&(h->haasmutex));
}

void Haas_closeAll(struct soundhaas *h)
{
	pthread_mutex_lock(&(h->haasmutex));
	soundhaas_close(h);
	pthread_mutex_unlock(&(h->haasmutex));
}

// ADT


// Flanger

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
                        printf("Can't recover from underrun, prepare failed: %s\n", snd_strerror(err));
                return 0;
        } else if (err == -ESTRPIPE) {
                while ((err = snd_pcm_resume(handle)) == -EAGAIN)
                        sleep(1);       /* wait until the suspend flag is released */
                if (err < 0) {
                        err = snd_pcm_prepare(handle);
                        if (err < 0)
                                printf("Can't recover from suspend, prepare failed: %s\n", snd_strerror(err));
                }
                return 0;
        }
        return err;
}

gboolean setLevel8(gpointer data);

int play_period(snd_pcm_t *handle, struct audioqueue *p)
{
	snd_pcm_sframes_t avail;
	int err, framecount;

	//printf("Audio %d\n", gettid());
	//write_status(snd_pcm_state(handle));
	avail = snd_pcm_avail_update(handle);
	if (avail);
//printf("avail: %d audio playing %lld, bufsize %d\n", (int)avail, p->label, p->dst_bufsize);

	framecount = 0;
	if (p->dst_data)
	{
		if (p->dst_data[0])
		{
			framecount = p->dst_bufsize/(snd_pcm_format_width(format)/8*channels); // in frames
			if (framecount)
			{
				BiQuad_process(p->dst_data[0], p->dst_bufsize, snd_pcm_format_width(format)/8*channels);
				sounddelay_add((char*)p->dst_data[0], (int)p->dst_bufsize, &snddly);
				soundreverb_add((char*)p->dst_data[0], (int)p->dst_bufsize, &sndreverb);
				soundtremolo_add((char*)p->dst_data[0], (int)p->dst_bufsize, &sndtremolo);
				soundfoldingdistort_add((char*)p->dst_data[0], (int)p->dst_bufsize, &sndflddistort);
				soundmod_add((char*)p->dst_data[0], (int)p->dst_bufsize, &sndmod);
				soundcho_add((char*)p->dst_data[0], (int)p->dst_bufsize, &sndchorus);
				soundmono_add((char*)p->dst_data[0], (int)p->dst_bufsize, &sndmono1);
				soundhaas_add((char*)p->dst_data[0], (int)p->dst_bufsize, &sndhaas);
				soundmono_add((char*)p->dst_data[0], (int)p->dst_bufsize, &sndmono2);

				err = snd_pcm_writei(handle, p->dst_data[0], framecount);
			}
			av_freep(&(p->dst_data[0]));
		}
		av_freep(&(p->dst_data));

		if (!videoduration) // no video stream
		{
			now_playing_frame += framecount;
			if (!(p->label%10))
				gdk_threads_add_idle(update_hscale, NULL);
		}

		if (!(p->label%10))
			gdk_threads_add_idle(setLevel8, &aqLength);
	}
	else
		err = 0;

	av_packet_unref(p->packet);
	av_free(p->packet);
	free(p);

	if (err == -EAGAIN)
	{
		printf("EAGAIN\n");
		return err;
	}
	if (err < 0)
	{
		if (xrun_recovery(handle, err) < 0)
		{
			printf("Write error: %s\n", snd_strerror(err));
		}
	}
	return err;
}

static int write_loop(snd_pcm_t *handle, signed short *samples, snd_pcm_channel_area_t *areas)
{
	struct audioqueue *p;
	int err;

	while(1)
	{
		if ((p = aq_remove(&aq)) == NULL)
		{
			break;
		}

// play 1 period
        err=play_period(handle, p);
        if (err);
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
                printf("Unable to set buffer size %d for playback: %s\n", (int)buffer_size, snd_strerror(err));
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
	//channels = (ctx->channels>2?2:ctx->channels); // use when device = "default"
	channels = ctx->channels; // use when device = "plughw:0,0"

	snd_pcm_hw_params_alloca(&hwparams);
	snd_pcm_sw_params_alloca(&swparams);

	if ((err = snd_output_stdio_attach(&output, stdout, 0)) < 0)
	{
		printf("Output failed: %s\n", snd_strerror(err));
		return err;
	}
//	printf("Playback device is %s\n", device);
//	printf("Stream parameters are %iHz, %s, %i channels\n", rate, snd_pcm_format_name(format), channels);
//	printf("Using transfer method: %s\n", transfer_methods[method].name);
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
	int64_t label;
};
struct videoqueue *vq = NULL;
int vqLength;
int vqMaxLength = 20;
pthread_mutex_t vqmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t vqlowcond = PTHREAD_COND_INITIALIZER;
pthread_cond_t vqhighcond = PTHREAD_COND_INITIALIZER;

void vq_init(struct videoqueue **q, pthread_mutex_t *m, pthread_cond_t *cl, pthread_cond_t *ch)
{
	int ret;

	*q = NULL;
	vqLength = 0;

	pthread_mutex_destroy(m);
	pthread_cond_destroy(cl);
	pthread_cond_destroy(ch);

	if((ret=pthread_mutex_init(m, NULL))!=0 )
		printf("mutex init failed, %d\n", ret);

	if((ret=pthread_cond_init(ch, NULL))!=0 )
		printf("cond init failed, %d\n", ret);

	if((ret=pthread_cond_init(cl, NULL))!=0 )
		printf("cond init failed, %d\n", ret);
}

void vq_add(struct videoqueue **q,  AVPacket *packet, char *rgba, int64_t label)
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
		if ((playerstatus==playing) || (playerstatus==paused))
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
		case paused:
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
	int ret;

	pthread_mutex_destroy(m);
	pthread_cond_destroy(cl);
	pthread_cond_destroy(ch);

	if((ret=pthread_mutex_init(m, NULL))!=0 )
		printf("destroy, mutex init failed, %d\n", ret);

	if((ret=pthread_cond_init(ch, NULL))!=0 )
		printf("destroy,cond init failed, %d\n", ret);

	if((ret=pthread_cond_init(cl, NULL))!=0 )
		printf("destroy,cond init failed, %d\n", ret);
}

int getNumberOfCpus( void )
{
    long nprocs       = -1;
    long nprocs_max   = -1;

# ifdef _SC_NPROCESSORS_ONLN
    nprocs = sysconf( _SC_NPROCESSORS_ONLN );
    if ( nprocs < 1 )
    {
        printf("Could not determine number of CPUs on line\n");
        return 0;
    }
    nprocs_max = sysconf( _SC_NPROCESSORS_CONF );
    if ( nprocs_max < 1 )
    {
        printf("Could not determine number of CPUs in host. Error is\n");
        return 0;
    }

    printf("nprocs %ld, nprocs_max %ld\n", nprocs, nprocs_max);
    return nprocs; 
#else
    printf("Could not determine number of CPUs\n");
    return 0;
#endif
}

void list_uniforms_of_program()
{
	// List uniforms of program
	UserData *userData = p_state->user_data;
	int i;
	int total = -1;
	glGetProgramiv( userData->programObject, GL_ACTIVE_UNIFORMS, &total ); 
	for(i=0; i<total; ++i)
	{
		int name_len=-1, num=-1;
		GLenum type = GL_ZERO;
		char name[100];
		glGetActiveUniform( userData->programObject, (GLuint)i, sizeof(name)-1, &name_len, &num, &type, name );
		name[name_len] = 0;
		GLuint location = glGetUniformLocation( userData->programObject, name );
		printf("%s, %d\n", name, location);
	}
}

char* strreplace(char *src, char *search, char *replace)
{
	char *p;
	char *front;
	char *dest;

	dest = (char*)malloc(1);
	dest[0] = '\0';
	for(front = p = src; (p = strstr(p, search)); front = p)
	{
		p[0] = '\0';
		//printf("%s\n", front);
		dest = (char*)realloc(dest, strlen(dest)+strlen(front)+1);
		strcat(dest, front);

		dest = (char*)realloc(dest, strlen(dest)+strlen(replace)+1);
		strcat(dest, replace);

		p += strlen(search);
	}
	//printf("%s\n", front);
	dest = (char*)realloc(dest, strlen(dest)+strlen(front)+1);
	strcat(dest, front);
	
	//printf("\n%s\n", dest);
	return dest;
}

char* strlastpart(char *src, char *search, int lowerupper)
{
	char *p;
	char *q;
	int i;

	q = &src[strlen(src)];
	for(p = src; (p = strstr(p, search)); p += strlen(search))
	{
		q = p;
	}
	switch (lowerupper)
	{
		case 0:
			break;
		case 1:
			for(i=0;q[i];i++) q[i] = tolower(q[i]);
			break;
		case 2:
			for(i=0;q[i];i++) q[i] = toupper(q[i]);
			break;
	}
	
	return q;
}

int nomediafile(char *filepath)
{
	return
	(
		strcmp(strlastpart(filepath, ".", 1), ".mp4") &&
		strcmp(strlastpart(filepath, ".", 1), ".mp3") &&
		strcmp(strlastpart(filepath, ".", 1), ".mov") &&
		strcmp(strlastpart(filepath, ".", 1), ".mkv")
	);
}

void destroynotify(guchar *pixels, gpointer data)
{
//printf("destroy notify\n");
	free(pixels);
}

gboolean invalidate(gpointer data)
{
	GdkWindow *dawin = gtk_widget_get_window(dwgarea);
	cairo_region_t *region = gdk_window_get_clip_region(dawin);
	gdk_window_invalidate_region (dawin, region, TRUE);
	//gdk_window_process_updates (dawin, TRUE);
	cairo_region_destroy(region);
	return FALSE;
}

void initPixbuf(int width, int height)
{
	int i;

	guchar *imgdata = malloc(width*height*4); // RGBA
	for(i=0;i<width*height;i++)
	{
		((unsigned int *)imgdata)[i]=0xFF000000; // ABGR
	}
	g_mutex_lock(&pixbufmutex);
	if (pixbuf)
		g_object_unref(pixbuf);
    pixbuf = gdk_pixbuf_new_from_data(imgdata, GDK_COLORSPACE_RGB, TRUE, 8, width, height, width*4, destroynotify, NULL);
	g_mutex_unlock(&pixbufmutex);
	gdk_threads_add_idle(invalidate, NULL);
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
	{
		printf("avformat_open_input()\n");
		return -1; // Couldn't open file
	}
  
    // Retrieve stream information
	if(avformat_find_stream_info(pFormatCtx, NULL)<0)
	{
		printf("avformat_find_stream_info()\n");
		return -1; // Couldn't find stream information
	}
  
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
	if (videoStream==-1)
	{
//printf("No video stream found\n");
		videoduration = 0;
//		return -1; // Didn't find a video stream
	}
	else
	{
		// Get a pointer to the codec context for the video stream
		pCodecCtx=pFormatCtx->streams[videoStream]->codec;

		// Find the decoder for the video stream
		pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
		if(pCodec==NULL)
		{
			printf("Unsupported video codec\n");
			return -1; // Codec not found
		}

		pCodecCtx->thread_count = thread_count;
		// Open codec
		if(avcodec_open2(pCodecCtx, pCodec, &optionsDict)<0)
		{
			printf("Video avcodec_open2()\n");
			return -1; // Could not open video codec
		}

		AVStream *st = pFormatCtx->streams[videoStream];
		if (st->avg_frame_rate.den)
		{
			frame_rate = st->avg_frame_rate.num / (double)st->avg_frame_rate.den;
			frametime = 1000000 / frame_rate; // usec
		}
		else
		{
			frame_rate = 0;
			frametime = 1000000; // 1s
		}
//printf("Frame rate = %2.2f\n", frame_rate);
//printf("frametime = %d usec\n", frametime);
//printf("Width : %d, Height : %d\n", pCodecCtx->width, pCodecCtx->height);
		videoduration = (pFormatCtx->duration / AV_TIME_BASE) * frame_rate; // in frames
	}

	audioStream = -1;
	for(i=0; i<pFormatCtx->nb_streams; i++)
	{
		if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO)
		{
			audioStream = i;
			break;
		}
	}
	if (audioStream==-1)
	{
//printf("No audio stream found\n");
//		return -1; // Didn't find an audio stream
	}
	else
	{
		// Get a pointer to the codec context for the audio stream
		pCodecCtxA=pFormatCtx->streams[audioStream]->codec;

		// Find the decoder for the audio stream
		pCodecA=avcodec_find_decoder(pCodecCtxA->codec_id);

		if(pCodecA==NULL)
		{
			printf("Unsupported audio codec!\n");
			return -1; // Codec not found
		}

		// Open codec
		if(avcodec_open2(pCodecCtxA, pCodecA, &optionsDictA)<0)
		{
			printf("Audio avcodec_open2()\n");
			return -1; // Could not open audio codec
		}

		// Set up SWR context once you've got codec information
		swr = swr_alloc();
		av_opt_set_int(swr, "in_channel_layout",  pCodecCtxA->channel_layout, 0);
		av_opt_set_int(swr, "out_channel_layout", pCodecCtxA->channel_layout,  0);
		av_opt_set_int(swr, "in_sample_rate",     pCodecCtxA->sample_rate, 0);
		av_opt_set_int(swr, "out_sample_rate",    pCodecCtxA->sample_rate, 0);
		av_opt_set_sample_fmt(swr, "in_sample_fmt",  pCodecCtxA->sample_fmt, 0);

		av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_S16,  0);
		swr_init(swr);

		sample_rate = pCodecCtxA->sample_rate;
		audioduration = (pFormatCtx->duration / AV_TIME_BASE) * sample_rate; // in samples
		//printf("audio frame count %lld\n", audioduration);
/*
		int64_t last_ts = pFormatCtx->duration / AV_TIME_BASE;
		last_ts = (last_ts * (pFormatCtx->streams[audioStream]->time_base.den)) / (pFormatCtx->streams[audioStream]->time_base.num);
		printf("timebase last_ts %lld\n", last_ts);
*/
	}

	//initialize ALSA lib
	init_sound(pCodecCtxA);

	BiQuad_initAll();

	// initialize Delay Processor
	Delay_initAll(SND_PCM_FORMAT_S16, pCodecCtxA->sample_rate, pCodecCtxA->channels, &snddly);
	Reverb_initAll(SND_PCM_FORMAT_S16, pCodecCtxA->sample_rate, pCodecCtxA->channels, &sndreverb);
	Tremolo_initAll(SND_PCM_FORMAT_S16, pCodecCtxA->sample_rate, pCodecCtxA->channels, &sndtremolo);
	FoldingDistortion_initAll(SND_PCM_FORMAT_S16, pCodecCtxA->sample_rate, pCodecCtxA->channels, &sndflddistort);
	Modulator_initAll(SND_PCM_FORMAT_S16, pCodecCtxA->sample_rate, pCodecCtxA->channels, &sndmod);
	Chorus_initAll(SND_PCM_FORMAT_S16, pCodecCtxA->sample_rate, pCodecCtxA->channels, &sndchorus);
	Mono_initAll(SND_PCM_FORMAT_S16, pCodecCtxA->sample_rate, pCodecCtxA->channels, &sndmono1);
	Haas_initAll(SND_PCM_FORMAT_S16, pCodecCtxA->sample_rate, pCodecCtxA->channels, &sndhaas);
	Mono_initAll(SND_PCM_FORMAT_S16, pCodecCtxA->sample_rate, pCodecCtxA->channels, &sndmono2);
	return 0;
}

gboolean enable_play_button(gpointer data)
{
	gtk_widget_set_sensitive(button2, FALSE);
	gtk_widget_set_sensitive(button8, FALSE);
	gtk_widget_set_sensitive(button9, FALSE);
	gtk_widget_set_sensitive(button10, FALSE);
	gtk_widget_set_sensitive(button1, TRUE);
	return FALSE;
}

gboolean setLevel1(gpointer data)
{
	gtk_level_bar_set_value(GTK_LEVEL_BAR(levelbar1), *((int*)data));
	sprintf(leveltext1, "%d", *((int*)data));
	gtk_label_set_text(GTK_LABEL(label1), leveltext1);
	return FALSE;
}
gboolean setLevel2(gpointer data)
{
	gtk_level_bar_set_value(GTK_LEVEL_BAR(levelbar2), *((int*)data));
	sprintf(leveltext2, "%d", *((int*)data));
	gtk_label_set_text(GTK_LABEL(label2), leveltext2);
	return FALSE;
}
gboolean setLevel3(gpointer data)
{
	gtk_level_bar_set_value(GTK_LEVEL_BAR(levelbar3), *((int*)data));
	sprintf(leveltext3, "%d", *((int*)data));
	gtk_label_set_text(GTK_LABEL(label3), leveltext3);
	return FALSE;
}
gboolean setLevel4(gpointer data)
{
	gtk_level_bar_set_value(GTK_LEVEL_BAR(levelbar4), *((int*)data));
	sprintf(leveltext4, "%d", *((int*)data));
	gtk_label_set_text(GTK_LABEL(label4), leveltext4);
	return FALSE;
}
gboolean setLevel5(gpointer data)
{
	gtk_level_bar_set_value(GTK_LEVEL_BAR(levelbar5), *((int*)data));
	sprintf(leveltext5, "%d", *((int*)data));
	gtk_label_set_text(GTK_LABEL(label5), leveltext5);
	return FALSE;
}
gboolean setLevel6(gpointer data)
{
	gtk_level_bar_set_value(GTK_LEVEL_BAR(levelbar6), *((int*)data));
	sprintf(leveltext6, "%d", *((int*)data));
	gtk_label_set_text(GTK_LABEL(label6), leveltext6);
	return FALSE;
}
gboolean setLevel7(gpointer data)
{
	gtk_level_bar_set_value(GTK_LEVEL_BAR(levelbar7), *((float*)data));
	sprintf(leveltext7, "%3.2f %%", *((float*)data));
	gtk_label_set_text(GTK_LABEL(label7), leveltext7);
	return FALSE;
}
gboolean setLevel8(gpointer data)
{
	sprintf(leveltext8, "%d", *((int*)data));
	gtk_label_set_text(GTK_LABEL(parmlabel12), leveltext8);
	return FALSE;
}
gboolean setLevel9(gpointer data)
{
	sprintf(leveltext9, "%d", *((int*)data));
	gtk_label_set_text(GTK_LABEL(parmlabel22), leveltext9);
	return FALSE;
}

void resetLevels()
{
	diff1 = diff2 = diff3 = diff4 = diff5 = diff6 = 0;
	diff7 = 0.0;
	gdk_threads_add_idle(setLevel1, &diff1);
	gdk_threads_add_idle(setLevel2, &diff2);
	gdk_threads_add_idle(setLevel3, &diff3);
	gdk_threads_add_idle(setLevel4, &diff4);
	gdk_threads_add_idle(setLevel5, &diff5);
	gdk_threads_add_idle(setLevel6, &diff6);
	gdk_threads_add_idle(setLevel7, &diff7);
}

gboolean play_prev(gpointer data);
gboolean play_next(gpointer data);

static gpointer read_frames(gpointer args)
{
	int ctype = PTHREAD_CANCEL_ASYNCHRONOUS;
	int ctype_old;
	pthread_setcanceltype(ctype, &ctype_old);

    int frameFinished, i;

//printf("starting read_frames\n");
    AVFrame *pFrame = NULL;
    pFrame=av_frame_alloc();

    /* initialize packet, set data to NULL, let the demuxer fill it */
    AVPacket *packet;

    packet=av_malloc(sizeof(AVPacket));
//printf("AVPacket av_malloc %d\n", sizeof(AVPacket));
    av_init_packet(packet);
    packet->data = NULL;
    packet->size = 0;

	now_decoding_frame = now_playing_frame = 0;
    int64_t fnum, aperiod=0;

	char *rgba;

get_first_time_microseconds_2();
    while ((av_read_frame(pFormatCtx, packet)>=0) && (!stoprequested))
    {
diff1=get_next_time_microseconds_2();
//printf("%lu usec av_read_frame\n", diff);
		//printf("Readframes %d\n", gettid());
		if (!(now_playing_frame%10))
			gdk_threads_add_idle(setLevel1, &diff1);

		if (packet->stream_index==videoStream) 
		{
//printf("videoStream %d\n", now_decoding_frame); printf("vqlength %d\n", vqLength);
get_first_time_microseconds_2();
			av_frame_unref(pFrame);
			avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, packet); // Decode video frame
diff2=get_next_time_microseconds_2();
//printf("%lu usec avcodec_decode_video2\n", diff2);
			if (!(now_playing_frame%10))
				gdk_threads_add_idle(setLevel2, &diff2);

			if (frameFinished)
			{
				if (!videoduration) // no video stream
				{
					AVFrame *pFrameRGB = NULL;
					pFrameRGB = av_frame_alloc();
					av_frame_unref(pFrameRGB);

					uint8_t *rgbbuffer = NULL;
					int numBytes = avpicture_get_size(AV_PIX_FMT_RGB32, playerWidth, playerHeight);
					//printf("numbytes %d\n", numBytes);

					rgbbuffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
					avpicture_fill((AVPicture *)pFrameRGB, rgbbuffer, AV_PIX_FMT_RGB32, playerWidth, playerHeight);

					struct SwsContext *sws_ctx = sws_getContext( pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
					playerWidth, playerHeight, AV_PIX_FMT_RGB32, SWS_BILINEAR, NULL, NULL, NULL);

					get_first_time_microseconds_2();
					//printf("sws_scale %d %d\n", playerWidth, playerHeight);
					sws_scale(sws_ctx, (uint8_t const* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
					//printf("sws_scale done\n");
					diff4=get_next_time_microseconds_2();
					gdk_threads_add_idle(setLevel4, &diff4);

					sws_freeContext(sws_ctx);

					rgba = malloc(pFrameRGB->linesize[0] * playerHeight);
					//printf("malloc %d %d\n", pFrameRGB->linesize[0], playerHeight);
					memcpy(rgba, pFrameRGB->data[0], pFrameRGB->linesize[0] * playerHeight);
					//printf("memcpy %d\n", pFrameRGB->linesize[0] * playerHeight);

					av_free(rgbbuffer);
					//printf("av_free\n");
					av_frame_unref(pFrameRGB);
					//printf("av_frame_unref\n");
					av_frame_free(&pFrameRGB);
				}
				else
				{
					int width = pCodecCtx->width;
					int height = pCodecCtx->height;
					rgba = malloc(width * height*3/2);
					memcpy(&rgba[0], pFrame->data[0], width*height); //y
					memcpy(&rgba[width*height], pFrame->data[1], width*height/4); //u
					memcpy(&rgba[width*height*5/4], pFrame->data[2], width*height/4); //v
				}

				pthread_mutex_lock(&framemutex);
				if (!videoduration)
					fnum = now_decoding_frame = 0;
				else
					fnum = now_decoding_frame++;
				pthread_mutex_unlock(&framemutex);
				vq_add(&vq, packet, rgba, fnum);

				pthread_mutex_lock(&seekmutex);
				pthread_mutex_unlock(&seekmutex);
			}
		}
		else if (packet->stream_index==audioStream)
		{
//printf("audioStream %d\n", i); printf("aqlength %d\n", aqLength);
			aq_add(&aq, packet, aperiod++);

			pthread_mutex_lock(&seekmutex);
			pthread_mutex_unlock(&seekmutex);
		}
		packet=av_malloc(sizeof(AVPacket));
//printf("AVPacket av_malloc %d\n", sizeof(AVPacket));
		av_init_packet(packet);
		packet->data = NULL;
		packet->size = 0;
get_first_time_microseconds_2();

	}

	av_frame_free(&pFrame);
//printf("av_frame_free\n");

	av_packet_unref(packet);
//printf("av_packet_unref\n");
	av_free(packet);

	if (videoStream!=-1)
	{
		avcodec_flush_buffers(pCodecCtx);
		avcodec_close(pFormatCtx->streams[videoStream]->codec);
		//av_free(pCodecCtx);
	}
	if (audioStream!=-1)
	{
		avcodec_flush_buffers(pCodecCtxA);
		avcodec_close(pFormatCtx->streams[audioStream]->codec);
		//av_free(pCodecCtxA);
	}

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

	if (audioStream!=-1)
		if ((i=pthread_join(tid[0], NULL)))
			printf("pthread_join error, tid[0], %d\n", i);
	if (videoStream!=-1)
		if ((i=pthread_join(tid[1], NULL)))
			printf("pthread_join error, tid[1], %d\n", i);

	aq_destroy(&aqmutex, &aqlowcond, &aqhighcond);
	vq_destroy(&vqmutex, &vqlowcond, &vqhighcond);

	pthread_mutex_destroy(&seekmutex);
	pthread_mutex_destroy(&framemutex);

	Delay_closeAll(&snddly);
	Reverb_closeAll(&sndreverb);
	Tremolo_closeAll(&sndtremolo);
	FoldingDistortion_closeAll(&sndflddistort);
	Modulator_closeAll(&sndmod);
	Chorus_closeAll(&sndchorus);
	Mono_closeAll(&sndmono1);
	Haas_closeAll(&sndhaas);
	Mono_closeAll(&sndmono2);
	close_sound();

	free(swr);

	if (!stoprequested)
		gdk_threads_add_idle(enable_play_button, NULL);

	playerstatus = idle;

	if (!stoprequested)
		gdk_threads_add_idle(play_next, NULL);

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

//printf("starting audioPlayFromQueue\n");
	if((err = transfer_methods[method].transfer_loop(handle, samples, areas)) < 0)
		printf("Transfer failed: %s\n", snd_strerror(err));
//printf("exiting audioPlayFromQueue\n");
	retval_audio = 0;
	pthread_exit((void*)&retval_audio);
}

void* videoPlayFromQueue(void *arg)
{
	struct videoqueue *p;
	UserData *userData;
	long diff = 0;
	long framesskipped = 0;

	int ctype = PTHREAD_CANCEL_ASYNCHRONOUS;
	int ctype_old;
	pthread_setcanceltype(ctype, &ctype_old);

//printf("starting videoPlayFromQueue\n");
    bcm_host_init();

    //init_ogl(p_state); // Render to screen
    init_ogl2(p_state); // Render to Frame Buffer
    if (Init(p_state) == GL_FALSE)
    {
		printf("Init\n");
		exit(0);
	}

	int width = pCodecCtx->width;
	int height = pCodecCtx->height;
	userData = p_state->user_data;
    setSize(width/4, height*3/2);

	initPixbuf(p_state->screen_width, p_state->screen_height);
	userData->outrgb = (char*)gdk_pixbuf_get_pixels(pixbuf);

	GLfloat picSize[2] = { (GLfloat)width, (GLfloat)height*3/2 };
	glUniform2fv(userData->sizeLoc, 1, picSize);
	GLfloat yuv2rgbmatrix[9] = { 1.0, 0.0, 1.5958, 1.0, -0.3917, -0.8129, 1.0, 2.017, 0.0 };
	glUniformMatrix3fv(userData->cmatrixLoc, 1, FALSE, yuv2rgbmatrix);

	while (1)
	{
		if ((p = vq_remove(&vq)) == NULL)
			break;
//printf("Video %d\n", gettid());

		if (videoduration)
			now_playing_frame = p->label;

		if (!(p->label%10))
			gdk_threads_add_idle(setLevel9, &vqLength);

		if (diff > 0)
		{
			diff-=frametime;
			if (diff<0)
			{
				usleep(0-diff);
				diff = 0;
			}
//printf("skip %lld\n", p->label);
			framesskipped++;
			diff7 = (float)(framesskipped*100)/(float)now_playing_frame;
			gdk_threads_add_idle(setLevel7, &diff7);

			free(p->rgba);
//printf("free rgba\n");
			av_packet_unref(p->packet);
//printf("av_packet_unref\n");
			av_free(p->packet);
			free(p);
//printf("free vq\n");

			continue;
		}

		if (!videoduration) // No video stream || single jpeg album art with mp3
		{
			g_mutex_lock(&pixbufmutex);
			//printf("pixbuf memcpy\n");
			memcpy(userData->outrgb, p->rgba, p_state->screen_width*p_state->screen_height*4);
			//printf("pixbuf memcpy %d %d\n", p_state->screen_width, p_state->screen_height);
			g_mutex_unlock(&pixbufmutex);
			diff3 = diff4 = diff5 = 0;
			gdk_threads_add_idle(invalidate, NULL);
		}
		else
		{
			get_first_time_microseconds();
			texImage2D(p->rgba, width/4, height*3/2);
			diff3=get_next_time_microseconds();
//printf("%lu usec glTexImage2D\n", diff3);

			get_first_time_microseconds();
			redraw_scene(p_state);
			diff4=get_next_time_microseconds();
//printf("%lu usec redraw\n", diff4);

			get_first_time_microseconds();

			g_mutex_lock(&pixbufmutex);
			glReadPixels(0, 0, p_state->screen_width, p_state->screen_height, GL_RGBA, GL_UNSIGNED_BYTE, userData->outrgb);
			g_mutex_unlock(&pixbufmutex);

			diff5=get_next_time_microseconds();
//printf("%lu usec glReadPixels\n", diff5);

		//checkNoGLES2Error();

			if (!(now_playing_frame%10))
			{
				gdk_threads_add_idle(setLevel3, &diff3);
				gdk_threads_add_idle(setLevel4, &diff4);
				gdk_threads_add_idle(setLevel5, &diff5);
				gdk_threads_add_idle(update_hscale, NULL);
			}
		}

		free(p->rgba);
//printf("free rgba\n");
		av_packet_unref(p->packet);
//printf("av_packet_unref\n");
		av_free(p->packet);
		free(p);
//printf("free vq\n");

		diff = diff3 + diff4 + diff5 + 2000;
		//printf("%5lu usec frame, frametime %5d\n", diff, frametime);
		diff -= frametime;
		gdk_threads_add_idle(invalidate, NULL);
		if (diff<0)
		{
			usleep(0-diff);
			diff = 0;
		}
	}
	exit_func();

//printf("exiting videoPlayFromQueue\n");
	retval_video = 0;
	pthread_exit((void*)&retval_video);
}

void create_threads_av()
{
	int err;

	if (audioStream!=-1)
	{
		err = pthread_create(&(tid[0]), NULL, &audioPlayFromQueue, NULL);
		if (err)
		{}
		CPU_ZERO(&(cpu[1]));
		CPU_SET(1, &(cpu[1]));
		err = pthread_setaffinity_np(tid[0], sizeof(cpu_set_t), &(cpu[1]));
		if (err)
		{}
	}

	if (videoStream!=-1)
	{
		err = pthread_create(&(tid[1]), NULL, &videoPlayFromQueue, NULL);
		if (err)
		{}
		CPU_ZERO(&(cpu[2]));
		CPU_SET(2, &(cpu[2]));
		err = pthread_setaffinity_np(tid[1], sizeof(cpu_set_t), &(cpu[2]));
		if (err)
		{}
	}
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

/* Redraw the screen from the surface. Note that the ::draw
 * signal receives a ready-to-be-used cairo_t that is already
 * clipped to only draw the exposed areas of the widget
 */
static gboolean draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
//printf("Draw %d\n", gettid());
	get_first_time_microseconds_1();

	g_mutex_lock(&pixbufmutex);
	//cr = gdk_cairo_create (gtk_widget_get_window(dwgarea));
	gdk_cairo_set_source_pixbuf(cr, pixbuf, 0, 0);
	cairo_paint(cr);
	//cairo_destroy(cr);
	g_mutex_unlock(&pixbufmutex);

	diff6=get_next_time_microseconds_1();
//printf("%lu usec cairo draw\n", diff);
	if (!(now_playing_frame%10))
		gdk_threads_add_idle(setLevel6, &diff6);

    return FALSE;
}

static void push_message(GtkWidget *widget, gint cid, char *msg)
{
  gchar *buff = g_strdup_printf("%s", msg);
  gtk_statusbar_push(GTK_STATUSBAR(statusbar), cid, buff);
  g_free (buff);
}

static void pop_message(GtkWidget *widget, gint cid)
{
  gtk_statusbar_pop(GTK_STATUSBAR(widget), cid);
}

void point2comma(char *c)
{
	int i;

	for(i=0;c[i];i++)
		if (c[i]=='.')
			c[i]=',';
}

int select_eqpreset_callback(void *NotUsed, int argc, char **argv, char **azColName) 
{
	char* errCheck;
	double f;

	point2comma(argv[1]);
	f=strtod(argv[1], &errCheck);
	if (errCheck == argv[1])
	{
		f=0.0;
		printf("Conversion error %s %s\n", azColName[1], argv[1]);
	}
	gtk_adjustment_set_value(vadj0, -f);

	point2comma(argv[2]);
	f=strtod(argv[2], &errCheck);
	if (errCheck == argv[2])
	{
		f=0.0;
		printf("Conversion error %s %s\n", azColName[2], argv[2]);
	}
	gtk_adjustment_set_value(vadj1, -f);

	point2comma(argv[3]);
	f=strtod(argv[3], &errCheck);
	if (errCheck == argv[3])
	{
		f=0.0;
		printf("Conversion error %s %s\n", azColName[3], argv[3]);
	}
	gtk_adjustment_set_value(vadj2, -f);

	point2comma(argv[4]);
	f=strtod(argv[4], &errCheck);
	if (errCheck == argv[4])
	{
		f=0.0;
		printf("Conversion error %s %s\n", azColName[4], argv[4]);
	}
	gtk_adjustment_set_value(vadj3, -f);

	point2comma(argv[5]);
	f=strtod(argv[5], &errCheck);
	if (errCheck == argv[5])
	{
		f=0.0;
		printf("Conversion error %s %s\n", azColName[5], argv[5]);
	}
	gtk_adjustment_set_value(vadj4, -f);

	point2comma(argv[6]);
	f=strtod(argv[6], &errCheck);
	if (errCheck == argv[6])
	{
		f=0.0;
		printf("Conversion error %s %s\n", azColName[6], argv[6]);
	}
	gtk_adjustment_set_value(vadj5, -f);

	point2comma(argv[7]);
	f=strtod(argv[7], &errCheck);
	if (errCheck == argv[7])
	{
		f=0.0;
		printf("Conversion error %s %s\n", azColName[7], argv[7]);
	}
	gtk_adjustment_set_value(vadj6, -f);

	point2comma(argv[8]);
	f=strtod(argv[8], &errCheck);
	if (errCheck == argv[8])
	{
		f=0.0;
		printf("Conversion error %s %s\n", azColName[8], argv[8]);
	}
	gtk_adjustment_set_value(vadj7, -f);

	point2comma(argv[9]);
	f=strtod(argv[9], &errCheck);
	if (errCheck == argv[9])
	{
		f=0.0;
		printf("Conversion error %s %s\n", azColName[9], argv[9]);
	}
	gtk_adjustment_set_value(vadj8, -f);

	point2comma(argv[10]);
	f=strtod(argv[10], &errCheck);
	if (errCheck == argv[10])
	{
		f=0.0;
		printf("Conversion error %s %s\n", azColName[10], argv[10]);
	}
	gtk_adjustment_set_value(vadj9, -f);

/*
	point2comma(argv[11]);
	f=strtod(argv[11], &errCheck);
	if (errCheck == argv[11])
	{
		f=0.0;
		printf("Conversion error %s %s\n", azColName[11], argv[11]);
	}
	gtk_adjustment_set_value(vadjA, 16.0-f);
*/
	effectivegain();

	return 0;
}

void select_eqpreset(char* id)
{
	sqlite3 *db;
	char *err_msg = NULL;
	char sql[256];
	int rc;

	if((rc = sqlite3_open("/var/sqlite3DATA/mediaplayer.db", &db)))
	{
		printf("Can't open database: %s\n", sqlite3_errmsg(db));
	}
	else
	{
//printf("Opened database successfully\n");
		sql[0] = '\0';
		strcat(sql, "SELECT * FROM eqpresets where id=");
		strcat(sql, id);
		strcat(sql, ";");
		if((rc = sqlite3_exec(db, sql, select_eqpreset_callback, 0, &err_msg)) != SQLITE_OK)
		{
			printf("Failed to select data, %s\n", err_msg);
			sqlite3_free(err_msg);
		}
		else
		{
// success
		}
	}
	sqlite3_close(db);
}

void resize_containers(void)
{
	GtkAllocation *alloc = g_new(GtkAllocation, 1);
	gtk_widget_get_allocation(hscale, alloc);
	int scaleheight = alloc->height;
	g_free(alloc);
	//printf("Scale height %d\n", scaleheight);
	gtk_widget_set_size_request(scrolled_window, playerWidth, playerHeight+scaleheight);

	gtk_widget_set_size_request(vscaleeq0, playerWidth/12, playerHeight);
	gtk_widget_set_size_request(vscaleeq1, playerWidth/12, playerHeight);
	gtk_widget_set_size_request(vscaleeq2, playerWidth/12, playerHeight);
	gtk_widget_set_size_request(vscaleeq3, playerWidth/12, playerHeight);
	gtk_widget_set_size_request(vscaleeq4, playerWidth/12, playerHeight);
	gtk_widget_set_size_request(vscaleeq5, playerWidth/12, playerHeight);
	gtk_widget_set_size_request(vscaleeq6, playerWidth/12, playerHeight);
	gtk_widget_set_size_request(vscaleeq7, playerWidth/12, playerHeight);
	gtk_widget_set_size_request(vscaleeq8, playerWidth/12, playerHeight);
	gtk_widget_set_size_request(vscaleeq9, playerWidth/12, playerHeight);
	gtk_widget_set_size_request(vscaleeqA, playerWidth/12, playerHeight);
}

/* Called when the windows are realized */
static void realize_cb(GtkWidget *widget, gpointer data)
{
	resize_containers();
	g_object_set((gpointer)combopreset, "active-id", "0", NULL);
	g_object_set((gpointer)combodelaytype, "active-id", "0", NULL);
	g_object_set((gpointer)comborvrbdelays, "active-id", "12", NULL);
}

static void button1_clicked(GtkWidget *button, gpointer data)
{
	int ret;

	if (!now_playing)
		return;

//g_print("Button 1 clicked\n");
	gtk_widget_set_sensitive(button1, FALSE);
	gtk_widget_set_sensitive(button2, TRUE);
	gtk_widget_set_sensitive(button8, TRUE);
	gtk_widget_set_sensitive(button9, TRUE);
	gtk_widget_set_sensitive(button10, TRUE);

	// Init Video Queue
	vq_init(&vq, &vqmutex, &vqlowcond, &vqhighcond);
//printf("vq_init\n");
	// Init Audio Queue
	aq_init(&aq, &aqmutex, &aqlowcond, &aqhighcond);
//printf("aq_init\n");

	if ((ret=pthread_mutex_init(&seekmutex, NULL))!=0 )
		printf("Seek mutex init failed, %d\n", ret);

	if ((ret=pthread_mutex_init(&framemutex, NULL))!=0 )
		printf("Frame mutex init failed, %d\n", ret);

	resetLevels();
	ret = open_file(now_playing);
	if (ret == 0)
	{
//printf("opened %s\n", now_playing);
	}
	else
	{
		printf("Error opening file %s\n", now_playing);
		return;
	}

	if (videoduration)
	{
		playerHeight = playerWidth * pCodecCtx->height / pCodecCtx->width;
		gtk_widget_set_size_request(dwgarea, playerWidth, playerHeight);
		gtk_adjustment_set_upper(hadjustment, videoduration);
	}
	else
	{
		if (pCodecCtxA->width)
			playerHeight = playerWidth * pCodecCtxA->height / pCodecCtxA->width;
		else
			playerHeight = 0;
		int pminheight = playerWidth * 9 / 16;
		playerHeight = (playerHeight<pminheight?pminheight:playerHeight);
		gtk_widget_set_size_request(dwgarea, playerWidth, playerHeight);
		initPixbuf(playerWidth, playerHeight);
		gtk_adjustment_set_upper(hadjustment, audioduration);
	}

	gtk_adjustment_set_value(hadjustment, 0);

	hscaleupd = TRUE;
	stoprequested = 0;
	playerstatus = playing;
	create_thread_framereader();
//printf("create_thread_framereader\n");
	create_threads_av();
//printf("create_threads_av\n");

	pop_message(statusbar, context_id);
	char msg[256];
	char msgtext[256];
	char *txt;
	int pos;

	if (videoduration)
	{
		txt = strlastpart(now_playing, "/", 0);
		pos = strlen(txt);
		strcpy(msgtext, txt);
		if (pos>70)
		{
			msgtext[30] = '\0';
			strcat(msgtext, "...");
			pos -= 30;
			strcat(msgtext, txt+pos);
		}
		sprintf(msg, "%2.2f fps, %d*%d, %s (%02i:%02i)", frame_rate, pCodecCtx->width, pCodecCtx->height, msgtext, ((int)(videoduration/frame_rate))/60, ((int)(videoduration/frame_rate)%60));
	}
	else
	{
		txt = strlastpart(now_playing, "/", 0);
		pos = strlen(txt);
		strcpy(msgtext, txt);
		if (pos>100)
		{
			msgtext[45] = '\0';
			strcat(msgtext, "...");
			pos -= 45;
			strcat(msgtext, txt+pos);
		}
		sprintf(msg, "%s (%02i:%02i)", msgtext, ((int)(audioduration/sample_rate))/60, ((int)(audioduration/sample_rate)%60));
	}
	push_message(statusbar, context_id, msg);

	gtk_window_resize(GTK_WINDOW(window), 100, 100);
	resize_containers();
}

static void button2_clicked(GtkWidget *button, gpointer data)
{
//g_print("Button 2 clicked\n");
	stoprequested = 1;
	gtk_widget_set_sensitive(button2, FALSE);
	gtk_widget_set_sensitive(button8, FALSE);
	gtk_widget_set_sensitive(button9, FALSE);
	gtk_widget_set_sensitive(button10, FALSE);
	gtk_widget_set_sensitive(button1, TRUE);
}

static void button8_clicked(GtkWidget *button, gpointer data)
{
//g_print("Button 8 clicked\n");
	stoprequested = 1;
	while(!(playerstatus == idle))
		usleep(100000); // 0.1s
	play_next(NULL);
}

static void button9_clicked(GtkWidget *button, gpointer data)
{
//g_print("Button 9 clicked\n");
	stoprequested = 1;
	while(!(playerstatus == idle))
		usleep(100000); // 0.1s
	play_prev(NULL);
}

static void button10_clicked(GtkWidget *button, gpointer data)
{
//g_print("Button 10 clicked\n");
	if (playerstatus==playing)
	{
		pthread_mutex_lock(&seekmutex);
		playerstatus = paused;
		gtk_button_set_label(GTK_BUTTON(button10), "Resume");
		gtk_widget_set_sensitive(button2, FALSE);
		gtk_widget_set_sensitive(button8, FALSE);
		gtk_widget_set_sensitive(button9, FALSE);
	}
	else
	{
		gtk_widget_set_sensitive(button2, TRUE);
		gtk_widget_set_sensitive(button8, TRUE);
		gtk_widget_set_sensitive(button9, TRUE);
		gtk_button_set_label(GTK_BUTTON(button10), "Pause");
		playerstatus = playing;
		pthread_mutex_unlock(&seekmutex);
	}
}

/*
static void hscale_adjustment(GtkWidget *widget, gpointer data)
{
    double value = gtk_range_get_value(GTK_RANGE(widget));
    printf("Adjustment value: %f\n", value);
}
*/

enum
{
  COL_ID = 0,
  COL_FILEPATH,
  NUM_COLS
};

int select_callback(void *NotUsed, int argc, char **argv, char **azColName) 
{
//    NotUsed = 0;
//    for (int i = 0; i < argc; i++)
//    {
//     printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
//     if (!strcmp(azColName[i],"response"))
//     {
//      gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (combobox), argv[i], argv[i]);
//     }
//    }
//    printf("\n");
	//gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (combobox), argv[0], argv[1]);

	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, COL_ID, atoi(argv[0]), COL_FILEPATH, argv[1], -1);

//g_print("%s, %s\n", argv[0], argv[1]);
	return 0;
}

static GtkTreeModel* create_and_fill_model(int mode, int argc, char** argv)
{
	sqlite3 *db;
	char *err_msg = NULL;
	char *sql = NULL;
	int rc;

	store = gtk_list_store_new(NUM_COLS, G_TYPE_UINT, G_TYPE_STRING);

	switch(mode)
	{
		case 0:
			break;
		case 1:
			if((rc = sqlite3_open("/var/sqlite3DATA/mediaplayer.db", &db)))
			{
				printf("Can't open database: %s\n", sqlite3_errmsg(db));
			}
			else
			{
//printf("Opened database successfully\n");
				sql = "SELECT * FROM mediafiles order by id;";
				if((rc = sqlite3_exec(db, sql, select_callback, 0, &err_msg)) != SQLITE_OK)
				{
					printf("Failed to select data, %s\n", err_msg);
					sqlite3_free(err_msg);
				}
				else
				{
// success
				}
			}
			sqlite3_close(db);
			break;
		case 2:
			for(rc=1;rc<argc;rc++)
			{
				gtk_list_store_append(store, &iter);
				gtk_list_store_set(store, &iter, COL_ID, rc, COL_FILEPATH, argv[rc], -1);
			}
		default:
			break;
	}

	return GTK_TREE_MODEL(store);
}

static GtkWidget* create_view_and_model(int argc, char** argv)
{
	GtkCellRenderer *renderer;
	GtkTreeModel *model;
	GtkWidget *view;

	view = gtk_tree_view_new();

	// Column 1
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "ID", renderer, "text", COL_ID, NULL);

	// Column 2
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(view), -1, "File Path", renderer, "text", COL_FILEPATH, NULL);

	if (argc>1)
		model = create_and_fill_model(2, argc, argv); // insert rows from command line
	else
		model = create_and_fill_model(0, 0, NULL); // do not insert rows

	gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);

/* The tree view has acquired its own reference to the model, so we can drop ours. That way the model will
   be freed automatically when the tree view is destroyed */
//g_object_unref(model);

	return view;
}

void listview_onRowActivated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer userdata)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	//gchar *s;

	if (playerstatus==paused)
	{
		stoprequested = 1;
		playerstatus = playing;
		gtk_button_set_label(GTK_BUTTON(button10), "Pause");
		pthread_mutex_unlock(&seekmutex);
	}

//g_print("double-clicked\n");
	model = gtk_tree_view_get_model(treeview);
	if (gtk_tree_model_get_iter(model, &iter, path))
	{
		//s=gtk_tree_path_to_string(path);
		//printf("%s\n",s);
		//g_free(s);
		if (!(playerstatus == idle))
		{
			button2_clicked(button2, NULL);
			while(!(playerstatus == idle))
				usleep(100000); // 0.1s
		}
		if (now_playing)
		{
			g_free(now_playing);
			now_playing = NULL;
		}
		gtk_tree_model_get(model, &iter, COL_FILEPATH, &now_playing, -1);
//g_print ("Double-clicked path %s\n", now_playing);
		button1_clicked(button1, NULL);
	}
}

static void button3_clicked(GtkWidget *button, gpointer data)
{
	GtkTreeModel *model;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(listview));
	gtk_list_store_clear((GtkListStore*)model);

	gtk_tree_view_set_model(GTK_TREE_VIEW(listview), NULL); /* Detach model from view */
	g_object_unref(model);
	model = create_and_fill_model(0, 0, NULL); // insert rows
	gtk_tree_view_set_model(GTK_TREE_VIEW(listview), model); /* Re-attach model to view */
}

static void button4_clicked(GtkWidget *button, gpointer data)
{
	GtkTreeModel *model;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(listview));
	gtk_list_store_clear((GtkListStore*)model);

	gtk_tree_view_set_model(GTK_TREE_VIEW(listview), NULL); /* Detach model from view */
	g_object_unref(model);
	model = create_and_fill_model(1, 0, NULL); // insert rows
	gtk_tree_view_set_model(GTK_TREE_VIEW(listview), model); /* Re-attach model to view */
}

void listdir(const char *name, sqlite3 *db, int *id)
{
	char *err_msg = NULL;
	char sql[1024];
	int rc;
	char sid[10];
	char path[1024];
	char *strname;
	char *strdest;

	DIR *dir;
	struct dirent *entry;

	if (!(dir = opendir(name)))
		return;
	if (!(entry = readdir(dir)))
		return;

	do
	{
		if (entry->d_type == DT_DIR)
		{
			int len = snprintf(path, sizeof(path)-1, "%s/%s", name, entry->d_name);
			path[len] = 0;
			if ((!strcmp(entry->d_name, ".")) || (!strcmp(entry->d_name, "..")))
				continue;
//printf("%*s[%s]\n", level*2, "", entry->d_name);
			listdir(path, db, id);
		}
		else
		{
//printf("%*s- %s/%s\n", level*2, "", name, entry->d_name);
			if (nomediafile(entry->d_name))
				continue;

			(*id)++;
			sprintf(sid, "%d", *id);
			sql[0] = '\0';
			strcat(sql, "INSERT INTO mediafiles VALUES(");
			strcat(sql, sid);
			strcat(sql, ", '");
			strcat(sql, name);
			strcat(sql, "/");
				//strcat(sql, entry->d_name);
				strname=(char*)malloc(sizeof(entry->d_name)+1);
				strcpy(strname, entry->d_name);
				strdest = strreplace(strname, "'", "''");
				free(strname);
				strcat(sql, strdest);
				free(strdest);
			strcat(sql, "');");
//printf("%s\n", sql);
			if((rc = sqlite3_exec(db, sql, 0, 0, &err_msg)) != SQLITE_OK)
			{
				printf("Failed to insert data, %s\n", err_msg);
				sqlite3_free(err_msg);
			}
			else
			{
// success
			}
		}
	}
	while((entry = readdir(dir)));
	closedir(dir);
}

static void button6_clicked(GtkWidget *button, gpointer data)
{
	GtkWidget *dialog;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;

	char *err_msg = NULL;
	sqlite3 *db;
	char *sql;
	int rc;
	int id;

	if (!catalog_folder)
	{
		g_free(catalog_folder);
		catalog_folder = NULL;
	}

	dialog = gtk_file_chooser_dialog_new("Open Folder for Catalog", GTK_WINDOW(window), action, "Cancel", GTK_RESPONSE_CANCEL, "Select Folder", GTK_RESPONSE_ACCEPT, NULL);
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
	{
		GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
		catalog_folder = gtk_file_chooser_get_filename(chooser);

		if ((rc = sqlite3_open("/var/sqlite3DATA/mediaplayer.db", &db)))
		{
			printf("Can't open database: %s\n", sqlite3_errmsg(db));
		}
		else
		{
//printf("Opened database successfully\n");
			sql = "DELETE FROM mediafiles;";
			if ((rc = sqlite3_exec(db, sql, 0, 0, &err_msg)) != SQLITE_OK)
			{
				printf("Failed to delete data, %s\n", err_msg);
				sqlite3_free(err_msg);
			}
			else
			{
// success
			}
			id = 0;
			listdir(catalog_folder, db, &id);
		}
		sqlite3_close(db);

	}
	gtk_widget_destroy (dialog);
}

int last_id;

int select_add_callback(void *NotUsed, int argc, char **argv, char **azColName) 
{
	last_id = atoi(argv[0]);
	return 0;
}

int select_add_lastid()
{
	sqlite3 *db;
	char *err_msg = NULL;
	char *sql = NULL;
	int rc;

	last_id = 0;
	if((rc = sqlite3_open("/var/sqlite3DATA/mediaplayer.db", &db)))
	{
		printf("Can't open database: %s\n", sqlite3_errmsg(db));
	}
	else
	{
//printf("Opened database successfully\n");
		sql = "SELECT max(id) as id FROM mediafiles;";
		if((rc = sqlite3_exec(db, sql, select_add_callback, 0, &err_msg)) != SQLITE_OK)
		{
			printf("Failed to select data, %s\n", err_msg);
			sqlite3_free(err_msg);
		}
		else
		{
// success
		}
	}
	sqlite3_close(db);

	return(last_id);
}

static void button7_clicked(GtkWidget *button, gpointer data)
{
	char *err_msg = NULL;
	sqlite3 *db;
	int rc;
	int id;
	char sql[1024];
	char sid[10];
	char *strname;
	char *strdest;

	GtkWidget *dialog;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
	GSList *chosenfile;

	dialog = gtk_file_chooser_dialog_new("Add File", GTK_WINDOW(window), action, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL);
	GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
	gtk_file_chooser_set_select_multiple(chooser, TRUE);
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
	{
		if ((rc = sqlite3_open("/var/sqlite3DATA/mediaplayer.db", &db)))
		{
			printf("Can't open database: %s\n", sqlite3_errmsg(db));
		}
		else
		{
			id = select_add_lastid();
			GSList *filelist = gtk_file_chooser_get_filenames(chooser);
			for(chosenfile=filelist;chosenfile;chosenfile=chosenfile->next)
			{
//printf("%s\n", (char*)chosenfile->data);
				if (nomediafile((char*)chosenfile->data))
					continue;

				id++;
				sprintf(sid, "%d", id);
				sql[0] = '\0';
				strcat(sql, "INSERT INTO mediafiles VALUES(");
				strcat(sql, sid);
				strcat(sql, ", '");
					//strcat(sql, (char*)chosenfile->data));
					strname=(char*)malloc(1024);
					strcpy(strname, (char*)chosenfile->data);
					strdest = strreplace(strname, "'", "''");
					free(strname);
					strcat(sql, strdest);
					free(strdest);
				strcat(sql, "');");
//printf("%s\n", sql);
				if ((rc = sqlite3_exec(db, sql, 0, 0, &err_msg)) != SQLITE_OK)
				{
					printf("Failed to insert data, %s\n", err_msg);
					sqlite3_free(err_msg);
				}
				else
				{
// success
				}
			}
			button3_clicked(button3, NULL);
			button4_clicked(button4, NULL);
		}
		sqlite3_close(db);
	}
	gtk_widget_destroy(dialog);
}

gboolean play_next(gpointer data)
{
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreeIter iter;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(listview));
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(listview));

	gtk_tree_selection_get_selected(selection, &model, &iter);
	if (gtk_tree_model_iter_next(model, &iter))
	{
		gtk_tree_selection_select_iter(selection, &iter);
	}
	else
	{
		if (gtk_tree_model_iter_nth_child (model, &iter, NULL, 0))
		{
			gtk_tree_selection_select_iter(selection, &iter);
		}
		else
		{
			printf("no entries\n");
			return FALSE;
		}
	}

	if (now_playing)
	{
		g_free(now_playing);
		now_playing = NULL;
	}
	gtk_tree_model_get(model, &iter, COL_FILEPATH, &now_playing, -1);
	//g_print("Next %s\n", now_playing);

	button1_clicked(button1, NULL);
	return FALSE;
}

gboolean play_prev(gpointer data)
{
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreeIter iter;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(listview));
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(listview));

	gtk_tree_selection_get_selected(selection, &model, &iter);
	if (gtk_tree_model_iter_previous(model, &iter))
	{
		gtk_tree_selection_select_iter(selection, &iter);
	}
	else
	{
		gint nodecount = gtk_tree_model_iter_n_children (model, NULL);
		if (nodecount)
		{
			if (gtk_tree_model_iter_nth_child (model, &iter, NULL, nodecount-1))
			{
				gtk_tree_selection_select_iter(selection, &iter);
			}
			else
			{
				printf("no entries\n");
				return FALSE;
			}
		}
	}

	if (now_playing)
	{
		g_free(now_playing);
		now_playing = NULL;
	}
	gtk_tree_model_get(model, &iter, COL_FILEPATH, &now_playing, -1);
	//g_print("Next %s\n", now_playing);

	button1_clicked(button1, NULL);
	return FALSE;
}

/* seek to exact packet
void AV_seek(AV * av, size_t frame)
{
    int frame_delta = frame - av->frame_id;
    if (frame_delta < 0 || frame_delta > 5)
        av_seek_frame(av->fmt_ctx, av->video_stream_id,
                frame, AVSEEK_FLAG_BACKWARD);
    while (av->frame_id != frame)
        AV_read_frame(av);
}

void AV_read_frame(AV * av)
{
    AVPacket packet;
    int frame_done;

    while (av_read_frame(av->fmt_ctx, &packet) >= 0) {
        if (packet.stream_index == av->video_stream_id) {
            avcodec_decode_video2(av->codec_ctx, av->frame, &frame_done, &packet);
            if (frame_done) {
                ...
                av->frame_id = packet.dts;
                av_free_packet(&packet);
                return;
            }
        }
        av_free_packet(&packet);
    }
}
*/

gboolean scale_pressed(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	if (playerstatus==playing)
	{
		pthread_mutex_lock(&seekmutex);
		hscaleupd = FALSE;
	}
	return FALSE;
}

gboolean scale_released(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	int64_t seek_ts;

	double value = gtk_range_get_value(GTK_RANGE(widget));
//printf("Released value: %f\n", value);

	if (playerstatus==playing)
	{
		if (videoduration)
		{
			AVStream *st = pFormatCtx->streams[videoStream];
			double seektime = value / frame_rate; // seconds
//printf("Seek time %5.2f\n", seektime);
			int flgs = AVSEEK_FLAG_ANY;
			seek_ts = (seektime * (st->time_base.den)) / (st->time_base.num);
			if (av_seek_frame(pFormatCtx, videoStream, seek_ts, flgs) < 0)
			{
				printf("Failed to seek Video\n");
			}
			else
			{
				avcodec_flush_buffers(pCodecCtx);
				if (audioStream!=-1)
					avcodec_flush_buffers(pCodecCtxA);

				vq_drain(&vq);
				aq_drain(&aq);
			}

			pthread_mutex_lock(&framemutex);
			now_decoding_frame = (int64_t)value;
			pthread_mutex_unlock(&framemutex);
		}
		else
		{
			AVStream *st = pFormatCtx->streams[audioStream];
			double seektime = value / sample_rate; // seconds
//printf("Seek time %5.2f\n", seektime);
			int flgs = AVSEEK_FLAG_ANY;
			seek_ts = (seektime * (st->time_base.den)) / (st->time_base.num);
//printf("seek_ts %lld\n", seek_ts);
			if (av_seek_frame(pFormatCtx, audioStream, seek_ts, flgs) < 0)
			{
				printf("Failed to seek Audio\n");
			}
			else
			{
				avcodec_flush_buffers(pCodecCtxA);
				vq_drain(&vq);
				aq_drain(&aq);
			}
			now_playing_frame = (int64_t)value;
		}

		hscaleupd = TRUE;
		pthread_mutex_unlock(&seekmutex);
	}
	return FALSE;
}

gchar* scale_valueformat(GtkScale *scale, gdouble value, gpointer user_data)
{
	return g_strdup_printf("%0.1f", -value);
}

gchar* scale_valuepreamp(GtkScale *scale, gdouble value, gpointer user_data)
{
	return g_strdup_printf("%0.2f", 16.0-value);
}

int select_eqpresetnames_callback(void *NotUsed, int argc, char **argv, char **azColName) 
{
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combopreset), argv[0], argv[12]);
	return 0;
}

void select_eqpreset_names()
{
	sqlite3 *db;
	char *err_msg = NULL;
	char* sql;
	int rc;

	if((rc = sqlite3_open("/var/sqlite3DATA/mediaplayer.db", &db)))
	{
		printf("Can't open database: %s\n", sqlite3_errmsg(db));
	}
	else
	{
//printf("Opened database successfully\n");
		sql = "SELECT * FROM eqpresets order by id;";
		if ((rc = sqlite3_exec(db, sql, select_eqpresetnames_callback, 0, &err_msg)) != SQLITE_OK)
		{
			printf("Failed to select data, %s\n", err_msg);
			sqlite3_free(err_msg);
		}
		else
		{
// success
		}
	}
	sqlite3_close(db);
}

static void preset_changed(GtkWidget *combo, gpointer data)
{
	gchar *strval;

	g_object_get((gpointer)combo, "active-id", &strval, NULL);
	//printf("Selected id %s\n", strval);
	select_eqpreset(strval);
	g_free(strval);
}

static void eq_toggled(GtkWidget *togglebutton, gpointer data)
{
	pthread_mutex_lock(&eqmutex);
	eqenabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton));
	pthread_mutex_unlock(&eqmutex);
	//printf("toggle state %d\n", gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(eqenable)));
}

static void eq_autotoggled(GtkWidget *togglebutton, gpointer data)
{
	pthread_mutex_lock(&eqmutex);
	eqautoleveling = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton));
	if (eqautoleveling) effectivegain();
	pthread_mutex_unlock(&eqmutex);
	//printf("toggle state %d\n", gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(eqenable)));
}

static void aqmaxlength_changed(GtkWidget *widget, gpointer data)
{
	int newvalue = (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton1));

	pthread_mutex_lock(&aqmutex);
	if (newvalue > aqMaxLength)
		pthread_cond_signal(&aqhighcond); // Should wake up *one* thread
	else
		pthread_cond_signal(&aqlowcond); // Should wake up *one* thread
	aqMaxLength = newvalue;
	pthread_mutex_unlock(&aqmutex);
}

static void vqmaxlength_changed(GtkWidget *widget, gpointer data)
{
	int newvalue = (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton2));

	pthread_mutex_lock(&vqmutex);
	if (newvalue > vqMaxLength)
		pthread_cond_signal(&vqhighcond); // Should wake up *one* thread
	else
		pthread_cond_signal(&vqlowcond); // Should wake up *one* thread
	vqMaxLength = newvalue;
	pthread_mutex_unlock(&vqmutex);
}

static void threadcount_changed(GtkWidget *widget, gpointer data)
{
	thread_count = (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton3));
}

static void playerwidth_changed(GtkWidget *widget, gpointer data)
{
	playerWidth = (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton4));
}

static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
//g_print ("delete event occurred\n");
	if (playerstatus==paused)
	{
		stoprequested = 1;
		playerstatus = playing;
		gtk_button_set_label(GTK_BUTTON(button10), "Pause");
		pthread_mutex_unlock(&seekmutex);
	}

	if (!(playerstatus == idle))
	{
		
		button2_clicked(button2, NULL);
		while(!(playerstatus == idle))
			usleep(100000); // 0.1s
	}
	pthread_mutex_destroy(&eqmutex);
    return FALSE; // return FALSE to emit destroy signal
}

static void destroy(GtkWidget *widget, gpointer data)
{
//printf("gtk_main_quit\n");
    gtk_main_quit();
}

static void buttonParameters_clicked(GtkWidget *button, gpointer data)
{
	if (gtk_widget_get_visible(windowparm))
		gtk_widget_hide(windowparm);
	else
		gtk_widget_show_all(windowparm);
}

static gboolean delete_event_parm(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    /* If you return FALSE in the "delete-event" signal handler,
     * GTK will emit the "destroy" signal. Returning TRUE means
     * you don't want the window to be destroyed.
     * This is useful for popping up 'are you sure you want to quit?'
     * type dialogs. */
//g_print ("delete event occurred\n");
	buttonParameters_clicked(buttonParameters, NULL);
    return TRUE;
}

/* Called when the windows are realized */
static void realize_parm(GtkWidget *widget, gpointer data)
{
}

static void dly_toggled(GtkWidget *togglebutton, gpointer data)
{
	gchar *strval;

	pthread_mutex_lock(&(snddly.delaymutex));

	g_object_get((gpointer)combodelaytype, "active-id", &strval, NULL);
	//printf("Selected id %s\n", strval);
	snddly.delaytype = atoi((const char *)strval);
	g_free(strval);
	snddly.millisec = (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton5));
	snddly.feedback = (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton6));

	snddly.enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton));
	if (snddly.enabled)
	{
		sounddelay_init(1, snddly.delaytype, snddly.millisec, snddly.feedback, snddly.format, snddly.rate, snddly.channels, &snddly);
	}
	else
	{
		sounddelay_close(&snddly);
	}

	pthread_mutex_unlock(&(snddly.delaymutex));
	//printf("toggle state %d\n", gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(dlyenable)));
}

static void delay_changed(GtkWidget *widget, gpointer data)
{
	pthread_mutex_lock(&(snddly.delaymutex));
	float newvalue = (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	if (snddly.enabled)
	{
		sounddelay_close(&snddly);
		sounddelay_init(1, snddly.delaytype, newvalue, snddly.feedback, snddly.format, snddly.rate, snddly.channels, &snddly);
	}
	pthread_mutex_unlock(&(snddly.delaymutex));
}

void select_delay_types()
{
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combodelaytype), "0", "Echo");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combodelaytype), "1", "Delay");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combodelaytype), "2", "Reverb");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combodelaytype), "3", "Late");
}

void select_delay_lines()
{
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comborvrbdelays), "1", "1 Delay");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comborvrbdelays), "2", "2 Delays");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comborvrbdelays), "3", "3 Delays");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comborvrbdelays), "4", "4 Delays");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comborvrbdelays), "5", "5 Delays");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comborvrbdelays), "6", "6 Delays");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comborvrbdelays), "7", "7 Delays");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comborvrbdelays), "8", "8 Delays");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comborvrbdelays), "9", "9 Delays");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comborvrbdelays), "10", "10 Delays");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comborvrbdelays), "11", "11 Delays");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comborvrbdelays), "12", "12 Delays");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comborvrbdelays), "13", "13 Delays");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comborvrbdelays), "14", "14 Delays");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comborvrbdelays), "15", "15 Delays");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comborvrbdelays), "16", "16 Delays");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comborvrbdelays), "17", "17 Delay");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comborvrbdelays), "18", "18 Delays");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comborvrbdelays), "19", "19 Delays");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comborvrbdelays), "20", "20 Delays");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comborvrbdelays), "21", "21 Delays");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comborvrbdelays), "22", "22 Delays");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comborvrbdelays), "23", "23 Delays");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comborvrbdelays), "24", "24 Delays");
}

static void delaytype_changed(GtkWidget *combo, gpointer data)
{
	gchar *strval;

	pthread_mutex_lock(&(snddly.delaymutex));
	if (snddly.enabled)
	{
		sounddelay_close(&snddly);
		g_object_get((gpointer)combo, "active-id", &strval, NULL);
		//printf("Selected id %s\n", strval);
		snddly.delaytype = atoi((const char *)strval);
		g_free(strval);
		sounddelay_init(1, snddly.delaytype, snddly.millisec, snddly.feedback, snddly.format, snddly.rate, snddly.channels, &snddly);
	}
	pthread_mutex_unlock(&(snddly.delaymutex));
}

static void feedback_changed(GtkWidget *widget, gpointer data)
{
	pthread_mutex_lock(&(snddly.delaymutex));
	float newvalue = (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	if (snddly.enabled)
	{
		sounddelay_close(&snddly);
		sounddelay_init(1, snddly.delaytype, snddly.millisec, newvalue, snddly.format, snddly.rate, snddly.channels, &snddly);
	}
	pthread_mutex_unlock(&(snddly.delaymutex));
}

static void rvrb_toggled(GtkWidget *togglebutton, gpointer data)
{
	pthread_mutex_lock(&(sndreverb.reverbmutex));

	sndreverb.feedback = (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton7));

	sndreverb.enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton));
	if (sndreverb.enabled)
	{
		soundreverb_init(sndreverb.reverbdelaylines, sndreverb.feedback, sndreverb.presence, sndreverb.LPFfreq, sndreverb.LSHfreq, sndreverb.format, sndreverb.rate, sndreverb.channels, &sndreverb);
	}
	else
	{
		soundreverb_close(&sndreverb);
	}

	pthread_mutex_unlock(&(sndreverb.reverbmutex));
	//printf("toggle state %d\n", gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rvrbenable)));
}

static void rvrbfeedback_changed(GtkWidget *widget, gpointer data)
{
	pthread_mutex_lock(&(sndreverb.reverbmutex));
	float newvalue = (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	if (sndreverb.enabled)
	{
		soundreverb_close(&sndreverb);
		soundreverb_init(sndreverb.reverbdelaylines, newvalue, sndreverb.presence, sndreverb.LPFfreq, sndreverb.LSHfreq, sndreverb.format, sndreverb.rate, sndreverb.channels, &sndreverb);
	}
	pthread_mutex_unlock(&(sndreverb.reverbmutex));
}

static void rvrbpresence_changed(GtkWidget *widget, gpointer data)
{
	pthread_mutex_lock(&(sndreverb.reverbmutex));
	float newvalue = (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	if (sndreverb.enabled)
	{
		soundreverb_close(&sndreverb);
		soundreverb_init(sndreverb.reverbdelaylines, sndreverb.feedback, newvalue, sndreverb.LPFfreq, sndreverb.LSHfreq, sndreverb.format, sndreverb.rate, sndreverb.channels, &sndreverb);
	}
	pthread_mutex_unlock(&(sndreverb.reverbmutex));
}

static void rvrbLPF_changed(GtkWidget *widget, gpointer data)
{
	pthread_mutex_lock(&(sndreverb.reverbmutex));
	float newvalue = (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	if (sndreverb.enabled)
	{
		soundreverb_close(&sndreverb);
		soundreverb_init(sndreverb.reverbdelaylines, sndreverb.feedback, sndreverb.presence, newvalue, sndreverb.LSHfreq, sndreverb.format, sndreverb.rate, sndreverb.channels, &sndreverb);
	}
	pthread_mutex_unlock(&(sndreverb.reverbmutex));
}

static void delaylines_changed(GtkWidget *combo, gpointer data)
{
	gchar *strval;

	pthread_mutex_lock(&(sndreverb.reverbmutex));
	if (sndreverb.enabled)
	{
		soundreverb_close(&sndreverb);
		g_object_get((gpointer)combo, "active-id", &strval, NULL);
		//printf("Selected id %s\n", strval);
		sndreverb.reverbdelaylines = atoi((const char *)strval);
		g_free(strval);
		soundreverb_init(sndreverb.reverbdelaylines, sndreverb.feedback, sndreverb.presence, sndreverb.LPFfreq, sndreverb.LSHfreq, sndreverb.format, sndreverb.rate, sndreverb.channels, &sndreverb);
	}
	pthread_mutex_unlock(&(sndreverb.reverbmutex));
}

static void rvrbLSH_changed(GtkWidget *widget, gpointer data)
{
	pthread_mutex_lock(&(sndreverb.reverbmutex));
	float newvalue = (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	if (sndreverb.enabled)
	{
		soundreverb_close(&sndreverb);
		soundreverb_init(sndreverb.reverbdelaylines, sndreverb.feedback, sndreverb.presence, sndreverb.LPFfreq, newvalue, sndreverb.format, sndreverb.rate, sndreverb.channels, &sndreverb);
	}
	pthread_mutex_unlock(&(sndreverb.reverbmutex));
}

static void tremolo_toggled(GtkWidget *togglebutton, gpointer data)
{
	pthread_mutex_lock(&(sndtremolo.tremolomutex));

	sndtremolo.enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton));
	if (sndtremolo.enabled)
	{
		soundtremolo_init((float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton9)), (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton10)), 
						sndtremolo.format, sndtremolo.rate, sndtremolo.channels, &sndtremolo);
	}
	else
	{
		soundtremolo_close(&sndtremolo);
	}

	pthread_mutex_unlock(&(sndtremolo.tremolomutex));
	//printf("toggle state %d\n", gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(tremoloenable)));
}

static void tremolodepth_changed(GtkWidget *widget, gpointer data)
{
	pthread_mutex_lock(&(sndtremolo.tremolomutex));
	float newvalue = (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	if (sndtremolo.enabled)
	{
		soundtremolo_close(&sndtremolo);
		soundtremolo_init(newvalue, sndtremolo.tremolorate, sndtremolo.format, sndtremolo.rate, sndtremolo.channels, &sndtremolo);
	}
	pthread_mutex_unlock(&(sndtremolo.tremolomutex));
}

static void tremolorate_changed(GtkWidget *widget, gpointer data)
{
	pthread_mutex_lock(&(sndtremolo.tremolomutex));
	float newvalue = (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	if (sndtremolo.enabled)
	{
		soundtremolo_close(&sndtremolo);
		soundtremolo_init(sndtremolo.depth, newvalue, sndtremolo.format, sndtremolo.rate, sndtremolo.channels, &sndtremolo);
	}
	pthread_mutex_unlock(&(sndtremolo.tremolomutex));
}

static void fd_toggled(GtkWidget *togglebutton, gpointer data)
{
	pthread_mutex_lock(&(sndflddistort.fdmutex));

	sndflddistort.enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton));
	if (sndflddistort.enabled)
	{
		soundfoldingdistort_init((float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton12)), (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton13)),
								sndflddistort.format, sndflddistort.rate, sndflddistort.channels, &sndflddistort);
	}
	else
	{
		soundfoldingdistort_close(&sndflddistort);
	}

	pthread_mutex_unlock(&(sndflddistort.fdmutex));
	//printf("toggle state %d\n", gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(tremoloenable)));
}

static void fdthreshold_changed(GtkWidget *widget, gpointer data)
{
	pthread_mutex_lock(&(sndflddistort.fdmutex));
	float newvalue = (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	if (sndflddistort.enabled)
	{
		soundfoldingdistort_close(&sndflddistort);
		soundfoldingdistort_init(newvalue, sndflddistort.gain, sndflddistort.format, sndflddistort.rate, sndflddistort.channels, &sndflddistort);
	}
	pthread_mutex_unlock(&(sndflddistort.fdmutex));
}

static void fdgain_changed(GtkWidget *widget, gpointer data)
{
	pthread_mutex_lock(&(sndflddistort.fdmutex));
	float newvalue = (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	if (sndflddistort.enabled)
	{
		soundfoldingdistort_close(&sndflddistort);
		soundfoldingdistort_init(sndflddistort.threshold, newvalue, sndflddistort.format, sndflddistort.rate, sndflddistort.channels, &sndflddistort);
	}
	pthread_mutex_unlock(&(sndflddistort.fdmutex));
}

static void mod_toggled(GtkWidget *togglebutton, gpointer data)
{
	pthread_mutex_lock(&(sndmod.modmutex));

	sndmod.enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton));
	if (sndmod.enabled)
	{
		soundmod_init((float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton14)), (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton15)), 
					sndmod.format, sndmod.rate, sndmod.channels, &sndmod);
	}
	else
	{
		soundmod_close(&sndmod);
	}
	pthread_mutex_unlock(&(sndmod.modmutex));
	//printf("toggle state %d\n", gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(modenable)));
}

static void modfreq_changed(GtkWidget *widget, gpointer data)
{
	pthread_mutex_lock(&(sndmod.modmutex));
	float newvalue = (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	if (sndmod.enabled)
	{
		soundmod_close(&sndmod);
		soundmod_init(newvalue, sndmod.moddepth, sndmod.format, sndmod.rate, sndmod.channels, &sndmod);
	}
	pthread_mutex_unlock(&(sndmod.modmutex));
}

static void moddepth_changed(GtkWidget *widget, gpointer data)
{
	pthread_mutex_lock(&(sndmod.modmutex));
	float newvalue = (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	if (sndmod.enabled)
	{
		soundmod_close(&sndmod);
		soundmod_init(sndmod.modfreq, newvalue, sndmod.format, sndmod.rate, sndmod.channels, &sndmod);
	}
	pthread_mutex_unlock(&(sndmod.modmutex));
}

static void cho_toggled(GtkWidget *togglebutton, gpointer data)
{
	pthread_mutex_lock(&(sndchorus.chomutex));

	sndchorus.enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton));
	if (sndchorus.enabled)
	{
		soundcho_init((int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton16)), sndchorus.format, sndchorus.rate, sndchorus.channels, &sndchorus);
	}
	else
	{
		soundcho_close(&sndchorus);
	}
	pthread_mutex_unlock(&(sndchorus.chomutex));
	//printf("toggle state %d\n", gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(choenable)));
}

static void cho_changed(GtkWidget *widget, gpointer data)
{
	pthread_mutex_lock(&(sndchorus.chomutex));
	int newvalue = (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	if (sndchorus.enabled)
	{
		soundcho_close(&sndchorus);
		soundcho_init(newvalue, sndchorus.format, sndchorus.rate, sndchorus.channels, &sndchorus);
	}
	pthread_mutex_unlock(&(sndchorus.chomutex));
}

static void mn_toggled(GtkWidget *togglebutton, gpointer data)
{
	struct soundmono *mn = (struct soundmono *)data;

	pthread_mutex_lock(&(mn->monomutex));
	mn->enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton));
	if (mn->enabled)
	{
		soundmono_close(mn);
		soundmono_init(mn->format, mn->rate, mn->channels, mn);
	}
	else
	{
		soundmono_close(mn);
	}
	pthread_mutex_unlock(&(mn->monomutex));
	//printf("toggle state %d\n", gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton)));
}

static void haas_toggled(GtkWidget *togglebutton, gpointer data)
{
	pthread_mutex_lock(&(sndhaas.haasmutex));
	sndhaas.enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton));
	if (sndhaas.enabled)
	{
		soundhaas_init((float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton17)), sndhaas.format, sndhaas.rate, sndhaas.channels, &sndhaas);
	}
	else
	{
		soundhaas_close(&sndhaas);
	}
	pthread_mutex_unlock(&(sndhaas.haasmutex));
	//printf("toggle state %d\n", gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(modenable)));
}

static void haasdly_changed(GtkWidget *widget, gpointer data)
{
	pthread_mutex_lock(&(sndhaas.haasmutex));
	float newvalue = (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	if (sndhaas.enabled)
	{
		soundhaas_close(&sndhaas);
		soundhaas_init(newvalue, sndhaas.format, sndhaas.rate, sndhaas.channels, &sndhaas);
	}
	pthread_mutex_unlock(&(sndhaas.haasmutex));
}

int main(int argc, char** argv)
{
	playerWidth = 800;
	playerHeight = playerWidth * 9 / 16;

	int dawidth = playerWidth;
	int daheight = playerHeight;

	int ret;
	if ((ret=pthread_mutex_init(&eqmutex, NULL)))
	{
		printf("EQ mutex init failed, %d\n", ret);
		return -1;
	}

	if ((ret=pthread_mutex_init(&(snddly.delaymutex), NULL)))
	{
		printf("Delay mutex init failed, %d\n", ret);
	}

	if ((ret=pthread_mutex_init(&(sndreverb.reverbmutex), NULL)))
	{
		printf("Reverb mutex init failed, %d\n", ret);
		return -1;
	}
	int i;
	for(i=0; i<REVERBDLINES; i++)
	{
		if ((ret=pthread_mutex_init(&(sndreverb.snddlyrev[i].delaymutex), NULL)))
		{
			printf("Reverb delay mutex init failed, %d\n", ret);
			return -1;
		}
	}

	if ((ret=pthread_mutex_init(&(sndtremolo.tremolomutex), NULL)))
	{
		printf("Tremolo mutex init failed, %d\n", ret);
		return -1;
	}

	if ((ret=pthread_mutex_init(&(sndflddistort.fdmutex), NULL)))
	{
		printf("Distortion mutex init failed, %d\n", ret);
		return -1;
	}

	if ((ret=pthread_mutex_init(&(sndmod.modmutex), NULL)))
	{
		printf("Modulator mutex init failed, %d\n", ret);
		return -1;
	}

	if ((ret=pthread_mutex_init(&(sndchorus.chomutex), NULL)))
	{
		printf("Chorus mutex init failed, %d\n", ret);
		return -1;
	}

	if ((ret=pthread_mutex_init(&(sndmono1.monomutex), NULL)))
	{
		printf("Mono mutex1 init failed, %d\n", ret);
		return -1;
	}

	if ((ret=pthread_mutex_init(&(sndhaas.haasmutex), NULL)))
	{
		printf("Haas mutex init failed, %d\n", ret);
		return -1;
	}

	if ((ret=pthread_mutex_init(&(sndmono2.monomutex), NULL)))
	{
		printf("Mono mutex2 init failed, %d\n", ret);
		return -1;
	}

     /* This is called in all GTK applications. Arguments are parsed
     * from the command line and are returned to the application. */
    gtk_init (&argc, &argv);
    
    /* create a new window */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    /* Sets the border width of the window. */
    gtk_container_set_border_width (GTK_CONTAINER (window), 2);
	gtk_widget_set_size_request(window, 100, 100);
	gtk_window_set_title(GTK_WINDOW(window), "Video Player");
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    /* When the window is given the "delete-event" signal (this is given
     * by the window manager, usually by the "close" option, or on the
     * titlebar), we ask it to call the delete_event () function
     * as defined above. The data passed to the callback
     * function is NULL and is ignored in the callback function. */
    g_signal_connect (window, "delete-event", G_CALLBACK (delete_event), NULL);
    
    /* Here we connect the "destroy" event to a signal handler.  
     * This event occurs when we call gtk_widget_destroy() on the window,
     * or if we return FALSE in the "delete-event" callback. */
    g_signal_connect (window, "destroy", G_CALLBACK (destroy), NULL);

    g_signal_connect (window, "realize", G_CALLBACK (realize_cb), NULL);
//printf("realized\n");

// vertical box
    playerbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), playerbox);

// box1 contents begin
// vertical box
    box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);
    //gtk_container_add(GTK_CONTAINER(window), box1);

// pixbuf
	pixbuf = NULL;
	initPixbuf(dawidth, daheight);

// drawing area
    dwgarea = gtk_drawing_area_new ();
    gtk_widget_set_size_request (dwgarea, dawidth, daheight);
    //gtk_widget_set_size_request (dwgarea, 1280, 720);
    gtk_container_add(GTK_CONTAINER(box1), dwgarea);

    // Signals used to handle the backing surface
    g_signal_connect (dwgarea, "draw", G_CALLBACK(draw_cb), NULL);
    gtk_widget_set_app_paintable(dwgarea, TRUE);

// horizontal scale
    hadjustment = gtk_adjustment_new(50, 0, 100, 1, 10, 0);
    hscale = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL, GTK_ADJUSTMENT(hadjustment));
    gtk_scale_set_digits(GTK_SCALE(hscale), 0);
    //g_signal_connect(hscale, "value-changed", G_CALLBACK(hscale_adjustment), NULL);
    g_signal_connect(hscale, "button-press-event", G_CALLBACK(scale_pressed), NULL);
    g_signal_connect(hscale, "button-release-event", G_CALLBACK(scale_released), NULL);
    gtk_container_add(GTK_CONTAINER(box1), hscale);

// horizontal box
    horibox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(box1), horibox);

// horizontal button box
    button_box = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout((GtkButtonBox *)button_box, GTK_BUTTONBOX_START);
    gtk_container_add(GTK_CONTAINER(horibox), button_box);

// button prev
    button9 = gtk_button_new_with_label("Prev");
    gtk_widget_set_sensitive (button9, FALSE);
    g_signal_connect(GTK_BUTTON(button9), "clicked", G_CALLBACK(button9_clicked), NULL);
    gtk_container_add(GTK_CONTAINER(button_box), button9);

// button play
    button1 = gtk_button_new_with_label("Play");
    g_signal_connect(GTK_BUTTON(button1), "clicked", G_CALLBACK(button1_clicked), NULL);
    gtk_container_add(GTK_CONTAINER(button_box), button1);

// button pause/resume
    button10 = gtk_button_new_with_label("Pause");
    gtk_widget_set_sensitive (button10, FALSE);
    g_signal_connect(GTK_BUTTON(button10), "clicked", G_CALLBACK(button10_clicked), NULL);
    gtk_container_add(GTK_CONTAINER(button_box), button10);

// button next
    button8 = gtk_button_new_with_label("Next");
    gtk_widget_set_sensitive (button8, FALSE);
    g_signal_connect(GTK_BUTTON(button8), "clicked", G_CALLBACK(button8_clicked), NULL);
    gtk_container_add(GTK_CONTAINER(button_box), button8);

// button stop
    button2 = gtk_button_new_with_label("Stop");
    gtk_widget_set_sensitive (button2, FALSE);
    g_signal_connect(GTK_BUTTON(button2), "clicked", G_CALLBACK(button2_clicked), NULL);
    gtk_container_add(GTK_CONTAINER(button_box), button2);

// button Parameters
    buttonParameters = gtk_button_new_with_label("AV");
    g_signal_connect(GTK_BUTTON(buttonParameters), "clicked", G_CALLBACK(buttonParameters_clicked), NULL);
    gtk_container_add(GTK_CONTAINER(button_box), buttonParameters);
// box1 contents end

// box2 contents begin
    box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);

    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_set_border_width(GTK_CONTAINER(scrolled_window), 10);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    gtk_widget_set_size_request(scrolled_window, dawidth, daheight);
    gtk_container_add(GTK_CONTAINER(box2), scrolled_window);

    listview = create_view_and_model(argc, argv);
    g_signal_connect(listview, "row-activated", (GCallback)listview_onRowActivated, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window), listview);

    button_box2 = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout((GtkButtonBox *)button_box2, GTK_BUTTONBOX_START);
    gtk_container_add(GTK_CONTAINER(box2), button_box2);

    button3 = gtk_button_new_with_label("Clear");
    g_signal_connect(GTK_BUTTON(button3), "clicked", G_CALLBACK(button3_clicked), NULL);
    gtk_container_add(GTK_CONTAINER(button_box2), button3);

    button6 = gtk_button_new_with_label("Catalog");
    g_signal_connect(GTK_BUTTON(button6), "clicked", G_CALLBACK(button6_clicked), NULL);
    gtk_container_add(GTK_CONTAINER(button_box2), button6);

    button7 = gtk_button_new_with_label("Add File");
    g_signal_connect(GTK_BUTTON(button7), "clicked", G_CALLBACK(button7_clicked), NULL);
    gtk_container_add(GTK_CONTAINER(button_box2), button7);

    button4 = gtk_button_new_with_label("Load");
    g_signal_connect(GTK_BUTTON(button4), "clicked", G_CALLBACK(button4_clicked), NULL);
    gtk_container_add(GTK_CONTAINER(button_box2), button4);
// box2 contents end

// eqvbox contents begin
// vertical box
    eqvbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);

// horizontal box    
    eqbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(eqvbox), eqbox);

// vertical scale 0
    eqbox0 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(eqbox), eqbox0);
    vadj0 = gtk_adjustment_new(0, -12, 12, 0.1, 1, 0);
    vscaleeq0 = gtk_scale_new(GTK_ORIENTATION_VERTICAL, GTK_ADJUSTMENT(vadj0));
    gtk_scale_set_digits(GTK_SCALE(vscaleeq0), 1);
    g_signal_connect(vscaleeq0, "value-changed", G_CALLBACK(vscale0), NULL);
	g_signal_connect(vscaleeq0, "format-value", G_CALLBACK(scale_valueformat), NULL);
    gtk_container_add(GTK_CONTAINER(eqbox0), vscaleeq0);
	eqlabel0 = gtk_label_new(eqlabels[0]);
	gtk_container_add(GTK_CONTAINER(eqbox0), eqlabel0);

// vertical scale 1
    eqbox1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(eqbox), eqbox1);
    vadj1 = gtk_adjustment_new(0, -12, 12, 0.1, 1, 0);
    vscaleeq1 = gtk_scale_new(GTK_ORIENTATION_VERTICAL, GTK_ADJUSTMENT(vadj1));
    gtk_scale_set_digits(GTK_SCALE(vscaleeq1), 1);
    g_signal_connect(vscaleeq1, "value-changed", G_CALLBACK(vscale1), NULL);
	g_signal_connect(vscaleeq1, "format-value", G_CALLBACK(scale_valueformat), NULL);
    gtk_container_add(GTK_CONTAINER(eqbox1), vscaleeq1);
	eqlabel1 = gtk_label_new(eqlabels[1]);
	gtk_container_add(GTK_CONTAINER(eqbox1), eqlabel1);

// vertical scale 2
    eqbox2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(eqbox), eqbox2);
    vadj2 = gtk_adjustment_new(0, -12, 12, 0.1, 1, 0);
    vscaleeq2 = gtk_scale_new(GTK_ORIENTATION_VERTICAL, GTK_ADJUSTMENT(vadj2));
    gtk_scale_set_digits(GTK_SCALE(vscaleeq2), 1);
    g_signal_connect(vscaleeq2, "value-changed", G_CALLBACK(vscale2), NULL);
	g_signal_connect(vscaleeq2, "format-value", G_CALLBACK(scale_valueformat), NULL);
    gtk_container_add(GTK_CONTAINER(eqbox2), vscaleeq2);
	eqlabel2 = gtk_label_new(eqlabels[2]);
	gtk_container_add(GTK_CONTAINER(eqbox2), eqlabel2);

// vertical scale 3
    eqbox3 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(eqbox), eqbox3);
    vadj3 = gtk_adjustment_new(0, -12, 12, 0.1, 1, 0);
    vscaleeq3 = gtk_scale_new(GTK_ORIENTATION_VERTICAL, GTK_ADJUSTMENT(vadj3));
    gtk_scale_set_digits(GTK_SCALE(vscaleeq3), 1);
    g_signal_connect(vscaleeq3, "value-changed", G_CALLBACK(vscale3), NULL);
	g_signal_connect(vscaleeq3, "format-value", G_CALLBACK(scale_valueformat), NULL);
    gtk_container_add(GTK_CONTAINER(eqbox3), vscaleeq3);
	eqlabel3 = gtk_label_new(eqlabels[3]);
	gtk_container_add(GTK_CONTAINER(eqbox3), eqlabel3);

// vertical scale 4
    eqbox4 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(eqbox), eqbox4);
    vadj4 = gtk_adjustment_new(0, -12, 12, 0.1, 1, 0);
    vscaleeq4 = gtk_scale_new(GTK_ORIENTATION_VERTICAL, GTK_ADJUSTMENT(vadj4));
    gtk_scale_set_digits(GTK_SCALE(vscaleeq4), 1);
    g_signal_connect(vscaleeq4, "value-changed", G_CALLBACK(vscale4), NULL);
	g_signal_connect(vscaleeq4, "format-value", G_CALLBACK(scale_valueformat), NULL);
    gtk_container_add(GTK_CONTAINER(eqbox4), vscaleeq4);
	eqlabel4 = gtk_label_new(eqlabels[4]);
	gtk_container_add(GTK_CONTAINER(eqbox4), eqlabel4);

// vertical scale 5
    eqbox5 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(eqbox), eqbox5);
    vadj5 = gtk_adjustment_new(0, -12, 12, 0.1, 1, 0);
    vscaleeq5 = gtk_scale_new(GTK_ORIENTATION_VERTICAL, GTK_ADJUSTMENT(vadj5));
    gtk_scale_set_digits(GTK_SCALE(vscaleeq5), 1);
    g_signal_connect(vscaleeq5, "value-changed", G_CALLBACK(vscale5), NULL);
	g_signal_connect(vscaleeq5, "format-value", G_CALLBACK(scale_valueformat), NULL);
    gtk_container_add(GTK_CONTAINER(eqbox5), vscaleeq5);
	eqlabel5 = gtk_label_new(eqlabels[5]);
	gtk_container_add(GTK_CONTAINER(eqbox5), eqlabel5);

// vertical scale 6
    eqbox6 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(eqbox), eqbox6);
    vadj6 = gtk_adjustment_new(0, -12, 12, 0.1, 1, 0);
    vscaleeq6 = gtk_scale_new(GTK_ORIENTATION_VERTICAL, GTK_ADJUSTMENT(vadj6));
    gtk_scale_set_digits(GTK_SCALE(vscaleeq6), 1);
    g_signal_connect(vscaleeq6, "value-changed", G_CALLBACK(vscale6), NULL);
	g_signal_connect(vscaleeq6, "format-value", G_CALLBACK(scale_valueformat), NULL);
    gtk_container_add(GTK_CONTAINER(eqbox6), vscaleeq6);
	eqlabel6 = gtk_label_new(eqlabels[6]);
	gtk_container_add(GTK_CONTAINER(eqbox6), eqlabel6);

// vertical scale 7
    eqbox7 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(eqbox), eqbox7);
    vadj7 = gtk_adjustment_new(0, -12, 12, 0.1, 1, 0);
    vscaleeq7 = gtk_scale_new(GTK_ORIENTATION_VERTICAL, GTK_ADJUSTMENT(vadj7));
    gtk_scale_set_digits(GTK_SCALE(vscaleeq7), 1);
    g_signal_connect(vscaleeq7, "value-changed", G_CALLBACK(vscale7), NULL);
	g_signal_connect(vscaleeq7, "format-value", G_CALLBACK(scale_valueformat), NULL);
    gtk_container_add(GTK_CONTAINER(eqbox7), vscaleeq7);
	eqlabel7 = gtk_label_new(eqlabels[7]);
	gtk_container_add(GTK_CONTAINER(eqbox7), eqlabel7);

// vertical scale 8
    eqbox8 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(eqbox), eqbox8);
    vadj8 = gtk_adjustment_new(0, -12, 12, 0.1, 1, 0);
    vscaleeq8 = gtk_scale_new(GTK_ORIENTATION_VERTICAL, GTK_ADJUSTMENT(vadj8));
    gtk_scale_set_digits(GTK_SCALE(vscaleeq8), 1);
    g_signal_connect(vscaleeq8, "value-changed", G_CALLBACK(vscale8), NULL);
	g_signal_connect(vscaleeq8, "format-value", G_CALLBACK(scale_valueformat), NULL);
    gtk_container_add(GTK_CONTAINER(eqbox8), vscaleeq8);
	eqlabel8 = gtk_label_new(eqlabels[8]);
	gtk_container_add(GTK_CONTAINER(eqbox8), eqlabel8);

// vertical scale 9
    eqbox9 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(eqbox), eqbox9);
    vadj9 = gtk_adjustment_new(0, -12, 12, 0.1, 1, 0);
    vscaleeq9 = gtk_scale_new(GTK_ORIENTATION_VERTICAL, GTK_ADJUSTMENT(vadj9));
    gtk_scale_set_digits(GTK_SCALE(vscaleeq9), 1);
    g_signal_connect(vscaleeq9, "value-changed", G_CALLBACK(vscale9), NULL);
	g_signal_connect(vscaleeq9, "format-value", G_CALLBACK(scale_valueformat), NULL);
    gtk_container_add(GTK_CONTAINER(eqbox9), vscaleeq9);
	eqlabel9 = gtk_label_new(eqlabels[9]);
	gtk_container_add(GTK_CONTAINER(eqbox9), eqlabel9);

// vertical scale preamp
    eqboxA = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(eqbox), eqboxA);
    vadjA = gtk_adjustment_new(0, 0, 16.0, 0.01, 0.1, 0);
    vscaleeqA = gtk_scale_new(GTK_ORIENTATION_VERTICAL, GTK_ADJUSTMENT(vadjA));
    gtk_scale_set_digits(GTK_SCALE(vscaleeqA), 2);
    g_signal_connect(vscaleeqA, "value-changed", G_CALLBACK(vscaleA), NULL);
	g_signal_connect(vscaleeqA, "format-value", G_CALLBACK(scale_valuepreamp), NULL);
    gtk_container_add(GTK_CONTAINER(eqboxA), vscaleeqA);
	eqlabelA = gtk_label_new("Preamp");
	gtk_container_add(GTK_CONTAINER(eqboxA), eqlabelA);

// horizontal box
    eqbox2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(eqvbox), eqbox2);

// checkbox
	eqenable = gtk_check_button_new_with_label("EQ Enable");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(eqenable), TRUE);
	g_signal_connect(GTK_TOGGLE_BUTTON(eqenable), "toggled", G_CALLBACK(eq_toggled), NULL);
	gtk_container_add(GTK_CONTAINER(eqbox2), eqenable);

// checkbox
	eqautolevel = gtk_check_button_new_with_label("EQ Auto Level");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(eqautolevel), TRUE);
	g_signal_connect(GTK_TOGGLE_BUTTON(eqautolevel), "toggled", G_CALLBACK(eq_autotoggled), NULL);
	gtk_container_add(GTK_CONTAINER(eqbox2), eqautolevel);

// combobox
    combopreset = gtk_combo_box_text_new();
    select_eqpreset_names();
    g_signal_connect(GTK_COMBO_BOX(combopreset), "changed", G_CALLBACK(preset_changed), NULL);
    gtk_container_add(GTK_CONTAINER(eqbox2), combopreset);

// eqvbox contents end

// dlyvbox contents begin
    dlyvbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);

// delay frame
    framedelay1 = gtk_frame_new("Delay");
    gtk_container_add(GTK_CONTAINER(dlyvbox), framedelay1);

// horizontal box
    dlybox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(framedelay1), dlybox1);

// checkbox
	snddly.enabled = FALSE;
	dlyenable = gtk_check_button_new_with_label("Enable");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dlyenable), snddly.enabled);
	g_signal_connect(GTK_TOGGLE_BUTTON(dlyenable), "toggled", G_CALLBACK(dly_toggled), NULL);
	gtk_container_add(GTK_CONTAINER(dlybox1), dlyenable);

// combobox
	delaytypelabel = gtk_label_new("Delay Type");
	gtk_widget_set_size_request(delaytypelabel, 100, 30);
	gtk_container_add(GTK_CONTAINER(dlybox1), delaytypelabel);
    combodelaytype = gtk_combo_box_text_new();
    select_delay_types();
    g_signal_connect(GTK_COMBO_BOX(combodelaytype), "changed", G_CALLBACK(delaytype_changed), NULL);
    gtk_container_add(GTK_CONTAINER(dlybox1), combodelaytype);

// delay
	delaylabel1 = gtk_label_new("Delay (ms)");
	gtk_widget_set_size_request(delaylabel1, 100, 30);
	gtk_container_add(GTK_CONTAINER(dlybox1), delaylabel1);
	spinbutton5 = gtk_spin_button_new_with_range(0.0, 2000.0, 10.0);
	gtk_widget_set_size_request(spinbutton5, 120, 30);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton5), 500.0);
	g_signal_connect(GTK_SPIN_BUTTON(spinbutton5), "value-changed", G_CALLBACK(delay_changed), NULL);
	gtk_container_add(GTK_CONTAINER(dlybox1), spinbutton5);

// feedback
	feedbacklabel1 = gtk_label_new("Feedback");
	gtk_widget_set_size_request(feedbacklabel1, 100, 30);
	gtk_container_add(GTK_CONTAINER(dlybox1), feedbacklabel1);
	spinbutton6 = gtk_spin_button_new_with_range(0.0, 0.95, 0.01);
	gtk_widget_set_size_request(spinbutton6, 120, 30);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton6), 0.7);
	g_signal_connect(GTK_SPIN_BUTTON(spinbutton6), "value-changed", G_CALLBACK(feedback_changed), NULL);
	gtk_container_add(GTK_CONTAINER(dlybox1), spinbutton6);

// reverb frame
	framervrb1 = gtk_frame_new("Reverb");
	gtk_container_add(GTK_CONTAINER(dlyvbox), framervrb1);

// vertical box
	rvrbboxv = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);
	gtk_container_add(GTK_CONTAINER(framervrb1), rvrbboxv);

// horizontal box
	rvrbbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_container_add(GTK_CONTAINER(rvrbboxv), rvrbbox1);

// checkbox
	sndreverb.enabled = FALSE;
	rvrbenable = gtk_check_button_new_with_label("Enable");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rvrbenable),sndreverb.enabled);
	g_signal_connect(GTK_TOGGLE_BUTTON(rvrbenable), "toggled", G_CALLBACK(rvrb_toggled), NULL);
	gtk_container_add(GTK_CONTAINER(rvrbbox1), rvrbenable);

// presence
	rvrblabel2 = gtk_label_new("Presence");
	gtk_widget_set_size_request(rvrblabel2, 100, 30);
	gtk_container_add(GTK_CONTAINER(rvrbbox1), rvrblabel2);
	spinbutton8 = gtk_spin_button_new_with_range(0.1, 2.00, 0.1);
	gtk_widget_set_size_request(spinbutton8, 120, 30);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton8), 0.6);
	g_signal_connect(GTK_SPIN_BUTTON(spinbutton8), "value-changed", G_CALLBACK(rvrbpresence_changed), NULL);
	gtk_container_add(GTK_CONTAINER(rvrbbox1), spinbutton8);

// feedback
	rvrblabel1 = gtk_label_new("Feedback");
	gtk_widget_set_size_request(rvrblabel1, 100, 30);
	gtk_container_add(GTK_CONTAINER(rvrbbox1), rvrblabel1);
	spinbutton7 = gtk_spin_button_new_with_range(0.0, 0.95, 0.01);
	gtk_widget_set_size_request(spinbutton7, 120, 30);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton7), 0.65);
	g_signal_connect(GTK_SPIN_BUTTON(spinbutton7), "value-changed", G_CALLBACK(rvrbfeedback_changed), NULL);
	gtk_container_add(GTK_CONTAINER(rvrbbox1), spinbutton7);

// horizontal box
	rvrbbox2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_container_add(GTK_CONTAINER(rvrbboxv), rvrbbox2);

// combobox
	rvrbdelays = gtk_label_new("Delay Lines");
	gtk_widget_set_size_request(rvrbdelays, 100, 30);
	gtk_container_add(GTK_CONTAINER(rvrbbox2), rvrbdelays);
    comborvrbdelays = gtk_combo_box_text_new();
    select_delay_lines();
    g_signal_connect(GTK_COMBO_BOX(comborvrbdelays), "changed", G_CALLBACK(delaylines_changed), NULL);
    gtk_container_add(GTK_CONTAINER(rvrbbox2), comborvrbdelays);

// LSH
	rvrblabel4 = gtk_label_new("LSH (Hz)");
	gtk_widget_set_size_request(rvrblabel4, 100, 30);
	gtk_container_add(GTK_CONTAINER(rvrbbox2), rvrblabel4);
	spinbutton18 = gtk_spin_button_new_with_range(1.0, 1000.0, 100.0);
	gtk_widget_set_size_request(spinbutton18, 120, 30);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton18), 500.0);
	g_signal_connect(GTK_SPIN_BUTTON(spinbutton18), "value-changed", G_CALLBACK(rvrbLSH_changed), NULL);
	gtk_container_add(GTK_CONTAINER(rvrbbox2), spinbutton18);

// LPF
	rvrblabel3 = gtk_label_new("LPF (Hz)");
	gtk_widget_set_size_request(rvrblabel3, 100, 30);
	gtk_container_add(GTK_CONTAINER(rvrbbox2), rvrblabel3);
	spinbutton11 = gtk_spin_button_new_with_range(32.0, 20000.0, 100.0);
	gtk_widget_set_size_request(spinbutton11, 120, 30);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton11), 5000.0);
	g_signal_connect(GTK_SPIN_BUTTON(spinbutton11), "value-changed", G_CALLBACK(rvrbLPF_changed), NULL);
	gtk_container_add(GTK_CONTAINER(rvrbbox2), spinbutton11);

// tremolo frame
    frametremolo1 = gtk_frame_new("Tremolo");
    gtk_container_add(GTK_CONTAINER(dlyvbox), frametremolo1);

// horizontal box
    tremolobox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(frametremolo1), tremolobox1);

// checkbox
	sndtremolo.enabled = FALSE;
	tremoloenable = gtk_check_button_new_with_label("Enable");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tremoloenable), sndtremolo.enabled);
	g_signal_connect(GTK_TOGGLE_BUTTON(tremoloenable), "toggled", G_CALLBACK(tremolo_toggled), NULL);
	gtk_container_add(GTK_CONTAINER(tremolobox1), tremoloenable);

// depth
	tremololabel1 = gtk_label_new("Depth");
	gtk_widget_set_size_request(tremololabel1, 100, 30);
	gtk_container_add(GTK_CONTAINER(tremolobox1), tremololabel1);
	spinbutton9 = gtk_spin_button_new_with_range(0.00, 1.00, 0.1);
	gtk_widget_set_size_request(spinbutton9, 120, 30);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton9), 0.5);
	g_signal_connect(GTK_SPIN_BUTTON(spinbutton9), "value-changed", G_CALLBACK(tremolodepth_changed), NULL);
	gtk_container_add(GTK_CONTAINER(tremolobox1), spinbutton9);

// tremolo rate
	tremololabel2 = gtk_label_new("Rate (Hz)");
	gtk_widget_set_size_request(tremololabel2, 100, 30);
	gtk_container_add(GTK_CONTAINER(tremolobox1), tremololabel2);
	spinbutton10 = gtk_spin_button_new_with_range(0.1, 10.0, 0.1);
	gtk_widget_set_size_request(spinbutton10, 120, 30);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton10), 3.0);
	g_signal_connect(GTK_SPIN_BUTTON(spinbutton10), "value-changed", G_CALLBACK(tremolorate_changed), NULL);
	gtk_container_add(GTK_CONTAINER(tremolobox1), spinbutton10);

// folding distortion frame
    framefd1 = gtk_frame_new("Folding Distortion");
    gtk_container_add(GTK_CONTAINER(dlyvbox), framefd1);

// horizontal box
    fdbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(framefd1), fdbox1);

// checkbox
	sndflddistort.enabled = FALSE;
	fdenable = gtk_check_button_new_with_label("Enable");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fdenable), sndflddistort.enabled);
	g_signal_connect(GTK_TOGGLE_BUTTON(fdenable), "toggled", G_CALLBACK(fd_toggled), NULL);
	gtk_container_add(GTK_CONTAINER(fdbox1), fdenable);

// threshold
	fdlabel1 = gtk_label_new("Threshold");
	gtk_widget_set_size_request(fdlabel1, 100, 30);
	gtk_container_add(GTK_CONTAINER(fdbox1), fdlabel1);
	spinbutton12 = gtk_spin_button_new_with_range(100.0, 32767.0, 1000.0);
	gtk_widget_set_size_request(spinbutton12, 120, 30);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton12), 2000.0);
	g_signal_connect(GTK_SPIN_BUTTON(spinbutton12), "value-changed", G_CALLBACK(fdthreshold_changed), NULL);
	gtk_container_add(GTK_CONTAINER(fdbox1), spinbutton12);

// gain
	fdlabel2 = gtk_label_new("Gain");
	gtk_widget_set_size_request(fdlabel2, 100, 30);
	gtk_container_add(GTK_CONTAINER(fdbox1), fdlabel2);
	spinbutton13 = gtk_spin_button_new_with_range(1.0, 20.0, 0.1);
	gtk_widget_set_size_request(spinbutton13, 120, 30);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton13), 2.0);
	g_signal_connect(GTK_SPIN_BUTTON(spinbutton13), "value-changed", G_CALLBACK(fdgain_changed), NULL);
	gtk_container_add(GTK_CONTAINER(fdbox1), spinbutton13);

// modulation frame
    framemod1 = gtk_frame_new("Modulation");
    gtk_container_add(GTK_CONTAINER(dlyvbox), framemod1);

// horizontal box
    modbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(framemod1), modbox1);

// checkbox
	sndmod.enabled = FALSE;
	modenable = gtk_check_button_new_with_label("Enable");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(modenable), sndmod.enabled);
	g_signal_connect(GTK_TOGGLE_BUTTON(modenable), "toggled", G_CALLBACK(mod_toggled), NULL);
	gtk_container_add(GTK_CONTAINER(modbox1), modenable);

// rate
	modlabel1 = gtk_label_new("Rate (Hz)");
	gtk_widget_set_size_request(modlabel1, 100, 30);
	gtk_container_add(GTK_CONTAINER(modbox1), modlabel1);
	spinbutton14 = gtk_spin_button_new_with_range(0.1, 20.0, 0.1);
	gtk_widget_set_size_request(spinbutton14, 120, 30);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton14), 1.0);
	g_signal_connect(GTK_SPIN_BUTTON(spinbutton14), "value-changed", G_CALLBACK(modfreq_changed), NULL);
	gtk_container_add(GTK_CONTAINER(modbox1), spinbutton14);

// depth
	modlabel2 = gtk_label_new("Depth");
	gtk_widget_set_size_request(modlabel2, 100, 30);
	gtk_container_add(GTK_CONTAINER(modbox1), modlabel2);
	spinbutton15 = gtk_spin_button_new_with_range(0.001, 0.100, 0.001);
	gtk_widget_set_size_request(spinbutton15, 120, 30);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton15), 0.003);
	g_signal_connect(GTK_SPIN_BUTTON(spinbutton15), "value-changed", G_CALLBACK(moddepth_changed), NULL);
	gtk_container_add(GTK_CONTAINER(modbox1), spinbutton15);

// chorus frame
    framecho1 = gtk_frame_new("Chorus");
    gtk_container_add(GTK_CONTAINER(dlyvbox), framecho1);

// horizontal box
    chobox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(framecho1), chobox1);

// checkbox
	sndchorus.enabled = FALSE;
	choenable = gtk_check_button_new_with_label("Enable");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(choenable), sndchorus.enabled);
	g_signal_connect(GTK_TOGGLE_BUTTON(choenable), "toggled", G_CALLBACK(cho_toggled), NULL);
	gtk_container_add(GTK_CONTAINER(chobox1), choenable);

// max chorus
	cholabel1 = gtk_label_new("Max");
	gtk_widget_set_size_request(cholabel1, 100, 30);
	gtk_container_add(GTK_CONTAINER(chobox1), cholabel1);
	spinbutton16 = gtk_spin_button_new_with_range(1, MAXCHORUS, 1);
	gtk_widget_set_size_request(spinbutton16, 120, 30);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton16), 1);
	g_signal_connect(GTK_SPIN_BUTTON(spinbutton16), "value-changed", G_CALLBACK(cho_changed), NULL);
	gtk_container_add(GTK_CONTAINER(chobox1), spinbutton16);

// haas frame
    framehaas1 = gtk_frame_new("Haas");
    gtk_container_add(GTK_CONTAINER(dlyvbox), framehaas1);

// horizontal box
    haasbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(framehaas1), haasbox1);
    
// mono checkbox 1
	sndmono1.enabled = FALSE;
	mnenable1 = gtk_check_button_new_with_label("Mono");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(mnenable1), sndmono1.enabled);
	g_signal_connect(GTK_TOGGLE_BUTTON(mnenable1), "toggled", G_CALLBACK(mn_toggled), (gpointer)&sndmono1);
	gtk_container_add(GTK_CONTAINER(haasbox1), mnenable1);

// haas enable
	sndhaas.enabled = FALSE;
	haasenable = gtk_check_button_new_with_label("Haas");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(haasenable), sndhaas.enabled);
	g_signal_connect(GTK_TOGGLE_BUTTON(haasenable), "toggled", G_CALLBACK(haas_toggled), NULL);
	gtk_container_add(GTK_CONTAINER(haasbox1), haasenable);

// haas delay
	haaslabel1 = gtk_label_new("Delay (ms)");
	gtk_widget_set_size_request(haaslabel1, 100, 30);
	gtk_container_add(GTK_CONTAINER(haasbox1), haaslabel1);
	spinbutton17 = gtk_spin_button_new_with_range(1.0, 30.0 , 0.1);
	gtk_widget_set_size_request(spinbutton17, 120, 30);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton17), 20.0);
	g_signal_connect(GTK_SPIN_BUTTON(spinbutton17), "value-changed", G_CALLBACK(haasdly_changed), NULL);
	gtk_container_add(GTK_CONTAINER(haasbox1), spinbutton17);

// mono checkbox 2
	sndmono2.enabled = FALSE;
	mnenable2 = gtk_check_button_new_with_label("Mono");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(mnenable2), sndmono2.enabled);
	g_signal_connect(GTK_TOGGLE_BUTTON(mnenable2), "toggled", G_CALLBACK(mn_toggled), (gpointer)&sndmono2);
	gtk_container_add(GTK_CONTAINER(haasbox1), mnenable2);



// ADT


// dlyvbox contents end

// stack switcher
    stack = gtk_stack_new();
    stackswitcher = gtk_stack_switcher_new();
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(stackswitcher), GTK_STACK(stack));
    gtk_container_add(GTK_CONTAINER(playerbox), stackswitcher);
    gtk_container_add(GTK_CONTAINER(playerbox), stack);

    gtk_stack_add_titled(GTK_STACK(stack), box1, "box1", "Player");
    gtk_stack_add_titled(GTK_STACK(stack), box2, "box2", "Playlist");
    gtk_stack_add_titled(GTK_STACK(stack), eqvbox, "eqvbox", "Equalizer");
    gtk_stack_add_titled(GTK_STACK(stack), dlyvbox, "dlyvbox", "Delay");


    /* create a new window for AV Parameters */
    windowparm = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(windowparm), GTK_WIN_POS_CENTER);
    /* When the window is given the "delete-event" signal (this is given
     * by the window manager, usually by the "close" option, or on the
     * titlebar), we ask it to call the delete_event () function
     * as defined above. The data passed to the callback
     * function is NULL and is ignored in the callback function. */
    g_signal_connect (windowparm, "delete-event", G_CALLBACK(delete_event_parm), NULL);
	g_signal_connect (windowparm, "realize", G_CALLBACK (realize_parm), NULL);

    gtk_container_set_border_width (GTK_CONTAINER (windowparm), 2);
	//gtk_widget_set_size_request(windowparm, 100, 100);
	gtk_window_set_title(GTK_WINDOW(windowparm), "AV Parameters");
	gtk_window_set_resizable(GTK_WINDOW(windowparm), FALSE);

// vertical box
    parmvbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(windowparm), parmvbox);

// horizontal box 1
	parmhbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_container_add(GTK_CONTAINER(parmvbox), parmhbox1);
// audio queue max length
	parmlabel11 = gtk_label_new("AQ Length");
	gtk_widget_set_size_request(parmlabel11, 170, 30);
	gtk_container_add(GTK_CONTAINER(parmhbox1), parmlabel11);
	parmlabel12 = gtk_label_new("0");
	gtk_widget_set_size_request(parmlabel12, 30, 30);
	gtk_container_add(GTK_CONTAINER(parmhbox1), parmlabel12);
	spinbutton1 = gtk_spin_button_new_with_range (10.0, 100.0, 1.0);
	gtk_widget_set_size_request(spinbutton1, 120, 30);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton1), aqMaxLength);
	g_signal_connect(GTK_SPIN_BUTTON(spinbutton1), "value-changed", G_CALLBACK(aqmaxlength_changed), NULL);
	gtk_container_add(GTK_CONTAINER(parmhbox1), spinbutton1);

// horizontal box 2
	parmhbox2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_container_add(GTK_CONTAINER(parmvbox), parmhbox2);
// video queue max length
	parmlabel21 = gtk_label_new("VQ Length");
	gtk_widget_set_size_request(parmlabel21, 170, 30);
	gtk_container_add(GTK_CONTAINER(parmhbox2), parmlabel21);
	parmlabel22 = gtk_label_new("0");
	gtk_widget_set_size_request(parmlabel22, 30, 30);
	gtk_container_add(GTK_CONTAINER(parmhbox2), parmlabel22);
	spinbutton2 = gtk_spin_button_new_with_range (10.0, 100.0, 1.0);
	gtk_widget_set_size_request(spinbutton2, 120, 30);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton2), vqMaxLength);
	g_signal_connect(GTK_SPIN_BUTTON(spinbutton2), "value-changed", G_CALLBACK(vqmaxlength_changed), NULL);
	gtk_container_add(GTK_CONTAINER(parmhbox2), spinbutton2);

// horizontal box 3
	parmhbox3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_container_add(GTK_CONTAINER(parmvbox), parmhbox3);
// thread count
	parmlabel3 = gtk_label_new("Thread Count");
	gtk_widget_set_size_request(parmlabel3, 200, 30);
	gtk_container_add(GTK_CONTAINER(parmhbox3), parmlabel3);
	spinbutton3 = gtk_spin_button_new_with_range (1.0, 8.0, 1.0);
	gtk_widget_set_size_request(spinbutton3, 120, 30);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton3), thread_count);
	g_signal_connect(GTK_SPIN_BUTTON(spinbutton3), "value-changed", G_CALLBACK(threadcount_changed), NULL);
	gtk_container_add(GTK_CONTAINER(parmhbox3), spinbutton3);

// horizontal box 4
	parmhbox4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_container_add(GTK_CONTAINER(parmvbox), parmhbox4);
// player width
	parmlabel4 = gtk_label_new("Player Width");
	gtk_widget_set_size_request(parmlabel4, 200, 30);
	gtk_container_add(GTK_CONTAINER(parmhbox4), parmlabel4);
	spinbutton4 = gtk_spin_button_new_with_range (320.0, 1920.0, 80.0);
	gtk_widget_set_size_request(spinbutton4, 120, 30);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton4), playerWidth);
	g_signal_connect(GTK_SPIN_BUTTON(spinbutton4), "value-changed", G_CALLBACK(playerwidth_changed), NULL);
	gtk_container_add(GTK_CONTAINER(parmhbox4), spinbutton4);

// levelbars
	parmhlevel1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_container_add(GTK_CONTAINER(parmvbox), parmhlevel1);
	levellabel1 = gtk_label_new("Read Frame");
	gtk_widget_set_size_request(levellabel1, 200, 30);
	gtk_container_add(GTK_CONTAINER(parmhlevel1), levellabel1);
	levelbar1 = gtk_level_bar_new_for_interval(0.0, 20000.0);
	gtk_level_bar_set_value(GTK_LEVEL_BAR(levelbar1), 0.0);
	gtk_widget_set_size_request(levelbar1, 100, 30);
	gtk_container_add(GTK_CONTAINER(parmhlevel1), levelbar1);
	label1 = gtk_label_new("0");
	gtk_widget_set_size_request(label1, 50, 30);
	gtk_container_add(GTK_CONTAINER(parmhlevel1), label1);

	parmhlevel2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_container_add(GTK_CONTAINER(parmvbox), parmhlevel2);
	levellabel2 = gtk_label_new("Decode");
	gtk_widget_set_size_request(levellabel2, 200, 30);
	gtk_container_add(GTK_CONTAINER(parmhlevel2), levellabel2);
	levelbar2 = gtk_level_bar_new_for_interval(0.0, 20000.0);
	gtk_level_bar_set_value(GTK_LEVEL_BAR(levelbar2), 0.0);
	gtk_widget_set_size_request(levelbar2, 100, 30);
	gtk_container_add(GTK_CONTAINER(parmhlevel2), levelbar2);
	label2 = gtk_label_new("0");
	gtk_widget_set_size_request(label2, 50, 30);
	gtk_container_add(GTK_CONTAINER(parmhlevel2), label2);

	parmhlevel3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_container_add(GTK_CONTAINER(parmvbox), parmhlevel3);
	levellabel3 = gtk_label_new("TexImage2D");
	gtk_widget_set_size_request(levellabel3, 200, 30);
	gtk_container_add(GTK_CONTAINER(parmhlevel3), levellabel3);
	levelbar3 = gtk_level_bar_new_for_interval(0.0, 20000.0);
	gtk_level_bar_set_value(GTK_LEVEL_BAR(levelbar3), 0.0);
	gtk_widget_set_size_request(levelbar3, 100, 30);
	gtk_container_add(GTK_CONTAINER(parmhlevel3), levelbar3);
	label3 = gtk_label_new("0");
	gtk_widget_set_size_request(label3, 50, 30);
	gtk_container_add(GTK_CONTAINER(parmhlevel3), label3);

	parmhlevel4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_container_add(GTK_CONTAINER(parmvbox), parmhlevel4);
	levellabel4 = gtk_label_new("Draw");
	gtk_widget_set_size_request(levellabel4, 200, 30);
	gtk_container_add(GTK_CONTAINER(parmhlevel4), levellabel4);
	levelbar4 = gtk_level_bar_new_for_interval(0.0, 20000.0);
	gtk_level_bar_set_value(GTK_LEVEL_BAR(levelbar4), 0.0);
	gtk_widget_set_size_request(levelbar4, 100, 30);
	gtk_container_add(GTK_CONTAINER(parmhlevel4), levelbar4);
	label4 = gtk_label_new("0");
	gtk_widget_set_size_request(label4, 50, 30);
	gtk_container_add(GTK_CONTAINER(parmhlevel4), label4);

	parmhlevel5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_container_add(GTK_CONTAINER(parmvbox), parmhlevel5);
	levellabel5 = gtk_label_new("Read Pixels");
	gtk_widget_set_size_request(levellabel5, 200, 30);
	gtk_container_add(GTK_CONTAINER(parmhlevel5), levellabel5);
	levelbar5 = gtk_level_bar_new_for_interval(0.0, 20000.0);
	gtk_level_bar_set_value(GTK_LEVEL_BAR(levelbar5), 0.0);
	gtk_widget_set_size_request(levelbar5, 100, 30);
	gtk_container_add(GTK_CONTAINER(parmhlevel5), levelbar5);
	label5 = gtk_label_new("0");
	gtk_widget_set_size_request(label5, 50, 30);
	gtk_container_add(GTK_CONTAINER(parmhlevel5), label5);

	parmhlevel6 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_container_add(GTK_CONTAINER(parmvbox), parmhlevel6);
	levellabel6 = gtk_label_new("Cairo");
	gtk_widget_set_size_request(levellabel6, 200, 30);
	gtk_container_add(GTK_CONTAINER(parmhlevel6), levellabel6);
	levelbar6 = gtk_level_bar_new_for_interval(0.0, 20000.0);
	gtk_level_bar_set_value(GTK_LEVEL_BAR(levelbar6), 0.0);
	gtk_widget_set_size_request(levelbar6, 100, 30);
	gtk_container_add(GTK_CONTAINER(parmhlevel6), levelbar6);
	label6 = gtk_label_new("0");
	gtk_widget_set_size_request(label6, 50, 30);
	gtk_container_add(GTK_CONTAINER(parmhlevel6), label6);

	parmhlevel7 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_container_add(GTK_CONTAINER(parmvbox), parmhlevel7);
	levellabel7 = gtk_label_new("Frame Drop");
	gtk_widget_set_size_request(levellabel7, 200, 30);
	gtk_container_add(GTK_CONTAINER(parmhlevel7), levellabel7);
	levelbar7 = gtk_level_bar_new_for_interval(0.0, 100.0);
	gtk_level_bar_set_value(GTK_LEVEL_BAR(levelbar7), 0.0);
	gtk_widget_set_size_request(levelbar7, 100, 30);
	gtk_container_add(GTK_CONTAINER(parmhlevel7), levelbar7);
	label7 = gtk_label_new("0 %");
	gtk_widget_set_size_request(label7, 50, 30);
	gtk_container_add(GTK_CONTAINER(parmhlevel7), label7);

    statusbar = gtk_statusbar_new();
    gtk_container_add(GTK_CONTAINER(playerbox), statusbar);
    push_message(statusbar, context_id, "");

    gtk_widget_show_all(window);
//printf("Show window\n");

	gtk_widget_show_all(windowparm);
	gtk_widget_hide(windowparm);

	// Commandline
	if (argc>1)
	{
		GtkTreePath *tp = gtk_tree_path_new_from_string("0");
		GtkTreeSelection *ts = gtk_tree_view_get_selection(GTK_TREE_VIEW(listview));
		gtk_tree_selection_select_path(ts, tp);
		listview_onRowActivated(GTK_TREE_VIEW(listview), tp, NULL, NULL);
		gtk_tree_path_free(tp);
	}

    /* All GTK applications must have a gtk_main(). Control ends here
     * and waits for an event to occur (like a key press or mouse event). */
    gtk_main();

	pthread_mutex_destroy(&seekmutex);

    return 0;
}
