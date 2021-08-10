package com.example.touchpoint;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.opengl.GLSurfaceView.Renderer;

import java.util.Arrays;

public class MyRenderer implements Renderer {

    public volatile float x, y;
    public volatile int actionUp;

    public MyRenderer(Context ctx) {

    }

    public void setCoords(float xPos, float yPos, int action_up) {
        x = xPos;
        y = yPos;
        actionUp = action_up;
        //arr[counter] = x;
        //counter += 1;
        //arr[counter] = y;
        //counter += 1;
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        MainActivity.step(x, y, actionUp);
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        MainActivity.resize(width, height);
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        MainActivity.init();
    }
}
