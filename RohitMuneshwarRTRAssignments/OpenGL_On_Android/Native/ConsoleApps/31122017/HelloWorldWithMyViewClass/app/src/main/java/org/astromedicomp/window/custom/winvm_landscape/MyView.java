package org.astromedicomp.window.custom.winvm_landscape;

import android.widget.TextView;
import android.content.Context;
import android.graphics.Color;
import android.view.Gravity;

public class MyView extends TextView
{
	public MyView(Context context)
	{
		super(context);
		setText("hi There...");
		setTextSize(60);
		setTextColor(Color.GREEN);
		setGravity(Gravity.CENTER);
	}
}