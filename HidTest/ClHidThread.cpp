#include "ClHidThread.h"
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include "hidapi.h"
#include <QThread>
#include <QDebug>
#include <QElapsedTimer>

#define MAX_STR 255

ClHidThread::ClHidThread()
{
	_bActive = true;
	_thread = new QThread;
	moveToThread(_thread);
	connect(_thread, SIGNAL(started()), this, SLOT(procThread()));
	connect(_thread, SIGNAL(finished()), this, SLOT(deleteLater()));
	_thread->start();
}

ClHidThread::~ClHidThread()
{
	_bActive = false;
	_thread->terminate();
	_thread = NULL;
}

void ClHidThread::procThread()
{
	int res = 0;
	unsigned char buf[256];
	hid_device *handle;
//	unsigned short vid, pid;
	struct hid_device_info *devs, *cur_dev;
INIT:
	devs = hid_enumerate(0x0, 0x0);
	cur_dev = devs;
	for(int i = 0; i < 10; i++){
		if(!cur_dev){
			break;
		}
#if 0
		qDebug("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls",
			cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
		qDebug("  Manufacturer: %ls", cur_dev->manufacturer_string);
		qDebug("  Product:      %ls", cur_dev->product_string);
		qDebug("  Release:      %hx", cur_dev->release_number);
		qDebug("  Interface:    %d",  cur_dev->interface_number);
#endif
		if(cur_dev->interface_number == 0){
			m_iVid = cur_dev->vendor_id;
			m_iPid = cur_dev->product_id;
		}
		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);
//	m_iVid = 0x09a1;
//	m_iPid = 0x0400;
	handle = hid_open(m_iVid, m_iPid, NULL);
	if (!handle) {
		qDebug("unable to open device");
		if(_bActive){
			QThread::sleep(1);
			goto INIT;
		}
		return;
	}
#if 0	// open対象の情報取得
	wchar_t wstr[MAX_STR];
	wstr[0] = 0x0000;
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	if (res < 0){
		qDebug("Unable to read manufacturer string");
	}
	qDebug("Manufacturer String: %ls", wstr);

	wstr[0] = 0x0000;
	res = hid_get_product_string(handle, wstr, MAX_STR);
	if (res < 0){
		qDebug("Unable to read product string");
	}
	qDebug("Product String: %ls", wstr);

	wstr[0] = 0x0000;
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	if (res < 0){
		qDebug("Unable to read serial number string");
	}
	qDebug("Serial Number String: (%d) %ls", wstr[0], wstr);

	wstr[0] = 0x0000;
	res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	if (res < 0){
		qDebug("Unable to read indexed string 1");
	}
	qDebug("Indexed String 1: %ls", wstr);
#endif
	hid_set_nonblocking(handle, 1);

	res = hid_read(handle, buf, 17);	// 事前空読み

	res = 0;
	qDebug("thread start");
#if 0
	int px = 0,py = 0;
	QElapsedTimer et;
	et.start();
#endif
	while(_bActive){
		res = hid_read(handle, buf, sizeof(buf));
		if (res < 0){
			qDebug("Unable to read");
			hid_close(handle);
			hid_exit();
			goto INIT;
		}
		if (res){
#if 0
			qint32 x,y,z;
			if(vid == 0x09a1){	//elmo
				x = (buf[3] << 8) | buf[2];
				y = (buf[5] << 8) | buf[4];
				z = (buf[7] << 8) | buf[6];
			}
			else{	//wacom
				x = (buf[3] << 8) | buf[4];
				y = (buf[5] << 8) | buf[6];
				z = buf[7];
			}
			qDebug() << et.elapsed() << "X:" << x << "Y:" << y << "Z:" << z
					 << "Dist:" << (int)sqrt(pow(px - x, 2) + pow(py - y, 2));
			px = x;
			py = y;
#endif
			emit sigRecv(QByteArray((char*)buf, res));
		}
		else{
			QThread::msleep(10);
		}
	}
	hid_close(handle);
	hid_exit();
	qDebug("thread end");
}

bool ClHidThread::isRunning()
{
	return _thread->isRunning();
}
