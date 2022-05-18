package com.example.sample_p;

import android.os.AsyncTask;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.nio.charset.StandardCharsets;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

public class HttpRequestAsync extends AsyncTask<Void, Void, String> {

    MainActivity main_ = null;

    HttpRequestAsync(MainActivity mainActivity){
        main_ = mainActivity;
    }

    @Override
    protected void onPreExecute() {
        super.onPreExecute();
        // doInBackground前処理
    }

    @Override
    protected String doInBackground(Void... params) {
        HttpURLConnection con = null;
        URL url = null;
        String mstdnURL = "http://httpbin.org/post";
        String strCommand = "{\"id\":1,\"jsonrpc\":\"2.0\",\"method\":\"getFWVersion\",\"params\":[]}";
        String returnString = null;

        try{
            url = new URL(mstdnURL);
            con = (HttpURLConnection)url.openConnection();
            con.setRequestMethod("POST");
            con.setInstanceFollowRedirects(false);
            con.setDoInput(true);
            con.setDoOutput(true);

            //header
//            con.setRequestProperty("Authorization", "Bearer nantokatoken");

            //body
            OutputStream os = con.getOutputStream();
            os.write(strCommand.getBytes(StandardCharsets.UTF_8));
            os.close();

            //接続
            con.connect();

            //レスポンス
            int status = con.getResponseCode();
            switch(status){
            case HttpURLConnection.HTTP_OK:
                returnString = "OK!";
                break;
            default:
                returnString = "ERROR!";
                break;
            }
            // 受信処理
            BufferedReader reader = new BufferedReader(new InputStreamReader(con.getInputStream()));
            String line;
            while ((line = reader.readLine()) != null) {
                System.out.println(line);
            }
        } catch (MalformedURLException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

        con.disconnect();
        return returnString;
    }

    @Override
    protected void onPostExecute(String result) {
        super.onPostExecute(result);
        // doInBackground後処理
        main_.showToast(result);
    }

    private static String getNowDate(){
        final DateFormat df = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
        final Date date = new Date(System.currentTimeMillis());
        return df.format(date);
    }
}