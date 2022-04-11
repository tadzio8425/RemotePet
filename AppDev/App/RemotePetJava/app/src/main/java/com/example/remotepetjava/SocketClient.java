package com.example.remotepetjava;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import org.json.JSONException;
import org.json.JSONObject;

import java.net.URI;
import java.net.URISyntaxException;
import tech.gusavila92.websocketclient.WebSocketClient;


public class SocketClient extends tech.gusavila92.websocketclient.WebSocketClient {

    private String _root_json_str;
    private static JSONObject _root_json;
    public static boolean isConnected = false;

    public SocketClient(URI uri) {
        super(uri);
    }

    @Override
    public void onOpen() {
        Log.i("WebSocket", "Session is starting");
        this.send("Hello, I am the RemotePet app!");
    }

    @Override
    public void onTextReceived(String s) {

        if (s.equals("Pong.")){
            isConnected = true;}

        else{
            _root_json_str = s;
            setJSON();

            _root_json.has("Sensors");}
        }


    @Override
    public void onBinaryReceived(byte[] data) {
    }

    @Override
    public void onPingReceived(byte[] data) {
    }

    @Override
    public void onPongReceived(byte[] data) {
    }

    @Override
    public void onException(Exception e) {
        System.out.println(e.getMessage());
        isConnected = false;
    }

    @Override
    public void onCloseReceived() {
        Log.i("WebSocket", "Closed ");
        System.out.println("onCloseReceived");
    }




    public void setJSON(){

        try{_root_json = new JSONObject(_root_json_str);} catch (JSONException e) {
            e.printStackTrace();
        }

    }

    public static JSONObject getJSON(){
        return _root_json;
    }
}

