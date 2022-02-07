package com.staler2019.voiceserver;

import static java.net.NetworkInterface.getNetworkInterfaces;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.widget.TextView;

import org.json.JSONObject;

import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;
import java.util.Objects;

public class MainActivity extends AppCompatActivity {

    TextView tv_status;
    // socket
    ServerThread serverThread;
    private final Handler mhandler = new Handler(Looper.getMainLooper()) {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            String recvStr = msg.obj.toString();
            tv_status.append(recvStr+"\n");
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        initViewElement();
        serverThread = new ServerThread();
        serverThread.start();
        serverThread.setHandler(mhandler);
    }

    void initViewElement() {
        tv_status = findViewById(R.id.tv_status);

        try {
            tv_status.setText(getIPAddress()+'\n');
        } catch (SocketException e) {
            e.printStackTrace();
        }
    }

    private static String getIPAddress() throws SocketException {
        StringBuilder IFCONFIG = new StringBuilder();
        Enumeration<NetworkInterface> en = getNetworkInterfaces();
        while (en.hasMoreElements()) {
            NetworkInterface intf = en.nextElement();
            Enumeration<InetAddress> enumIpAddr = intf.getInetAddresses();
            while (enumIpAddr.hasMoreElements()) {
                InetAddress inetAddress = enumIpAddr.nextElement();
                if (!inetAddress.isLoopbackAddress() && !inetAddress.isLinkLocalAddress() && inetAddress.isSiteLocalAddress()) {
                    IFCONFIG.append(Objects.requireNonNull(inetAddress.getHostAddress()));
                }
            }
        }
        Log.d("SERVER_IP", IFCONFIG.toString());
        return IFCONFIG.toString();
    }
}