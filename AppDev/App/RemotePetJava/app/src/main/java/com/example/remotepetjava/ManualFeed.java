package com.example.remotepetjava;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.CompoundButton;
import android.widget.ToggleButton;

public class ManualFeed extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_manual_feed);

        //Botón para soltar agua
        ToggleButton toggleWater = (ToggleButton) findViewById(R.id.toggleWater);
        toggleWater.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    MainActivity.webSocketClient.send("WATER:1");
                } else {
                    MainActivity.webSocketClient.send("WATER:0");
                }
            }});

    //Botón para soltar comida
    ToggleButton toggleKibble = (ToggleButton) findViewById(R.id.toggleKibble);
        toggleKibble.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
            if (isChecked) {
                MainActivity.webSocketClient.send("KIBBLE:1");
            } else {
                MainActivity.webSocketClient.send("KIBBLE:0");
            }
        }
    });

    }}