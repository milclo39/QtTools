/*--------------------------------------------------------------------------------*/
/*!
	@file	ClDiskSpaceManager.h
	@brief	主に機種依存な部分を纏めたクラス
			ドライブ種類取得、HDD容量取得等
			きっと#ifdefの嵐
 */
/*--------------------------------------------------------------------------------*/
#ifndef CLDISKSPACEMANAGER_H
#define CLDISKSPACEMANAGER_H

#include <QtCore>




/*================================================================================*/
/*--------------------------------------------------------------------------------*/
/*!
	@class	ClDiskSpaceManager
	@brief	主に機種依存な部分を纏めたクラス
	@note	ドライブ種類取得、HDD容量取得等
 */
/*--------------------------------------------------------------------------------*/
class ClDiskSpaceManager
{
public:
	ClDiskSpaceManager();			//コンストラクタ
	virtual ~ClDiskSpaceManager();	//デストラクタ

	static quint64	getDiskCapacity(QString in_qstrDriveName);	//指定されたドライブの最大容量を返す
	static quint64	getDiskUsed(QString in_qstrDriveName);		//指定されたドライブの現在の使用済み容量を返す
	static bool		getDiskProperty(							//指定されたドライブについて容量と使用量を返す
						QString in_qstrDriveName,				//ドライブ名の指定
						quint64 &out_ullDiskCapacity,			//ディスク容量の書き込み先
						quint64 &out_ullDiskUsed				//使用量の書き込み先
					);
};
/*================================================================================*/

#endif // CLDISKSPACEMANAGER_H
