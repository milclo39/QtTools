package com.example.sample_p;

import android.os.AsyncTask;

import java.net.UnknownHostException;
import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

import org.apache.commons.net.telnet.TelnetClient;

/**
 * Telnet接続用クラス
 */
public class TelnetConnector extends AsyncTask<Void, Void, String> {
    private final static int DEFAULT_TELNET_PORT = 23;

    private final static byte IAC = (byte) 255;
    private final static byte DONT = (byte) 254;
    private final static byte DO = (byte) 253;
    private final static byte WONT = (byte) 252;
    private final static byte WILL = (byte) 251;
    MainActivity main_ = null;

    TelnetConnector(MainActivity mainActivity){
        main_ = mainActivity;
    }
    /**
     * NVTとしてのネゴシエーションを行う
     * @param os
     * @param is
     * @throws IOException
     */
    private void negotiation(OutputStream os, BufferedInputStream is) throws IOException {
        byte[] buff = new byte[3];
        while (true) {
            is.mark(buff.length);
            if (is.available() >= buff.length) {
                is.read(buff);
                // ネゴシエーション完了
                if (buff[0] != IAC) {
                    is.reset();
                    return;
                    // DOコマンドに対してWONTで返答する
                } else if (buff[1] == DO) {
                    buff[1] = WONT;
                    os.write(buff);
                }
            }
        }
    }

    @Override
    protected String doInBackground(Void... params) {
        TelnetClient socket = null;
        OutputStream os = null;
        BufferedInputStream is = null;
        String host = "192.168.1.99";
        int port = 23456;
        String returnString = null;

        try {
//            socket = new Socket(host, port);
//            os = socket.getOutputStream();
//            is = new BufferedInputStream(socket.getInputStream());

            // 23番ポートへの接続の場合ネゴシエーションを行う
//            if (port == DEFAULT_TELNET_PORT) {
//                negotiation(os, is);
//            }
            socket = new TelnetClient();
            socket.connect(host,port);

            // コマンド送信
            String str = "ifconfig"+"\n";
            socket.getOutputStream().write(str.getBytes());
            // 受信処理
//            BufferedReader bufreader = new BufferedReader(reader);
//            String line;
//            while ((line = bufreader.readLine()) != null) {
//                System.out.println(line);
//            }
            int n;
            while((n = socket.getInputStream().available()) > 0){
                byte[] buf = new byte[n];
                socket.getInputStream().read(buf, 0, n);
                String str2 = new String(buf);
                System.out.println(str2);
                Thread.sleep(100);
            }
            returnString = "OK!";
        } catch (UnknownHostException e) {
            e.printStackTrace();
            returnString = "ERROR!";
        } catch (IOException e) {
            e.printStackTrace();
            returnString = "ERROR!";
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        return returnString;
    }

    @Override
    protected void onPostExecute(String result) {
        super.onPostExecute(result);
        // doInBackground後処理
        main_.showToast(result);
    }
}