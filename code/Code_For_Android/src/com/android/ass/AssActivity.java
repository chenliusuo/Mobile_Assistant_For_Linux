package com.android.ass;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.util.Log;
import android.view.Window;
import android.widget.ImageView;

public class AssActivity extends Activity {

	private Intent serviceIntent;
	public ImageView imageView;
	private ConnectStateReceiver connectStateReceiver = new ConnectStateReceiver();
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_ass);
        
		//注册BroadcastReceiver监测连接状态
		IntentFilter connectStateFilter = new IntentFilter();
		connectStateFilter.addAction("android.intent.action.CONNECT_LINKED");
		connectStateFilter.addAction("android.intent.action.CONNECT_UNLINKED");
		registerReceiver(connectStateReceiver, connectStateFilter);
		
        serviceIntent = new Intent(getBaseContext(),com.android.ass.AssService.class);
		startService(serviceIntent);
		
		Log.d("mine","start service");
    }
    
    //监测应用是否更改
    public class ConnectStateReceiver extends BroadcastReceiver {

    	@Override
    	public void onReceive(Context context, Intent intent) {

    		if (intent.getAction().equals("android.intent.action.CONNECT_LINKED")) {
    	        imageView  = (ImageView) findViewById(R.id.imageView1);
    	        imageView.setImageResource(R.drawable.link);
    		}
    		if (intent.getAction().equals("android.intent.action.CONNECT_UNLINKED")) {
    			imageView  = (ImageView) findViewById(R.id.imageView1);
    	        imageView.setImageResource(R.drawable.nolink);  
    		}
    	}
    }
    
    @Override
    protected void onDestroy(){
    	super.onDestroy();
    	//停止服务
    	serviceIntent = new Intent(getBaseContext(),com.android.ass.AssService.class);
		stopService(serviceIntent);
		Log.d("mine","stop service");
		//杀死当前进程
//		android.os.Process.killProcess(android.os.Process.myPid());
    }
}