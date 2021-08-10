package com.example.touchpoint;

import android.app.Activity;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.MotionEvent;
import android.view.VelocityTracker;

public class MyView extends GLSurfaceView {

    //private static final String LOG_TAG = GL10JNIView.class.getSimpleName();
    MyRenderer myRenderer;
    private static final String DEBUG_TAG = "Velocity";
    private VelocityTracker mVelocityTracker = null;
    int isActionUp = 0;

    public MyView(Context context) {
        super(context);
        setEGLContextClientVersion(3);
        setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        myRenderer = new MyRenderer(context);
        setRenderer(myRenderer);
        // Render the view only when there is a change in the drawing data
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        setPreserveEGLContextOnPause(true);
    }

    @Override
    public boolean onTouchEvent(MotionEvent e){
        int index = e.getActionIndex();
        //int action = e.getActionMasked();
        int pointerId = e.getPointerId(index);

        switch (e.getAction()){
            //switch (action){
            case MotionEvent.ACTION_DOWN: {
                //float x = e.getX();
                //float y = e.getY();
                //myRenderer.mPreviousX = x;
                //myRenderer.mPreviousY = y;
                //mActivePointerId = e.getPointerId(0);

                if(mVelocityTracker == null) {
                    /** Retrieve a new VelocityTracker object to watch the
                     velocity of a motion.*/
                    mVelocityTracker = VelocityTracker.obtain();
                }
                else {
                    // Reset the velocity tracker back to its initial state.
                    mVelocityTracker.clear();
                }
                // Add a user's movement to the tracker.
                mVelocityTracker.addMovement(e);
                break;
            }

            case MotionEvent.ACTION_MOVE: {
                //final int pointerIndex = e.findPointerIndex(mActivePointerId);
                //final float x = e.getX(pointerIndex);
                //final float y = e.getY(pointerIndex);
                //myRenderer.x = x;
                //myRenderer.y = y;
                //myRenderer.mPreviousX = x;
                //myRenderer.mPreviousY = y;

                mVelocityTracker.addMovement(e);
                float x = e.getX();
                float y = e.getY();
                myRenderer.setCoords(x, y, isActionUp);
                /** When you want to determine the velocity, call
                 computeCurrentVelocity(). Then call getXVelocity()
                 and getYVelocity() to retrieve the velocity for each pointer ID.*/
                mVelocityTracker.computeCurrentVelocity(1000);
                /** Log velocity of pixels per second
                 Best practice to use VelocityTrackerCompat where possible.*/
                Log.d(DEBUG_TAG, "X velocity: " + mVelocityTracker.getXVelocity(pointerId));
                Log.d(DEBUG_TAG, "Y velocity: " + mVelocityTracker.getYVelocity(pointerId));

                requestRender();
                isActionUp = 0;
                break;
            }

            case MotionEvent.ACTION_UP: {
                Log.d(DEBUG_TAG, "Action Up: " + "finished");
                isActionUp = 1;
                break;
                //mActivePointerId = INVALID_POINTER_ID;

            }
            /*
            case MotionEvent.ACTION_UP:

            case MotionEvent.ACTION_CANCEL:
                // Return a VelocityTracker object back to be re-used by others.
                mVelocityTracker.recycle();
                break;
             */
        }
        return true;
    }
}
