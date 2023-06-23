//--------------------------------------------------------------------------------//
/*!
	@file	ClImageManager.h
	@brief	画像管理クラスヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLIMAGEMANAGER_H
#define CLIMAGEMANAGER_H
#include <QObject>
#include <QImage>

//--------------------------------------------------------------------------------//
/*!
	@brief	画像管理クラス
*/
//--------------------------------------------------------------------------------//
class ClImageManager : public QObject
{
	Q_OBJECT

public:
	ClImageManager(QString strFileName = "", QObject *pclParent = NULL);	//コンストラクタ
	virtual ~ClImageManager();	//デストラクタ

	bool setImage(QString strFileName);		//画像設定
	const QImage *getImage(qint32 lNumber);	//画像取得
	const QImage *getCurrentImage();		//現在の画像
	bool setImageNumber(qint32 lNumber);	//画像番号設定
	qint32 getImageCount();					//画像枚数取得

	void startAnimation();	//アニメーション開始
	void stopAnimation();	//アニメーション停止

	//内部用
	/*!
		@brief	表示時間リスト取得
		@retval	表示時間リスト
	*/
	QList<int> getTimeList(){return m_clDelays;}
	void updateState(qint32 lImageNumber);	//状態更新

signals:
	/*!
		@brief	更新通知
	*/
	void sigUpdate();

protected:
	QList<QImage> m_clImages;		//!< 画像リスト
	QList<int> m_clDelays;			//!< 画像表示時間リスト
	qint32 m_lCurrentImageNumber;	//!< 現在の画像番号
};
//--------------------------------------------------------------------------------//
#endif	//CLIMAGEMANAGER_H
