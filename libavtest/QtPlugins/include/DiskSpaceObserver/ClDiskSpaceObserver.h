/*--------------------------------------------------------------------------------*/
/*!
	@file	ClDiskSpaceObserver.h
	@brief	HDD容量表示プラグインのベースクラス
 */
/*--------------------------------------------------------------------------------*/
#ifndef CLDiskSpaceObserver_H
#define CLDiskSpaceObserver_H


#include "ClDiskSpaceGetThread.h"	//容量取得スレッド
#include <QWidget>					//継承元




/*================================================================================*/
/*--------------------------------------------------------------------------------*/
/*!
	@class	ClDiskSpaceObserver
	@brief	HDD容量表示プラグインのベースクラス
 */
/*--------------------------------------------------------------------------------*/
class ClDiskSpaceObserver : public QWidget
{
	Q_OBJECT


	/*----------------------------------------*/
	/*!
		@property	warningRatio
		@brief		警告SIGNALを発行するHDD使用率(単位：%)
		@note		0の場合は警告が発行されない
	*/
	/*----------------------------------------*/
	Q_PROPERTY(unsigned char m_ucWarningRatio READ warningRatio WRITE setWarningRatio)
private:
	unsigned char	m_ucWarningRatio;
public:
	unsigned char	warningRatio(){ return m_ucWarningRatio; }
	void			setWarningRatio(unsigned char in_ucWarningRatio){ m_ucWarningRatio = in_ucWarningRatio; }

	/*----------------------------------------*/
	/*!
		@property	targetPath
		@brief		監視対象のドライブ名を含むパス
	*/
	/*----------------------------------------*/
	Q_PROPERTY(QString m_qstrTargetPath READ targetPath WRITE setTargetPath)
private:
	QString	m_qstrTargetPath;
public:
	QString	targetPath(){ return m_qstrTargetPath; }
	void	setTargetPath(QString in_qstrTargetPath)
	{
		m_qstrTargetPath = in_qstrTargetPath;
		m_qstrTargetPath.replace("\\", "/");
	}

	/*----------------------------------------*/
	/*!
		@property	checkInterval_ms
		@brief		監視間隔
	*/
	/*----------------------------------------*/
	Q_PROPERTY(unsigned long m_ulCheckInterval READ checkInterval_ms WRITE setCheckInterval_ms)
private:
	unsigned long	m_ulCheckInterval_ms;
public:
	unsigned long	checkInterval_ms(){ return m_ulCheckInterval_ms; }
	void			setCheckInterval_ms(unsigned long in_ulCheckInterval_ms){ m_ulCheckInterval_ms = in_ulCheckInterval_ms; }


	/*----------------------------------------*/
	/*
		メンバー変数定義
	*/
	/*----------------------------------------*/
private:
	ClDiskSpaceGetThread	m_clDiskSpaceGetThread;	//!< ディスク使用容量を監視するスレッド
	bool					m_bIsSendWarning;		//!< 警告Signalが発行済みかどうかフラグ
	QTimer*					m_pclTimerResize;		//!< メーター長更新用タイマー
	bool					m_bIsStopped;			//!< スレッド停止フラグ、諸事情で必要


	/*----------------------------------------*/
	/*
		メンバー関数定義
	*/
	/*----------------------------------------*/
public:
	ClDiskSpaceObserver(QWidget* in_pclParent = 0);	//コンストラクタ
	virtual ~ClDiskSpaceObserver();					//デストラクタ

	bool	bIsCapWarning();						//現在のディスク容量が警告状態かどうかを返す

	/*----------------------------------------*/
	/*
		slot
	*/
	/*----------------------------------------*/
public slots:
	void	slotStart();						//ディスク容量監視開始
	void	slotStop();							//ディスク容量監視終了
	void	slotUsedRatio(char ucUsedRatio);	//ディスクの使用率を受け取る

	/*----------------------------------------*/
	/*
		signal
	*/
	/*----------------------------------------*/
signals:
	void	sigUsedRatio(char cUsedRatio);	//!< ディスク使用率を取得するたびに発行する
	void	sigCapWarning();				//!< ディスク使用容量が設定された割合を超えたときに発行される
	void	sigFailedGetUsedRatio();		//!< ディスク使用率取得失敗を通知する
};
/*================================================================================*/

#endif // CLDiskSpaceObserver_H
