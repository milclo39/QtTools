// --------------------------------------------------------------------------------------------- //
  /*! =========================================================================================

		@file   ClAdaptRicohDeviceCtl.cpp

		@author （Ｓ技１）熊崎

		@brief  リコーのデバイスコントロールクラスの仲介クラスの実装

	==========================================================================================　*/
// --------------------------------------------------------------------------------------------- //

#include "ClAdaptRicohDeviceCtl.h"

#include <qDebug>

typedef ClAdaptRicohDeviceCtl::stUVC_PARAM stUVC_PARAM;
typedef struct _USB_DEVICE_DESCRIPTOR {
	UCHAR bLength;
	UCHAR bDescriptorType;
	USHORT bcdUSB;
	UCHAR bDeviceClass;
	UCHAR bDeviceSubClass;
	UCHAR bDeviceProtocol;
	UCHAR bMaxPacketSize0;
	USHORT idVendor;		//!< ベンダID
	USHORT idProduct;		//!< プロダクトID
	USHORT bcdDevice;
	UCHAR iManufacturer;
	UCHAR iProduct;
	UCHAR iSerialNumber;
	UCHAR bNumConfigurations;
} USB_DEVICE_DESCRIPTOR, *PUSB_DEVICE_DESCRIPTOR;

#ifdef DEVICE_DESCRIPTOR_DUMP

// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		デバイスディスクリプタ１６進出力

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
void	dump(BYTE* buf,size_t adrs,size_t size,int padding)
{
	int		n;
	size_t	i;

	// ヌル
	if((NULL != buf) && (0 != size))
	{
		for(i = 0;i < size;i++ )
		{
			// ４桁（サイズ）取り出して０と比較（改行）
			if(0x00 == (i & 0x0F))
			{
				// 余白と改行
				for(n = 0;n < padding;n++ )	printf(" ");
				printf("%04X :",i + adrs);
			}

			// １６進で表示
			printf(" %02X",buf[i]);
			if(0x0F == (i & 0x0F))	printf("\n");
		}
		if(	(0x00 != (size & 0x0F)) &&
			(0x0F != (size & 0x0F)) )
		{
			printf("\n");
		}
	}
}
#endif

// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		デバイスデータ出力

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
void procDispDeviceData(USBDEVICEDATA* pclUsbDeviceData, ULONG nest = 0)
{
	ULONG					i;			//!< カウンタ
	PUSB_DEVICE_DESCRIPTOR	pDvcDesc;	//!< デバイスディスクリプタ
	LPWSTR					str;		//!< ?

	// BYTEから変換
	pDvcDesc = (PUSB_DEVICE_DESCRIPTOR)pclUsbDeviceData->pDeviceDescriptor;
	for(i = 0;i <= nest;i++ )	printf("  ");

#ifdef DEVICE_DESCRIPTORPOINTEROUTPUT
	// デバイスディスクリプタのポインタを出力
	qDebug() << QString("pDeviceDescriptor = %1\n").arg(reinterpret_cast<qlonglong>(pclUsbDeviceData->pDeviceDescriptor),
														_CLADAPTRICOHDEVICECTL_OUTPUT_POINTER_FIELDWIDTH,
														_CLADAPTRICOHDEVICECTL_OUTPUT_POINTER_BASE);
#endif

	// ヌルチェック
	if(NULL != pclUsbDeviceData->pDeviceDescriptor)
	{

#ifdef DEVICE_DESCRIPTOR_DUMP

		// デバイスディスクリプタをなんかきれいに並べてるっぽい
		dump(pclUsbDeviceData->pDeviceDescriptor,
			0,
			(size_t)(*(pclUsbDeviceData->pDeviceDescriptor)),	// 最初の１バイトがサイズ
			2 * (nest + 2));		
#endif

		for(i = 0;i <= nest;i++ )	printf("  ");

		// ベンダーID
		printf("Vendor ID  = %04X\n",pDvcDesc->idVendor);
		for(i = 0;i <= nest;i++ )	printf("  ");

		// プロダクトID
		printf("Product ID = %04X\n",pDvcDesc->idProduct);
		for(i = 0;i <= nest;i++ )	printf("  ");

		// リビジョン？
		printf("Revision   = %04X\n",pDvcDesc->bcdDevice);

		if(0 != pDvcDesc->iManufacturer)
		{
			// メーカー名文字列取得？
			if( GetStringDescData(pclUsbDeviceData,pDvcDesc->iManufacturer,&str) )
			{
				for(i = 0;i <= nest;i++ )	printf("  ");
				printf("Manufacturer (%u) %S\n",pDvcDesc->iManufacturer,str);
				delete[]	str;
			}
		}
		if(0 != pDvcDesc->iProduct)
		{
			// プロダクト名文字列取得？
			if( GetStringDescData(pclUsbDeviceData,pDvcDesc->iProduct,&str) )
			{
				for(i = 0;i <= nest;i++ )	printf("  ");
				printf("Product      (%u) %S\n",pDvcDesc->iProduct,str);
				delete[]	str;
			}
		}

		if(0 != pDvcDesc->iSerialNumber)
		{
			// たぶんシリアルナンバー
			if( GetStringDescData(pclUsbDeviceData,pDvcDesc->iSerialNumber,&str) )
			{
				for(i = 0;i <= nest;i++ )	printf("  ");
				printf("SerialNumber (%u) %S\n",pDvcDesc->iSerialNumber,str);
				delete[]	str;
			}
		}
	}

	// -------------------------//
	//							//
	// ** 以下そのたもろもろ ** //
	//							//
	// -------------------------//

	for(i = 0;i <= nest;i++ )	printf("  ");
	printf("pConfigurationDescriptor = %p\n",pclUsbDeviceData->pConfigurationDescriptor);

	for(i = 0;i <= nest;i++ )	printf("  ");
	printf("hDevInfoSet = %p\n",pclUsbDeviceData->hDevInfoSet);

	for(i = 0;i <= nest;i++ )	printf("  ");
	printf("DeviceInterfaceData::InterfaceClassGuid =\n");
	for(i = 0;i <= (nest + 1);i++ )	printf("  ");
	printf("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}\n",
		pclUsbDeviceData->DeviceInterfaceData.InterfaceClassGuid.Data1,
		pclUsbDeviceData->DeviceInterfaceData.InterfaceClassGuid.Data2,
		pclUsbDeviceData->DeviceInterfaceData.InterfaceClassGuid.Data3,
		pclUsbDeviceData->DeviceInterfaceData.InterfaceClassGuid.Data4[0],
		pclUsbDeviceData->DeviceInterfaceData.InterfaceClassGuid.Data4[1],
		pclUsbDeviceData->DeviceInterfaceData.InterfaceClassGuid.Data4[2],
		pclUsbDeviceData->DeviceInterfaceData.InterfaceClassGuid.Data4[3],
		pclUsbDeviceData->DeviceInterfaceData.InterfaceClassGuid.Data4[4],
		pclUsbDeviceData->DeviceInterfaceData.InterfaceClassGuid.Data4[5],
		pclUsbDeviceData->DeviceInterfaceData.InterfaceClassGuid.Data4[6],
		pclUsbDeviceData->DeviceInterfaceData.InterfaceClassGuid.Data4[7]);
/*
	for(i = 0;i <= nest;i++ )	printf("  ");
	printf("DeviceInterfaceData::Flags = %08X\n",pclUsbDeviceData->DeviceInterfaceData.Flags);

	for(i = 0;i <= nest;i++ )	printf("  ");
	printf("DevInfoData::ClassGuid =\n");
	for(i = 0;i <= (nest + 1);i++ )	printf("  ");
	printf("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}\n",
		pclUsbDeviceData->DevInfoData.ClassGuid.Data1,
		pclUsbDeviceData->DevInfoData.ClassGuid.Data2,
		pclUsbDeviceData->DevInfoData.ClassGuid.Data3,
		pclUsbDeviceData->DevInfoData.ClassGuid.Data4[0],
		pclUsbDeviceData->DevInfoData.ClassGuid.Data4[1],
		pclUsbDeviceData->DevInfoData.ClassGuid.Data4[2],
		pclUsbDeviceData->DevInfoData.ClassGuid.Data4[3],
		pclUsbDeviceData->DevInfoData.ClassGuid.Data4[4],
		pclUsbDeviceData->DevInfoData.ClassGuid.Data4[5],
		pclUsbDeviceData->DevInfoData.ClassGuid.Data4[6],
		pclUsbDeviceData->DevInfoData.ClassGuid.Data4[7]);

	for(i = 0;i <= nest;i++ )	printf("  ");
	_tprintf(TEXT("DevInfoData::DevInst = %08X\n"),pclUsbDeviceData->DevInfoData.DevInst);

	for(i = 0;i <= nest;i++ )	printf("  ");
	_tprintf(TEXT("HardwareID = %s\n"),pclUsbDeviceData->HardwareID);

	for(i = 0;i <= nest;i++ )	printf("  ");
	_tprintf(TEXT("DriverKey = %s\n"),pclUsbDeviceData->DriverKey);
	for(i = 0;i <= nest;i++ )	printf("  ");
	_tprintf(TEXT("DeviceInstance = %s\n"),pclUsbDeviceData->DeviceInstance);
	for(i = 0;i <= nest;i++ )	printf("  ");
	_tprintf(TEXT("SymbolicLinkName = %s\n"),pclUsbDeviceData->SymbolicLinkName);
	for(i = 0;i <= nest;i++ )	printf("  ");
	_tprintf(TEXT("UsbPath = %s\n"),pclUsbDeviceData->UsbPath);
	*/
}

// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		コンストラクタ

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
ClAdaptRicohDeviceCtl::ClAdaptRicohDeviceCtl( QObject *pclParent ) :
	QObject( pclParent )
{

	// ** カメラを扱うクラス？
	m_pclEnumDevice = new cEnumDevices;
	if( NULL == m_pclEnumDevice )
	{
		printf("Insufficient resources.\n");
		return;
	}

	// ** USBデバイスの探索
	m_pclUsbDeviceData = m_pclEnumDevice->Find( KSCATEGORY_CAPTURE, GUID_DEVCLASS_IMAGE );
	if(NULL == m_pclUsbDeviceData)
	{
		printf("USB camera device is not found.\n");
		return;
	}

	procDispDeviceData( m_pclUsbDeviceData );

	m_clRicohDeviceCtl.Initialize( m_pclUsbDeviceData );
}

// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		ズーム値の取得

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::getZoom( stUVC_PARAM* pstRetVal )
{
	return m_clRicohDeviceCtl.GetZoom( reinterpret_cast< LONG* >( &pstRetVal->qlCur ),
									   reinterpret_cast< LONG* >( &pstRetVal->qlMin ),
									   reinterpret_cast< LONG* >( &pstRetVal->qlMax ),
									   reinterpret_cast< LONG* >( &pstRetVal->qlRes ),
									   reinterpret_cast< LONG* >( &pstRetVal->qlDef ));
}
// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		露出の取得

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::getExposure( stUVC_PARAM*	pstRetVal, stUVC_AUTO_PARAM*	pstRetAutoVal )
{
	return m_clRicohDeviceCtl.GetExposure( reinterpret_cast< LONG* >( &pstRetVal->qlCur ),
										   reinterpret_cast< LONG* >( &pstRetVal->qlMin ),
										   reinterpret_cast< LONG* >( &pstRetVal->qlMax ),
										   reinterpret_cast< LONG* >( &pstRetVal->qlRes ),
										   reinterpret_cast< LONG* >( &pstRetVal->qlDef ),
										   &pstRetAutoVal->qlCap,
										   &pstRetAutoVal->qlCur,
										   &pstRetAutoVal->qlDef);
}
// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		自動露出の優先度？の取得

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::getAutoExposurePriority( stUVC_PARAM* pstRetVal )
{
	return m_clRicohDeviceCtl.GetAutoExposurePriority( reinterpret_cast< LONG* >( &pstRetVal->qlCur ),
													   reinterpret_cast< LONG* >( &pstRetVal->qlMin ),
													   reinterpret_cast< LONG* >( &pstRetVal->qlMax ),
													   reinterpret_cast< LONG* >( &pstRetVal->qlRes ),
													   reinterpret_cast< LONG* >( &pstRetVal->qlDef ));
}
// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		フォーカス値の取得

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::getFocus( stUVC_PARAM*	pstRetVal, stUVC_AUTO_PARAM* pstRetAutoVal )
{
	return m_clRicohDeviceCtl.GetFocus( reinterpret_cast< LONG* >( &pstRetVal->qlCur ),
										reinterpret_cast< LONG* >( &pstRetVal->qlMin ),
										reinterpret_cast< LONG* >( &pstRetVal->qlMax ),
										reinterpret_cast< LONG* >( &pstRetVal->qlRes ),
										reinterpret_cast< LONG* >( &pstRetVal->qlDef ),
										&pstRetAutoVal->qlCap,
										&pstRetAutoVal->qlCur,
										&pstRetAutoVal->qlDef);
}


// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		明度の取得

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::getBrightness( stUVC_PARAM*	pstRetVal )
{
	return m_clRicohDeviceCtl.GetBrightness( reinterpret_cast< LONG* >( &pstRetVal->qlCur ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlMin ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlMax ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlRes ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlDef ));
}

// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		コントラストの取得

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::getContrast( stUVC_PARAM* pstRetVal )
{
	return m_clRicohDeviceCtl.GetContrast( reinterpret_cast< LONG* >( &pstRetVal->qlCur ),
										   reinterpret_cast< LONG* >( &pstRetVal->qlMin ),
										   reinterpret_cast< LONG* >( &pstRetVal->qlMax ),
										   reinterpret_cast< LONG* >( &pstRetVal->qlRes ),
										   reinterpret_cast< LONG* >( &pstRetVal->qlDef ));
}
// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		色相の取得

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::getHue( stUVC_PARAM*	pstRetVal, stUVC_AUTO_PARAM*	pstRetAutoVal )
{
	return m_clRicohDeviceCtl.GetWhiteBalanceTemperature( reinterpret_cast< LONG* >( &pstRetVal->qlCur ),
														  reinterpret_cast< LONG* >( &pstRetVal->qlMin ),
														  reinterpret_cast< LONG* >( &pstRetVal->qlMax ),
														  reinterpret_cast< LONG* >( &pstRetVal->qlRes ),
														  reinterpret_cast< LONG* >( &pstRetVal->qlDef ),
														  &pstRetAutoVal->qlCap,
														  &pstRetAutoVal->qlCur,
														  &pstRetAutoVal->qlDef);
}
// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		彩度の取得

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::getSaturation( stUVC_PARAM* pstRetVal )
{
	return m_clRicohDeviceCtl.GetSaturation( reinterpret_cast< LONG* >( &pstRetVal->qlCur ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlMin ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlMax ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlRes ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlDef ));
}
// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		シャープネスの取得

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::getSharpness( stUVC_PARAM* pstRetVal )
{
	return m_clRicohDeviceCtl.GetSharpness( reinterpret_cast< LONG* >( &pstRetVal->qlCur ),
											reinterpret_cast< LONG* >( &pstRetVal->qlMin ),
											reinterpret_cast< LONG* >( &pstRetVal->qlMax ),
											reinterpret_cast< LONG* >( &pstRetVal->qlRes ),
											reinterpret_cast< LONG* >( &pstRetVal->qlDef ));
}
// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		ガンマ値の取得

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::getGamma( stUVC_PARAM* pstRetVal )
{
	return m_clRicohDeviceCtl.GetGamma( reinterpret_cast< LONG* >( &pstRetVal->qlCur ),
										reinterpret_cast< LONG* >( &pstRetVal->qlMin ),
										reinterpret_cast< LONG* >( &pstRetVal->qlMax ),
										reinterpret_cast< LONG* >( &pstRetVal->qlRes ),
										reinterpret_cast< LONG* >( &pstRetVal->qlDef ));
}
// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		ホワイトバランス・色温度の取得

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::getWhiteBalanceTemperature( stUVC_PARAM*	pstRetVal , stUVC_AUTO_PARAM*	pstRetAutoVal )
{
	return m_clRicohDeviceCtl.GetWhiteBalanceTemperature( reinterpret_cast< LONG* >( &pstRetVal->qlCur ),
														  reinterpret_cast< LONG* >( &pstRetVal->qlMin ),
														  reinterpret_cast< LONG* >( &pstRetVal->qlMax ),
														  reinterpret_cast< LONG* >( &pstRetVal->qlRes ),
														  reinterpret_cast< LONG* >( &pstRetVal->qlDef ),
														  &pstRetAutoVal->qlCap,
														  &pstRetAutoVal->qlCur,
														  &pstRetAutoVal->qlDef);
}
// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		逆光補正値の取得

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::getBacklightCompensation( stUVC_PARAM* pstRetVal )
{
	return m_clRicohDeviceCtl.GetBacklightCompensation( reinterpret_cast< LONG* >( &pstRetVal->qlCur ),
														reinterpret_cast< LONG* >( &pstRetVal->qlMin ),
														reinterpret_cast< LONG* >( &pstRetVal->qlMax ),
														reinterpret_cast< LONG* >( &pstRetVal->qlRes ),
														reinterpret_cast< LONG* >( &pstRetVal->qlDef ));
}
// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		ゲイン値の取得

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::getGain( stUVC_PARAM* pstRetVal )
{
	return m_clRicohDeviceCtl.GetGain( reinterpret_cast< LONG* >( &pstRetVal->qlCur ),
									   reinterpret_cast< LONG* >( &pstRetVal->qlMin ),
									   reinterpret_cast< LONG* >( &pstRetVal->qlMax ),
									   reinterpret_cast< LONG* >( &pstRetVal->qlRes ),
									   reinterpret_cast< LONG* >( &pstRetVal->qlDef ));
}
// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		明度の取得

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::getPowerLineFrequency( stUVC_PARAM* pstRetVal )
{
	return m_clRicohDeviceCtl.GetPowerLineFrequency( reinterpret_cast< LONG* >( &pstRetVal->qlCur ),
													 reinterpret_cast< LONG* >( &pstRetVal->qlMin ),
													 reinterpret_cast< LONG* >( &pstRetVal->qlMax ),
													 reinterpret_cast< LONG* >( &pstRetVal->qlRes ),
													 reinterpret_cast< LONG* >( &pstRetVal->qlDef ));
}
// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		ホワイトバランスのフルパラメータ取得

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::getWhiteBalanceFullComponent( stUVC_PARAM*	pstRed,
									  stUVC_PARAM*	pstGreen,
									  stUVC_PARAM*	pstBlue,
									  stUVC_AUTO_PARAM*	pstRetAutoVal )
{
	return m_clRicohDeviceCtl.GetWhiteBalanceFullComponent( reinterpret_cast< LONG* >( &pstRed->qlCur ), reinterpret_cast< LONG* >( &pstGreen->qlCur ), reinterpret_cast< LONG* >( &pstBlue->qlCur ),
															reinterpret_cast< LONG* >( &pstRed->qlMin ), reinterpret_cast< LONG* >( &pstGreen->qlMin ), reinterpret_cast< LONG* >( &pstBlue->qlMin ),
															reinterpret_cast< LONG* >( &pstRed->qlMax ), reinterpret_cast< LONG* >( &pstGreen->qlMax ), reinterpret_cast< LONG* >( &pstBlue->qlMax ),
															reinterpret_cast< LONG* >( &pstRed->qlRes ), reinterpret_cast< LONG* >( &pstGreen->qlRes ), reinterpret_cast< LONG* >( &pstBlue->qlRes ),
															reinterpret_cast< LONG* >( &pstRed->qlDef ), reinterpret_cast< LONG* >( &pstGreen->qlDef ), reinterpret_cast< LONG* >( &pstBlue->qlDef ),
															&pstRetAutoVal->qlCap,
															&pstRetAutoVal->qlCur,
															&pstRetAutoVal->qlDef);
}

// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		超露出の取得

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::getExposureEx( stUVC_PARAM*	pstRetVal, stUVC_AUTO_PARAM*	pstRetAutoVal )
{
	return m_clRicohDeviceCtl.GetExposureEx( reinterpret_cast< LONG* >( &pstRetVal->qlCur ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlMin ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlMax ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlRes ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlDef ));
}
// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		超ゲインの取得

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::getGainEx( stUVC_PARAM*	pstRetVal, stUVC_AUTO_PARAM*	pstRetAutoVal )
{
	return m_clRicohDeviceCtl.GetGainEx( reinterpret_cast< LONG* >( &pstRetVal->qlCur ),
										 reinterpret_cast< LONG* >( &pstRetVal->qlMin ),
										 reinterpret_cast< LONG* >( &pstRetVal->qlMax ),
										 reinterpret_cast< LONG* >( &pstRetVal->qlRes ),
										 reinterpret_cast< LONG* >( &pstRetVal->qlDef ));
}
// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		超電源周波数の取得

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::getPowerLineFrequencyEx( stUVC_PARAM*	pstRetVal, stUVC_AUTO_PARAM*	pstRetAutoVal )
{
	return m_clRicohDeviceCtl.GetPowerLineFrequencyEx( reinterpret_cast< LONG* >( &pstRetVal->qlCur ),
													  reinterpret_cast< LONG* >( &pstRetVal->qlMin ),
													  reinterpret_cast< LONG* >( &pstRetVal->qlMax ),
													  reinterpret_cast< LONG* >( &pstRetVal->qlRes ),
													  reinterpret_cast< LONG* >( &pstRetVal->qlDef ));
}
// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		エクステンション１取得

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::getExtension1	( stUVC_PARAM* pstRetVal )
{
	return m_clRicohDeviceCtl.GetExtension1( reinterpret_cast< LONG* >( &pstRetVal->qlCur ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlMin ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlMax ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlRes ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlDef ));
}
// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		エクステンション２取得

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::getExtension2	( stUVC_PARAM* pstRetVal )
{
	return m_clRicohDeviceCtl.GetExtension2( reinterpret_cast< LONG* >( &pstRetVal->qlCur ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlMin ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlMax ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlRes ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlDef ));
}
// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		エクステンション３取得

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::getExtension3	( stUVC_PARAM* pstRetVal )
{
	return m_clRicohDeviceCtl.GetExtension3( reinterpret_cast< LONG* >( &pstRetVal->qlCur ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlMin ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlMax ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlRes ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlDef ));
}
// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		エクステンション４取得

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::getExtension4	( stUVC_PARAM* pstRetVal )
{
	return m_clRicohDeviceCtl.GetExtension4( reinterpret_cast< LONG* >( &pstRetVal->qlCur ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlMin ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlMax ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlRes ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlDef ));
}
// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		エクステンション５取得

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::getExtension5	( stUVC_PARAM* pstRetVal )
{
	return m_clRicohDeviceCtl.GetExtension5( reinterpret_cast< LONG* >( &pstRetVal->qlCur ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlMin ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlMax ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlRes ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlDef ));
}
// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		エクステンション６取得

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::getExtension6	( stUVC_PARAM* pstRetVal )
{
	return m_clRicohDeviceCtl.GetExtension6( reinterpret_cast< LONG* >( &pstRetVal->qlCur ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlMin ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlMax ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlRes ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlDef ));
}
// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		エクステンション７取得

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::getExtension7	( stUVC_PARAM* pstRetVal )
{
	return m_clRicohDeviceCtl.GetExtension7( reinterpret_cast< LONG* >( &pstRetVal->qlCur ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlMin ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlMax ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlRes ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlDef ));
}
// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		エクステンション８取得

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::getExtension8	( stUVC_PARAM* pstRetVal )
{
	return m_clRicohDeviceCtl.GetExtension8( reinterpret_cast< LONG* >( &pstRetVal->qlCur ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlMin ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlMax ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlRes ),
											 reinterpret_cast< LONG* >( &pstRetVal->qlDef ));
}


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////


// ---------------------------------------------------------------------------------------- //
/*! ====================================================================================

		  @brief		ズーム値のセット

  =====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::slotSetZoom( qlonglong qlParam )
{
  return m_clRicohDeviceCtl.SetZoom( static_cast< LONG >( qlParam ) );
}

// ---------------------------------------------------------------------------------------- //
/*! ====================================================================================

		  @brief		露出値のセット

  =====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::slotSetExposure( qlonglong qlParam, bool bAuto)
{
	return m_clRicohDeviceCtl.SetExposure( static_cast< LONG >( qlParam ), bAuto );
}

// ---------------------------------------------------------------------------------------- //
/*! ====================================================================================

		  @brief		自動露出補正値のセット

  =====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::slotSetAutoExposurePriority( qlonglong qlParam )
{
	return m_clRicohDeviceCtl.SetAutoExposurePriority( static_cast< LONG >( qlParam ) );
}

// ---------------------------------------------------------------------------------------- //
/*! ====================================================================================

		  @brief		フォーカス値のセット

  =====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::slotSetFocus( qlonglong qlParam, bool bAuto)
{
	return m_clRicohDeviceCtl.SetFocus( static_cast< LONG >( qlParam ), bAuto );
}

// ---------------------------------------------------------------------------------------- //
/*! ====================================================================================

		  @brief		明度のセット

  =====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::slotSetBrightness( qlonglong qlParam )
{
  return m_clRicohDeviceCtl.SetBrightness( static_cast< LONG >( qlParam ) );
}

// ---------------------------------------------------------------------------------------- //
/*! ====================================================================================

		  @brief		コントラストのセット

  =====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::slotSetContrast		( qlonglong qlParam )
{
  return m_clRicohDeviceCtl.SetContrast( static_cast< LONG >( qlParam ) );
}

// ---------------------------------------------------------------------------------------- //
/*! ====================================================================================

		  @brief		色相のセット

  =====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::slotSetHue			( qlonglong qlParam, bool bAuto)
{
	return m_clRicohDeviceCtl.SetHue( static_cast< LONG >( qlParam ), bAuto );
}

// ---------------------------------------------------------------------------------------- //
/*! ====================================================================================

		  @brief		彩度のセット

  =====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::slotSetSaturation	( qlonglong qlParam )
{
  return m_clRicohDeviceCtl.SetSaturation( static_cast< LONG >( qlParam ) );
}

// ---------------------------------------------------------------------------------------- //
/*! ====================================================================================

		  @brief		シャープネスのセット

  =====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::slotSetSharpness	( qlonglong qlParam )
{
  return m_clRicohDeviceCtl.SetSharpness( static_cast< LONG >( qlParam ) );
}

// ---------------------------------------------------------------------------------------- //
/*! ====================================================================================

		  @brief		ガンマ値のセット

  =====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::slotSetGamma		( qlonglong qlParam )
{
  return m_clRicohDeviceCtl.SetGamma( static_cast< LONG >( qlParam ) );
}

// ---------------------------------------------------------------------------------------- //
/*! ====================================================================================

		  @brief		ホワイトバランス・色温度のセット

  =====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::slotSetWhiteBalanceTemperature	( qlonglong qlParam, bool bAuto)
{
	return m_clRicohDeviceCtl.SetWhiteBalanceTemperature( static_cast< LONG >( qlParam ), bAuto );
}

// ---------------------------------------------------------------------------------------- //
/*! ====================================================================================

		  @brief		逆行補正値のセット

  =====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::slotSetBacklightCompensation	( qlonglong qlParam )
{
  return m_clRicohDeviceCtl.SetBacklightCompensation( static_cast< LONG >( qlParam ) );
}

// ---------------------------------------------------------------------------------------- //
/*! ====================================================================================

		  @brief		ゲイン値のセット

  =====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::slotSetGain						( qlonglong qlParam )
{
  return m_clRicohDeviceCtl.SetGain( static_cast< LONG >( qlParam ) );
}

// ---------------------------------------------------------------------------------------- //
/*! ====================================================================================

		  @brief		電源周波数のセット

  =====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::slotSetPowerLineFrequency		( qlonglong qlParam )
{
  return m_clRicohDeviceCtl.SetPowerLineFrequency( static_cast< LONG >( qlParam ) );
}

// ---------------------------------------------------------------------------------------- //
/*! ====================================================================================

		  @brief		ホワイトバランスフルパラメータのセット

  =====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::slotSetWhiteBalanceFullComponent( qlonglong qlRed, qlonglong qlGreen, qlonglong qlBlue, bool bAuto)
{
	return m_clRicohDeviceCtl.SetWhiteBalanceFullComponent( static_cast< LONG >( qlRed ), static_cast< LONG >( qlGreen ), static_cast< LONG >( qlBlue ), bAuto);
}

// ---------------------------------------------------------------------------------------- //
/*! ====================================================================================

		  @brief		超露出のセット

  =====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::slotSetExposureEx			( qlonglong qlParam, bool bAuto)
{
	return m_clRicohDeviceCtl.SetExposureEx( static_cast< LONG >( qlParam ), bAuto );
}

// ---------------------------------------------------------------------------------------- //
/*! ====================================================================================

		  @brief		超ゲイン値のセット

  =====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::slotSetGainEx				( qlonglong qlParam, bool bAuto)
{
	return m_clRicohDeviceCtl.SetGainEx( static_cast< LONG >( qlParam ), bAuto );
}

// ---------------------------------------------------------------------------------------- //
/*! ====================================================================================

		  @brief		超電源補正値のセット

  =====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::slotSetPowerLineFrequencyEx	( qlonglong qlParam, bool bAuto)
{
	return m_clRicohDeviceCtl.SetPowerLineFrequencyEx( static_cast< LONG >( qlParam ), bAuto );
}

// ---------------------------------------------------------------------------------------- //
/*! ====================================================================================

		  @brief		エクステンション１のセット

  =====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::slotSetExtension1( qlonglong qlParam )
{
  return m_clRicohDeviceCtl.SetExtension1( static_cast< LONG >( qlParam ) );
}

// ---------------------------------------------------------------------------------------- //
/*! ====================================================================================

		  @brief		エクステンション２のセット

  =====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::slotSetExtension2( qlonglong qlParam )
{
  return m_clRicohDeviceCtl.SetExtension2( static_cast< LONG >( qlParam ) );
}

// ---------------------------------------------------------------------------------------- //
/*! ====================================================================================

		  @brief		エクステンション３のセット

  =====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::slotSetExtension3( qlonglong qlParam )
{
  return m_clRicohDeviceCtl.SetExtension3( static_cast< LONG >( qlParam ) );
}

// ---------------------------------------------------------------------------------------- //
/*! ====================================================================================

		  @brief		エクステンション４のセット

  =====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::slotSetExtension4( qlonglong qlParam )
{
  return m_clRicohDeviceCtl.SetExtension4( static_cast< LONG >( qlParam ) );
}

// ---------------------------------------------------------------------------------------- //
/*! ====================================================================================

		  @brief		エクステンション５のセット

  =====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::slotSetExtension5( qlonglong qlParam )
{
  return m_clRicohDeviceCtl.SetExtension5( static_cast< LONG >( qlParam ) );
}

// ---------------------------------------------------------------------------------------- //
/*! ====================================================================================

		  @brief		エクステンション６のセット

  =====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::slotSetExtension6( qlonglong qlParam )
{
  return m_clRicohDeviceCtl.SetExtension6( static_cast< LONG >( qlParam ) );
}

// ---------------------------------------------------------------------------------------- //
/*! ====================================================================================

		  @brief		エクステンション７のセット

  =====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::slotSetExtension7( qlonglong qlParam )
{
  return m_clRicohDeviceCtl.SetExtension7( static_cast< LONG >( qlParam ) );
}

// ---------------------------------------------------------------------------------------- //
/*! ====================================================================================

		  @brief		エクステンション８のセット

  =====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
bool	ClAdaptRicohDeviceCtl::slotSetExtension8( qlonglong qlParam )
{
  return m_clRicohDeviceCtl.SetExtension8( static_cast< LONG >( qlParam ) );
}


