package org.astromedicomp.win.persp_blue_window;

import android.app.Activity;
import android.os.Bundle;
import android.view.Window;
import android.view.WindowManager;
import android.content.pm.ActivityInfo;
import android.widget.TextView;
import android.graphics.Color;
import android.view.Gravity;
import org.astromedicomp.win.winvm_gles.GLESView;

public class MainActivity extends Activity {
	private GLESView glesView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //setContentView(R.layout.activity_main);
		
		//this is to get rid of action bar
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		
		//this is done to make full screen
		this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,WindowManager.LayoutParams.FLAG_FULLSCREEN);
		
		//for landscape orientation
		MainActivity.this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		
		glesView = new GLESView(this);
		
		setContentView(glesView);
    }
	
	@Override
	protected void onPause()
	{
		super.onPause();
	}
	
	@Override
	protected void onResume()
	{
		super.onResume();
	}
}
