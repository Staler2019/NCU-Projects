package com.staler2019.zenbo_fp_bluetoothmcsazureservice;

import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.TextView;

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

import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;
import java.util.Objects;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;

import static android.Manifest.permission.*;
import static java.net.NetworkInterface.getNetworkInterfaces;

public class MainActivity extends RobotActivity {

    // layout
    private TextView tv_msg;
    private Button btn_speech;

    // speech luis
    private SpeechConfig speechConfig;
    private IntentRecognizer reco;

    // socket
    private ServerThread serverThread;

    private int lightLevel = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // MY CODE STARTS
        String[] permissionList = {RECORD_AUDIO, INTERNET};
        ActivityCompat.requestPermissions(MainActivity.this, permissionList, 5);

        initViewElement();
        initSpeechLuis();

        serverThread = new ServerThread();
        serverThread.start();
    }

    // MY OVERWRITE
    @Override
    protected void onDestroy() {
        super.onDestroy();
        // socket
        serverThread.terminate();
        // speech luis
        reco.close();
        speechConfig.close();
    }

    // MY FUNCTION
    private void initViewElement() {
        tv_msg = findViewById(R.id.tv_msg);
        btn_speech = findViewById(R.id.btn_speech);

        try {
            tv_msg.setText("Server IP addr: " + getIPAddress() + "\n");
        } catch (SocketException e) {
            e.printStackTrace();
        }
        btn_speech.setOnClickListener(view -> {
            new Thread(() -> {
                recoVoice();
            }).start();
        });
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
                lightLevel = 10;
                ServerThread.sendMsg(String.valueOf(lightLevel));
                runOnUiThread(() -> {
                    tv_msg.append("set brightness " + String.valueOf(lightLevel) + "\n");
                });
                break;
            }
            case "turn off the light": {
                lightLevel = 0;
                ServerThread.sendMsg(String.valueOf(lightLevel));
                int finalBrightness = brightness;
                runOnUiThread(() -> {
                    tv_msg.append("set brightness " + String.valueOf(lightLevel) + "\n");
                });
                break;
            }
            case "help me turn the light up": {
                lightLevel = Math.round((brightness / 65535) * 10);
                lightLevel += 2;
                if (lightLevel > 10) lightLevel = 10;
                ServerThread.sendMsg(String.valueOf(lightLevel));
                runOnUiThread(() -> {
                    tv_msg.append("set brightness " + String.valueOf(lightLevel) + "\n");
                });
                break;
            }
            case "help me turn the light dim": {
                lightLevel = Math.round((brightness / 65535) * 10);
                lightLevel -= 2;
                if (lightLevel < 0) lightLevel = 0;
                ServerThread.sendMsg(String.valueOf(lightLevel));
                runOnUiThread(() -> {
                    tv_msg.append("set brightness " + String.valueOf(lightLevel) + "\n");
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
        return Integer.valueOf(dataStr);
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