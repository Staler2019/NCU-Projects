package com.staler2019.zenbo_fp_bluetoothmcsazureservice;

import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;

public class MCScl {

    public static String getMCSData(String datachannel_id) throws Exception {
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
}
