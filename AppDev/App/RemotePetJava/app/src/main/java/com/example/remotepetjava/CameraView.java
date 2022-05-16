package com.example.remotepetjava;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.webkit.WebView;

public class CameraView extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera_view);

        WebView myWebView = (WebView) findViewById(R.id.cameraWebView);
        myWebView.clearCache(true);
        myWebView.getSettings().setJavaScriptEnabled(true);
        myWebView.loadUrl("192.168.4.2:81/stream");


    }


    @Override
    public void onDestroy() {
        super.onDestroy();
        WebView myWebView = (WebView) findViewById(R.id.cameraWebView);
        myWebView.clearCache(true);
        myWebView.loadUrl("google.com");
    }


}