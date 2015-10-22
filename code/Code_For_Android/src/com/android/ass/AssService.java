package com.android.ass;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;
import java.util.Locale;

import android.app.PendingIntent;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.ContentUris;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ApplicationInfo;
import android.content.pm.IPackageStatsObserver;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageStats;
import android.database.ContentObserver;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteDatabase.CursorFactory;
import android.database.sqlite.SQLiteOpenHelper;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.media.ThumbnailUtils;
import android.net.Uri;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.RemoteException;
import android.provider.ContactsContract;
import android.provider.ContactsContract.CommonDataKinds.Phone;
import android.provider.ContactsContract.CommonDataKinds.StructuredName;
import android.provider.ContactsContract.Data;
import android.provider.ContactsContract.RawContacts;
import android.provider.MediaStore;
import android.telephony.SmsManager;
import android.util.Log;


public class AssService extends Service {
	
	/**********************************************界面切换标志量*******************************************/
	private boolean connectState = false;									//false未连接，true已连接
	/********************Handler绑定对应HandlerThread,post对应Runnable*************************************/
	private Handler mHandler;
	private HandlerThread mThread;
	private Handler contactHandler;
	private HandlerThread contactThread;
	private Handler messageHandler;
	private HandlerThread messageThread;
	private Handler appHandler;
	private HandlerThread appThread;
	private Handler imageHandler;
	private HandlerThread imageThread;
	private Handler audioHandler;
	private HandlerThread audioThread;
	private Handler videoHandler;
	private HandlerThread videoThread;
	/*******************************更新标志量,true则代表需要创建*****************************************/
	public boolean contactSign = true;									//联系人
	public boolean messageSign = true;									//短信
	public boolean appSign = true;										//应用
	public boolean imageSign = true;										//图片
	public boolean audioSign = true;										//音频
	public boolean videoSign = true;										//视频
	/******************************准备标志量,true则代表已准备好*******************************************/
	public boolean contactReady = false;									//联系人
	public boolean messageReady = false;									//短信
	public boolean appReady = false;										//应用
	public boolean imageReady = false;									//图片
	public boolean audioReady = false;									//音频
	public boolean videoReady = false;									//视频
	/****************************用于判断数据是否发生更改(ContentObserver/BroadcastReceiver)****************/
	private ContactObserver contactObserver = new ContactObserver(null);
	private MessageObserver messageObserver = new MessageObserver(null);
	private AppReceiver appReceiver = new AppReceiver();
	private ImageObserver imageObserver = new ImageObserver(null);
	private AudioObserver audioObserver = new AudioObserver(null);
	private VideoObserver videoObserver = new VideoObserver(null);
	
	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}

	public void onCreate(){
		super.onCreate();

		//初始化创建C端服务器线程，及发送线程
		pthread_init();
		/*******************************创建HandlerThread,绑定Handler****************************/
		//接收命令
		mThread = new HandlerThread("mThread");
		mThread.start();
		mHandler = new Handler(mThread.getLooper());
		//contact更新
		contactThread = new HandlerThread("contactThread");
		contactThread.start();
		contactHandler = new Handler(contactThread.getLooper());
		//message更新
		messageThread = new HandlerThread("messageThread");
		messageThread.start();
		messageHandler = new Handler(messageThread.getLooper());
		//app更新
		appThread = new HandlerThread("appThread");
		appThread.start();
		appHandler = new Handler(appThread.getLooper());
		//image更新
		imageThread = new HandlerThread("imageThread");
		imageThread.start();
		imageHandler = new Handler(imageThread.getLooper());
		//audio更新
		audioThread = new HandlerThread("audioThread");
		audioThread.start();
		audioHandler = new Handler(audioThread.getLooper());
		//video更新
		videoThread = new HandlerThread("videoThread");
		videoThread.start();
		videoHandler = new Handler(videoThread.getLooper());
		/******************************启动时开始更新**************************************/
		mHandler.post(mRunnable);
		contactHandler.post(contactRunnable);
		messageHandler.post(messageRunnable);
		appHandler.post(appRunnable);
		imageHandler.post(imageRunnable);
		audioHandler.post(audioRunnable);
		videoHandler.post(videoRunnable);
		
		//注册ContentObserver
		//监测通讯录的uri
		getContentResolver().registerContentObserver(ContactsContract.Contacts.CONTENT_URI, true,contactObserver);
		//监测短信的uri
		getContentResolver().registerContentObserver(Uri.parse("content://sms/"), true,messageObserver);
		//监测图片的uri
		getContentResolver().registerContentObserver(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, false,imageObserver);
		//监测音频的uri
		getContentResolver().registerContentObserver(MediaStore.Audio.Media.EXTERNAL_CONTENT_URI, false,audioObserver);
		//监测视频的uri
		getContentResolver().registerContentObserver(MediaStore.Video.Media.EXTERNAL_CONTENT_URI, false,videoObserver);
		
		//注册BroadcastReceiver监测应用安装卸载
		IntentFilter appFilter = new IntentFilter();
		appFilter.addAction("android.intent.action.PACKAGE_ADDED");
		appFilter.addAction("android.intent.action.PACKAGE_REMOVED");
		appFilter.addDataScheme("package");
		registerReceiver(appReceiver, appFilter);
	}
	
	public void onDestroy(){

		super.onDestroy();
		
		//结束C进程
		set_end_flag(1);
		//注销ContentObserver
		getContentResolver().unregisterContentObserver(contactObserver);
		getContentResolver().unregisterContentObserver(messageObserver);
		getContentResolver().unregisterContentObserver(imageObserver);
		getContentResolver().unregisterContentObserver(audioObserver);
		getContentResolver().unregisterContentObserver(videoObserver);
		//注销BroadcastReceiver
		unregisterReceiver(appReceiver);
		
		//从消息队列中移除mRunnable,并结束线程
		mHandler.removeCallbacks(mRunnable);
		//从消息队列中移除contactRunnable,并结束线程
		contactHandler.removeCallbacks(contactRunnable);
		contactThread.interrupt();
		//从消息队列中移除messageRunnable,并结束线程
		messageHandler.removeCallbacks(messageRunnable);
		messageThread.interrupt();
		//从消息队列中移除appRunnable,并结束线程
		appHandler.removeCallbacks(appRunnable);
		appThread.interrupt();
		//从消息队列中移除imageRunnable,并结束线程
		imageHandler.removeCallbacks(imageRunnable);
		imageThread.interrupt();
		//从消息队列中移除audioRunnable,并结束线程
		audioHandler.removeCallbacks(audioRunnable);
		audioThread.interrupt();
		//从消息队列中移除videoRunnable,并结束线程
		videoHandler.removeCallbacks(videoRunnable);
		videoThread.interrupt();
		
		//退出JVM
		System.exit(0);
	}
	
	//监听C端变量recv_cmd_flag,若接收到命令则处理
	private Runnable mRunnable = new Runnable() {
		
		@Override
		public void run() {
			switch(get_cmd_flag())
			{
				case 0x01:									//获取contact
					//若界面显示为未连接界面，则发送广播切换
					if(connectState == false){
						Intent intent=new Intent();
						intent.setAction("android.intent.action.CONNECT_LINKED");//action与接收器相同
						sendBroadcast(intent);
						//设置连接状态为已连接
						connectState = true;
					}
					//判断联系人信息是否已创建
					while(contactReady == false){
						try {
							HandlerThread.sleep(500);
						} catch (InterruptedException e) {
							e.printStackTrace();
						}
					}
					send_cmd_flag(0x01);
					break;
				case 0x02:									//插入联系人
					//若界面显示为未连接界面，则发送广播切换
					if(connectState == false){
						Intent intent=new Intent();
						intent.setAction("android.intent.action.CONNECT_LINKED");//action与接收器相同
						sendBroadcast(intent);
						//设置连接状态为已连接
						connectState = true;
					}
					//执行插入操作
					addContacts(get_contact_cmd(1),get_contact_cmd(2));
					send_cmd_flag(0x02);
					break;
				case 0x03:									//删除联系人
					//若界面显示为未连接界面，则发送广播切换
					if(connectState == false){
						Intent intent=new Intent();
						intent.setAction("android.intent.action.CONNECT_LINKED");//action与接收器相同
						sendBroadcast(intent);
						//设置连接状态为已连接
						connectState = true;
					}
					//执行删除操作
					deleteContacts(get_contact_cmd(0),get_contact_cmd(1),get_contact_cmd(2));
					send_cmd_flag(0x03);
					break;
				case 0x04:									//修改联系人
					//若界面显示为未连接界面，则发送广播切换
					if(connectState == false){
						Intent intent=new Intent();
						intent.setAction("android.intent.action.CONNECT_LINKED");//action与接收器相同
						sendBroadcast(intent);
						//设置连接状态为已连接
						connectState = true;
					}
					//执行更新操作
					updateContacts(get_contact_cmd(0),get_contact_cmd(1),get_contact_cmd(2),get_contact_cmd(3),get_contact_cmd(4));
					send_cmd_flag(0x04);
					break;
				case 0x05:									//获取Message
					//若界面显示为未连接界面，则发送广播切换
					if(connectState == false){
						Intent intent=new Intent();
						intent.setAction("android.intent.action.CONNECT_LINKED");//action与接收器相同
						sendBroadcast(intent);
						//设置连接状态为已连接
						connectState = true;
					}
					//判断信息数据是否已生成
					while(messageReady == false){
						try {
							HandlerThread.sleep(500);
						} catch (InterruptedException e) {
							e.printStackTrace();
						}
					}
					send_cmd_flag(0x05);
					break;
				case 0x07:									//发送短信
					//若界面显示为未连接界面，则发送广播切换
					if(connectState == false){
						Intent intent=new Intent();
						intent.setAction("android.intent.action.CONNECT_LINKED");//action与接收器相同
						sendBroadcast(intent);
						//设置连接状态为已连接
						connectState = true;
					}
					//执行发送操作
					sendMessage(get_message_cmd(0), get_message_cmd(1));
					send_cmd_flag(0x07);
					break;
				case 0x08:									//获取app信息
					//若界面显示为未连接界面，则发送广播切换
					if(connectState == false){
						Intent intent=new Intent();
						intent.setAction("android.intent.action.CONNECT_LINKED");//action与接收器相同
						sendBroadcast(intent);
						//设置连接状态为已连接
						connectState = true;
					}
					//判断应用信息是否已创建
					while(appReady == false){
						try {
							HandlerThread.sleep(500);
						} catch (InterruptedException e) {
							e.printStackTrace();
						}
					}
					send_cmd_flag(0x08);
					break;
				case 0x09:									//获取图片信息
					//若界面显示为未连接界面，则发送广播切换
					if(connectState == false){
						Intent intent=new Intent();
						intent.setAction("android.intent.action.CONNECT_LINKED");//action与接收器相同
						sendBroadcast(intent);
						//设置连接状态为已连接
						connectState = true;
					}
					//判断图片信息是否已创建
					while(imageReady == false){
						try {
							HandlerThread.sleep(500);
						} catch (InterruptedException e) {
							e.printStackTrace();
						}
					}
					send_cmd_flag(0x09);
					break;
				case 0x0a:									//获取音频信息
					//若界面显示为未连接界面，则发送广播切换
					if(connectState == false){
						Intent intent=new Intent();
						intent.setAction("android.intent.action.CONNECT_LINKED");//action与接收器相同
						sendBroadcast(intent);
						//设置连接状态为已连接
						connectState = true;
					}
					//判断音频信息是否已创建
					while(audioReady == false){
						try {
							HandlerThread.sleep(500);
						} catch (InterruptedException e) {
							e.printStackTrace();
						}
					}
					send_cmd_flag(0x0a);
					break;
				case 0x0b:									//获取视频信息
					//若界面显示为未连接界面，则发送广播切换
					if(connectState == false){
						Intent intent=new Intent();
						intent.setAction("android.intent.action.CONNECT_LINKED");//action与接收器相同
						sendBroadcast(intent);
						//设置连接状态为已连接
						connectState = true;
					}
					//判断视频信息是否已创建
					while(videoReady == false){
						try {
							HandlerThread.sleep(500);
						} catch (InterruptedException e) {
							e.printStackTrace();
						}
					}
					send_cmd_flag(0x0b);
					break;
				case 0x11:									//查询是否新收到短信
					//返回结果
					send_cmd_flag(0x11);
					//若界面显示为未连接界面，则发送广播切换
					if(connectState == false){
						Intent intent=new Intent();
						intent.setAction("android.intent.action.CONNECT_LINKED");//action与接收器相同
						sendBroadcast(intent);
						//设置连接状态为已连接
						connectState = true;
					}
					break;
				case 0x12:									//连接超时
					//结束C进程
					set_end_flag(0);
					//若界面显示为已连接界面，则发送广播切换
					if(connectState == true){
						Intent intent=new Intent();
						intent.setAction("android.intent.action.CONNECT_UNLINKED");
						sendBroadcast(intent);
						//设置连接状态为未连接
						connectState = false;
					}
					try {
						HandlerThread.sleep(1000);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
					//重新启动server端
					pthread_init();
					break;
				default:
					break;
			}
			//每隔1000ms重新post
			mHandler.postDelayed(mRunnable, 1000);
		}
	};

    /****************************************************用于获取各类信息********************************************************/
	
	//后台更新联系人数据contact.db3
	private Runnable contactRunnable = new Runnable() {
		
		@Override
		public void run() {

			//数据库创建标志设为false
			contactSign = false;
			contactReady = false;

			//创建联系人数据库			
			MyDatabaseHelper contactDBHelper = new MyDatabaseHelper(getBaseContext(),"contacts.db3");
		    SQLiteDatabase contactDataBase = contactDBHelper.getReadableDatabase();
		    //若已存在则清空表
		    contactDataBase.execSQL("drop table if exists contactDB");
			contactDataBase.execSQL("create table contactDB(personId varchar(4),personName varchar(50),phoneNumber varchar(50))");

			//使用ContentResolver查找联系人数据
			Cursor cursor = getContentResolver().query(ContactsContract.Contacts.CONTENT_URI, null, null, null, null);
			//遍历联系人,若发生更改则停止
			while(cursor.moveToNext() && contactSign == false){
				//获取联系人ID
				String contactId = cursor.getString(cursor.getColumnIndex(ContactsContract.Contacts._ID));
				//获取联系人名字
		        String contactName = cursor.getString(cursor.getColumnIndex(ContactsContract.Contacts.DISPLAY_NAME));
		        //使用ContentResolver查找联系人的电话号码
		        Cursor phone = getContentResolver().query(ContactsContract.CommonDataKinds.Phone.CONTENT_URI, null,ContactsContract.CommonDataKinds.Phone.CONTACT_ID + " = " + contactId, null, null);
		        //遍历联系人手机号
		        while(phone.moveToNext()) {
		        	String contactNumber = phone.getString(phone.getColumnIndex(ContactsContract.CommonDataKinds.Phone.NUMBER));
		        	//将联系人插入数据库
		        	String insert = "insert into contactDB(personId,personName,phoneNumber) values(?,?,?)";
		        	contactDataBase.execSQL(insert,new String[]{contactId,contactName,contactNumber});
		        }
		        phone.close();
			}
			cursor.close();
			//关闭数据库及数据库
			if(contactDBHelper != null){
				contactDBHelper.close();
			}
			if(contactDataBase != null && contactDataBase.isOpen()){
				contactDataBase.close();
			}
			//判断遍历是否因为更改中断
			if(contactSign == true){
				//若发生更改则结束
				Log.d("contact","need recreate");
			}
			else{
				contactReady = true;
				//若未发生更改，将联系人更新标识设为已更新
				Log.d("contact", "contact create");
			}
		}
	};
    
    //添加联系人(姓名，手机号）
    private void addContacts(String name,String number){
		if(name == null || number == null){
			return ;
		}
    	//创建一个空的contentValues
		ContentValues values = new ContentValues();
		//向RawContacts.Content_URI执行一个空值插入,获取系统返回的ContactId
		Uri rawContactUri = getContentResolver().insert(RawContacts.CONTENT_URI, values);
		long contact_id = ContentUris.parseId(rawContactUri);
		
		values.clear();
		//设置联系人_id
		values.put(Data.RAW_CONTACT_ID, contact_id);
		//设置内容类型
		values.put(Data.MIMETYPE, StructuredName.CONTENT_ITEM_TYPE);
		//设置联系人姓名
		values.put(StructuredName.GIVEN_NAME, name);
		//向联系人Uri中添加联系人姓名
		getContentResolver().insert(ContactsContract.Data.CONTENT_URI, values);
		
		values.clear();
		values.put(Data.RAW_CONTACT_ID, contact_id);
		values.put(Data.MIMETYPE, Phone.CONTENT_ITEM_TYPE);
		//设置联系人手机号
		values.put(Phone.NUMBER, number);
		//设置电话类型
		values.put(Phone.TYPE, Phone.TYPE_MOBILE);
		//向联系人电话号码Uri中添加电话号码
		getContentResolver().insert(ContactsContract.Data.CONTENT_URI, values);
    	Log.d("contact", "insert "+name+" "+number);
	}
    
    //删除联系人(id,手机号,删除对应手机号,若手机号为空则删除联系人)
    private void deleteContacts(String id,String name,String number){
    	if(id == null && name == null){
    		return;
    	}
    	//若名称不为空，默认获取id
    	else if(name != null){
    		Cursor cursor = getContentResolver().query(RawContacts.CONTENT_URI,new String[]{ContactsContract.Contacts._ID},ContactsContract.Contacts.DISPLAY_NAME + "='" + name + "'", null, null);
    		if(cursor.moveToNext()) {
    			id = cursor.getString(cursor.getColumnIndex(RawContacts._ID));
    		}
    	}
    	if(id == null){
    		return;
    	}
    	if(number == null){
    		//根据id删除raw_contact中的相应数据
    		getContentResolver().delete(RawContacts.CONTENT_URI,"contact_id=?", new String[]{id});
    		//根据id删除data中的相应数据
    		getContentResolver().delete(ContactsContract.Data.CONTENT_URI,"raw_contact_id=?", new String[]{id});
    	}
    	else{
    		//根据id删除data中的相应数据
    		getContentResolver().delete(ContactsContract.Data.CONTENT_URI,"raw_contact_id=? and data1=?", new String[]{id,number});
    	}
    	Log.d("contact", "delete "+id+" "+number);
	}
    
	//修改联系人(id,姓名,手机号,修改后姓名,修改后手机号)
    private void updateContacts(String id,String name,String number,String newName,String newNumber){
		ContentValues values = new ContentValues();
    	if(id == null || name == null || number == null || newName == null || newNumber == null){
    		return ;
    	}
    	//修改联系人的姓名
    	if( name.equals(newName) == false){
    		// 更改raw_contacts表中的姓名
    		values.clear();
    		values.put(RawContacts.DISPLAY_NAME_PRIMARY, newName);
    		values.put(RawContacts.DISPLAY_NAME_ALTERNATIVE, newName);
    		values.put(RawContacts.SORT_KEY_PRIMARY, newName);
    		values.put(RawContacts.SORT_KEY_ALTERNATIVE, newName);
    		//根据id修改raw_contact中的相应数据
    		getContentResolver().update(RawContacts.CONTENT_URI,values, "contact_id=?", new String[]{id});
    		
    		// 更改data表中的姓名
    		values.clear();
    		values.put(StructuredName.GIVEN_NAME, newName);
    		values.put(StructuredName.DISPLAY_NAME, newName);
    		//根据id修改data中的相应数据
    		getContentResolver().update(ContactsContract.Data.CONTENT_URI,values, "raw_contact_id=? and mimetype=?", new String[]{id,StructuredName.CONTENT_ITEM_TYPE});
    		Log.d("contact", "update "+name+" "+newName);
    	}
    	//修改联系人手机号
    	if( number.equals(newNumber) == false){
    		// 更改data表中的手机号
    		values.clear();
    		values.put(Phone.NUMBER, newNumber);  
    		//根据id修改data中的相应数据
    		getContentResolver().update(ContactsContract.Data.CONTENT_URI,values, "raw_contact_id=? and mimetype=? and data1=?", new String[]{id,Phone.CONTENT_ITEM_TYPE,number});

    		Log.d("contact", "update "+number+" "+newNumber);
    	}
    }

    //后台更新短信数据message_pack
  	private Runnable messageRunnable = new Runnable() {
  		
  		@Override
  		public void run() {
  			
  			//更新标识设为false
  			messageSign = false;
  			messageReady = false;

  			int ret = 0;
  			int count = 1;
  			String[] projection = new String[] {"address","body", "date", "type","read" };
  			Uri uri = Uri.parse("content://sms/");
  			Cursor cursor = getContentResolver().query(uri, projection, null, null, "date desc");
  			
  			if (cursor.moveToFirst()) {
  				do {
  					String message_num = cursor.getString(cursor.getColumnIndex("address"));
  					String message_name = getMessageName(message_num);
  					String message_body = cursor.getString(cursor.getColumnIndex("body"));
  					//规范日期格式
  					SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss",Locale.getDefault());
  					Date date = new Date(Long.parseLong(cursor.getString(cursor.getColumnIndex("date"))));
  					String message_date = dateFormat.format(date);
  					
  					int message_type = cursor.getInt(cursor.getColumnIndex("type"));	//1接收，2发送
  					int message_read = cursor.getInt(cursor.getColumnIndex("read"));	//0未读，1已读

  					//将获取的短信存储到message_pack.message[count]
  					ret = message_pack_create(message_name, message_num, message_body, message_date, message_type, message_read, count++);
  					if(ret == 1){
  						break;
  					}
  					//Log.d("message",message_name+ message_num+ message_body+ message_date+ message_type+ message_read);
  				} while (cursor.moveToNext() && messageSign == false);
  			}
  			
  			cursor.close();

  			//判断遍历是否因为更改中断
  			if(messageSign == true){
  				//若发生更改，则将message—_sign置为待更新
  				Log.d("message","need recreate");
  			}
  			else{
  				//设置更新标识为已更新
  				messageReady = true;  			
  	  			Log.d("message","message_pack create");
  			}
  		}
  	};
	
	//通过通讯录获取联系人姓名
	private String getMessageName(String address){
		if(address == null || address == ""){
            return null;
        }

        String message_name = "null";
        String[] projection = new String[] {Phone.DISPLAY_NAME, Phone.NUMBER};

        Uri uri_Person = Uri.withAppendedPath(ContactsContract.CommonDataKinds.Phone.CONTENT_FILTER_URI, address);
        Cursor cursor = getContentResolver().query(uri_Person, projection, null, null, null);

        if(cursor.moveToFirst()){
            message_name = cursor.getString(cursor.getColumnIndex(Phone.DISPLAY_NAME));
        }
        else{
        	message_name = address;
        }
        cursor.close();
        
		return message_name;
	}
	
	//发送短信
	private void sendMessage(String number,String body){
		Log.d("message",number+" "+body);
		PendingIntent sentIntent = PendingIntent.getActivity(getBaseContext(), 0, new Intent(), 0);
		SmsManager messageManager = SmsManager.getDefault();
		messageManager.sendTextMessage(number, null, body, sentIntent, null);
	}

	//后台更新应用数据appinfo.db3,appicon/
  	private Runnable appRunnable = new Runnable() {
  		
  		@Override
  		public void run() {

  			//更新标识设为false
  			appSign = false;
  			appReady = false;
  			//创建或打开数据库appinfo.db3
  	    	MyDatabaseHelper appInfoDBHelper = new MyDatabaseHelper(getBaseContext(),"appinfo.db3");
  		    SQLiteDatabase appInfoDataBase = appInfoDBHelper.getReadableDatabase(); 
  	    	PackageSizeObserver pso = new PackageSizeObserver();
  	    	//用于插入操作
  	    	String insertSystem = "insert into systemDB(appId,appSize,appName,packageName,appPath) values(?,?,?,?,?)";
  	    	String insertThird = "insert into thirdDB(appId,appSize,appName,packageName,appPath) values(?,?,?,?,?)";
  	    	//若表已存在则删除
  			appInfoDataBase.execSQL("drop table if exists systemDB");
  			appInfoDataBase.execSQL("drop table if exists thirdDB");
  			//若应用图标文件夹已存在则删除
  			File dir = new File(getFilesDir().getPath()+ "/appicon/");
  			if(dir.isDirectory() == true){
  				File[] files = dir.listFiles();
  				for (File f:files){
  					f.delete();
  				}
  			}
  			//创建两张表(系统应用表systemDB，第三方应用表thirdDB)
  			appInfoDataBase.execSQL("create table systemDB(appId varchar(3),appSize varchar(8),appName varchar(20),packageName varchar(50),appPath varchar(80))");	//存放系统应用
  			appInfoDataBase.execSQL("create table thirdDB(appId varchar(3),appSize varchar(8),appName varchar(20),packageName varchar(50),appPath varchar(80))");	//存放第三方应用

  			List<PackageInfo> packages = getPackageManager().getInstalledPackages(0);

  			for(int i=0;i<packages.size() && appSign == false;i++) {
  				PackageInfo packageInfo = packages.get(i); 
  				
  				String appid = String.valueOf(i);
  				String appName = packageInfo.applicationInfo.loadLabel(getPackageManager()).toString(); 
  				String packageName = packageInfo.packageName; 
  				String appPath = packageInfo.applicationInfo.sourceDir;
  				String appSize = null;
  				
  	    		//使用放射机制得到PackageManager类的隐藏函数getPackageSizeInfo
  	    		PackageManager pm = getPackageManager();  //得到pm对象
  				//通过反射机制获得该隐藏函数
  				Method getPackageSizeInfo;

  			    //调用该函数，并且给其分配参数 ，待调用流程完成后会回调PkgSizeObserver类的函数
  			    try {
  			    	getPackageSizeInfo = pm.getClass().getMethod("getPackageSizeInfo", String.class,IPackageStatsObserver.class);
  					getPackageSizeInfo.invoke(pm, packageName,pso);
  					//获取应用程序大小
  					appSize = pso.getPackageSize();	
  				} catch (IllegalAccessException e1) {
  					e1.printStackTrace();
  				} catch (IllegalArgumentException e1) {
  					e1.printStackTrace();
  				} catch (InvocationTargetException e1) {
  					e1.printStackTrace();
  				} catch (NoSuchMethodException e) {
  					e.printStackTrace();
  				}
  			    		
  				//判断是否为系统应用插入不同的表，只收录第三方应用图标
  				if((packageInfo.applicationInfo.flags & ApplicationInfo.FLAG_SYSTEM) > 0){
  			    	appInfoDataBase.execSQL(insertSystem,new String[]{appid,appSize,appName,packageName,appPath});
  				}
  				else{
  					Drawable appIcon = packageInfo.applicationInfo.loadIcon(getPackageManager());
  					try {
  						saveBitmap(appid, ((BitmapDrawable)appIcon).getBitmap(), "/appicon/");
  					} catch (IOException e) {
  						e.printStackTrace();
  					}
  			    	appInfoDataBase.execSQL(insertThird,new String[]{appid,appSize,appName,packageName,appPath});
  				}
  			}
			if(appInfoDBHelper != null){
				appInfoDBHelper.close();
			}
			if(appInfoDataBase != null && appInfoDataBase.isOpen()){
				appInfoDataBase.close();
			}
  			
  			
  			//判断遍历是否因为更改中断
  			if(appSign == true){
				//设置连接状态为已连接
  				Log.d("appinfo","need recreate");
  			}
  			else{
  				//将更新标识为已更新
  				appReady = true;
  	  			Log.d("appinfo", "app create");
  			}
  		}
  	};
   
    //aidl文件形成的Bindler机制服务类,获取应用程序大小
    public class PackageSizeObserver extends IPackageStatsObserver.Stub{
    	boolean ready = false;
    	int codesize = 0;
		@Override
		public void onGetStatsCompleted(PackageStats pStats, boolean succeeded)
		
				throws RemoteException {
		    codesize = (int) pStats.codeSize  ;  //应用程序大小
		    ready = true;
		}
		
		public String getPackageSize(){
			
			//等待回调函数执行
			while(ready == false){
				try {
					HandlerThread.sleep(50);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
			ready = false;
			
			return String.valueOf(codesize);
		}
    }
    
    //后台更新图片数据image_pack,imageThumbnail/
  	private Runnable imageRunnable = new Runnable() {
  		
  		@Override
  		public void run() {

  			//更新标识设为false
			imageSign = false;
  			imageReady = false;
  			
  			Bitmap image = null;
  			int count = 1;
  			int ret = 0;
  	    	File dir = new File(getFilesDir().getPath()+ "/imageThumbnail/");	//若图片缩略图文件夹已存在则删除
  			if(dir.isDirectory() == true){
  				File[] files = dir.listFiles();
  				for (File f:files){
  					f.delete();
  				}
  			}
  	    	Cursor cursor = getBaseContext().getContentResolver().query(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, null, null,null, null);
  			if (cursor != null) {
  				while (cursor.moveToNext() && imageSign == false) {
  					//图片显示名
  					String name = cursor.getString(cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DISPLAY_NAME));
  					//获取图片的保存位置的数据
  					String path = cursor.getString(cursor.getColumnIndex(MediaStore.Images.Media.DATA));
  					//获取图片大小
  					int size = cursor.getInt(cursor.getColumnIndex(MediaStore.Images.Media.SIZE));
  					try {
  						//将创建的缩略图保存至/imageThumbnail
  						image = getImageThumbnail(path, 96, 96);
  						saveBitmap(String.valueOf(count), image,"/imageThumbnail/");
  		  				if(image != null && !image.isRecycled()){
  		  					image.recycle();
  		  				}
  					} catch (IOException e) {
  						e.printStackTrace();
  					}
  					//将获取信息插入结构体
  					ret = image_pack_create(count++,size,name,path);
  					if(ret == 1){
  						break;
  					}
//  					Log.d("image", count + " " + name + " " + path + " " + String.valueOf(size));
  				}
  				cursor.close();
  			}
  			
  			//判断遍历是否因为更改中断
  			if(imageSign == true){
  				//若发生修改则结束
  				Log.d("image","need recreate");
  			}
  			else{
  				//若未发生修改则将图片更新标识设为已更新
  				imageReady = true;
  	  			Log.d("image", "image create");
  			}
  		}
  	};
    
    //获取图片缩略图，参数为图片路径，图片宽高
    private Bitmap getImageThumbnail(String imagePath, int width, int height) {
		Bitmap bitmap = null;
		BitmapFactory.Options options = new BitmapFactory.Options();
		options.inJustDecodeBounds = true;
		// 获取这个图片的宽和高，此处的bitmap为null
		bitmap = BitmapFactory.decodeFile(imagePath, options);
		options.inJustDecodeBounds = false; // 设为 false
		// 计算缩放比
		int h = options.outHeight;
		int w = options.outWidth;
		int beWidth = w / width;
		int beHeight = h / height;
		int be = 1;
		if (beWidth < beHeight) {
			be = beWidth;
		} else {
			be = beHeight;
		}
		if (be <= 0) {
			be = 1;
		}
		options.inSampleSize = be;
		// 重新读入图片，读取缩放后的bitmap
		bitmap = BitmapFactory.decodeFile(imagePath, options);
		// 利用ThumbnailUtils来创建缩略图
		bitmap = ThumbnailUtils.extractThumbnail(bitmap, width, height,ThumbnailUtils.OPTIONS_RECYCLE_INPUT);
		return bitmap;
	}

    //后台更新音频数据audio_pack
  	private Runnable audioRunnable = new Runnable() {
  		
  		@Override
  		public void run() {
  			
  			//更新标识设为false
			audioSign = false;
  			audioReady = false;
  			
  			int count = 1;
  			int ret = 0;
  			Cursor cursor = getBaseContext().getContentResolver().query(MediaStore.Audio.Media.EXTERNAL_CONTENT_URI, null, null,null, null);
  			if (cursor != null) {
  				while (cursor.moveToNext() && audioSign == false) {
  					String title = cursor.getString(cursor.getColumnIndexOrThrow(MediaStore.Audio.Media.TITLE));	//歌曲名
  					String artist = cursor.getString(cursor.getColumnIndexOrThrow(MediaStore.Audio.Media.ARTIST));	//歌手名
  					String path = cursor.getString(cursor.getColumnIndexOrThrow(MediaStore.Audio.Media.DATA));	//路径
  					int duration = cursor.getInt(cursor.getColumnIndexOrThrow(MediaStore.Audio.Media.DURATION));//时长 
  					int size = cursor.getInt(cursor.getColumnIndexOrThrow(MediaStore.Audio.Media.SIZE));	//大小
  					//将audio信息添加至audio_pack
  					ret = audio_pack_create(count++, size, duration, title, artist, path);
  					if(ret == 1){
  						break;
  					}
  				}
  				cursor.close();
  			}
  			
  			//判断遍历是否因为更改中断
  			if(audioSign == true){
  				//若发生修改，则结束
  				Log.d("audio","need recreate");
  			}
  			else{
  				//若未发生修改，则将音频标识设为已更新
  				audioReady = true;
  	  			Log.d("audio", "audio create");
  			}
  		}
  	};

    //后台更新视频数据video_pack,videoThumbnail/
  	private Runnable videoRunnable = new Runnable() {
  		
  		@Override
  		public void run() {

  			//更新标识设为false
			videoSign = false;
  			videoReady = false;
  			
  			Bitmap video = null;
  			int count = 1;
  			int ret = 0;
  	    	File dir = new File(getFilesDir().getPath()+ "/videoThumbnail/");	//若视频缩略图文件夹已存在则删除
  			if(dir.isDirectory() == true){
  				File[] files = dir.listFiles();
  				for (File f:files)
  				{
  					f.delete();
  				}
  			}
  	    	Cursor cursor = getBaseContext().getContentResolver().query(MediaStore.Video.Media.EXTERNAL_CONTENT_URI, null, null,null, null);
  			if (cursor != null) {
  				while (cursor.moveToNext() && videoSign == false) {
  					String title = cursor.getString(cursor.getColumnIndexOrThrow(MediaStore.Video.Media.TITLE));	//视频名
  					String path = cursor.getString(cursor.getColumnIndexOrThrow(MediaStore.Video.Media.DATA));	//视频路径
  					int duration = cursor.getInt(cursor.getColumnIndexOrThrow(MediaStore.Video.Media.DURATION));	//视频时长
  					int size = cursor.getInt(cursor.getColumnIndexOrThrow(MediaStore.Video.Media.SIZE));	//视频大小
  					//创建视频缩略图保存至/videoThumbnail/
  					try {
  						video = getVideoThumbnail(path, 96, 96);
  						saveBitmap(String.valueOf(count), video,"/videoThumbnail/");
  		  				if(video != null && !video.isRecycled()){
  		  					video.recycle();
  		  				}
  					} catch (IOException e) {
  						e.printStackTrace();
  					}
  					//将视频信息添加video_pack
  					ret = video_pack_create(count++, size, duration, title, path);
  					if(ret == 1){
  						break;
  					}
  				}
  				cursor.close();
  			}
  			
  			//判断遍历是否因为更改中断
  			if(videoSign == true){
  				//若发生修改，则重新创建
  				Log.d("video","need recreate");
  			}
  			else{
  				//若成功完成，将视频更新标识设为已更新
  				videoReady = true;
  	  			Log.d("video", "video create");
  			}
  		}
  	};

	//获取视频的缩略图,参数为视频路径，视频宽高   
    private Bitmap getVideoThumbnail(String videoPath, int width, int height) {
		Bitmap bitmap = null;
		bitmap = ThumbnailUtils.createVideoThumbnail(videoPath,MediaStore.Images.Thumbnails.MICRO_KIND);
		bitmap = ThumbnailUtils.extractThumbnail(bitmap, width, height,ThumbnailUtils.OPTIONS_RECYCLE_INPUT);
		return bitmap;
	}
    
    //将bitmap保存为directory下名为name的png文件
    private void saveBitmap(String name,Bitmap bitmap,String directory) throws IOException{
    	if(bitmap == null){
    		Log.e("image", "save bitmap null");
    		return;
    	}
    	//判断directory目录是否存在，不存在则创建
    	File dir = new File(getFilesDir().getPath()+directory);
        FileOutputStream outfile = null;
        if(dir.isDirectory() == false){
    		dir.mkdir();
        }
        //将bitmap保存至outfile
        try {
        	outfile = new FileOutputStream(dir.getPath() + "/" +name + ".png");
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
        bitmap.compress(Bitmap.CompressFormat.PNG, 100, outfile);
        try {
        	outfile.flush();
        	outfile.close();
		} catch (IOException e1) {
			e1.printStackTrace();
		}
    }

    //sqlite打开
	public class MyDatabaseHelper extends SQLiteOpenHelper {

		private static final int VERSION = 1;
		public MyDatabaseHelper(Context context, String name,
				CursorFactory factory, int version) {
			super(context, name, factory, version);
		}
		
		public MyDatabaseHelper(Context context, String name,int version) {
			super(context, name, null, version);
		}
		
		public MyDatabaseHelper(Context context, String name) {
			super(context, name, null, VERSION);
		}

		@Override
		public void onCreate(SQLiteDatabase db) {
		}

		@Override
		public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
			Log.d("dbhelper", "update"+oldVersion+"--->"+newVersion);
		}
	}

	/************************************************用于监测各类信息是否更改******************************************************/
    
	//监测联系人是否更改
	private class ContactObserver extends ContentObserver{

		public ContactObserver(Handler handler) {
			super(handler);
		}
		
		public void onChange(boolean selfChange){
			super.onChange(selfChange);
			//联系人修改标识置为已修改
			contactSign = true;
			//联系人更新标识置为待更新
			contactReady = false;
			//重新post
			contactHandler.removeCallbacks(contactRunnable);
			contactHandler.postDelayed(contactRunnable,1000);
		}
	}
	
    //监测短信是否更改
    private class MessageObserver extends ContentObserver{
    	
        public MessageObserver(Handler handler) {
			super(handler);
		}

		public void onChange(boolean selfChange) {
            super.onChange(selfChange);
            //获取短信id，号码，内容，日期，状态（发送/接收，已读/未读）
    		String[] projection = new String[] {"_id","address","body", "date", "type","read" };
    		Uri uri = Uri.parse("content://sms/");
    		Cursor cursor = getContentResolver().query(uri, projection, null, null, "date desc");
    		//创建一个ContentValues用于修改短信为已读状态
    		ContentValues values = new ContentValues();
    		values.clear();
    		values.put("read","1");
    	
    		//若connectState为已连接，则实时接收短信，并置为已读
    		if (cursor.moveToFirst() && connectState == true) {
				String message_num = cursor.getString(cursor.getColumnIndex("address"));
				String message_name = getMessageName(message_num);
				String message_body = cursor.getString(cursor.getColumnIndex("body"));
				//规范日期格式
				SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss",Locale.getDefault());
				Date date = new Date(Long.parseLong(cursor.getString(cursor.getColumnIndex("date"))));
				String message_date = dateFormat.format(date);

				int message_id = cursor.getInt(cursor.getColumnIndex("_id"));
				int message_type = cursor.getInt(cursor.getColumnIndex("type"));	//1接收，2发送
				int message_read = cursor.getInt(cursor.getColumnIndex("read"));	//0未读，1已读
				Log.d("message", "message_id "+message_id);
	    		//将新收到的短信存储到new_message
				if(message_type == 1 && message_read == 0){
					new_message_create(message_name, message_num, message_body, message_date);
					Log.d("message",message_name+ message_num+ message_body+ message_date);
					 //修改短信为已读模式
		    		getContentResolver().update(Uri.parse("content://sms/"),values,"_id=?", new String[]{String.valueOf(message_id)});
				}
				cursor.close();
    		}
			
			//短信修改标识设为已修改
			messageSign = true;
			//短信更新标识为待更新
			messageReady = false;
			//重新post
			messageHandler.removeCallbacks(messageRunnable);
			messageHandler.postDelayed(messageRunnable,1000);
        }
    };
    
    //监测应用是否更改
    public class AppReceiver extends BroadcastReceiver {
    	@Override
    	public void onReceive(Context context, Intent intent) {
    		//若匹配到安装及卸载发出的广播
    		if (intent.getAction().equals("android.intent.action.PACKAGE_ADDED") || intent.getAction().equals("android.intent.action.PACKAGE_REMOVED")) {
    			//设置应用修改标识为已修改
    			appSign = true;
    			//设置应用更新标识为待更新
    			appReady = false;
    			//重新post
    			appHandler.removeCallbacks(appRunnable);
    			appHandler.postDelayed(appRunnable, 1000);
    		}
    	}
    }
    
	//监测图片是否更改
  	private class ImageObserver extends ContentObserver{

  		public ImageObserver(Handler handler) {
  			super(handler);
  		}
  		
  		public void onChange(boolean selfChange){
  			super.onChange(selfChange);
  			//设置图片修改标识
  			imageSign = true;
  			//设置图片更新标识
			imageReady = false;
			//重新post
			imageHandler.removeCallbacks(imageRunnable);
			imageHandler.postDelayed(imageRunnable, 1000);
  		}
  	}
  	
  	//监测音频是否更改
  	private class AudioObserver extends ContentObserver{

  		public AudioObserver(Handler handler) {
  			super(handler);
  		}
  		
  		public void onChange(boolean selfChange){
  			super.onChange(selfChange);
  			//设置音频修改标识为已修改
  			audioSign = true;
  			//设置音频更新标识为待更新
			audioReady = false;
			//重新post
			audioHandler.removeCallbacks(audioRunnable);
			audioHandler.postDelayed(audioRunnable, 1000);
  		}
  	}
  	
  	//监测视频是否更改
  	private class VideoObserver extends ContentObserver{

  		public VideoObserver(Handler handler) {
  			super(handler);
  		}
  		
  		public void onChange(boolean selfChange){
  			super.onChange(selfChange);
  			//设置视频修改标识为已修改
  			videoSign = true;
  			//设置视频更新标识为待更新
			videoReady = false;
			//重新post
			videoHandler.removeCallbacks(videoRunnable);
			videoHandler.postDelayed(videoRunnable, 1000);
  		}
  	}
    
    /*****************************************************本地函数声明(用法详见cpp文件)**********************************************************/
	
    public native static int pthread_init();
    public native static int get_cmd_flag();
    public native static String get_contact_cmd(int item);
    public native static String get_message_cmd(int item);
    public native static void send_cmd_flag(int send_flag);
    public native static void set_end_flag(int option);
    public native static void new_message_create(String message_name,String  message_num,String message_body,String message_date);
    public native static int message_pack_create(String message_name,String  message_num,String message_body,String message_date,int message_type,int message_read,int count);
    public native static int image_pack_create(int count,int img_size,String img_name,String img_path);
    public native static int audio_pack_create(int count,int audio_size,int audio_duration,String audio_title,String audio_artist,String audio_path);
    public native static int video_pack_create(int count,int audio_size,int audio_duration,String audio_title,String audio_path);
    
    //加载libass.so
	static{
		System.loadLibrary("Ass");
	}
}
