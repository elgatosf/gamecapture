#pragma once

namespace ElgatoGameCapture
{
	/*=============================================================================
	// IElgatoVideoCaptureFilterEnumeration
	// (available since Elgato Game Capture software version 2.11.23)
	=============================================================================*/

	typedef enum VIDEO_CAPTURE_FILTER_BUS_TYPES
	{
		VIDEO_CAPTURE_FILTER_BUS_TYPE_USB2,								//!< USB 2.0
		VIDEO_CAPTURE_FILTER_BUS_TYPE_PCI,								//!< PCI/PCIe
		NUM_VIDEO_CAPTURE_FILTER_BUS_TYPES
	};

	//! Video Capture device type
	typedef enum VIDEO_CAPTURE_FILTER_DEVICE_TYPE
	{
		VIDEO_CAPTURE_FILTER_DEVICE_TYPE_INVALID                = 0,	//!< Invalid
		VIDEO_CAPTURE_FILTER_DEVICE_TYPE_GAME_CAPTURE_HD        = 2,	//!< Game Capture HD       (USB  VID: 0x0FD9 PID: 0x0044, 0x004e, 0x0051, 0x005D)
		VIDEO_CAPTURE_FILTER_DEVICE_TYPE_GAME_CAPTURE_HD60_S    = 5,	//!< Game Capture HD60 S   (USB  VID: 0x0FD9 PID: 0x004F)
		VIDEO_CAPTURE_FILTER_DEVICE_TYPE_GAME_CAPTURE_HD60      = 8,	//!< Game Capture HD60     (USB  VID: 0x0FD9 PID: 0x005C)
		VIDEO_CAPTURE_FILTER_DEVICE_TYPE_GAME_CAPTURE_HD60_PRO  = 9,	//!< Game Capture HD60 Pro (PCIe PCI\VEN_12AB&DEV_0380&SUBSYS_00031CFA)
		NUM_VIDEO_CAPTURE_FILTER_DEVICE_TYPES
	};

	const int VIDEO_CAPTURE_FILTER_MAX_NUM_DEVICES = 8;


#ifndef SWIG
#pragma pack(push, 4)

	typedef struct _VIDEO_CAPTURE_FILTER_HARDWARE_ID
	{
		VIDEO_CAPTURE_FILTER_BUS_TYPES		busType;
		union UNION
		{
			struct USB_ID
			{
				WORD vendorId;
				WORD productId;
			}Usb;
			struct PCI_ID
			{
				WORD vendorId;
				WORD deviceId;
				WORD subVendorId;
				WORD subDeviceId;
			}Pci;
		}u;
	}VIDEO_CAPTURE_FILTER_HARDWARE_ID;

	typedef struct _VIDEO_CAPTURE_FILTER_DEVICE_CAPS
	{
		wchar_t								deviceId[256];							//!< Unique device ID e.g. "vid_0fd9&pid_005c#000c1dbaa4"
		wchar_t								deviceName[256];						//!< Device name e.g. "Game Capture HD60"
		VIDEO_CAPTURE_FILTER_DEVICE_TYPE	deviceType;								//!< Type of Elgato game capture device
		VIDEO_CAPTURE_FILTER_HARDWARE_ID	hwId;

		bool								hardwareDeliversRawVideoData;			//!< if true, this is the preferred pin for preview
		bool								hardwareDeliversCompressedVideoData;	//!< if true, data on the MPEG-TS pin are provided by a hardware encoder

		int									availableVideoInputs;					//!< Video inputs available for this device
		int									availableAudioInputs;					//!< Audio inputs available for this device

		BYTE								reserved[54];							//!< for later use
	}VIDEO_CAPTURE_FILTER_DEVICE_CAPS;

	typedef struct _VIDEO_CAPTURE_FILTER_DEVICELIST
	{
		int numAvailableDevices;
		int selectedDeviceIndex;
		VIDEO_CAPTURE_FILTER_DEVICE_CAPS caps[VIDEO_CAPTURE_FILTER_MAX_NUM_DEVICES];
	}VIDEO_CAPTURE_FILTER_DEVICELIST;

#pragma pack(pop)  // reset struct alignment
#endif // #ifndef SWIG

	/*=============================================================================
	// Types for IElgatoVideoCaptureFilter
	=============================================================================*/

	//! Input device
	typedef enum VIDEO_CAPTURE_FILTER_INPUT_DEVICE
	{
		VIDEO_CAPTURE_FILTER_INPUT_DEVICE_INVALID 		= 0,			//!< Invalid
		VIDEO_CAPTURE_FILTER_INPUT_DEVICE_XBOX360 		= 1,			//!< Microsoft Xbox 360
		VIDEO_CAPTURE_FILTER_INPUT_DEVICE_PLAYSTATION3	= 2,			//!< Sony PlayStation 3
		VIDEO_CAPTURE_FILTER_INPUT_DEVICE_IPAD 			= 3,			//!< Apple iPad
		VIDEO_CAPTURE_FILTER_INPUT_DEVICE_IPOD_IPHONE	= 4,			//!< Apple iPod or iPhone
		VIDEO_CAPTURE_FILTER_INPUT_DEVICE_WII 			= 5,			//!< Nintendo Wii
		VIDEO_CAPTURE_FILTER_INPUT_DEVICE_OTHER 		= 6,			//!< Other
		VIDEO_CAPTURE_FILTER_INPUT_DEVICE_WII_U 		= 7,			//!< Nintendo Wii U
		VIDEO_CAPTURE_FILTER_INPUT_DEVICE_XBOX_ONE 		= 8,			//!< Microsoft Xbox One
		VIDEO_CAPTURE_FILTER_INPUT_DEVICE_PLAYSTATION4	= 9,			//!< Sony PlayStation 4
		// EXTEND_INPUT_DEVICE
		NUM_VIDEO_CAPTURE_FILTER_INPUT_DEVICES
	};

	//! Video inputs
	typedef enum VIDEO_CAPTURE_FILTER_VIDEO_INPUT
	{
		VIDEO_CAPTURE_FILTER_VIDEO_INPUT_INVALID 		= 0,			//!< Invalid
		VIDEO_CAPTURE_FILTER_VIDEO_INPUT_COMPOSITE 		= 1,			//!< Composite
		VIDEO_CAPTURE_FILTER_VIDEO_INPUT_SVIDEO 		= 2,			//!< S-Video
		VIDEO_CAPTURE_FILTER_VIDEO_INPUT_COMPONENT 		= 3,			//!< Component
		VIDEO_CAPTURE_FILTER_VIDEO_INPUT_HDMI 			= 4,			//!< HDMI
	};

	//! Video encoder profile
	typedef enum VIDEO_CAPTURE_FILTER_VID_ENC_PROFILE
	{
		VIDEO_CAPTURE_FILTER_VID_ENC_PROFILE_INVALID	= 0x00000000,	//!< Invalid
		VIDEO_CAPTURE_FILTER_VID_ENC_PROFILE_240 		= 0x00000001,	//!< 320x240
		VIDEO_CAPTURE_FILTER_VID_ENC_PROFILE_360 		= 0x00000002,	//!< 480x360
		VIDEO_CAPTURE_FILTER_VID_ENC_PROFILE_480 		= 0x00000004,	//!< 640x480
		VIDEO_CAPTURE_FILTER_VID_ENC_PROFILE_720 		= 0x00000008,	//!< 1280x720
		VIDEO_CAPTURE_FILTER_VID_ENC_PROFILE_1080 		= 0x00000010,	//!< 1920x1080
	};

	//! Color range
	typedef enum VIDEO_CAPTURE_FILTER_COLOR_RANGE
	{
		VIDEO_CAPTURE_FILTER_COLOR_RANGE_INVALID		= 0,			//!< Invalid
		VIDEO_CAPTURE_FILTER_COLOR_RANGE_FULL			= 1,			//!< 0-255
		VIDEO_CAPTURE_FILTER_COLOR_RANGE_LIMITED		= 2,			//!< 16-235
		VIDEO_CAPTURE_FILTER_COLOR_RANGE_SHOOT			= 3,			//!<
	};

	//! Settings
	typedef struct _VIDEO_CAPTURE_FILTER_SETTINGS
	{
		wchar_t									deviceName[256];				//!< Device name (get only)
		VIDEO_CAPTURE_FILTER_INPUT_DEVICE		inputDevice;					//!< Input device (e.g. Xbox360)
		VIDEO_CAPTURE_FILTER_VIDEO_INPUT		videoInput;						//!< Video input (e.g. HDMI)
		VIDEO_CAPTURE_FILTER_VID_ENC_PROFILE	profile;						//!< Video encoder profile (maximum resolution)
		BOOL									useAnalogAudioInput;			//!< for HDMI with analog audio input
		VIDEO_CAPTURE_FILTER_COLOR_RANGE		hdmiColorRange;					//!< HDMI color range
		int										brightness;						//!< Brightness (0-10000)
		int										contrast;						//!< Contrast   (0-10000)
		int										saturation;						//!< Saturation (0-10000)
		int										hue;							//!< Hue        (0-10000)
		int										analogAudioGain;				//!< Analog audio gain  (-60 - 12 dB)
		int										digitalAudioGain;				//!< Digital audio gain (-60 - 12 dB)
		BOOL									preserveInputFormat;			//!< Input Format will be preserved (e.g. do not convert interlaced to progressive)
		BOOL									stretchStandardDefinitionInput;	//!< Stretch SD input to 16:9
	}VIDEO_CAPTURE_FILTER_SETTINGS, *PVIDEO_CAPTURE_FILTER_SETTINGS;
	typedef const VIDEO_CAPTURE_FILTER_SETTINGS* PCVIDEO_CAPTURE_FILTER_SETTINGS;



	//! Notification messages
	typedef enum VIDEO_CAPTURE_FILTER_NOTIFICATION
	{
		//! Current device was removed
		VIDEO_CAPTURE_FILTER_NOTIFICATION_DEVICE_REMOVED				= 101,	//!< Data: none

		//! Delay of the device has changed. Call GetDelayMs() to get the new delay.
		VIDEO_CAPTURE_FILTER_NOTIFICATION_DEVICE_DELAY_CHANGED			= 110,	//!< Data: none

		//! Output format has changed. Update your signal path accordingly.
		VIDEO_CAPTURE_FILTER_NOTIFICATION_CAPTURE_OUTPUT_FORMAT_CHANGED = 305,	//!< Data: none

		//! Video signal state has changed (e.g. present or lost)
		VIDEO_CAPTURE_FILTER_NOTIFICATION_VIDEO_SIGNAL_STATE			= 401,	//!< Data: VIDEO_CAPTURE_FILTER_VIDEO_SIGNAL_STATE
	};

	//! Video signal state
	typedef enum VIDEO_CAPTURE_FILTER_VIDEO_SIGNAL_STATE
	{
		VIDEO_CAPTURE_FILTER_VIDEO_SIGNAL_STATE_PRESENT					= 0,	//!< Video present
		VIDEO_CAPTURE_FILTER_VIDEO_SIGNAL_STATE_LOST					= 1,	//!< No video
		VIDEO_CAPTURE_FILTER_VIDEO_SIGNAL_STATE_NOT_SUPPORTED			= 2,	//!< Format not supported (e.g. resolution or framerate to high)
		VIDEO_CAPTURE_FILTER_VIDEO_SIGNAL_STATE_DOLBY_NOT_SUPPORTED		= 3,	//!< Audio format not supported (Dolby Digital or Dolby)
		VIDEO_CAPTURE_FILTER_VIDEO_SIGNAL_STATE_DEVICE_IN_USE			= 4		//!< Device is in use by another application
	};


	//! Extended Settings
	typedef struct _VIDEO_CAPTURE_FILTER_SETTINGS_EX
	{
		VIDEO_CAPTURE_FILTER_SETTINGS		Settings;

		BOOL								enableFullFrameRate;				//!< Enable full frame rate (50/59.94/60 fps)
		INT32								deviceIndex;						//!< Capture device index
		BOOL								audioGainshowDb;					//!< % or dB display for audio gain

		wchar_t								lastSkippedUpdate[MAX_PATH];		//!< Update check: Version number of the last skipped update
		BOOL								checkForBetaVersions;				//!< Update check: Check for beta versions during auto update
		
		BYTE								reserved[20 * 1024 - sizeof(BOOL) - sizeof(INT32) - sizeof(BOOL) - (sizeof(wchar_t) * MAX_PATH) - sizeof(BOOL)];
		DWORD								interfaceVersion;					//!< Clients need to set this value to VIDEO_CAPTURE_FILTER_INTERFACE_VERSION
	}VIDEO_CAPTURE_FILTER_SETTINGS_EX, *PVIDEO_CAPTURE_FILTER_SETTINGS_EX;
	typedef const VIDEO_CAPTURE_FILTER_SETTINGS_EX* PCVIDEO_CAPTURE_FILTER_SETTINGS_EX;

}

