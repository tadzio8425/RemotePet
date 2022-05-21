package com.example.remotepetjava;

import androidx.appcompat.app.AppCompatActivity;

import android.app.TimePickerDialog;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TimePicker;

import com.google.android.material.slider.Slider;

import java.util.Locale;

public class TimerFeedActivity extends AppCompatActivity {

    int hour;
    int minute;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_timer_feed);
    }

    public void popTimePicker(View view){

        TimePickerDialog.OnTimeSetListener onTimeSetListener = new TimePickerDialog.OnTimeSetListener() {
            @Override
            public void onTimeSet(TimePicker timePicker, int selectedHour, int selectedMinute) {
                hour = selectedHour;
                minute = selectedMinute;
                Button timeButton = (Button) findViewById(R.id.setStartTime);
                timeButton.setText(String.format(Locale.getDefault(), "%02d:%02d", hour, minute));

            }
        };

        TimePickerDialog timePickerDialog = new TimePickerDialog(this, onTimeSetListener, hour, minute, true);
        timePickerDialog.setTitle("Select Time");
        timePickerDialog.show();
    }

    public void setFeedTimeInterval(View view){
        Slider hourly_slider = (Slider) findViewById(R.id.slider);
        float hourly_interval = hourly_slider.getValue();

        MainActivity.webSocketClient.send("INTERVAL: " + hour + " " + minute + " "+ hourly_interval);

    };
}