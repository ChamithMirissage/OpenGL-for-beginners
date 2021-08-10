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

static const char glFragmentShaderNew[] =
        "precision mediump float;\n"
        //"varying vec4 vertexColor;\n"
        "void main()\n"
        "{\n"
        //"  gl_FragColor = vertexColor;\n"
        "  gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
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
GLuint shaderProgram1;
GLuint vPosition;
GLuint vColor;

vector<float> points;
GLfloat rectangleVertices[12];

float vertices[5][2] = {{0.0f, 0.2f},
                        {-0.2f, 0.2f},
                        {-0.4f, 0.2f},
                        {-0.6f, 0.2f},
                        {-0.8f, 0.2f}};
// control points
float p0x;
float p0y;
float p1x;
float p1y;
float p2x;
float p2y;
//float p3x;
//float p3y;
float ctrlPoints[4][2] = {{p0x, p0y},
                          {p1x, p1y},
                          {p2x, p2y}};
float dx = 0.005f;
float dy = 0.01f;

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

float findMidpointT(float x0, float y0, float x1, float y1, float x2, float y2){
    float ac = sqrt(pow(x0-x2, 2) + pow(y0-y2, 2));
    float ab = sqrt(pow(x0-x1, 2) + pow(y0-y1, 2));
    float theta_ac = atan2(abs(y0-y2), abs(x0-x2));
    float theta_ab = atan2(abs(y0-y1), abs(x0-x1));
    float ab_costheta = ab * cos(abs(theta_ab-theta_ac));
    float t = ab_costheta/ac;
    return t;
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

void setControlPoints(float x0, float y0, float x1, float y1, float x2, float y2, float t){
    p0x = x0;
    p0y = y0;
    p2x = x2;
    p2y = y2;
    p1x = (x1 - (1.0f-t) * (1.0f-t) * p0x - t * t * p2x) / (2.0f * t * (1.0f-t));
    p1y = (y1 - (1.0f-t) * (1.0f-t) * p0y - t * t * p2y) / (2.0f * t * (1.0f-t));

    // solve the linear system for cubic curve
    //ax+by=e
    //cx+dy=f
    /*
    float a = 3*(1.0f-t1)*(1.0f-t1)*t1;
    float b = 3*(1.0f-t1)*t1*t1;
    float c = 3*(1.0f-t2)*(1.0f-t2)*t2;
    float d = 3*(1.0f-t2)*t2*t2;
    float e_x = x1 - (1.0f-t1)*(1.0f-t1)*(1.0f-t1)*p0x - t1*t1*t1*p3x;
    float f_x = x2 - (1.0f-t2)*(1.0f-t2)*(1.0f-t2)*p0x - t2*t2*t2*p3x;
    float e_y = y1 - (1.0f-t1)*(1.0f-t1)*(1.0f-t1)*p0y - t1*t1*t1*p3y;
    float f_y = y2 - (1.0f-t2)*(1.0f-t2)*(1.0f-t2)*p0y - t2*t2*t2*p3y;
    float determinant = a*d - b*c;
    p1x = (d * e_x - b * f_x) / determinant;
    p2x = (a * f_x - c * e_x) / determinant;
    p1y = (d * e_y - b * f_y) / determinant;
    p2y = (a * f_y - c * e_y) / determinant;
    */
}

void drawRectangle(float xCoord, float yCoord){
    setRectangle(xCoord, yCoord);
    glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, rectangleVertices);
    //glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, colours);
    glEnableVertexAttribArray(vPosition);
    //glEnableVertexAttribArray(vColor);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
}

void renderFrame(){
    for (int j=0; j<3; j++){
        float mid_t = findMidpointT(vertices[j][0], vertices[j][1], vertices[j+1][0], vertices[j+1][1], vertices[j+2][0], vertices[j+2][1]);
        //float mid2_t = findMidpointT(vertices[j][0], vertices[j][1], vertices[j+2][0], vertices[j+2][1], vertices[j+3][0], vertices[j+3][1]);
        setControlPoints(vertices[j][0], vertices[j][1], vertices[j+1][0], vertices[j+1][1], vertices[j+2][0], vertices[j+2][1], mid_t);

        if (j == 0){
            for (float t=0.0f; t<1.0f; t+=0.005f){
                points.push_back(bezierFunctionX(t));
                points.push_back(bezierFunctionY(t));
            }
        }else{
            for (float t=mid_t; t<1.0f; t+=0.005f){
                points.push_back(bezierFunctionX(t));
                points.push_back(bezierFunctionY(t));
            }
        }
        points.push_back(vertices[j+2][0]);
        points.push_back(vertices[j+2][1]);

        shaderProgram = createProgram(glVertexShader, glFragmentShader);

        glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);

        vPosition = glGetAttribLocation(shaderProgram, "vPosition");
        //vColor = glGetAttribLocation(shaderProgram, "vColor");

        for (int i = 0; i < points.size()/2; i++){
            //__android_log_print(ANDROID_LOG_INFO, "tag", "count = %d i = %d x = %f y = %f", counter, i, touchPoints[i*2], touchPoints[i*2 + 1]);
            drawRectangle(points[i*2], points[i*2 + 1]);
        }
    }

    shaderProgram1 = createProgram(glVertexShader, glFragmentShaderNew);

    //glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram1);

    vPosition = glGetAttribLocation(shaderProgram1, "vPosition");
    //vColor = glGetAttribLocation(shaderProgram1, "vColor");

    glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    //glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, colours);

    glEnableVertexAttribArray(vPosition);
    //glEnableVertexAttribArray(vColor);

    //glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    //glDrawElements(GL_TRIANGLE, 12, GL_UNSIGNED_SHORT, indices);
    //glDrawElements(GL_TRIANGLES, indices.length, GL_UNSIGNED_BYTE, indexBuffer);
    glDrawArrays(GL_LINE_STRIP, 0, 5);
    //glFlush();

}

extern "C" JNIEXPORT void JNICALL Java_com_example_beziercurve_MainActivity_resize(JNIEnv* env, jclass obj, jint width, jint height) {
    resize(width, height);
}

extern "C" JNIEXPORT void JNICALL Java_com_example_beziercurve_MainActivity_step(JNIEnv* env, jclass obj) {
    renderFrame();
}

extern "C" JNIEXPORT void JNICALL Java_com_example_beziercurve_MainActivity_init(JNIEnv* env, jclass obj) {
    init();
}
