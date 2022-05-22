package com.example.remotepetjava;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ToggleButton;

public class AutoFeed extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_auto_feed);

        //Toma la información respecto a la cantidad de comida
        EditText kibbleAmount = (EditText)  findViewById(R.id.editTextKibble);
        EditText waterVolume = (EditText)  findViewById(R.id.editTextWater);

        //Botón para autofeed
        ToggleButton toggleAuto = (ToggleButton) findViewById(R.id.toggleAuto);
        toggleAuto.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    MainActivity.webSocketClient.send("AUTO:1"+":"+kibbleAmount.getText()+":"+waterVolume.getText());
                } else {
                    MainActivity.webSocketClient.send("AUTO:0");
                }
            }});

    }


}