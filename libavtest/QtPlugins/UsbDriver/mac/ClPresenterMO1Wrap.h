#ifndef __CLPRESENTERMO1WRAP_MAC_H_
#define __CLPRESENTERMO1WRAP_MAC_H_

#include <stdbool.h>

#include <QByteArray>
#include <QStringList>

#if 0
#define DISPLAY_TRACE
#include <QDebug>
#endif

class IMOCameraCtrl
{
public:
	virtual ~IMOCameraCtrl( void ){}												//destructor

	virtual bool		openCamera( void ) = 0;										//open
	virtual bool		closeCamera( void ) = 0;									//close

	virtual bool		isOpen( void ) = 0;											//check open

	virtual QByteArray	getPicture( void ) = 0;										//get picture

	virtual QStringList	getResolutionList( void ) = 0;								//get resolution list
	virtual QString		getResolution() = 0;										//get current resolution
	virtual bool		setResolution( int iWidth, int iHeight ) = 0;				//set current resolution

	virtual bool		getAutoFucus( void ) = 0;									//get auto focus state (false:off,true:on)
	virtual void		setAutoFucus( bool bValue ) = 0;							//set auto focus state (false:off,true:on)

	virtual void		getFocusInf( int &iValue, int &iMin, int &iMax ) = 0;		//get focus information
	virtual void		setFocus( int iValue ) = 0;									//set focus

	virtual void		getBrightnessInf( int &iValue, int &iMin, int &iMax ) = 0;	//get brightness information
	virtual void		setBrightness( int iValue ) = 0;							//set brightness

	virtual void		getZoomInf( int &iValue, int &iMin, int &iMax ) = 0;		//get zoom information
	virtual void		setZoom( int iValue ) = 0;									//set zoom

	virtual quint16		getPictureWidth(){return m_usWidth;}
	virtual quint16		getPictureHeight(){return m_usHeight;}
	virtual qint32		getPictureSize(){return m_iPictureSize;}

	char*				m_pcImageData;												//image data
	quint16				m_usWidth;													//image width
	quint16				m_usHeight;													//image height
	qint32				m_iPictureSize;												//image size

	bool				m_bIsOpen;													//is open flag
};

IMOCameraCtrl*			createMOCameraCtrlClass( void );							//create camera control class
void					deleteMOCameraCtrlClass( IMOCameraCtrl  *pclMOCameraCtrl );	//delete camera control class


#endif //__CLPRESENTERMO1WRAP_MAC_H_
