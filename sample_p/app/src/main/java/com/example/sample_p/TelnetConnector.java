package com.example.sample_p;

import android.os.AsyncTask;

import java.net.UnknownHostException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

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
    protected void onPreExecute() {
        super.onPreExecute();
        // doInBackground前処理
    }

    @Override
    protected String doInBackground(Void... params) {
        Socket socket = null;
        OutputStream os = null;
        BufferedInputStream is = null;
        String host = "192.168.1.99";
        int port = 23456;
        String returnString = null;

        try {
            socket = new Socket(host, port);
            os = socket.getOutputStream();
            is = new BufferedInputStream(socket.getInputStream());

            // 23番ポートへの接続の場合ネゴシエーションを行う
            if (port == DEFAULT_TELNET_PORT) {
                negotiation(os, is);
            }

            StreamConnector stdinToSocket = new StreamConnector(System.in, os);
            StreamConnector socketToStdout = new StreamConnector(is, System.out);

            Thread input = new Thread(stdinToSocket);
            Thread output = new Thread(socketToStdout);

            input.start();
            output.start();
//            String str = "ls -l\r\n";
//            byte[] outputInBytes = str.getBytes(StandardCharsets.UTF_8);
//            System.in.read(outputInBytes, 0, 7);
            returnString = "OK!";
        } catch (UnknownHostException e) {
            e.printStackTrace();
            returnString = "ERROR!";
        } catch (IOException e) {
            e.printStackTrace();
            returnString = "ERROR!";
        }
        return returnString;
    }

    /**
     * ストリームの読み書きクラス
     */
    class StreamConnector implements Runnable {

        private InputStream is = null;
        private OutputStream os = null;

        public StreamConnector(InputStream in, OutputStream out) {
            this.is = in;
            this.os = out;
        }

        @Override
        public void run() {
            byte[] buff = new byte[1024];
            try {
                while (true) {
                    int n = is.read(buff);
                    if (n > 0) {
                        os.write(buff, 0, n);
                    }
                }
            } catch (IOException e) {
                e.printStackTrace(System.out);
                System.out.println("入出力の書き込み中に例外が発生しました");
                System.exit(1);
            }
        }
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