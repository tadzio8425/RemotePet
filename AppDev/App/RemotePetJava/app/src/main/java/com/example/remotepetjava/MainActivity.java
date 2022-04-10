package com.example.remotepetjava;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import org.json.JSONObject;

import java.net.URI;
import java.net.URISyntaxException;

import tech.gusavila92.websocketclient.WebSocketClient;


public class MainActivity extends AppCompatActivity {

    public static SocketClient webSocketClient;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        createWebSocketClient();
    }

    public void openSensorActivity(View view){

        Intent intent = new Intent(this, SensorStatusActivity.class);
        startActivity(intent);

    }

    public void createWebSocketClient(){

        URI uri;
        try {
            // Connect to local host
            uri = new URI("ws://192.168.1.40:80/websocket");
        }
        catch (URISyntaxException e) {
            e.printStackTrace();
            return;
        }

        webSocketClient = new SocketClient(uri);
        webSocketClient.setConnectTimeout(10000);
        webSocketClient.setReadTimeout(60000);
        webSocketClient.enableAutomaticReconnection(5000);
        webSocketClient.connect();
    }

}