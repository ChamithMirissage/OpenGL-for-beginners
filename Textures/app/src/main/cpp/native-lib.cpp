#include <jni.h>
#include <android/log.h>

#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cstdio>
#include <cstdlib>

#define  LOG_TAG    "native-lib-new"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

bool init(){
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepthf(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_DITHER);

    return true;
}

const GLfloat PI = 3.1415f;
/*
static void _gluPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar){
    GLfloat top = zNear * ((GLfloat) tan(fovy * PI / 360.0));
    GLfloat bottom = -top;
    GLfloat left = bottom * aspect;
    GLfloat right = top * aspect;
    glFrustumf(left, right, bottom, top, zNear, zFar);
}
*/

void resize(int height, int width){
    if (height == 0){
        height = 1;
    }

    glViewport(0, 0, width, height);
    //GLfloat aspect = (GLfloat)width / (GLfloat)height;
    //_gluPerspective(45.0f, aspect, 0.1f, 100.0f);

    /* Setup the perspective. */
    //int projectionMatrix = 0;
    //matrixPerspective(projectionMatrix, 45, (float)width / (float)height, 0.1f, 100);
    //glEnable(GL_DEPTH_TEST);
}

static const char glVertexShader[] =
        "attribute vec4 vertexPosition;\n"
        "attribute vec2 vertexTextureCord;\n"
        "varying vec2 textureCord;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 modelView;\n"
        "void main()\n"
        "{\n"
        //"    gl_Position = projection * modelView * vertexPosition;\n"
        "    gl_Position = vertexPosition;\n"
        "    textureCord = vertexTextureCord;\n"
        "}\n";

static const char glFragmentShader[] =
        "precision mediump float;\n"
        "uniform sampler2D texture;\n"
        "varying vec2 textureCord;\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = texture2D(texture, textureCord);\n"
        "}\n";

GLuint loadShader(GLenum shaderType, const char* shaderSource)
{
    GLuint shader = glCreateShader(shaderType);
    if (shader)
    {
        glShaderSource(shader, 1, &shaderSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen)
            {
                char * buf = (char*) malloc(infoLen);
                if (buf)
                {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGE("Could not Compile Shader %d:\n%s\n", shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

GLuint loadSimpleTexture()
{
    /* Texture Object Handle. */
    GLuint textureId;
    /* 3 x 3 Image,  R G B A Channels RAW Format. */
    GLubyte pixels[9 * 4] =
            {
                    18,  140, 171, 255, /* Some Colour Bottom Left. */
                    143, 143, 143, 255, /* Some Colour Bottom Middle. */
                    255, 255, 255, 255, /* Some Colour Bottom Right. */
                    255, 255, 0,   255, /* Yellow Middle Left. */
                    0,   255, 255, 255, /* Some Colour Middle. */
                    255, 0,   255, 255, /* Some Colour Middle Right. */
                    255, 0,   0,   255, /* Red Top Left. */
                    0,   255, 0,   255, /* Green Top Middle. */
                    0,   0,   255, 255, /* Blue Top Right. */
            };
    /* Use tightly packed data. */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    /* Generate a texture object. */
    glGenTextures(1, &textureId);
    /* Activate a texture. */
    glActiveTexture(GL_TEXTURE0);
    /* Bind the texture object. */
    glBindTexture(GL_TEXTURE_2D, textureId);
    /* Load the texture. */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 3, 3, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    /* Set the filtering mode. */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return textureId;
}

GLuint createProgram(const char* vertexSource, const char * fragmentSource)
{
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
    if (!vertexShader)
    {
        return 0;
    }
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (!fragmentShader)
    {
        return 0;
    }
    GLuint program = glCreateProgram();
    if (program)
    {
        glAttachShader(program , vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program , GL_LINK_STATUS, &linkStatus);
        if( linkStatus != GL_TRUE)
        {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength)
            {
                char* buf = (char*) malloc(bufLength);
                if (buf)
                {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGE("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

// Square
const GLfloat vertices[] = {0.0f, 1.0f,
                            -0.5f, 1.0f,
                            -0.5f, 0.0f,
                            0.0f, 0.0f};

GLfloat textureCords[] = {0.0f, 1.0f,
                          0.0f, 0.0f,
                          1.0f, 0.0f,
                          1.0f, 1.0f};
///
GLuint glProgram;

const int numSegments = 100;

void renderFrame(){
    // Circle
    GLfloat points[numSegments*2];
    GLfloat texCoords[numSegments*2];
    size_t size = 0;

    GLfloat angle;
    for (int i=0; i < numSegments; i++){
        angle = i * 2.0f * PI / numSegments;
        //points[size++] = -0.5f + cos(angle)*0.25f;
        //points[size++] = sin(angle)*0.5f;
        points[(i+1-1)*2+0] = -0.5f + cos(angle)*0.25f;
        points[(i+1-1)*2+1] = sin(angle)*0.5f;

        texCoords[(i+1-1)*2+0] = 0.5 + 0.5 * cos(angle);
        texCoords[(i+1-1)*2+1] = 0.5 - 0.5 * sin(angle);
    }
    ///

    glProgram = createProgram(glVertexShader, glFragmentShader);
    //if (!simpleTriangleProgram)
    //{
    //    LOGE ("Could not create program");
    //    return false;
    //}

    glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glUseProgram(glProgram);

    GLint vertexLocation = glGetAttribLocation(glProgram, "vertexPosition");
    GLint textureCordLocation = glGetAttribLocation(glProgram, "vertexTextureCord");
    //GLint projectionLocation = glGetUniformLocation(glProgram, "projection");
    //GLint modelViewLocation = glGetUniformLocation(glProgram, "modelView");
    GLint samplerLocation = glGetUniformLocation(glProgram, "texture");

    /* Load the Texture. */
    GLuint textureId = loadSimpleTexture();
    /*
    if(textureId == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
    */

    glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 0, points);
    glVertexAttribPointer(textureCordLocation, 2, GL_FLOAT, GL_FALSE, 0, texCoords);

    glEnableVertexAttribArray(vertexLocation);
    glEnableVertexAttribArray(textureCordLocation);

    //glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, projectionMatrix);
    //glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, modelViewMatrix);

    /* Set the sampler texture unit to 0. */
    glUniform1i(samplerLocation, 0);


    glDrawArrays(GL_TRIANGLE_FAN, 0, numSegments);
    //glDrawElements(GL_TRIANGLE, 12, GL_UNSIGNED_SHORT, indices);

    //glFlush();
}

extern "C" JNIEXPORT void JNICALL Java_com_example_textures_MainActivity_resize(JNIEnv* env, jclass obj, jint width, jint height) {
    resize(width, height);
}

extern "C" JNIEXPORT void JNICALL Java_com_example_textures_MainActivity_step(JNIEnv* env, jclass obj) {
    renderFrame();
}

extern "C" JNIEXPORT void JNICALL Java_com_example_textures_MainActivity_init(JNIEnv* env, jclass obj) {
    init();
}