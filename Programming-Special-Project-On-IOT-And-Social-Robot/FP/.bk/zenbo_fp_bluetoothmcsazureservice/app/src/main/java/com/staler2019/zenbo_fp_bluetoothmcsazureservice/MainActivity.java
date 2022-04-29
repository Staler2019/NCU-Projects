package com.staler2019.zenbo_fp_bluetoothmcsazureservice;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.widget.Button;
import android.widget.TextView;

import androidx.annotation.Nullable;
import androidx.core.app.ActivityCompat;

import com.asus.robotframework.API.RobotCallback;
import com.asus.robotframework.API.RobotCmdState;
import com.asus.robotframework.API.RobotCommand;
import com.asus.robotframework.API.RobotErrorCode;
import com.microsoft.cognitiveservices.speech.CancellationDetails;
import com.microsoft.cognitiveservices.speech.CancellationReason;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.ResultReason;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognitionResult;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognizer;
import com.microsoft.cognitiveservices.speech.intent.LanguageUnderstandingModel;

import org.json.JSONException;
import org.json.JSONObject;


import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.util.Set;
import java.util.UUID;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;

import static android.Manifest.permission.*;
import static android.content.ContentValues.TAG;

public class MainActivity extends RobotActivity {

    // layout
    private static TextView tv_msg;
    private static Button btn_speech;
    private Button btn_conn;

    // bluetooth
    public static Handler btHandler = new Handler(Looper.getMainLooper()) {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case CONNECTING_STATUS:
                    switch (msg.arg1) {
                        case 1: // connected
                            tv_msg.append("connected to bt\n");
                            btn_speech.setEnabled(false);
                            break;
                        case -1:
                            tv_msg.append("connecting to bt fail\n");
                            btn_speech.setEnabled(true);
                            break;
                    }
                    break;

                case MESSAGE_READ:
                    String linkitMsg = msg.obj.toString();
                    tv_msg.append(linkitMsg + "\n");
                    break;
            }
        }
    };
    public static BluetoothSocket mmSocket;
    public static ConnectedThread connectedThread;
    public static CreateConnectThread createConnectThread;

    private final static int CONNECTING_STATUS = 1; // used in bluetooth handler to identify message status
    private final static int MESSAGE_READ = 2; // used in bluetooth handler to identify message update

    static BluetoothAdapter bluetoothAdapter;
    private final static int REQUEST_ENABLE_BT = 1; // 自己命名代號意義
    private static final int REQ_PERMISSION_COARSE_LOCATION = 0;
    Set<BluetoothDevice> pairedDevices;
    private final BroadcastReceiver btReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.d("searchDeviceReceiver", "onReceive");
            String action = intent.getAction();
            if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                Log.d("BT paired device EXTRA", device.getName() + " (" + device.getAddress() + ")");
//                tv_msg.append("extra: " + device.getName() + " (" + device.getAddress() + ")\n");
//                if (device.getAddress().equals(Constant.BT_CLIENT_MAC)) {
//                    btConn();
//                }
            } else if (BluetoothAdapter.ACTION_DISCOVERY_STARTED.equals(action)) {
                Log.d("BT search device", "search start");
            } else if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action)) {
                Log.d("BT search device", "search finished");
            }
        }
    };

    // speech luis
    private SpeechConfig speechConfig;
    private IntentRecognizer reco;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // MY CODE STARTS
        String[] permissionList = {RECORD_AUDIO, INTERNET};
        ActivityCompat.requestPermissions(MainActivity.this, permissionList, 5);

        initViewElement();
        initSpeechLuis();
//        btConn();
        try {
            initBT();
        } catch (NoSuchFieldException e) {
            e.printStackTrace();
            tv_msg.append("device doesn't support bluetooth\n");
        }
    }

    // MY OVERWRITE
    @Override
    protected void onDestroy() { //todo. server&client
        super.onDestroy();
        // Terminate Bluetooth Connection and close app
        if (createConnectThread != null) {
            createConnectThread.cancel();
        }
        // bt
        unregisterReceiver(btReceiver);
        // speech luis
        reco.close();
        speechConfig.close();
    }

    private void initViewElement() {
        tv_msg = findViewById(R.id.tv_msg);
        btn_speech = findViewById(R.id.btn_speech);
        btn_conn = findViewById(R.id.btn_conn);

        tv_msg.append("\n");
        btn_speech.setOnClickListener(view -> {
            new Thread(() -> {
                recoVoice();
            }).start();
        });
        btn_conn.setOnClickListener(view -> {
            btConn();
        });
    }

    // MY CODE BLUETOOTH
    // ref: https://medium.com/swlh/create-custom-android-app-to-control-arduino-board-using-bluetooth-ff878e998aa8
    private void btConn() {
//        BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        createConnectThread = new CreateConnectThread(bluetoothAdapter, Constant.BT_CLIENT_MAC);
        createConnectThread.start();
    }

    // ref: https://github.com/chenmingtw/BTDemo/
    // ref: https://developer.android.com/guide/topics/connectivity/bluetooth#java
    private void initBT() throws NoSuchFieldException {
        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (bluetoothAdapter == null) {
            Log.e("bluetooth", "your device doesn't support bluetooth");
            throw new NoSuchFieldException();
        } else {
            // bt permission
            if (this.checkSelfPermission(Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                requestPermissions(new String[]{Manifest.permission.ACCESS_COARSE_LOCATION}, REQ_PERMISSION_COARSE_LOCATION);
            }

            if (!bluetoothAdapter.isEnabled()) turnOnBT();

            while (!bluetoothAdapter.isEnabled()) ;

            bluetoothAdapter.startDiscovery();

            pairedDevices = bluetoothAdapter.getBondedDevices();
            if (pairedDevices.size() > 0) {
                for (BluetoothDevice device : pairedDevices) {
                    Log.d("BT paired device", device.getName() + " (" + device.getAddress() + ")");
                    tv_msg.append(device.getName() + " (" + device.getAddress() + ")\n");
                }
            } else {
                Log.d("BT paired device", "no device");
            }
//
//            // find not paired devices
            IntentFilter filterFound = new IntentFilter(BluetoothDevice.ACTION_FOUND);
            registerReceiver(btReceiver, filterFound);
            IntentFilter filterStart = new IntentFilter(BluetoothAdapter.ACTION_DISCOVERY_STARTED);
            registerReceiver(btReceiver, filterStart);
            IntentFilter filterFinish = new IntentFilter(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
            registerReceiver(btReceiver, filterFinish);
        }
    }

    private void turnOnBT() {
        Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
        startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
    }
//
//    private void turnOffBT() {
//        bluetoothAdapter.disable();
//    }

    // bt enable or not log
    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        switch (requestCode) {
            case REQUEST_ENABLE_BT:
                if (resultCode == RESULT_OK)
                    Log.d("BT enable or not", "is enabled");
                else
                    Log.d("BT enable or not", "enabling is fail");
                break;
        }
    }

    // speech luis
    private void initSpeechLuis() {
        speechConfig = SpeechConfig.fromSubscription(Constant.speechSubscriptionKey, Constant.serviceRegion);
        assert (speechConfig != null);

        reco = new IntentRecognizer(speechConfig);
        assert (reco != null);

        LanguageUnderstandingModel model = LanguageUnderstandingModel.fromAppId(Constant.appId);
        reco.addIntent(model, "about_light_sentences");
    }

    private void recoVoice() {
        runOnUiThread(() -> {
            tv_msg.setText("listening...\n");
        });
        try {
            Future<IntentRecognitionResult> task = reco.recognizeOnceAsync();
            assert (task != null);

            IntentRecognitionResult result = null;
            result = task.get();

            assert (result != null);

            runOnUiThread(() -> {
                tv_msg.setText("end of listening\n");
            });

            String res = "";
            // Checks result.
            if (result.getReason() == ResultReason.RecognizedIntent) {
                res = res.concat("RECOGNIZED: Text=" + result.getText());
                res = res.concat("    Intent Id: " + result.getIntentId());
                res = res.concat("    Intent Service JSON: " + result.getProperties().getProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult));

                JSONObject jsonObject = new JSONObject(result.getProperties().getProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult));
                if (jsonObject.has("entities")) {
                    try {
                        JSONObject jsonObject2 = (JSONObject) jsonObject.getJSONArray("entities").get(0);
                        if (jsonObject2.has("entity")) {
                            if (jsonObject2.getDouble("score") > 0.6) {
                                runOnUiThread(() -> {
                                    try {
                                        tv_msg.append(jsonObject2.getString("entity\n"));
                                    } catch (JSONException e) {
                                        e.printStackTrace();
                                    }
                                });
                                handleRecoAction(jsonObject2.getString("entity"));
                            }
                        }
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                }

            } else if (result.getReason() == ResultReason.RecognizedSpeech) {
                res = res.concat("RECOGNIZED: Text=" + result.getText());
                res = res.concat("    Intent not recognized.");
            } else if (result.getReason() == ResultReason.NoMatch) {
                res = res.concat("NOMATCH: Speech could not be recognized.");
            } else if (result.getReason() == ResultReason.Canceled) {
                CancellationDetails cancellation = CancellationDetails.fromResult(result);
                res = res.concat("CANCELED: Reason=" + cancellation.getReason());

                if (cancellation.getReason() == CancellationReason.Error) {
                    res = res.concat("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                    res = res.concat("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                    res = res.concat("CANCELED: Did you update the subscription info?");
                }
            }

            Log.d("azure cognitive", res);

            result.close();
        } catch (ExecutionException | InterruptedException | JSONException e) {
            e.printStackTrace();
        }
    }

    private void handleRecoAction(String text) { // server & client
        int brightness = getBrightnessData();
        int finalBrightness1 = brightness;
        runOnUiThread(() -> {
            tv_msg.append("now brightness" + String.valueOf(finalBrightness1) + "\n");
        });
        switch (text) {
            case "turn on the light": {
                brightness = 3000;
//            BluetoothServerSocketThread.sendMsg(SendObj.BRIGHTNESS, String.valueOf(brightness));
                connectedThread.write(SendObj.BRIGHTNESS, String.valueOf(brightness));
                int finalBrightness = brightness;
                runOnUiThread(() -> {
                    tv_msg.append("set brightness " + String.valueOf(finalBrightness) + "\n");
                });
                break;
            }
            case "turn off the light": {
                brightness = 0;
//            BluetoothServerSocketThread.sendMsg(SendObj.BRIGHTNESS, String.valueOf(brightness));
                connectedThread.write(SendObj.BRIGHTNESS, String.valueOf(brightness));
                int finalBrightness = brightness;
                runOnUiThread(() -> {
                    tv_msg.append("set brightness " + String.valueOf(finalBrightness) + "\n");
                });
                break;
            }
            case "help me turn the light up": {
                brightness *= 1.2;
//            BluetoothServerSocketThread.sendMsg(SendObj.BRIGHTNESS, String.valueOf(brightness));
                connectedThread.write(SendObj.BRIGHTNESS, String.valueOf(brightness));
                int finalBrightness = brightness;
                runOnUiThread(() -> {
                    tv_msg.append("set brightness " + String.valueOf(finalBrightness) + "\n");
                });
                break;
            }
            case "help me turn the light dim": {
                brightness *= 0.8;
//            BluetoothServerSocketThread.sendMsg(SendObj.BRIGHTNESS, String.valueOf(brightness));
                connectedThread.write(SendObj.BRIGHTNESS, String.valueOf(brightness));
                int finalBrightness = brightness;
                runOnUiThread(() -> {
                    tv_msg.append("set brightness " + String.valueOf(finalBrightness) + "\n");
                });
                break;
            }
        }
    }

    private int getBrightnessData() {
        String dataStr = null;
        try {
            dataStr = MCScl.getMCSData(Constant.LIGHT_CHANNEL_ID);
        } catch (Exception e) {
            e.printStackTrace();
        }
        assert dataStr != null;
        return Integer.parseInt(dataStr);
    }

    // MY CLASSES
    /* ============================ Thread to Create Bluetooth Connection =================================== */
    public static class CreateConnectThread extends Thread {

        public CreateConnectThread(BluetoothAdapter bluetoothAdapter, String address) {
            /*
            Use a temporary object that is later assigned to mmSocket
            because mmSocket is final.
             */
            BluetoothDevice bluetoothDevice = bluetoothAdapter.getRemoteDevice(address);
            BluetoothSocket tmp = null;
//            UUID uuid = bluetoothDevice.getUuids()[0].getUuid();
            UUID uuid = Constant.BT_CLIENT_UUID;
//            UUID uuid = Constant.BT_SERVER_UUID;

            try {
                /*
                Get a BluetoothSocket to connect with the given BluetoothDevice.
                Due to Android device varieties,the method below may not work fo different devices.
                You should try using other methods i.e. :
                tmp = device.createRfcommSocketToServiceRecord(MY_UUID);
                 */
//                tmp = bluetoothDevice.createRfcommSocketToServiceRecord(uuid);
                tmp = bluetoothDevice.createInsecureRfcommSocketToServiceRecord(uuid);

            } catch (IOException e) {
                Log.e(TAG, "Socket's create() method failed", e);
            }
            mmSocket = tmp;
        }

        public void run() {
            // Cancel discovery because it otherwise slows down the connection.
//            BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
//            bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
            bluetoothAdapter.cancelDiscovery();
            try {
                // Connect to the remote device through the socket. This call blocks
                // until it succeeds or throws an exception.
                mmSocket.connect();
                Log.e("Status", "Device connected");
                btHandler.obtainMessage(CONNECTING_STATUS, 1, -1).sendToTarget();
            } catch (IOException connectException) {
                // Unable to connect; close the socket and return.
                try {
                    mmSocket.close();
                    Log.e("Status", "Cannot connect to device");
                    btHandler.obtainMessage(CONNECTING_STATUS, -1, -1).sendToTarget();
                } catch (IOException closeException) {
                    Log.e(TAG, "Could not close the client socket", closeException);
                }
                return;
            }

            // The connection attempt succeeded. Perform work associated with
            // the connection in a separate thread.
            connectedThread = new ConnectedThread(mmSocket);
            connectedThread.run();
        }

        // Closes the client socket and causes the thread to finish.
        public void cancel() {
            try {
                mmSocket.close();
            } catch (IOException e) {
                Log.e(TAG, "Could not close the client socket", e);
            }
        }
    }

    /* =============================== Thread for Data Transfer =========================================== */
    public static class ConnectedThread extends Thread {
        private final BluetoothSocket mmSocket;
        private final InputStream mmInStream;
        private final OutputStream mmOutStream;

        public ConnectedThread(BluetoothSocket socket) {
            mmSocket = socket;
            InputStream tmpIn = null;
            OutputStream tmpOut = null;

            // Get the input and output streams, using temp objects because
            // member streams are final
            try {
                tmpIn = socket.getInputStream();
                tmpOut = socket.getOutputStream();
            } catch (IOException e) {
            }

            mmInStream = tmpIn;
            mmOutStream = tmpOut;
        }

        public void run() {
            byte[] buffer = new byte[1024];  // buffer store for the stream
            int bytes = 0; // bytes returned from read()
            // Keep listening to the InputStream until an exception occurs
            while (true) {
                try {
                    /*
                    Read from the InputStream from Arduino until termination character is reached.
                    Then send the whole String message to GUI Handler.
                     */
                    buffer[bytes] = (byte) mmInStream.read();
                    String readMessage;
                    if (buffer[bytes] == '\n') {
                        readMessage = new String(buffer, 0, bytes);
                        Log.e("Arduino Message", readMessage);
                        btHandler.obtainMessage(MESSAGE_READ, readMessage).sendToTarget();
                        bytes = 0;
                    } else {
                        bytes++;
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                    break;
                }
            }
        }

        /* Call this from the main activity to send data to the remote device */
        public void write(String label, String msg) {
            BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(mmOutStream));

            try {
                JSONObject writeObj = new JSONObject();

                writeObj.put(label, msg);

                bw.write(writeObj + "\n");
                bw.flush();

            } catch (IOException | JSONException e) {
                e.printStackTrace();
            }
        }

//        public void write(String input) {
//            byte[] bytes = input.getBytes(); //converts entered String into bytes
//            try {
//                mmOutStream.write(bytes);
//            } catch (IOException e) {
//                Log.e("Send Error","Unable to send message",e);
//            }
//        }

        /* Call this from the main activity to shutdown the connection */
        public void cancel() {
            try {
                mmSocket.close();
            } catch (IOException e) {
            }
        }
    }

    // EXTENDS SETTINGS
    public MainActivity() {
        super(robotCallback, robotListenCallback);
    }

    public static RobotCallback robotCallback = new RobotCallback() {
        @Override
        public void onResult(int cmd, int serial, RobotErrorCode err_code, Bundle result) {
            super.onResult(cmd, serial, err_code, result);

            Log.d("RobotDevSample", "onResult:"
                    + RobotCommand.getRobotCommand(cmd).name()
                    + ", serial:" + serial + ", err_code:" + err_code
                    + ", result:" + result.getString("RESULT"));
        }

        @Override
        public void onStateChange(int cmd, int serial, RobotErrorCode err_code, RobotCmdState state) {
            super.onStateChange(cmd, serial, err_code, state);
        }

        @Override
        public void initComplete() {
            super.initComplete();
        }
    };

    public static RobotCallback.Listen robotListenCallback = new RobotCallback.Listen() {
        @Override
        public void onFinishRegister() {

        }

        @Override
        public void onVoiceDetect(JSONObject jsonObject) {

        }

        @Override
        public void onSpeakComplete(String s, String s1) {

        }

        @Override
        public void onEventUserUtterance(JSONObject jsonObject) {

        }

        @Override
        public void onResult(JSONObject jsonObject) {

        }

        @Override
        public void onRetry(JSONObject jsonObject) {

        }
    };
}