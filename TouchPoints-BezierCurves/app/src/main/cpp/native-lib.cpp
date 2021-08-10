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
#include <vector>

using namespace std;

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

float screen_width;
float screen_height;
//const GLfloat PI = 3.1415f;
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
    glViewport(0, 0, width*1, height*2);
    //GLfloat aspect = (GLfloat)width / (GLfloat)height;
    //_gluPerspective(45.0f, aspect, 0.1f, 100.0f);
    screen_width = width;
    screen_height = height;
    __android_log_print(ANDROID_LOG_INFO, "tag", "Screen height = %d Screen width = %d", width, height);
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
        "  gl_FragColor = vec4(0.8, 0.0, 0.8, 1.0);\n"
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

GLuint shaderProgram;
GLuint vPosition;
GLuint vColor;

vector<float> touchPoints;
vector<float> touchPointsAll;
GLfloat rectangleVertices[12];

// square dims
float dx = 0.005f;
float dy = 0.005f;
// control points
float p0x;
float p0y;
float p1x;
float p1y;
float p2x;
float p2y;

int counter = 0;
//float minGap = 2.0f;
//float previousX;
//float previousY;

void setRectangle(float x, float y){
    rectangleVertices[0] = x;
    rectangleVertices[1] = y;
    rectangleVertices[2] = x + dx;
    rectangleVertices[3] = y + dy;
    rectangleVertices[4] = x - dx;
    rectangleVertices[5] = y + dy;
    rectangleVertices[6] = x - dx;
    rectangleVertices[7] = y - dy;
    rectangleVertices[8] = x + dx;
    rectangleVertices[9] = y - dy;
    rectangleVertices[10] = x + dx;
    rectangleVertices[11] = y + dy;
}

void drawRectangle(float x_pixel, float y_pixel){
    float xCoord =  x_pixel * 2.0f / screen_width - 1.0f;
    //float yCoord =  y_pixel * -2.0f / screen_height + 2.75;
    float yCoord =  y_pixel * -1.0f / screen_height + 0.875f;
    //__android_log_print(ANDROID_LOG_INFO, "tag", "xcoord = %f ycoord = %f", xCoord, yCoord);

    setRectangle(xCoord, yCoord);

    glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, rectangleVertices);
    //glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, colours);

    glEnableVertexAttribArray(vPosition);
    //glEnableVertexAttribArray(vColor);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
    //glFlush();
}

/* find linear coordinates
float findYCoord(float x, float preX, float y, float preY, float xcoord){
    float deltaX = x - preX;
    float deltaY = y - preY;
    return ( preY + (xcoord-preX) * deltaY / deltaX );
}

float findXCoord(float x, float preX, float y, float preY, float ycoord){
    float deltaX = x - preX;
    float deltaY = y - preY;
    return ( preX + (ycoord-preY) * deltaX / deltaY );
}
*/

float findMidpointT(float x0, float y0, float x1, float y1, float x2, float y2){
    float ac = sqrt(pow(x0-x2, 2) + pow(y0-y2, 2));
    float ab = sqrt(pow(x0-x1, 2) + pow(y0-y1, 2));
    float theta_ac = atan2(abs(y0-y2), abs(x0-x2));
    float theta_ab = atan2(abs(y0-y1), abs(x0-x1));
    float ab_costheta = ab * cos(abs(theta_ab-theta_ac));
    float t = ab_costheta/ac;
    return t;
}

void setControlPoints(float x0, float y0, float x1, float y1, float x2, float y2, float t) {
    p0x = x0;
    p0y = y0;
    p2x = x2;
    p2y = y2;
    p1x = (x1 - (1.0f - t) * (1.0f - t) * p0x - t * t * p2x) / (2.0f * t * (1.0f - t));
    p1y = (y1 - (1.0f - t) * (1.0f - t) * p0y - t * t * p2y) / (2.0f * t * (1.0f - t));
}

float bezierFunctionX(float t){
    float xcoord = (1.0f-t)*(1.0f-t)*p0x + 2*(1.0f-t)*t*p1x + t*t*p2x;
    //float xcoord = (1.0f-t)*(1.0f-t)*(1.0f-t)*p0x + 3.0f*(1.0f-t)*(1.0f-t)*t*p1x + 3.0f*(1.0f-t)*t*t*p2x + t*t*t*p3x;
    return xcoord;
}

float bezierFunctionY(float t){
    float ycoord = (1.0f-t)*(1.0f-t)*p0y + 2*(1.0f-t)*t*p1y + t*t*p2y;
    //float ycoord = (1.0f-t)*(1.0f-t)*(1.0f-t)*p0y + 3.0f*(1.0f-t)*(1.0f-t)*t*p1y + 3.0f*(1.0f-t)*t*t*p2y + t*t*t*p3y;
    return ycoord;
}

void renderFrame(float x, float y, int actionUp){
    LOGI("Enter renderFrame()");
    shaderProgram = createProgram(glVertexShader, glFragmentShader);

    glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram);

    vPosition = glGetAttribLocation(shaderProgram, "vPosition");
    //vColor = glGetAttribLocation(shaderProgram, "vColor");

    __android_log_print(ANDROID_LOG_INFO, "tag", "x = %f y = %f", x, y);
    __android_log_print(ANDROID_LOG_INFO, "tag", "x = %f y = %f", x * 2.0f / screen_width - 1.0f, y * -1.0f / screen_height + 0.875f);

    if (actionUp == 1){
        counter = 0;
    }
    
    if (y != 0.0){ //check whether screen is touched
        touchPoints.push_back(x);
        touchPoints.push_back(y);
        int n = touchPoints.size();
        if (counter == 0){
            touchPointsAll.push_back(x);
            touchPointsAll.push_back(y);

        }else if (counter > 1){
            touchPointsAll.pop_back();
            touchPointsAll.pop_back();
            float mid_t = findMidpointT(touchPoints[n-6], touchPoints[n-5], touchPoints[n-4], touchPoints[n-3], touchPoints[n-2], touchPoints[n-1]);
            setControlPoints(touchPoints[n-6], touchPoints[n-5], touchPoints[n-4], touchPoints[n-3], touchPoints[n-2], touchPoints[n-1], mid_t);
            if (counter == 2){
                for (float t=0.0f; t<1.0f; t+=0.005f){
                    touchPointsAll.push_back(bezierFunctionX(t));
                    touchPointsAll.push_back(bezierFunctionY(t));
                }
            }else{
                for (float t=mid_t; t<1.0f; t+=0.005f){
                    touchPointsAll.push_back(bezierFunctionX(t));
                    touchPointsAll.push_back(bezierFunctionY(t));
                }
            }
            touchPointsAll.push_back(x);
            touchPointsAll.push_back(y);
        }
        counter += 1;
    }

    /* add missing points linearly
    if (y != 0.0){
        if (counter != 0) {
            if (abs(x - previousX) > abs(y - previousY)) {
                if (abs(x - previousX) > minGap) {
                    if (x > previousX) {
                        for (int xi = int(previousX) + minGap; xi < int(x); xi += minGap) {
                            touchPoints.push_back(xi);
                            touchPoints.push_back(
                                    findYCoord(x, previousX, y, previousY, float(xi)));
                        }
                    } else {
                        for (int xi = int(previousX) - minGap; xi > int(x); xi -= minGap) {
                            touchPoints.push_back(xi);
                            touchPoints.push_back(
                                    findYCoord(x, previousX, y, previousY, float(xi)));
                        }
                    }
                }
            } else {
                if (abs(y - previousY) > minGap) {
                    if (y > previousY) {
                        for (int yi = int(previousY) + minGap; yi < int(y); yi += minGap) {
                            touchPoints.push_back(
                                    findXCoord(x, previousX, y, previousY, float(yi)));
                            touchPoints.push_back(yi);
                        }
                    } else {
                        for (int yi = int(previousY) - minGap; yi > int(y); yi -= minGap) {
                            touchPoints.push_back(
                                    findXCoord(x, previousX, y, previousY, float(yi)));
                            touchPoints.push_back(yi);
                        }
                    }
                }
            }
        }else{
            counter += 1;
        }
        touchPoints.push_back(x);
        touchPoints.push_back(y);
        previousX = x;
        previousY = y;
    }*/

    //draw squares at bezier points
    for (int i = 0; i < touchPointsAll.size()/2; i++){
        drawRectangle(touchPointsAll[i*2], touchPointsAll[i*2 + 1]);
    }
}

extern "C" JNIEXPORT void JNICALL Java_com_example_touchpoint_MainActivity_resize(JNIEnv* env, jclass obj, jint width, jint height) {
    resize(width, height);
}

extern "C" JNIEXPORT void JNICALL Java_com_example_touchpoint_MainActivity_step(JNIEnv* env, jclass obj, jfloat x, jfloat y, jint action_up) {
    renderFrame(x, y, action_up);
}

extern "C" JNIEXPORT void JNICALL Java_com_example_touchpoint_MainActivity_init(JNIEnv* env, jclass obj) {
    init();
}
