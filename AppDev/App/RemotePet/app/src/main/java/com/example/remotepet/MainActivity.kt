package com.example.remotepet

import android.content.Intent
import android.os.Bundle
import android.view.View
import androidx.appcompat.app.AppCompatActivity


class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
    }

    fun openSensorActivity(view: View) : Void? {
        val intent = Intent(this, SensorStatus::class.java)
        startActivity(intent)
        return null
    }
}




