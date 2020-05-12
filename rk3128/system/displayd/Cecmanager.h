/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _CECMANAGER_H
#define _CECMANAGER_H


struct cec_framedata {
	char srcdestaddr;
	char opcode;
	char args[16];
	char argcount;
	char returnval;
};

#define HDMI_CEC_ENABLE 1

#define HDMI_CEC_VERSION 0x05
#define HDMI_CEC_VENDOR_ID 0x000001
#define HDMI_CEC_PORT_ID 0x000001
#define HDMI_CEC_HAL_VERSION "1.0"


#define HDMI_DEV_PATH   "/dev/cec"
#define HDMI_CONNECT_PATH "/sys/devices/virtual/switch/hdmi/state"
#define INPUT_DEV_PATH  "/dev/input/event0"
#define HDMI_STATE_PATH "/sys/devices/virtual/misc/cec/stat"

#define HDMI_CONNECTED 1
#define HDMI_NOT_CONNECTED 0

#define HDMI_CEC_UEVENT_THREAD_NAME "HdmiCecThread"
#define MAX_CMD_SIZE 16
#define MAKE_SRCDEST(src, dest)    ((src << 4) | dest)

enum {                   /* CEC Messages */
	CECOP_FEATURE_ABORT			= 0x00,
	CECOP_IMAGE_VIEW_ON			= 0x04,
	CECOP_TUNER_STEP_INCREMENT		= 0x05,
	CECOP_TUNER_STEP_DECREMENT		= 0x06,
	CECOP_TUNER_DEVICE_STATUS		= 0x07,
	CECOP_GIVE_TUNER_DEVICE_STATUS		= 0x08,
	CECOP_RECORD_ON				= 0x09,
	CECOP_RECORD_STATUS			= 0x0A,
	CECOP_RECORD_OFF			= 0x0B,
	CECOP_TEXT_VIEW_ON			= 0x0D,
	CECOP_RECORD_TV_SCREEN			= 0x0F,
	CECOP_GIVE_DECK_STATUS			= 0x1A,
	CECOP_DECK_STATUS			= 0x1B,
	CECOP_SET_MENU_LANGUAGE			= 0x32,
	CECOP_CLEAR_ANALOGUE_TIMER		= 0x33,     /* Spec 1.3A */
	CECOP_SET_ANALOGUE_TIMER		= 0x34,     /* Spec 1.3A */
	CECOP_TIMER_STATUS			= 0x35,     /* Spec 1.3A */
	CECOP_STANDBY				= 0x36,
	CECOP_PLAY				= 0x41,
	CECOP_DECK_CONTROL			= 0x42,
	CECOP_TIMER_CLEARED_STATUS		= 0x43,     /* Spec 1.3A */
	CECOP_USER_CONTROL_PRESSED		= 0x44,
	CECOP_USER_CONTROL_RELEASED		= 0x45,
	CECOP_GIVE_OSD_NAME			= 0x46,
	CECOP_SET_OSD_NAME			= 0x47,
	CECOP_SET_OSD_STRING			= 0x64,
	CECOP_SET_TIMER_PROGRAM_TITLE		= 0x67,	/* Spec 1.3A */
	CECOP_SYSTEM_AUDIO_MODE_REQUEST		= 0x70,	/* Spec 1.3A */
	CECOP_GIVE_AUDIO_STATUS			= 0x71,	/* Spec 1.3A */
	CECOP_SET_SYSTEM_AUDIO_MODE		= 0x72,	/* Spec 1.3A */
	CECOP_REPORT_AUDIO_STATUS		= 0x7A,	/* Spec 1.3A */
	CECOP_GIVE_SYSTEM_AUDIO_MODE_STATUS	= 0x7D,	/* Spec 1.3A */
	CECOP_SYSTEM_AUDIO_MODE_STATUS		= 0x7E,	/* Spec 1.3A */
	CECOP_ROUTING_CHANGE			= 0x80,
	CECOP_ROUTING_INFORMATION		= 0x81,
	CECOP_ACTIVE_SOURCE			= 0x82,
	CECOP_GIVE_PHYSICAL_ADDRESS		= 0x83,
	CECOP_REPORT_PHYSICAL_ADDRESS		= 0x84,
	CECOP_REQUEST_ACTIVE_SOURCE		= 0x85,
	CECOP_SET_STREAM_PATH			= 0x86,
	CECOP_DEVICE_VENDOR_ID			= 0x87,
	CECOP_VENDOR_COMMAND			= 0x89,
	CECOP_VENDOR_REMOTE_BUTTON_DOWN		= 0x8A,
	CECOP_VENDOR_REMOTE_BUTTON_UP		= 0x8B,
	CECOP_GIVE_DEVICE_VENDOR_ID		= 0x8C,
	CECOP_MENU_REQUEST			= 0x8D,
	CECOP_MENU_STATUS			= 0x8E,
	CECOP_GIVE_DEVICE_POWER_STATUS		= 0x8F,
	CECOP_REPORT_POWER_STATUS		= 0x90,
	CECOP_GET_MENU_LANGUAGE			= 0x91,
	CECOP_SELECT_ANALOGUE_SERVICE		= 0x92,     /* Spec 1.3A */
	CECOP_SELECT_DIGITAL_SERVICE		= 0x93,
	CECOP_SET_DIGITAL_TIMER			= 0x97,     /* Spec 1.3A */
	CECOP_CLEAR_DIGITAL_TIMER		= 0x99,     /* Spec 1.3A */
	CECOP_SET_AUDIO_RATE			= 0x9A,     /* Spec 1.3A */
	CECOP_INACTIVE_SOURCE			= 0x9D,     /* Spec 1.3A */
	CECOP_CEC_VERSION			= 0x9E,     /* Spec 1.3A */
	CECOP_GET_CEC_VERSION			= 0x9F,     /* Spec 1.3A */
	CECOP_VENDOR_COMMAND_WITH_ID		= 0xA0,     /* Spec 1.3A */
	CECOP_CLEAR_EXTERNAL_TIMER		= 0xA1,     /* Spec 1.3A */
	CECOP_SET_EXTERNAL_TIMER		= 0xA2,     /* Spec 1.3A */
	CDCOP_HEADER				= 0xF8,
	CECOP_ABORT				= 0xFF,

	CECOP_REPORT_SHORT_AUDIO		= 0xA3,     /* Spec 1.4 */
	CECOP_REQUEST_SHORT_AUDIO		= 0xA4,     /* Spec 1.4 */

	CECOP_ARC_INITIATE			= 0xC0,
	CECOP_ARC_REPORT_INITIATED		= 0xC1,
	CECOP_ARC_REPORT_TERMINATED		= 0xC2,

	CECOP_ARC_REQUEST_INITIATION		= 0xC3,
	CECOP_ARC_REQUEST_TERMINATION		= 0xC4,
	CECOP_ARC_TERMINATE			= 0xC5,

};

enum {                /* Operands for <Feature Abort> Opcode */
	CECAR_UNRECOG_OPCODE            = 0x00,
	CECAR_NOT_CORRECT_MODE,
	CECAR_CANT_PROVIDE_SOURCE,
	CECAR_INVALID_OPERAND,
	CECAR_REFUSED
};

enum {                        /* Operands for <Power Status> Opcode */
	CEC_POWERSTATUS_ON              = 0x00,
	CEC_POWERSTATUS_STANDBY         = 0x01,
	CEC_POWERSTATUS_STANDBY_TO_ON   = 0x02,
	CEC_POWERSTATUS_ON_TO_STANDBY   = 0x03,
};

enum {
	EVENT_RX_FRAME,
	EVENT_ENUMERATE,
};

enum {
	CEC_SEND_SUCCESS = 0,
	CEC_SEND_NACK,
	CEC_SEND_BUSY
};

enum {
	CEC_LOGADDR_TV          = 0x00,
	CEC_LOGADDR_RECDEV1     = 0x01,
	CEC_LOGADDR_RECDEV2     = 0x02,
	CEC_LOGADDR_TUNER1      = 0x03,     /* STB1 in Spev v1.3 */
	CEC_LOGADDR_PLAYBACK1   = 0x04,     /* DVD1 in Spev v1.3 */
	CEC_LOGADDR_AUDSYS      = 0x05,
	CEC_LOGADDR_TUNER2      = 0x06,     /* STB2 in Spec v1.3 */
	CEC_LOGADDR_TUNER3      = 0x07,     /* STB3 in Spec v1.3 */
	CEC_LOGADDR_PLAYBACK2   = 0x08,     /* DVD2 in Spec v1.3 */
	CEC_LOGADDR_RECDEV3     = 0x09,
	CEC_LOGADDR_TUNER4      = 0x0A,     /* RES1 in Spec v1.3 */
	CEC_LOGADDR_PLAYBACK3   = 0x0B,     /* RES2 in Spec v1.3 */
	CEC_LOGADDR_RES3        = 0x0C,
	CEC_LOGADDR_RES4        = 0x0D,
	CEC_LOGADDR_FREEUSE     = 0x0E,
	CEC_LOGADDR_UNREGORBC   = 0x0F
};

enum  {
	scecmakesure   = 0x0,
	scecup         = 0x1,
	scecdown       = 0x2,
	scecleft       = 0x3,
	scecright      = 0x4,
	scecback       = 0x0d,
	scecvendorback = 0x91,
};

class HdmicecManage {

public:
	static HdmicecManage *GetInstance()
	{
		if(m_pInstance == NULL)
			m_pInstance = new HdmicecManage();
		return m_pInstance;
	}

public:
	int isconnected();
	void cecenumeration();
	void init_uevent_thread();

	virtual ~HdmicecManage();
private:
	HdmicecManage();
	static HdmicecManage *m_pInstance;
	int stat_fd;
	int dev_fd;
	int input_fd;
	int address_phy;
	int address_logic;
	bool mflag;
	pthread_t uevent_thread;
	int sendmessage(cec_framedata cecframe);
	int getphysicaladdress();
	void setenable();
	void clearlogicaddress();
	int pinglogicaddress(char logic_addr);
	int addlogicaddress(int logic_addr);
	int sendimageview();
	int sendactivesource();
	void inputkey(int keycode);
	void uicontrol(int opcode);
	bool validatececmessage(struct cec_framedata *pcpi);
	int cecrxmsghandlerlast(struct cec_framedata *pcpi);
	static void* uevent_loop(void *param);
};

#endif
