package com.staler2019.zenbo_fp_bluetoothmcsazureservice;

import java.util.UUID;

public class Constant {

    // azure cognitive service: speech
    public static String speechSubscriptionKey = "df69eea892874d1d8a82777f9867962c";
    public static String serviceRegion = "westus";

    // LUIS
    public static String appId = "98489494-9d78-4b08-978f-ee700751e55d";

//    public static int SERVER_PORT = 12345;
    // MCS
    public static String MCS_SERVER = "59.124.152.117:3000";
    public static String MCS_DEVICE_ID = "ry8IekG3Y";
    public static String MCS_DEVICE_KEY = "f2a8a1ec59aa6b86ad7b375a28c05ed5de64c554569e71f89b4dc5ffc0397971";
    // MCS DATA_CHANNEL ID
    public static String LIGHT_CHANNEL_ID = "light";
    // BT server
    public static String BT_SERVER_NAME = "ZENBO";
    public static UUID BT_SERVER_UUID = UUID.fromString("b47d6b88-ccdd-48d2-aba6-bbebcfb450a9"); // get from https://www.uuidgenerator.net/
    public static UUID BT_CLIENT_UUID = UUID.fromString("4e38e0c3-ab04-4c5d-b54a-852900379bb3"); // get from https://www.uuidgenerator.net/
    public static String BT_CLIENT_MAC = "3C:0D:41:F9:65:9C";
//    public static String BT_CLIENT_MAC = "34:41:5D:D8:8F:87";
}
