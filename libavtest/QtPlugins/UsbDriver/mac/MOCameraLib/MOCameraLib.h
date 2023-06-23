//
//  MOCameraLib.h
//  MOCameraLib
//
//

#import <Foundation/Foundation.h>
#import <CoreFoundation/CoreFoundation.h>

#import <IOKit/IOKitLib.h>
#import <IOKit/IOMessage.h>
#import <IOKit/IOCFPlugIn.h>
#import <IOKit/usb/IOUSBLib.h>
#import <QTKit/QTKit.h>
#import <CoreAudio/CoreAudio.h>


typedef void
(*MOCallback)( void *refcon, BOOL Add_Remove );


#define UVC_INPUT_TERMINAL_ID 0x01
#define UVC_PROCESSING_UNIT_ID 0x02
#define UVC_EXTENSION_UNIT_ID 0x03

#define UVC_CONTROL_INTERFACE_CLASS 14
#define UVC_CONTROL_INTERFACE_SUBCLASS 1
#define AUDIO_CONTROL_INTERFACE_CLASS 1

#define UVC_SET_CUR	0x01
#define UVC_GET_CUR	0x81
#define UVC_GET_MIN	0x82
#define UVC_GET_MAX	0x83


typedef struct
{
	int unit;
	int selector;
	int size;
} uvc_control_info_t;


typedef struct
{
	uvc_control_info_t Focus;
	uvc_control_info_t autoFocus;
    uvc_control_info_t Zoom;
    uvc_control_info_t Backlight;
	uvc_control_info_t Brightness;
    uvc_control_info_t Frequency;
	uvc_control_info_t Sharpness;
	uvc_control_info_t Gamma;
	uvc_control_info_t WhiteBalance;
	uvc_control_info_t autoWhiteBalance;
    uvc_control_info_t Led;
    uvc_control_info_t Rotation;
} uvc_controls_t;


typedef struct
{
	UInt32 Count;
	struct
	{
		UInt32 width;
		UInt32 height;
	} SizeInfo[ 1 ];
} resolution_t;


@interface MOCameraLib : NSObject
{
	IOUSBInterfaceInterface190	**videointerface;
	AudioDeviceID				audiointerfaceID;
	QTCaptureDevice				*videoDevice;
	QTCaptureSession			*captureSession;
	QTCaptureDeviceInput		*videoInput;
	QTCaptureVideoPreviewOutput	*videoPreviewOutput;
	UInt32						DeviceLocationID;
	NSLock						*DataLock;
	void						*pDataBuffer;
	size_t						VideoWidth;
	size_t						VideoHeight;
	size_t						VideoLength;
	BOOL						VideoDataUpDate;
	BOOL						VideoRun;
	resolution_t				*pResolution_TBL;
	
	void*						CallbackRefcon;
	MOCallback					CallbackEntry;
	io_iterator_t				MO_1_AddIterator;
	IONotificationPortRef		NotifyPort;
	io_object_t					RemoveNotification;
}

- (id)initWithCallback:(MOCallback)Callback Param:(void*)refcon;

- (IOReturn)OpenMO_1;
- (IOReturn)CloseMO_1;
- (IOReturn)GetImage:(char*)Buffer Width:(UInt32*)width Height:(UInt32*)height;
- (IOReturn)GetResolutionList:(resolution_t*)resolutionList;
- (IOReturn)GetResolution:(UInt32*)width Height:(UInt32*)height;
- (IOReturn)SetResolution:(UInt32)width Height:(UInt32)height;
- (IOReturn)GetAutoFocus:(BOOL*)enable;
- (IOReturn)SetAutoFocus:(BOOL)enable;
- (IOReturn)GetFocus:(int*)value;
- (IOReturn)GetFocusInfo:(int*)value Min:(int*)min Max:(int*)max;
- (IOReturn)SetFocus:(int)value;
- (IOReturn)GetAutoWhiteBalance:(BOOL*)enable;
- (IOReturn)SetAutoWhiteBalance:(BOOL)enable;
- (IOReturn)GetWhiteBalance:(int*)value;
- (IOReturn)GetWhiteBalanceInfo:(int*)value Min:(int*)min Max:(int*)max;
- (IOReturn)SetWhiteBalance:(int)value;
- (IOReturn)GetBrightness:(int*)value;
- (IOReturn)GetBrightnessInfo:(int*)value Min:(int*)min Max:(int*)max;
- (IOReturn)SetBrightness:(int)value;
- (IOReturn)GetSharpnes:(int*)value;
- (IOReturn)GetSharpnesInfo:(int*)value Min:(int*)min Max:(int*)max;
- (IOReturn)SetSharpnes:(int)value;
- (IOReturn)GetGamma:(int*)value;
- (IOReturn)GetGammaInfo:(int*)value Min:(int*)min Max:(int*)max;
- (IOReturn)SetGamma:(int)value;
- (IOReturn)GetZoom:(int*)value;
- (IOReturn)GetZoomInfo:(int*)value Min:(int*)min Max:(int*)max;
- (IOReturn)SetZoom:(int)value;
- (IOReturn)GetMute:(BOOL*)enable;
- (IOReturn)SetMute:(BOOL)enable;
- (IOReturn)GetVolume:(int*)value;
- (IOReturn)GetVolumeInfo:(int*)value Min:(int*)min Max:(int*)max;
- (IOReturn)SetVolume:(int)value;
- (IOReturn)GetFrequency:(int*)value;
- (IOReturn)SetFrequency:(int)value;
- (IOReturn)GetBacklight:(BOOL*)enable;
- (IOReturn)SetBacklight:(BOOL)enable;
- (IOReturn)GetRotation:(BOOL*)enable;
- (IOReturn)SetRotation:(BOOL)enable;
- (IOReturn)GetLED:(BOOL*)enable;
- (IOReturn)SetLED:(BOOL)enable;

@end
