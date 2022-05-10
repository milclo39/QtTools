package com.example.sample_p;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {
    HttpRequestAsync http = null;
    TelnetConnector telnet = null;
    SSHConnector ssh = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button btnHttp = findViewById(R.id.btnHttp);
        btnHttp.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                setHttpAsync();
                http.execute();
            }
        });
        Button btnTelnet = findViewById(R.id.btnTelnet);
        btnTelnet.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                setTelnetAsync();
                telnet.execute();
            }
        });
        Button btnSSH = findViewById(R.id.btnSSH);
        btnSSH.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                try {
                    ssh = new SSHConnector("192.168.1.99","pino","",22);
                    ssh.execute("test");
                } catch (Exception e) {
                }
            }
        });
    }

    private void setHttpAsync(){
        http = new HttpRequestAsync(this);
    }
    private void setTelnetAsync(){
        telnet = new TelnetConnector(this);
    }

    public void showToast (String result){
        Toast.makeText(this, result, Toast.LENGTH_LONG).show();
    }
}
