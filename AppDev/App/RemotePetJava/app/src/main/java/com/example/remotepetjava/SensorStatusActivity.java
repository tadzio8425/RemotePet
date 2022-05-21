package com.example.remotepetjava;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.widget.ProgressBar;
import android.widget.TextView;

import org.json.JSONException;
import org.json.JSONObject;

public class SensorStatusActivity extends AppCompatActivity {

    JSONObject root_json = SocketClient.getJSON();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sensor_status);
    }

    @Override
    protected void onResume() {
        super.onResume();
        final Handler ha=new Handler();
        ha.postDelayed(new Runnable() {

            @Override
            public void run() {
                //call function
                root_json = SocketClient.getJSON();
                updateSensorValues();
                ha.postDelayed(this, 1);
            }
        }, 1);
    }



    void updateSensorValues() {

        //Se encuentra el elemento en el app de cada sensor
        ProgressBar lightView = (ProgressBar) findViewById(R.id.light_bar);
        TextView lightPercentageView = (TextView) findViewById(R.id.progress_percentage);

        ProgressBar waterView = (ProgressBar) findViewById(R.id.water_bar);
        TextView waterPercentageView = (TextView) findViewById(R.id.water_progress_percentage);

        ProgressBar foodView = (ProgressBar) findViewById(R.id.food_bar);
        TextView foodPercentageView = (TextView) findViewById(R.id.food_progress_percentage);

        ProgressBar roomTemp = (ProgressBar) findViewById(R.id.ambTemp_bar);
        TextView roomPercentageView = (TextView) findViewById(R.id.ambTemp_progress_percentage);


        //Desempaquetamiento de los valores de los sensores
        int light_sensor = 0;
        int water_sensor = 0;
        int weight_sensor = 0;
        int room_temp_sensor = 0;

        try {
            JSONObject sensors_json = root_json.getJSONObject("Sensors");
            light_sensor = stringToInt(sensors_json.getString("Light"));
            weight_sensor = stringToInt(sensors_json.getString("Weight"));
            water_sensor = stringToInt(sensors_json.getString("Water"));
            room_temp_sensor = stringToInt(sensors_json.getString("AmbTemp"));

        } catch (Exception e) {
            e.printStackTrace();
        }

        //Se cambia el valor en el app
        lightView.setProgress(light_sensor);
        lightPercentageView.setText((lightView.getProgress()) + "%");

        waterView.setMax(600);
        waterView.setProgress(water_sensor);
        waterPercentageView.setText((waterView.getProgress()) + "mL");

        foodView.setProgress(weight_sensor);
        foodPercentageView.setText(foodView.getProgress() + "g");

        roomTemp.setProgress(room_temp_sensor);
        roomPercentageView.setText(roomTemp.getProgress() + "C°");




    }

    int stringToInt(String s){
        return (int) Double.parseDouble(s);
    }

}