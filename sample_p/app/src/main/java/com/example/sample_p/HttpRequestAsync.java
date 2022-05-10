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
        String mstdnURL = "http://192.168.42.1/mjpeg/image.jpg";
        String returnString = null;

        try{
            url = new URL(mstdnURL);
            con = (HttpURLConnection)url.openConnection();
            con.setRequestMethod("POST");
            con.setInstanceFollowRedirects(false);
            con.setDoInput(true);
            con.setDoOutput(true);

            // https://stackoverflow.com/questions/20020902/android-httpurlconnection-how-to-set-post-data-in-http-body/20021028
            String ACCESS_TOKEN = "ここにアクセストークンを入れる";

            String TOOT = "#にょ by app \n"+getNowDate();

            //header
            con.setRequestProperty("Authorization", "Bearer "+ACCESS_TOKEN);

            //body
            String str = String.format("status=%s&visibility=unlisted", TOOT);
            byte[] outputInBytes = str.getBytes(StandardCharsets.UTF_8);
            OutputStream os = con.getOutputStream();
            os.write( outputInBytes );
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

                    InputStream in = con.getInputStream();
                    BufferedReader reader = new BufferedReader(new InputStreamReader(in));
                    String line;
                    String readStr = new String();

                    while (null != (line = reader.readLine())){
                        readStr += line;
                    }

                    Log.d("Error", readStr);
                    in.close();
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