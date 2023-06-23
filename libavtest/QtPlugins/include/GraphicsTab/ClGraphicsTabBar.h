//--------------------------------------------------------------------------------//
/*!
	@file	ClGraphicsTabBar.h
	@brief	タブバーヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLGRAPHICSTABBAR_H
#define CLGRAPHICSTABBAR_H
#include <QGraphicsObject>
#include <QFont>

class ClGraphicsTab;
//--------------------------------------------------------------------------------//
/*!
	@brief	タブバー
*/
//--------------------------------------------------------------------------------//
class ClGraphicsTabBar : public QGraphicsObject
{
	Q_OBJECT

public:
	ClGraphicsTabBar(QGraphicsItem *pclParent = NULL);	//コンストラクタ

	virtual void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);	//描画
	virtual QRectF boundingRect() const;	//領域取得

	void addTab(QString strText, QString strToolTip = "");	//タブ追加
	void addTab(QPixmap clPixmap, QString strText = "", QString strToolTip = "");	//タブ追加
	void setTabText(qint32 lIndex, QString strText);	//タブ文字列設定
	void setTabPixmap(qint32 lIndex, QPixmap clPixmap);	//タブ画像設定
	void setFont(QFont clFont);			//フォント設定
	void setPixmapSize(QSize clSize);	//画像サイズ設定
	/*!
		@brief		画像サイズ設定
		@param[in]	lWidth : 幅
		@param[in]	lHeight : 高さ
	*/
	void setPixmapSize(qint32 lWidth, qint32 lHeight){setPixmapSize(QSize(lWidth, lHeight));}
	/*!
		@brief		選択中のタブ番号取得
		@retval		選択中のタブ番号
	*/
	qint32 getSelectIndex(){return m_clTabList.indexOf(m_pclSelectTab);}
	/*!
		@brief		タブ数取得
		@retval		タブ数
	*/
	qint32 getTabCount(){return m_clTabList.count();}
	void setBaseTabSize(QSizeF clSize);	//タブの基本サイズ設定

	void setTabColor(QColor clColor);	//タブの色設定

protected:
	virtual void alignmentTab();	//タブ整列

public slots:
	void slotSelect(int iIndex);	//タブ選択

protected slots:
	//内部用
	void slotSelect();	//タブ選択通知

signals:
	/*!
		@brief		タブ選択変更通知
		@parma[in]	iIndex : タブ番号
	*/
	void sigSelectChange(int iIndex);

protected:
	QList<ClGraphicsTab*> m_clTabList;	//!< タブリスト
	ClGraphicsTab *m_pclSelectTab;		//!< 選択中タブ
	QSize m_clPixmapSize;	//!< 画像サイズ
	qint32 m_lTabHeight;	//!< タブの高さ
	QFont m_clTextFont;		//!< タブのフォント
	QSizeF m_clBaseTabSize;	//!< タブの基本サイズ
	QColor m_clColor;		//!< タブの色
};
//--------------------------------------------------------------------------------//
#endif //CLGRAPHICSTABBAR_H
