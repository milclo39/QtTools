#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <string.h>
#include "ClUvcManager.h"
#ifdef USE_XUCTRLLIB
#include "XuControlLib.h"
#else
#include <QCameraInfo>
#endif
#include <DShow.h>
#include <vidcap.h>
#include <Ks.h>
#include <KsMedia.h>
#include <KsProxy.h>
#include <QDebug>

using namespace std;

HWND g_hWndReceiver = NULL;	//イベント受信用ウィンドウハンドル
ClUvcManager *g_pclUvcManager = NULL;	//Uvc通信クラス
#ifdef USE_XUCTRLLIB
XUCTRL g_ctrl = NULL;
#endif
//--------------------------------------------------------------------------------//
/*!
	@brief		ウィンドウプロシージャ
	@param[in]	hWnd : メッセージを受けたウィンドウハンドル
	@param[in]	message : メッセージ
	@param[in]	wPram : WPARAM
	@param[in]	lParam : LPARAM
*/
//--------------------------------------------------------------------------------//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_DEVICECHANGE)
	{
		if(DBT_DEVNODES_CHANGED == wParam)
		{
			g_pclUvcManager->deviceChangeCheck();
		}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		イベント受信ウィンドウ生成
	@retval		成否
*/
//--------------------------------------------------------------------------------//
bool createEventReceiveWindow()
{
	if(g_hWndReceiver != NULL)
	{
		return true;
	}

	//ウィンドウクラス登録
	WNDCLASS wc;
	wc.style = NULL;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"EaventProcReceiver";
	RegisterClass(&wc);
	//ウィンドウ生成
	g_hWndReceiver = CreateWindow(wc.lpszClassName, L"", WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);
	if(g_hWndReceiver == NULL)
	{
		qDebug("CreateWindow error:%d", GetLastError());
		return false;	//ウィンドウ生成失敗
	}
	return true;
}
//--------------------------------------------------------------------------------//




ClUvcManager::ClUvcManager()
{
	g_pclUvcManager = this;
	m_bIsDeviceEnabled = false;
}

ClUvcManager::~ClUvcManager()
{

}

kTransferResult ClUvcManager::Init(const char*)
{
	if(createEventReceiveWindow() == false)	//デバイス変化通知受信ウィンドウ生成
	{
		return kErrorCritical;
	}
#ifndef USE_XUCTRLLIB
#if TARGET_MODEL == 0
	m_pUvcDriver = new ClUvcDriverTxLx1;
#else
	m_pUvcDriver = new ClUvcDriverMx1;
#endif
#endif
	bool bIsInit = deviceChangeCheck();

	if(false == bIsInit){
		return kErrorCritical;
	}

	return kSuccess;
}


bool ClUvcManager::deviceChangeCheck()
{
#ifdef USE_XUCTRLLIB
	HRESULT hr;
	ICreateDevEnum *pCreateDevEnum = NULL;
	IEnumMoniker *pEm = NULL;
	IMoniker *pM = NULL;
	ULONG cFetched;
	IBaseFilter *filter = NULL;
	XuCtrlStatus_t st = XUCTRL_ST_DEVICE_NOT_FOUND;

	// システムデバイス列挙子を生成
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pCreateDevEnum);
	if(hr != S_OK){
		qDebug(("CoCreateInstance failed: 0x%08X\n"), hr);
		goto Exit;
	}

	// ビデオキャプチャカテゴリの列挙子を生成
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);
	if (hr != S_OK) {
		if (hr == S_FALSE) {
			qDebug("video input device not found\n");
		} else {
			qDebug("CreateClassEnumerator failed: 0x%08X\n", hr);
		}
		goto Exit;
	}

	pEm->Reset();

	// モニカを列挙
	while (pEm->Next(1, &pM, &cFetched) == S_OK) {
		// キャプチャフィルタを生成
		hr = pM->BindToObject(0, 0, IID_IBaseFilter, (void**)&filter);
		if (hr != S_OK) {
			goto Next;
		}

		// キャプチャフィルタからXUコントロールを取得
		st = XuCtrl_GetCtrlByCaptureFilter(filter, &g_ctrl, &hr);

Next:
		if (filter != NULL) {
			filter->Release();
			filter = NULL;
		}
		if (pM != NULL) {
			pM->Release();
			pM = NULL;
		}

		// XUコントロールを取得できたのでループを抜ける
		if (st == XUCTRL_ST_OK) {
			break;
		}
	}

Exit:

	if (pEm != NULL){
		pEm->Release();
	}

	if (pCreateDevEnum != NULL){
		pCreateDevEnum->Release();
	}

	if(st == XUCTRL_ST_OK){
		emit sigChangedDevice(true);
	}else{
		emit sigChangedDevice(false);
	}

	return (st == XUCTRL_ST_OK) ? true : false;
#else
	if(false == m_bIsDeviceEnabled){
		foreach (const QCameraInfo &cameraInfo, QCameraInfo::availableCameras()){
			QString deviceName = cameraInfo.deviceName();
			deviceName.replace("@device:pnp:","");
			qDebug() << "deviceName" << deviceName;
			m_pUvcDriver->setDevicePath(deviceName);
			break;
		}
	}
	if(m_pUvcDriver->searchDevice()){
		emit sigChangedDevice(true);
		m_bIsDeviceEnabled = true;
	}
	else{
		emit sigChangedDevice(false);
		m_pUvcDriver->setDevicePath("");
		m_bIsDeviceEnabled = false;
	}
	return m_bIsDeviceEnabled;
#endif
}

QByteArray ClUvcManager::sendData(QByteArray qArray)
{
#ifdef USE_XUCTRLLIB
	UCHAR result[XUCTRL_DATA_LEN_MAX];
	INT resultSize = XUCTRL_DATA_LEN_MAX;
	XuCtrlStatus_t st;
	int retry = 10;
	QByteArray recvData = NULL;

	// リクエスト送信
	st = XuCtrl_Request(g_ctrl, (UCHAR*)qArray.data(), qArray.size(), NULL);
	if (st != XUCTRL_ST_OK) {
		qDebug(("XuCtrl_Request failed: %d\n"), st);
		return recvData;
	}

	do {
		// リクエスト結果取得
		st = XuCtrl_Result(g_ctrl, result, &resultSize, NULL);
		if (st == XUCTRL_ST_OK)
			break;
		if (st == XUCTRL_ST_DEVICE_IN_PROGRESS) {
			// リクエスト処理中
			Sleep(5);
			continue;
		}
		qDebug(("XuCtrl_Result failed: %d\n"), st);
		break;
	} while (--retry > 0);

	recvData = QByteArray((char*)result, resultSize);
#else
	QString clCmd(qArray);
#if TARGET_MODEL == 0
	ClUvcDriverTxLx1 *pUvcDriver = dynamic_cast<ClUvcDriverTxLx1*>(m_pUvcDriver);
#else
	ClUvcDriverMx1 *pUvcDriver = dynamic_cast<ClUvcDriverMx1*>(m_pUvcDriver);
#endif
	pUvcDriver->requestExtension(clCmd);
	clCmd = "dummy";
	pUvcDriver->resultExtension(clCmd);
	if("dummy" == clCmd){
		return QByteArray();
	}
	QByteArray recvData = clCmd.toLocal8Bit();
#endif
	return recvData;
}
