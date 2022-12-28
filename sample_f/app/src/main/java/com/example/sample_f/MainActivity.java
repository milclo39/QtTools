package com.example.sample_f;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import org.apache.ftpserver.ConnectionConfigFactory;
import org.apache.ftpserver.FtpServer;
import org.apache.ftpserver.FtpServerConfigurationException;
import org.apache.ftpserver.FtpServerFactory;
import org.apache.ftpserver.ftplet.Authority;
import org.apache.ftpserver.ftplet.FtpException;
import org.apache.ftpserver.ftplet.UserManager;
import org.apache.ftpserver.listener.ListenerFactory;
import org.apache.ftpserver.usermanager.PropertiesUserManagerFactory;
import org.apache.ftpserver.usermanager.SaltedPasswordEncryptor;
import org.apache.ftpserver.usermanager.impl.BaseUser;
import org.apache.ftpserver.usermanager.impl.WritePermission;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity {
    FtpServer server = null;
    boolean isAnonymous = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button btnFtp = findViewById(R.id.btnFtp);
        btnFtp.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                FtpServerFactory serverFactory = new FtpServerFactory();
                ListenerFactory listenerFactory = new ListenerFactory();
                listenerFactory.setPort(55555);
                serverFactory.addListener("default", listenerFactory.createListener());
                // 接続設定
                ConnectionConfigFactory connectionConfigFactory = new ConnectionConfigFactory();
                connectionConfigFactory.setAnonymousLoginEnabled(isAnonymous);
                serverFactory.setConnectionConfig(connectionConfigFactory.createConnectionConfig());
                // ユーザ管理
                PropertiesUserManagerFactory userManagerFactory = new PropertiesUserManagerFactory();
                userManagerFactory.setPasswordEncryptor(new SaltedPasswordEncryptor());
                UserManager userManager = userManagerFactory.createUserManager();
                // 認証設定
                BaseUser user = new BaseUser();
                if(isAnonymous){
                    user.setName("anonymous");
                }
                else {
                    user.setName("user");
                    user.setPassword("password");
                }
                user.setHomeDirectory("/sdcard");
                // 書込み許可
                List<Authority> authorities = new ArrayList<>();
                Authority authority = new WritePermission();
                authorities.add(authority);
                user.setAuthorities(authorities);

                try {
                    userManager.save(user);
                    serverFactory.setUserManager(userManager);
                    server = serverFactory.createServer();
                    server.start();
                    showToast("Start Server.");
                } catch (FtpServerConfigurationException | FtpException e) {
                    e.printStackTrace();
                }
                finish();
            }
        });
    }

    public void showToast (String result){
        Toast.makeText(this, result, Toast.LENGTH_LONG).show();
    }
}
