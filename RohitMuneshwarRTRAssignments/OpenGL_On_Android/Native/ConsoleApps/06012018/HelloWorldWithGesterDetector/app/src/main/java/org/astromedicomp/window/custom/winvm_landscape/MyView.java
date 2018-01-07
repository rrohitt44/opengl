package org.astromedicomp.window.custom.winvm_landscape;

import android.widget.TextView;
import android.content.Context;
import android.graphics.Color;
import android.view.Gravity;

import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;

public class MyView extends TextView implements OnGestureListener, OnDoubleTapListener
{
	private GestureDetector gestureDetector;
	
	public MyView(Context context)
	{
		super(context);
		setText("hi There...");
		setTextSize(60);
		setTextColor(Color.GREEN);
		setGravity(Gravity.CENTER);
		
		gestureDetector = new GestureDetector(context,this, null, false);
		gestureDetector.setOnDoubleTapListener(this);
	}
	
	@Override
	public boolean onTouchEvent(MotionEvent event)
	{
		int eventAction = event.getAction();
		if(!gestureDetector.onTouchEvent(event))
		{
			super.onTouchEvent(event);
		}
		return (true);
	}
	
	@Override
	public boolean onDoubleTap(MotionEvent e)
	{
		setText("Double Tap");
		return (true);
	}
	
	@Override
	public boolean onDoubleTapEvent(MotionEvent e)
	{
		return (true);
	}
	
	@Override
	public boolean onSingleTapConfirmed(MotionEvent e)
	{
		setText("Single Tap");
		return (true);
	}
	
	@Override
	public boolean onDown(MotionEvent e)
	{
		return (true);
	}
	
	@Override
	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY)
	{
		return (true);
	}
	
	@Override
	public void onLongPress(MotionEvent e)
	{
		setText("Long Press");
	}
	
	@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY)
	{
		setText("Scroll");
		return (true);
	}
	
	@Override
	public void onShowPress(MotionEvent e)
	{
		
	}
	
	@Override
	public boolean onSingleTapUp(MotionEvent e)
	{
		return (true);
	}
}