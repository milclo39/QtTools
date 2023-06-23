/*+------------------------------------------------------------------------+*/
/*+ Visual Presenter Command header file                                   +*/

/*+------------------------------------------------------------------------+*/

#ifndef _COMMANDPRESENTER_H
#define _COMMANDPRESENTER_H

/* Visual Presenter Family */
#define		VPRESENTER_L1N			1		/* L-1n / TT-02s */		
#define		VPRESENTER_L1EX			2		/* L-1ex / TT-02RX */
#define		VPRESENTER_P10S			3		/* P10S */
#define		VPRESENTER_P30S			4		/* P30S */
#define		VPRESENTER_IPOCHETTE	5		/* i-Pochette */
#define		VPRESENTER_P10EX		6		/* P10 */
#define		VPRESENTER_FF1			9		/* FF-1/HV-110u */

/* Get Version option */
#define 	VER_MAJOR				1		/* Major Version */
#define		VER_MINOR				2		/* Minor Version */
#define		VER_BUILD				3		/* Build Version */

/* Picture size option */
#define		SIZE_UNSUPPORT			0		/* Unsupport */
#define		SIZE_PICTURE_HIGH		1		/* Picture size High */
#define		SIZE_PICTURE_MID		2		/* Picture size Middle */
#define		SIZE_PICTURE_LOW		3		/* Picture size Low */

/* Rotetion option */
#define		PARAM_ROTATION_OFF		0		/* Enable the Rotetion */
#define		PARAM_ROTATION_ON		1		/* Disable the Rotation */

/* POSI_NEGA option */
#define		PARAM_POSI				1		/* Configure the POSI mode */
#define		PARAM_NEGA				2		/* Configure the NEGA mode */

/* COLOR_BW option */
#define		PARAM_COLOR				1		/* Configure the COLOR */
#define		PARAM_BW				2		/* Configure the BW */

/* Slow shutter option */
#define		PARAM_SLOW_SHUTTER_OFF	2		/* Enable the slow shutter of shutter speed */
#define		PARAM_SLOW_SHUTTER_ON	1		/* Disable the slow shutter of shutter speed */

/* Image mode option */
#define		PARAM_TEXT1				1		/*  */
#define		PARAM_TEXT2				2		/*  */
#define		PARAM_TEXT3				3		/*  */
#define		PARAM_GRAPHICS1			4		/*  */
#define		PARAM_GRAPHICS2			5		/*  */

/* Microscope mode option */
#define		PARAM_MICROSCOPE_OFF	0		/* Enable the Microscope mode */
#define		PARAM_MICROSCOPE_ON		1		/* Disable the Midroscope mode */


/* Get Picture Option */
#define		GET_PICTURE				100		/* Acquire the image from visual presenter */
#define		RELEASE_PICTURE			101		/* Release the JPEG area acquired by GET_PICTURE */
#define		GET_SIZE_PICTURE		102		/* Return the size for JPEG area ( byte ) acquired by GET_PICTURE */
#define		GET_RECT_PICTURE		103		/* The command valid for the model for which you can change the width and height of JPEG image. */
#define		SET_RECT_PICTURE		104		/* The command valid for the model for which you can change the width and height of JPEG image. */
#define		GET_HEIGHT_PICTURE		105		/* Return the vertical width ( pixel) of GET_PICTURE just executed. */
#define		GET_WIDTH_PICTURE		106		/* Return the horizontal width ( pixel ) of GET_WIDTH_PICTURE just executed. */

/* Model Status Command */
#define		GET_MODEL_NAME			107		/* Acquire the model name connected now. */
#define		GET_MODEL_VERSION		108		/* Acquire the version connected now. */

/* Zoom Command */
#define		ZOOM_WIDE				200		/* Move the ZOOM to wide. */
#define		ZOOM_TELE				201		/* Move the ZOOM to tele. */
#define		ZOOM_STOP				202		/* Stop the ZOOM move. */
#define		ZOOM_GET				203		/* Get */
#define		ZOOM_SET				204		/* Set */

/* Iris Command */
#define		IRIS_OPEN				300		/* Control the IRIS to open. */
#define		IRIS_CLOSE				301		/* Control the IRIS to close. */
#define		IRIS_STOP				302		/* Stop the IRIS move. */
#define		IRIS_INITIALIZE			303		/* Move the IRIS to the factory set position. */
#define		IRIS_GET				304		/* Get */
#define		IRIS_SET				305		/* Set */

/* Focus Command */
#define		FOCUS_AUTO				400		/* Adjust focus automatically. */
#define		FOCUS_NEAR				401		/* Focus on the near object. */
#define		FOCUS_FAR				402		/* Focus on the far object. */
#define		FOCUS_STOP				403		/* Stop the focus move. */
#define		FOCUS_GET				404		/* Get */
#define		FOCUS_SET				405		/* Set */
#define         FOCUS_AUTO_STOP                         406

/* Extend Command */
#define		ROTATION				502		/* Rotate the picture. */
#define		POSI_NEGA				503		/* Control the picture to the NEGA or POSI. */
#define		COLOR_BW				504		/* Control the picture to the COLOR or BW. */
#define		SLOW_SHUTTER			505		/* Control the picture to the slow shutter. */
#define		IMAGE_MODE				506		/* Configure the Image Mode which choose one from the five mode. */
#define		MICROSCOPE_MODE			507		/* Configure the microscope mode */

/* UVC VideoProcAmp */
#define		ROTATION_GET			600
#define		ROTATION_SET			601
#define		CONTRAST_UP				605
#define		CONTRAST_DOWN			606
#define		CONTRAST_GET			607
#define		CONTRAST_SET			608
#define		HUE_UP					610
#define		HUE_DOWN				611
#define		HUE_GET					612
#define		HUE_SET					613
#define		SATURATION_UP			615
#define		SATURATION_DOWN			616
#define		SATURATION_GET			617
#define		SATURATION_SET			618
#define		SHARPNESS_UP			620
#define		SHARPNESS_DOWN			621
#define		SHARPNESS_GET			622
#define		SHARPNESS_SET			623
#define		GAMMA_UP				625
#define		GAMMA_DOWN				626
#define		GAMMA_GET				627
#define		GAMMA_SET				628
#define		WB_UP					630
#define		WB_DOWN					631
#define		WB_AUTO					632
#define		WB_GET					633
#define		WB_SET					634
#define		BC_UP					635
#define		BC_DOWN					636
#define		BC_GET					637
#define		BC_SET					638
#define		GAIN_UP					640
#define		GAIN_DOWN				641
#define		GAIN_GET				642
#define		GAIN_SET				643
#define		POWERLINEFREQ_GET		645
#define		POWERLINEFREQ_SET		646

/* UVC CameraControl */
#define		EXPOSURE_OPEN			700
#define		EXPOSURE_CLOSE			701
#define		EXPOSURE_AUTO			702
#define		EXPOSURE_GET			703
#define		EXPOSURE_SET			704
#define		LOWLIGHT_GET			705
#define		LOWLIGHT_SET			706


/*! ------------------------------------------------------- //
//
//		@brief	UVCカメラ(RicohLib)向けコマンド
//
// -------------------------------------------------------- */
enum	eCOMMAND_UVC_R
{
	// フォーカス
	eUVC_R_FOCUS_CONTROL				= 1000,
	eUVC_R_FOCUS_GET_PARAM,

	// ズーム
	eUVC_R_ZOOM_CONTROL,
	eUVC_R_ZOOM_GET_PARAM,

	// アイリス
	eUVC_R_IRIS_CONTROL,
	eUVC_R_IRIS_GET_PARAM,

	// 露光
	eUVC_R_EXPOSE_SET_PARAM,
	eUVC_R_EXPOSE_GET_PARAM,

	// 露光オート設定
	eUVC_R_EXPOSE_SET_AUTO_STATE,
	eUVC_R_EXPOSE_GET_AUTO_STATE,

	// コントラスト
	eUVC_R_CONTRAST_SET_PARAM,
	eUVC_R_CONTRAST_GET_PARAM,

	// 色相
	eUVC_R_HUE_SET_PRRAM,
	eUVC_R_HUE_GET_PARAM,

	// 彩度
	eUVC_R_SATURATION_SET_PARAM,
	eUVC_R_SATURATION_GET_PARAM,

	// シャープネス
	eUVC_R_SHARPNESS_SET_PARAM,
	eUVC_R_SHARPNESS_GET_PARAM,

	// ガンマ
	eUVC_R_GAMMA_SET_PARAM,
	eUVC_R_GAMMA_GET_PARAM,

	// ホワイトバランス（色温度）
	eUVC_R_COLOR_TEMP_SET_PARAM,
	eUVC_R_COLOR_TEMP_GET_PARAM,

	// ホワイトバランスオート
	eUVC_R_WHITE_BALANCE_SET_AUTO_STATE,
	eUVC_R_WHITE_BALANCE_GET_AUTO_STATE,

	// フリッカ
	eUVC_R_FREQUENCY_SET_PARAM,
	eUVC_R_EREQUENCY_GET_PARAM,

	// 逆行補正
	eUVC_R_BACKLIGHT_SET_PARAM,
	eUVC_R_BACKLIGHT_GET_PARAM,

	// ゲイン
	eUVC_R_GAIN_SET_PARAM,
	eUVC_R_GAIN_GET_PARAM,

	// 拡張コマンド
	eUVC_R_EX1_SET_PARAM,
	eUVC_R_EX1_GET_PARAM,

	eUVC_R_EX2_SET_PARAM,
	eUVC_R_EX2_GET_PARAM,

	eUVC_R_EX3_SET_PARAM,
	eUVC_R_EX3_GET_PARAM,

	eUVC_R_EX4_SET_PARAM,
	eUVC_R_EX4_GET_PARAM,

	eUVC_R_EX5_SET_PARAM,
	eUVC_R_EX5_GET_PARAM,

	eUVC_R_EX6_SET_PARAM,
	eUVC_R_EX6_GET_PARAM,

	eUVC_R_EX7_SET_PARAM,
	eUVC_R_EX7_GET_PARAM,

	eUVC_R_EX8_SET_PARAM,
	eUVC_R_EX8_GET_PARAM

};

//	matsubara add. -------------------------
/* Get Picture Option V2*/
#define		GET_PICTURE_V2			600		/* Acquire the image from visual presenter*/

/* Get Stream V2*/
#define		START_STREAM_V2			610		/* Start Stream	*/
#define		GET_STREAM_V2			611		/* Get Stream	*/
#define		STOP_STREAM_V2			612		/* Stop Stream	*/

/* Upload File */
#define		SEND_UPIM_INFO_V2		700		/* Upload jpg image file info */
#define		SEND_UPFW_INFO_V2		701		/* Upload fw file info */
#define		UPLOAD_FILE_V2			702		/* Upload file */

/* Zoom Command */
#define		ZOOM_WIDE_V2			900		/* Move the ZOOM to wide. */
#define		ZOOM_TELE_V2			901		/* Move the ZOOM to tele. */
#define		ZOOM_STOP_V2			902		/* Stop the ZOOM move. */
#define		SET_VARIANT_ZOOM_V2		903
#define		GET_ZOOM_STATUS_V2		904

/* Focus Command */
#define		FOCUS_AUTO_V2			910		/* Adjust focus automatically. */
#define		FOCUS_NEAR_V2			911		/* Focus on the near object. */
#define		FOCUS_FAR_V2			912		/* Focus on the far object. */
#define		FOCUS_STOP_V2			913		/* Stop the focus move. */
#define		SET_VARIANT_FOCUS_V2	914
#define		GET_FOCUS_STATUS_V2		915
#define		SET_AFMODE_ON_V2		916
#define		SET_AFMODE_OFF_V2		917
#define		GET_AFMODE_STATUS_V2	918

/* Iris Command */
#define		IRIS_OPEN_V2			920		/* Control the IRIS to open. */
#define		IRIS_CLOSE_V2			921		/* Control the IRIS to close. */
#define		IRIS_STOP_V2			922		/* Stop the IRIS move. */
#define		IRIS_INITIALIZE_V2		923		/* Move the IRIS to the factory set position. */
#define		SET_VARIANT_IRIS_V2		924
#define		GET_IRIS_STATUS_V2		925
#define		SET_BRTMODE_ON_V2		926
#define		SET_BRTMODE_OFF_V2		927
#define		GET_BRTMODE_STATUS_V2	928

/* Model Status Command V2 */
#define		GET_MODEL_NAME_V2		800
#define		GET_MODEL_VERSION_V2	801
#define		GET_SH_VERSION_V2		802
#define		GET_LENS_VERSION_V2		803
#define		GET_FPGA_VERSION_V2		804
#define		GET_PRODUCTCODE_V2		805
#define		GET_CURRENT_MODE_V2		806
#define		GET_RES_SW_V2			807
#define		GET_IMOUT_SW_V2			808
#define		GET_CAPPUSH_FLG_V2		809

/* System Settings */
#define		SET_SYSTEM_TIME_V2		810
#define		SET_MODEL_NAME_V2		811
#define		SET_PRODUCTCODE_V2		812

/* White Balance */
#define		SET_WHITE_AUTO_V2		815
#define		SET_WHITE_ONEPUSH_V2	816
#define		SET_WHITE_MANUAL_V2		817
#define		GET_WHITE_STATUS_V2		818

/* Flicker */
#define		SET_FLICKER_50HZ_V2		820
#define		SET_FLICKER_60HZ_V2		821
#define		GET_FLICKER_STATUS_V2	822

/* Image Mode */
#define		SET_IM_MODE_TEXT1_V2	825
#define		SET_IM_MODE_TEXT2_V2	826
#define		SET_IM_MODE_TEXT3_V2	827
#define		SET_IM_MODE_COLOR1_V2	828
#define		SET_IM_MODE_COLOR2_V2	829
#define		GET_IM_MODE_STATUS_V2	830

/* Edge */
#define		SET_EDGE_LOW_V2			835
#define		SET_EDGE_MIDDLE_V2		836
#define		SET_EDGE_HIGH_V2		837
#define		GET_EDGE_STATUS_V2		838

/* Gamma */
#define		SET_GAMMA_LOW_V2		840
#define		SET_GAMMA_MIDDLE_V2		841
#define		SET_GAMMA_HIGH_V2		842
#define		GET_GAMMA_STATUS_V2		843

/* Freeze */
#define		SET_FREEZE_ON_V2		845
#define		SET_FREEZE_OFF_V2		846
#define		GET_FREEZE_STATUS_V2	847

/* Rotation */
#define		SET_ROTATION_ON_V2		850
#define		SET_ROTATION_OFF_V2		851
#define		GET_ROTATION_STATUS_V2	852

/* Nega Posi */
#define		SET_NEGAPOSI_POSI_V2	855
#define		SET_NEGAPOSI_NEGA_V2	856
#define		GET_NEGAPOSI_STATUS_V2	857

/* Color Mono */
#define		SET_COLORMONO_COLOR_V2	860
#define		SET_COLORMONO_MONO_V2	861
#define		GET_COLORMONO_STATUS_V2	862

/* Scope */
#define		SET_SCOPE_OFF_V2		865
#define		SET_SCOPE_ON_V2			866
#define		GET_SCOPE_STATUS_V2		867

/* Speaker */
#define		SET_SPEAKER_STATUS_V2	870
#define		GET_SPEAKER_STATUS_V2	871

/* Video */
#define		SET_VIDEO_NTSC_V2		875
#define		SET_VIDEO_PAL_V2		876
#define		GET_VIDEO_STATUS_V2		877

/* Language */
#define		SET_LANG_JPN_V2			880
#define		SET_LANG_ENG_V2			881
#define		GET_LANG_STATUS_V2		882

/* Guid */
#define		SET_GUIDE_ON_V2			885
#define		SET_GUIDE_OFF_V2		886
#define		GET_GUIDE_STATUS_V2		887

/* Settings */
#define		LOAD_SETTING_V2			890
#define		SAVE_SETTING_V2			891

/* External Command */
#define		EXTERNAL_INPUT			990		/* 外部入力コマンド V1 */
#define		EXTERNAL_INPUT_V2		991		/* 外部入力コマンド V2 */

/* Bus Reset */
#define		BUS_RESET				995
//	----------------------------------------

/* Error */
#define		ERROR_DRIVER_INITIALIZED	(0x8000F001)	/* Driver initiarize error */
#define		ERROR_CANNOT_OUTPUT_IMAGE	(0x8000F002)	/*  */

#endif
