package com.staler2019.zenbo_fp_bluetoothmcsazureservice;

import android.util.Log;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;

public class ServerThread extends Thread {

    private static final ArrayList<Socket> clients = new ArrayList<>();
    private int connCount = 0;
    private ServerSocket serverSocket;
    private final int SERVER_PORT = Constant.SERVER_PORT;

    // thread control
    @Override
    public void run() {
        try {
            serverSocket = new ServerSocket(SERVER_PORT);

            while (!serverSocket.isClosed()) {
                waitNewClient();
            }

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void terminate() {
        try {
            Thread.sleep(2000);
            serverSocket.close();

        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
    }

    // my functions
    private void waitNewClient() {
        try {

            Socket socket = serverSocket.accept();
            ++connCount;
            addNewClient(socket);

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void addNewClient(final Socket socket) {
        new Thread(() -> {
            clients.add(socket);

            while (socket.isConnected()) ;

            clients.remove(socket);
            --connCount;
            Log.d("users amount", String.valueOf(connCount));
        }).start();
    }

    // send msg
    public static void sendMsg(String msg) {
        castMsg(msg);
        Log.d("SendMsg", msg);
    }

    private static void castMsg(String msg) {
        Socket[] clientArrays = new Socket[clients.size()];
        clients.toArray(clientArrays);

        for (Socket socket : clientArrays) {
            new Thread(() -> {
                try {

                    BufferedWriter bw;
                    bw = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()));
                    bw.write(msg);
                    bw.flush();

                } catch (IOException e) {
                    e.printStackTrace();
                }
            }).start();
        }
    }
}
