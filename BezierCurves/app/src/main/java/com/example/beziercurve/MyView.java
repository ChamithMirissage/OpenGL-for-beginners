package com.example.beziercurve;

import android.app.Activity;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;

public class MyView extends GLSurfaceView {

    //private static final String LOG_TAG = GL10JNIView.class.getSimpleName();
    MyRenderer myRenderer;

    public MyView(Context context) {
        super(context);
        setEGLContextClientVersion(3);
        setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        myRenderer = new MyRenderer(context);
        setRenderer(myRenderer);
    }
}
