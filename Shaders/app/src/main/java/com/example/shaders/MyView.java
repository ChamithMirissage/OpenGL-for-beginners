package com.example.shaders;

import android.content.Context;
import android.opengl.GLSurfaceView;

public class MyView extends GLSurfaceView {

    MyRenderer myRenderer;

    public MyView(Context context) {
        super(context);
        setEGLContextClientVersion(3);
        setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        myRenderer = new MyRenderer(context);
        setRenderer(myRenderer);
    }
}