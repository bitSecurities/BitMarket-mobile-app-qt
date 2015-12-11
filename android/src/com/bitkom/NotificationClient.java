package com.bitkom;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.view.Window;
import android.view.WindowManager;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;

public class NotificationClient extends org.qtproject.qt5.android.bindings.QtActivity
{
    private static NotificationManager m_notificationManager;
    private static Notification.Builder m_builder;
    private static NotificationClient m_instance;
    private static Intent m_intent;
    private static String scannedtxt;

    public NotificationClient()
    {
        m_instance = this;
        m_notificationManager=null;        
        m_intent=new Intent();
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        Window window = this.getWindow();
        window.clearFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);
        window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
        window.setStatusBarColor(0xFF41BB1A);
    }

    public static void notify(String s,String txt)
    {
        if (m_notificationManager==null) {
            m_notificationManager = (NotificationManager)m_instance.getSystemService(Context.NOTIFICATION_SERVICE);
            m_builder = new Notification.Builder(m_instance);
            m_builder.setSmallIcon(R.drawable.icon);
            m_builder.setContentIntent(PendingIntent.getActivity(m_instance, 0, m_intent, 0));
            m_builder.setAutoCancel(true);
        }
        m_builder.setContentTitle(s);
        m_builder.setContentText(txt);
        m_builder.setStyle(new Notification.BigTextStyle().bigText(txt));
        m_notificationManager.notify(1, m_builder.build());
    }

    public static void scanner()
    {
        try {
            Intent intent = new Intent("com.google.zxing.client.android.SCAN");
            intent.putExtra("SCAN_MODE", "QR_CODE_MODE");
            m_instance.startActivityForResult(intent, 0);
        } catch (Exception e) {
            Uri marketUri = Uri.parse("market://details?id=com.google.zxing.client.android");
            Intent marketIntent = new Intent(Intent.ACTION_VIEW,marketUri);
            m_instance.startActivity(marketIntent);
        }
    }

    public static String getscannedtxt()
    {
        String ret;

        ret=scannedtxt;
        scannedtxt="";
        return(ret);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data){
            super.onActivityResult(requestCode, resultCode, data);
            if ((resultCode == RESULT_OK)&&(data!=null)) scannedtxt=data.getStringExtra("SCAN_RESULT");
    }
}
