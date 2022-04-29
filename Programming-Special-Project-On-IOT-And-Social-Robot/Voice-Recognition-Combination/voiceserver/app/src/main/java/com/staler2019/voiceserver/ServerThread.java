package com.staler2019.voiceserver;

import android.os.Handler;
import android.util.Log;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.HttpURLConnection;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.URL;

public class ServerThread extends Thread {
    ServerSocket serverSocket;
    int serverPort = Constant.SERVER_PORT;
    Socket client;
    BufferedReader br;
    BufferedWriter bw;
    Handler handler;

    @Override
    public void run() {
        try {

            serverSocket = new ServerSocket(serverPort);
            while (!serverSocket.isClosed()) {
                sendToHandlerMessage("waiting to connect");
                client = serverSocket.accept();
                sendToHandlerMessage("Connected");
                br = new BufferedReader(new InputStreamReader(client.getInputStream()));
                bw = new BufferedWriter(new OutputStreamWriter(client.getOutputStream()));
                String rl;
                while (client.isConnected()) {
                    if ((rl = br.readLine()) != null && !rl.equals("")) {
//                        sendToHandlerMessage(rl);
                        JSONObject readObj = new JSONObject(rl);
                        // handle command
                        String command = readObj.getString("Command");
                        sendToHandlerMessage("client ask for: " + command);
                        String data = null;
                        try {
                            if (command.equals("temperature")) {
                                data = getMCSData(Constant.TEMP_CHANNEL_ID);
                            } else if (command.equals("humidity")) {
                                data = getMCSData(Constant.HUMID_CHANNEL_ID);
                            } else {
                                Log.e("Command", "there's no \"" + command + "\".\n");
                                sendToHandlerMessage(" but command not found.");
                                throw new IllegalStateException("There's no \"" + command + "\".");
                            }
                            sendToHandlerMessage(" and value is " + data);
                        } catch (Exception e) {
                            e.printStackTrace();
                            sendToHandlerMessage(e.toString());
                        }
                        sendMsg(data);
                    }
                }
                Log.d("Client", "trying to reconnect.");
            }

        } catch (JSONException | IOException e) {
            e.printStackTrace();
            sendToHandlerMessage(e.toString());
        } finally {
            Log.d("ServerSocket: ", "ServerSocket shutting down!");
        }
    }

    private void sendMsg(String msg) {
        try {
            JSONObject writeObj = new JSONObject();
            writeObj.put("Value", msg);

            bw.write(writeObj + "\n");
            bw.flush();
            sendToHandlerMessage("sent data");
        } catch (JSONException | IOException e) {
            e.printStackTrace();
            sendToHandlerMessage(e.toString());
        }
    }

    private String getMCSData(String datachannel_id) throws Exception {
        // limit=1 read 1 data
        String urlPath = "http://" + Constant.MCS_SERVER + "/api/devices/" + Constant.MCS_DEVICE_ID + "/datachannels/" + datachannel_id + "/datapoints?limit=1";

        URL url = new URL(urlPath);
        HttpURLConnection connection = (HttpURLConnection) url.openConnection();

        connection.setRequestMethod("GET");
        connection.setRequestProperty("deviceKey", Constant.MCS_DEVICE_KEY);
        connection.setDoInput(true);

        BufferedReader bR = new BufferedReader(new InputStreamReader(connection.getInputStream()));
        String line;
        StringBuilder responseStrBuilder = new StringBuilder();

        while ((line = bR.readLine()) != null) {
            responseStrBuilder.append(line);
        }
        bR.close();

        JSONObject jsonPost = new JSONObject(responseStrBuilder.toString());
        JSONObject object = (JSONObject) jsonPost.getJSONArray("data").get(0);
        return (String) object.getJSONObject("values").getString("value");
    }

    private void sendToHandlerMessage(String msg) {
        handler.sendMessage(handler.obtainMessage(0, msg));
    }

    public void setHandler(Handler handler) {
        this.handler = handler;
    }
}

// TODO. ADD ALLOW TO HTTP 爬蟲