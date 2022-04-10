package com.example.remotepetjava;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.ProgressBar;
import android.widget.TextView;

import org.json.JSONException;
import org.json.JSONObject;

public class SensorStatusActivity extends AppCompatActivity {

    SocketClient webSocketClient = MainActivity.webSocketClient;
    JSONObject root_json = SocketClient.getJSON();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sensor_status);
        updateSensorValues();
    }

    void updateSensorValues() {

        //Se encuentra el elemento en el app de cada sensor
        ProgressBar lightView = (ProgressBar) findViewById(R.id.light_bar);
        TextView lightPercentageView = (TextView) findViewById(R.id.progress_percentage);
        //Desempaquetamiento de los valores de los sensores

        int light_sensor = 0;
        int water_sensor = 0;
        int weight_sensor = 0;

        try {
            JSONObject sensors_json = root_json.getJSONObject("Sensors");
            light_sensor = stringToInt(sensors_json.getString("Light"));
            weight_sensor = stringToInt(sensors_json.getString("Weight"));
            water_sensor = stringToInt(sensors_json.getString("Water"));

        } catch (Exception e) {
            e.printStackTrace();
        }

        //Se cambia el valor en el app
        lightView.setProgress(light_sensor);
        lightPercentageView.setText((lightView.getProgress()) + "%");

    }

    int stringToInt(String s){
        return (int) Double.parseDouble(s);
    }

}