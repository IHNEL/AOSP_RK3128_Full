#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <cutils/properties.h>
#include <cutils/log.h>
#include <linux/ioctl.h>
#include <linux/uinput.h>
#include <string.h>
#include <sys/prctl.h>
#include <poll.h>
#include "Cecmanager.h"

//#define LOG_TAG "Cecmanager"

HdmicecManage* HdmicecManage::m_pInstance = NULL;


#define HDMI_CEC_MAGIC 'N'
#define HDMI_IOCTL_CECSEND   _IOW(HDMI_CEC_MAGIC, 0 ,struct cec_framedata)
#define HDMI_IOCTL_CECENAB   _IOW(HDMI_CEC_MAGIC, 1, int)
#define HDMI_IOCTL_CECPHY    _IOR(HDMI_CEC_MAGIC, 2, int)
#define HDMI_IOCTL_CECLOGIC  _IOR(HDMI_CEC_MAGIC, 3, int)
#define HDMI_IOCTL_CECREAD   _IOR(HDMI_CEC_MAGIC, 4, struct cec_framedata)
#define HDMI_IOCTL_CECSETLA  _IOW(HDMI_CEC_MAGIC, 5, int)
#define HDMI_IOCTL_CECCLEARLA  _IOW(HDMI_CEC_MAGIC, 6, int)
#define HDMI_IOCTL_CECWAKESTATE  _IOR(HDMI_CEC_MAGIC, 7, int)

HdmicecManage::HdmicecManage() {
	ALOGI("%s", __func__);
	property_set("sys.hdmicec.version", HDMI_CEC_HAL_VERSION);
	dev_fd = open(HDMI_DEV_PATH, O_RDWR);
	if (dev_fd < 0) {
		ALOGE("%s:not able to open dev cec", __func__);
		return;
	}

	stat_fd = open(HDMI_STATE_PATH, O_RDONLY);
	if (stat_fd < 0) {
		ALOGE("%s:not able to open cec state", __func__);
		return;
	}

	input_fd = open(INPUT_DEV_PATH, O_RDWR);
	if (input_fd < 0) {
		ALOGE("%s:not able to open input event0", __func__);
		return;
	}
	mflag = true;
}

int HdmicecManage::isconnected() {
	int hdmi_state;
	int ret, fd;
	char statebuf[2];

	ALOGI("%s", __func__);
	memset(statebuf, 0, sizeof(statebuf));
	fd = open(HDMI_CONNECT_PATH, O_RDONLY);
	if (fd < 0){
		ALOGE("%s open error", __func__);
		return -1;
	}
	ret = read(fd, statebuf, sizeof(statebuf));
	close(fd);
	if (ret < 0) {
		ALOGE("%s read error", __func__);
		return -1;
	}
	hdmi_state = atoi(statebuf);
	ALOGI("hdmi_state = %d", hdmi_state);
	if (hdmi_state == 1)
		return HDMI_CONNECTED;
	else
		return HDMI_NOT_CONNECTED;
	return HDMI_NOT_CONNECTED;
}

void HdmicecManage::clearlogicaddress() {
	int ret;

	ALOGI("%s", __func__);
	ret = ioctl(dev_fd, HDMI_IOCTL_CECCLEARLA, NULL);
	if (ret)
		ALOGE("%s clear logic addresss error", __func__);
}

int HdmicecManage::sendmessage(cec_framedata cecframe) {
	int ret, i, cecwakestate;
	struct cec_framedata cecframe_temp;

	ALOGI("%s", __func__);
	ret = 0;
	i = 30;
	cecframe_temp = cecframe;
	while(i--) {
		ret = ioctl(dev_fd, HDMI_IOCTL_CECWAKESTATE, &cecwakestate);
		if (cecwakestate != 0) {
			ALOGI("cecwakestate = %d , i = %d", cecwakestate, i);
			usleep(40*1000);
		} else {
			break;
		}
		if (i == 0) {
			ALOGE("%s i = %d HDMI_RESULT_FAIL", __func__, i);
			return -1;
		}
	}

	if (cecframe.argcount > 15)
		cecframe.argcount = 0;

	ret = ioctl(dev_fd, HDMI_IOCTL_CECSEND, &cecframe);
	if (ret)
		return -1;
	if (cecframe.returnval == CEC_SEND_SUCCESS) {
		return CEC_SEND_SUCCESS;
	} else if ((cecframe.returnval == CEC_SEND_NACK) ||
	(cecframe.returnval == CEC_SEND_NACK)) {
		ret = ioctl(dev_fd, HDMI_IOCTL_CECSEND, &cecframe_temp);
		if (ret)
			return -1;
	}

	return cecframe_temp.returnval;
}

int HdmicecManage::pinglogicaddress(char logic_addr) {
	struct cec_framedata cecframe;
	int ret;

	ALOGI("%s", __func__);
	memset(&cecframe, 0, sizeof(struct cec_framedata));
	cecframe.srcdestaddr = logic_addr << 4 | logic_addr;
	cecframe.argcount = 0;
	ret = sendmessage(cecframe);
	return ret;
}

int HdmicecManage::addlogicaddress(int logic_addr) {
	int ret;

	ALOGI("%s", __func__);
	ret = ioctl(dev_fd, HDMI_IOCTL_CECSETLA, &logic_addr);
	if (ret) {
		ALOGE("%s add logic address error", __func__);
		return -1;
	}
	return 0;
}

int HdmicecManage::getphysicaladdress() {
	int ret;
	int val;

	ALOGI("%s", __func__);
	val = 0;
	ret = ioctl(dev_fd, HDMI_IOCTL_CECPHY, &val);
	address_phy = val;
	ALOGI("%s address_phy = 0x%x", __func__, val);
	if (ret < 0 )
		return -1;
	else
		return val;
}

int HdmicecManage::sendimageview() {
	struct cec_framedata cecframe;
	int ret;

	ALOGI("%s", __func__);
	memset(&cecframe, 0, sizeof(struct cec_framedata));
	cecframe.opcode 	   = CECOP_IMAGE_VIEW_ON;
	cecframe.srcdestaddr   = MAKE_SRCDEST(address_logic,
						CEC_LOGADDR_TV);
	cecframe.argcount	   = 0;
	ret = sendmessage(cecframe);
	return ret;
}

int HdmicecManage::sendactivesource() {
	struct cec_framedata cecframe;
	int ret;

	ALOGI("%s", __func__);
	memset(&cecframe, 0, sizeof(struct cec_framedata));
	cecframe.opcode        = CECOP_ACTIVE_SOURCE;
	cecframe.srcdestaddr   = MAKE_SRCDEST(address_logic,
						CEC_LOGADDR_UNREGORBC);
	cecframe.args[0]       = (address_phy & 0xFF00) >> 8;
	cecframe.args[1]       = (address_phy & 0x00FF);
	cecframe.argcount      = 2;
	ret = sendmessage(cecframe);
	return ret;
}

void HdmicecManage::inputkey(int keycode) {
	struct input_event event;
	int ret;

	memset(&event, 0, sizeof(event));
	event.type = EV_KEY;
	event.code = keycode;
	event.value = 1;
	ret = write(input_fd, &event, sizeof(event));
	if (ret < 0) {
		ALOGE("write /dev/input/event0  press error");
		return;
	}
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	ret = write(input_fd, &event, sizeof(event));
	if (ret < 0) {
		ALOGE("write /dev/input/event0 sysc error");
		return;
	}


	memset(&event, 0, sizeof(event));
	event.type = EV_KEY;
	event.code = keycode;
	event.value = 0;
	ret = write(input_fd, &event, sizeof(event));
	if (ret < 0) {
		ALOGE("write /dev/input/event0 release error");
		return;
	}

	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	ret = write(input_fd, &event, sizeof(event));
	if (ret < 0) {
		ALOGE("write /dev/input/event0 sync error");
		return;
	}
}

void HdmicecManage::uicontrol(int opcode) {
	if (input_fd > 0){
		switch (opcode) {
		case scecmakesure:
			inputkey(KEY_REPLY);
			break;
		case scecup:
			inputkey(KEY_UP);
			break;
		case scecdown:
			inputkey(KEY_DOWN);
			break;
		case scecleft:
			inputkey(KEY_LEFT);
			break;
		case scecright:
			inputkey(KEY_RIGHT);
			break;
		case scecback:
			inputkey(KEY_BACK);
			break;
		case scecvendorback:
			inputkey(KEY_BACK);
			break;
		}
	}
}

void HdmicecManage::cecenumeration() {
	char logicaddress[3] =
				{CEC_LOGADDR_PLAYBACK1,
				CEC_LOGADDR_PLAYBACK2,
				CEC_LOGADDR_PLAYBACK3};
	int i;
	int trynum;
	int rtvalue;
	int availablecnt;

	ALOGI("%s", __func__);
	if ((dev_fd < 0) || (input_fd < 0) || (stat_fd < 0)) {
		ALOGI("dev_fd = %d, input_fd = %d,stat_fd = %d",
			dev_fd, input_fd, stat_fd);
		return;
	}



	if (getphysicaladdress() > 0) {
		for (i = 0; i < 3; i++) {
			trynum = 0;
			rtvalue = 0;
			availablecnt = 0;
			for(trynum = 0; trynum < 3; trynum ++) {
				 rtvalue = pinglogicaddress(logicaddress[i]);
				 if(rtvalue == 1) {
					availablecnt ++;
					ALOGI("availableCnt: %d\n", availablecnt);
				 }
				 usleep(5);
			}
			if (availablecnt > 1) {
				address_logic = logicaddress[i];
				ALOGI("Logic Address is 0x%x\n",
						address_logic);
				break;
			}
		}
		if (i == 3)
			address_logic = CEC_LOGADDR_UNREGORBC;
		ALOGI("address_logic = %d", address_logic);
		addlogicaddress(address_logic);
		sendimageview();
		sendactivesource();
	}
}

bool HdmicecManage::validatececmessage(struct cec_framedata *pcpi)
{
	char parametercount = 0;
	bool countok = true;

	/* Determine required parameter count   */
	ALOGI("%s", __func__);
	switch (pcpi->opcode) {
	case CECOP_IMAGE_VIEW_ON:
	case CECOP_TEXT_VIEW_ON:
	case CECOP_STANDBY:
	case CECOP_GIVE_PHYSICAL_ADDRESS:
	case CECOP_GIVE_DEVICE_POWER_STATUS:
	case CECOP_GET_MENU_LANGUAGE:
	case CECOP_GET_CEC_VERSION:
		parametercount = 0;
		break;
	case CECOP_REPORT_POWER_STATUS:         /* power status*/
	case CECOP_CEC_VERSION:                 /* cec version*/
		parametercount = 1;
		break;
	case CECOP_INACTIVE_SOURCE:             /* physical address*/
	case CECOP_FEATURE_ABORT:
	case CECOP_ACTIVE_SOURCE:               /* physical address*/
		parametercount = 2;
		break;
	case CECOP_REPORT_PHYSICAL_ADDRESS:
	case CECOP_DEVICE_VENDOR_ID:            /* vendor id*/
		parametercount = 3;
		break;
	case CECOP_SET_OSD_NAME:                /* osd name (1-14 bytes)*/
	case CECOP_SET_OSD_STRING:
		parametercount = 1;    /* must have a minimum of 1 operands*/
		break;
	case CECOP_ABORT:
		break;
	case CECOP_ARC_INITIATE:
		break;
	case CECOP_ARC_REPORT_INITIATED:
		break;
	case CECOP_ARC_REPORT_TERMINATED:
		break;
	case CECOP_ARC_REQUEST_INITIATION:
		break;
	case CECOP_ARC_REQUEST_TERMINATION:
		break;
	case CECOP_ARC_TERMINATE:
		break;
	default:
		break;
	}

	/* Test for correct parameter count.    */

	if (pcpi->argcount < parametercount)
		countok = false;

	return countok;
}

int HdmicecManage::cecrxmsghandlerlast(struct cec_framedata *pcpi)
{
	bool			isdirectaddressed;
	struct cec_framedata	cecframe;

	ALOGI("%s", __func__);
	memset(&cecframe, 0, sizeof(struct cec_framedata));
	isdirectaddressed = !((pcpi->srcdestaddr & 0x0F) ==
					CEC_LOGADDR_UNREGORBC);
	ALOGI("isdirectaddressed %d\n", (int)isdirectaddressed);
	if (validatececmessage(pcpi)) {
		/* If invalid message, ignore it, but treat it as handled */
	if (isdirectaddressed) {
		switch (pcpi->opcode) {
		case CECOP_USER_CONTROL_PRESSED:
			uicontrol(pcpi->args[0]);
			break;

		case CECOP_VENDOR_REMOTE_BUTTON_DOWN:
			uicontrol(pcpi->args[0]);
			break;
		case CECOP_FEATURE_ABORT:
			//CecHandleFeatureAbort(pcpi);
			break;

		case CECOP_GIVE_OSD_NAME:
			cecframe.opcode        = CECOP_SET_OSD_NAME;
			cecframe.srcdestaddr =
				MAKE_SRCDEST(address_logic,
							CEC_LOGADDR_TV);
			cecframe.args[0]  = 'R';
			cecframe.args[1]  = 'K';
			cecframe.args[2]  = '-';
			cecframe.args[3]  = 'B';
			cecframe.args[4]  = 'O';
			cecframe.args[5]  = 'X';
			cecframe.argcount = 6;
			sendmessage(cecframe);
			break;

		case CECOP_VENDOR_COMMAND_WITH_ID:

		if (pcpi->args[2] == 00) {
			cecframe.opcode        = CECOP_SET_OSD_NAME;
			cecframe.srcdestaddr =
				MAKE_SRCDEST(address_logic,
							CEC_LOGADDR_TV);
			cecframe.args[0]  = '1';
			cecframe.args[1]  = '1';
			cecframe.args[2]  = '1';
			cecframe.args[3]  = '1';
			cecframe.args[4]  = '1';
			cecframe.args[5]  = '1';
			cecframe.argcount = 6;
			sendmessage(cecframe);
			}
			break;
		case CECOP_IMAGE_VIEW_ON:
		case CECOP_TEXT_VIEW_ON:
		/* In our case, respond the same to both these messages*/
		    break;

		case CECOP_GIVE_DEVICE_VENDOR_ID:
			cecframe.opcode        = CECOP_DEVICE_VENDOR_ID;
			cecframe.srcdestaddr   =
				MAKE_SRCDEST(address_logic,
						CEC_LOGADDR_UNREGORBC);
			cecframe.args[0]       = 0x1;
			cecframe.args[1]       = 0x2;
			cecframe.args[2]       = 0x3;
			cecframe.argcount      = 3;
			sendmessage(cecframe);
			break;

		case CECOP_STANDBY:             /* Direct and Broadcast*/
		/* Setting this here will let the main task know    */
		/* (via SI_CecGetPowerState) and at the same time   */
		/* prevent us from broadcasting a STANDBY message   */
		/* of our own when the main task responds by        */
		/* calling SI_CecSetPowerState( STANDBY );          */
			//cec_dev->powerstatus = CEC_POWERSTATUS_STANDBY;
		ALOGI("CECOP_STANDBY");
		inputkey(KEY_POWER);
			break;

		case CECOP_INACTIVE_SOURCE:
			//CecHandleInactiveSource(pcpi);
			break;

		case CECOP_GIVE_PHYSICAL_ADDRESS:

			cecframe.opcode        = CECOP_REPORT_PHYSICAL_ADDRESS;
			cecframe.srcdestaddr   =
				MAKE_SRCDEST(address_logic,
						CEC_LOGADDR_UNREGORBC);
			cecframe.args[0]   = (address_phy & 0xFF00) >> 8;
			cecframe.args[1]       = (address_phy & 0x00FF);
			cecframe.args[2]       = address_logic;
			cecframe.argcount      = 3;
			sendmessage(cecframe);
			break;

		case CECOP_GIVE_DEVICE_POWER_STATUS:
		/* TV responds with power status.   */

			cecframe.opcode        = CECOP_REPORT_POWER_STATUS;
			cecframe.srcdestaddr   =
				MAKE_SRCDEST(address_logic,
					(pcpi->srcdestaddr & 0xF0) >> 4);
			//powerstatus =  0x00;
			cecframe.args[0]       = 0x00;
			cecframe.argcount      = 1;
			sendmessage(cecframe);
			break;

		case CECOP_GET_MENU_LANGUAGE:
		/* TV Responds with a Set Menu language command.    */

			cecframe.opcode         = CECOP_SET_MENU_LANGUAGE;
			cecframe.srcdestaddr    =
				MAKE_SRCDEST(address_logic,
						CEC_LOGADDR_UNREGORBC);
			cecframe.args[0]        = 'e';
			cecframe.args[1]        = 'n';
			cecframe.args[2]        = 'g';
			cecframe.argcount       = 3;
			sendmessage(cecframe);
			break;

		case CECOP_GET_CEC_VERSION:
		/* TV responds to this request with it's CEC version support.*/

			cecframe.srcdestaddr   =
				MAKE_SRCDEST(address_logic,
						CEC_LOGADDR_TV);
			cecframe.opcode        = CECOP_CEC_VERSION;
			cecframe.args[0]       = 0x05;       /* Report CEC1.4b*/
			cecframe.argcount      = 1;
			sendmessage(cecframe);
			break;

		case CECOP_REPORT_POWER_STATUS:
		/*Someone sent us their power state.

			l_sourcePowerStatus = pCpi->args[0];

			let NEW SOURCE task know about it.

			if ( l_cecTaskState.task == SI_CECTASK_NEWSOURCE )
			{
			l_cecTaskState.cpiState = CPI_RESPONSE;
			}*/
			 break;

		/* Do not reply to directly addressed 'Broadcast' msgs.  */
		case CECOP_REQUEST_ACTIVE_SOURCE:
			sendactivesource();
			break;

		case CECOP_ACTIVE_SOURCE:
		case CECOP_REPORT_PHYSICAL_ADDRESS:
		case CECOP_ROUTING_CHANGE:
		case CECOP_ROUTING_INFORMATION:
		case CECOP_SET_STREAM_PATH:
		case CECOP_SET_MENU_LANGUAGE:
		case CECOP_DEVICE_VENDOR_ID:
			break;

		case CECOP_ABORT:
			break;
		default:
		/*CecSendFeatureAbort(pCpi, CECAR_UNRECOG_OPCODE);*/
			break;
			}
		} else {
			/* Respond to broadcast messages.   */
			switch (pcpi->opcode) {
			case CECOP_STANDBY:
			/* Setting this here will let the main task know    */
			/* (via SI_CecGetPowerState) and at the same time   */
			/* prevent us from broadcasting a STANDBY message   */
			/* of our own when the main task responds by        */
			/* calling SI_CecSetPowerState( STANDBY );          */
				ALOGI("CECOP_STANDBY");
				inputkey(KEY_POWER);
				break;

			case CECOP_ACTIVE_SOURCE:
				/*CecHandleActiveSource( pCpi );*/
				break;
		/* Do not reply to 'Broadcast' msgs that we don't need.*/
			case CECOP_REQUEST_ACTIVE_SOURCE:
				sendactivesource();
				break;
			case CECOP_ROUTING_CHANGE:
			case CECOP_ROUTING_INFORMATION:
			case CECOP_SET_STREAM_PATH:
			case CECOP_SET_MENU_LANGUAGE:
				break;
			}
		}
	}

	return 0;
}

void* HdmicecManage::uevent_loop(void *param)
{
	const int MAX_DATA = 64;
	static char vdata[MAX_DATA];
	char thread_name[64] = HDMI_CEC_UEVENT_THREAD_NAME;
	struct pollfd pfd[2];
	struct cec_framedata cecframe;

	ALOGI("%s", __func__);
	HdmicecManage *mHdmicecManage = HdmicecManage::GetInstance();
	if (mHdmicecManage->dev_fd < 0) {
		ALOGE("fd is NULL!");
		delete mHdmicecManage;
		return NULL;
	}
	prctl(PR_SET_NAME, (unsigned long) &thread_name, 0, 0, 0);
	//setpriority(PRIO_PROCESS, 0, HAL_PRIORITY_URGENT_DISPLAY);

	// Read once from the fds to clear the first notify
	pread(mHdmicecManage->stat_fd, vdata , MAX_DATA, 0);
	pfd[0].fd = mHdmicecManage->stat_fd;
	if (pfd[0].fd >= 0)
		pfd[0].events = POLLPRI | POLLERR;
	do {
		int err = poll(pfd, 1, -1);
		if(err > 0) {
			if (pfd[0].revents & POLLPRI) {
				int len = pread(pfd[0].fd, vdata, MAX_DATA, 0);
				if (len < 0) {
					// If the read was just interrupted - it is not a
					// fatal error. Just continue in this case
					ALOGE ("%s: Unable to read cec state %s",
						__FUNCTION__, strerror(errno));
					continue;
				}
				const char *str = vdata;
				int state = strtoull(str, NULL, 0);
				ALOGD("cec state is %d", state);
				memset(&cecframe, 0, sizeof(cecframe));
				if (state == 0 || state == 1) {
					ALOGD("%s sending hotplug: connected = %d ",
						  __FUNCTION__, state);
				} else if (state == 2) {
					int ret = ioctl(mHdmicecManage->dev_fd, HDMI_IOCTL_CECREAD, &cecframe);
					if (ret < 0) {
						ALOGE("%s hdmi cec read error", __FUNCTION__);
						continue;
					}
					mHdmicecManage->cecrxmsghandlerlast(&cecframe);
				}
			}
		} else {
			ALOGE("%s: cec poll failed errno: %s", __FUNCTION__,
					  strerror(errno));
			continue;
		}
		} while (mHdmicecManage->mflag);

	return NULL;
}

void HdmicecManage::init_uevent_thread()
{
	int ret;

	ALOGI("Initializing UEVENT Thread");
	ret = pthread_create(&uevent_thread, NULL, uevent_loop, NULL);
	if (ret) {
		ALOGE("%s: failed to create %s: %s", __FUNCTION__,
			HDMI_CEC_UEVENT_THREAD_NAME, strerror(ret));
	}
}

HdmicecManage::~HdmicecManage() {
	if (dev_fd > 0)
		close(dev_fd);
	if (input_fd > 0)
		close(input_fd);
	if (stat_fd > 0)
		close(stat_fd);
	mflag = false;
	pthread_join(uevent_thread, NULL);
}
