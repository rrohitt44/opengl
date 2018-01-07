package org.astromedicomp.window.custom.utv;

import android.app.Activity;
import android.os.Bundle;
import android.view.Window;
import android.view.WindowManager;
import android.content.pm.ActivityInfo;
import android.widget.TextView;
import android.graphics.Color;
import android.view.Gravity;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //setContentView(R.layout.activity_main);
		
		//this is done to get rid of ActionBar
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		//this is done to make fullscreen
		this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,WindowManager.LayoutParams.FLAG_FULLSCREEN);
		
		//force activity window orientation to landscape
		MainActivity.this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		
		TextView myTextView = new TextView(this);
		myTextView.setText("Hello Galaxy");
		myTextView.setTextSize(60);
		myTextView.setTextColor(Color.GREEN);
		myTextView.setGravity(Gravity.CENTER);
		
		setContentView(myTextView);
    }
}
