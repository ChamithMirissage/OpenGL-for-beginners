#include <jni.h>
#include <android/log.h>

#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cstdio>
#include <cstdlib>
#include <string>

#define  LOG_TAG    "native-lib-new"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

bool init(){
    LOGI("Enter init()");
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
    LOGI("Enter resize()");
    if (height == 0){
        height = 1;
    }
    glViewport(0, 0, width, height);
    //GLfloat aspect = (GLfloat)width / (GLfloat)height;
    //_gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}

static const char glVertexShader[] =
        "attribute vec4 vPosition;\n"
        //"attribute vec4 vColor;\n"
        //"varying vec4 vertexColor;\n"
        "void main()\n"
        "{\n"
        "  gl_Position = vPosition;\n"
        //"  vertexColor = vColor;\n"
        "}\n";

static const char glFragmentShader[] =
        "precision mediump float;\n"
        //"varying vec4 vertexColor;\n"
        "void main()\n"
        "{\n"
        //"  gl_FragColor = vertexColor;\n"
        "  gl_FragColor = vec4(0.8, 0.8, 0.0, 1.0);\n"
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

/* Square
const GLfloat vertices[] = {-0.1f, 0.05f,
                            -0.15f, 0.05f,
                            -0.15f, -0.05f,
                            -0.1f, -0.05};
GLfloat colours[] = {1.0f, 0.0f, 0.0f,
                     0.0f, 1.0f, 0.0f,
                     0.0f, 0.0f, 1.0f,
                     0.0f, 1.0f, 0.0f};
*/

// 2 squares
const GLfloat vertices[2][8] = {{-0.1f, 0.05f, -0.15f, 0.05f, -0.15f, -0.05f, -0.1f, -0.05},
                                {-0.7f, 0.05f, -0.75f, 0.05f, -0.75f, -0.05f, -0.7f, -0.05}};

/* Hexagon
const GLfloat vertices[] = {-0.25f, 1.0f,
                            -0.75f, 1.0f,
                            -1.0f, 0.5f,
                            -0.75f, 0.0f,
                            -0.25f, 0.0f,
                            0.0f, 0.5f};
GLfloat colours[] = {1.0f, 0.0f, 0.0f,
                     0.0f, 1.0f, 0.0f,
                     0.0f, 0.0f, 1.0f,
                     1.0f, 0.0f, 0.0f,
                     0.0f, 1.0f, 0.0f,
                     0.0f, 0.0f, 1.0f};
static GLushort indices[] = {0, 1, 2,
                             2, 3, 0,
                             0, 5, 4,
                             4, 3, 0};
*/

/* Line
const GLfloat vvertices[] = {-0.5f, 0.0f,
                             0.0f, 0.5f,
                             -0.5f, 1.0f};
*/

GLuint simpleTriangleProgram;
GLuint vPosition;
GLuint vColor;

const int numSegments = 100;

void renderFrame(){
    /* Circle
    GLfloat points[numSegments * 2];
    size_t size = 0;
    GLfloat angle;
    for (int i=0; i < numSegments; i++){
        angle = i * 2.0f * PI / numSegments;
        points[size++] = -0.5f + cos(angle)*0.25f;
        points[size++] = sin(angle)*0.5f;
    }
    */

    simpleTriangleProgram = createProgram(glVertexShader, glFragmentShader);
    //if (!simpleTriangleProgram)
    //{
    //    LOGE ("Could not create program");
    //    return false;
    //}

    glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glUseProgram(simpleTriangleProgram);

    vPosition = glGetAttribLocation(simpleTriangleProgram, "vPosition");
    //vColor = glGetAttribLocation(simpleTriangleProgram, "vColor");

    for (int i = 0; i < 2; i++){
        glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, vertices[i]);
        glEnableVertexAttribArray(vPosition);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

    //glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    //glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, colours);

    //glEnableVertexAttribArray(vPosition);
    //glEnableVertexAttribArray(vColor);

    //glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    //glDrawElements(GL_TRIANGLE, 12, GL_UNSIGNED_SHORT, indices);
    //glDrawElements(GL_TRIANGLES, indices.length, GL_UNSIGNED_BYTE, indexBuffer);
    //glDrawArrays(GL_LINE_STRIP, 0, numSegments);
    //glFlush();

}

extern "C" JNIEXPORT void JNICALL Java_com_example_draw2dshapes_MainActivity_resize(JNIEnv* env, jclass obj, jint width, jint height) {
    resize(width, height);
}

extern "C" JNIEXPORT void JNICALL Java_com_example_draw2dshapes_MainActivity_step(JNIEnv* env, jclass obj) {
    renderFrame();
}

extern "C" JNIEXPORT void JNICALL Java_com_example_draw2dshapes_MainActivity_init(JNIEnv* env, jclass obj) {
    init();
}