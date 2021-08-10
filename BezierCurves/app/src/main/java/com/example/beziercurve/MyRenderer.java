package com.example.beziercurve;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.opengl.GLSurfaceView.Renderer;

import java.util.Arrays;

public class MyRenderer implements Renderer {

    public MyRenderer(Context ctx) {

    }

    @Override
    public void onDrawFrame(GL10 gl) {
        MainActivity.step();
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
