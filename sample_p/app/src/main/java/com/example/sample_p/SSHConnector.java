package com.example.sample_p;

import android.os.AsyncTask;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.Properties;
import com.jcraft.jsch.ChannelExec;
import com.jcraft.jsch.JSch;
import com.jcraft.jsch.JSchException;
import com.jcraft.jsch.Session;

/**
 * SSH接続用クラス
 */
public class SSHConnector extends AsyncTask<Void, Void, String> {
	private Session session;
	private ChannelExec channel;
	MainActivity main_ = null;
	String ret = null;

	SSHConnector(MainActivity mainActivity){main_ = mainActivity;}

	@Override
	protected String doInBackground(Void... params) {
		try {
			JSch jsch = new JSch();
			session = jsch.getSession(main_.strUser, main_.strHost, main_.nPort);
			session.setPassword(main_.strPass);
			Properties prop = new Properties();
			prop.put("StrictHostKeyChecking", "no");
			session.setConfig(prop);
			session.connect();
			// コマンド送信
			channel = (ChannelExec)session.openChannel("exec");
			channel.setCommand(main_.strCommand);
			channel.setErrStream(System.err);
			channel.connect();
			// 受信処理
			BufferedReader reader = new BufferedReader(new InputStreamReader(channel.getInputStream()));
			String line;
			while ((line = reader.readLine()) != null) {
				System.out.println(line);
			}
			channel.disconnect();
			session.disconnect();
			int returnCode = channel.getExitStatus();	// コマンドの戻り値を取得する
			System.out.println(returnCode);
			ret = "OK!";
		} catch (JSchException e) {
			e.printStackTrace();
			ret = "error!";
		} catch (Exception e) {
			e.printStackTrace();
			ret = "error!";
		}
		return ret;
	}

	@Override
	protected void onPostExecute(String result) {
		super.onPostExecute(result);
		// doInBackground後処理
		main_.showToast(result);
	}
}