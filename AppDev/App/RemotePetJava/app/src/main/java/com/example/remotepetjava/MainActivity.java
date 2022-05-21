package com.example.remotepetjava;

import androidx.appcompat.app.AppCompatActivity;

import android.app.TimePickerDialog;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ProgressBar;
import android.widget.TimePicker;

import org.json.JSONObject;

import java.net.URI;
import java.net.URISyntaxException;
import java.sql.Time;
import java.util.Locale;

import tech.gusavila92.websocketclient.WebSocketClient;


public class MainActivity extends AppCompatActivity {

    public static SocketClient webSocketClient;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        createWebSocketClient();

        updateAlerts(null);
    }

    public void updateAlerts(View view) {

        webSocketClient.send("Ping?");

        ImageButton on_off_button = (ImageButton) findViewById(R.id.on_off_button);



        if(SocketClient.isConnected){
            on_off_button.setImageResource(R.mipmap.wifi_connection_img_foreground);
        }

        else{
            on_off_button.setImageResource(R.mipmap.wifi_no_connection_img_foreground);
        }


    }


    public void openCamera(View view){
        Intent intent = new Intent(this, CameraView.class);
        startActivity(intent);
    }

    public void openSensorActivity(View view){

        Intent intent = new Intent(this, SensorStatusActivity.class);
        startActivity(intent);

    }

    public void openTimerFeed(View view){
        Intent intent = new Intent(this, TimerFeedActivity.class);
        startActivity(intent);
    }

    public void createWebSocketClient(){

        URI uri;
        try {
            // Connect to local host
            uri = new URI("ws://192.168.4.1:80/websocket");
        }
        catch (URISyntaxException e) {
            e.printStackTrace();
            return;
        }

        webSocketClient = new SocketClient(uri);
        webSocketClient.setConnectTimeout(1000);
        webSocketClient.setReadTimeout(5000);
        webSocketClient.enableAutomaticReconnection(5000);
        webSocketClient.connect();
    }




}