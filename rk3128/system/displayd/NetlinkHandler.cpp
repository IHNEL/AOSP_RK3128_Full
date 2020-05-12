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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define LOG_TAG "DisplayD"

#include <cutils/log.h>

#include <string.h>
#include <sysutils/NetlinkEvent.h>
#include "Config.h"
#include "NetlinkHandler.h"
#include "NetlinkManager.h"
#include "ResponseCode.h"
#include "Hdcp.h"
#include "Cecmanager.h"

NetlinkHandler::NetlinkHandler(NetlinkManager *nm, int listenerSocket,
		int format) :
	NetlinkListener(listenerSocket, format) {
		mNm = nm;
	}

NetlinkHandler::~NetlinkHandler() {
}

int NetlinkHandler::start() {
	return this->startListener();
}

int NetlinkHandler::stop() {
	return this->stopListener();
}

void NetlinkHandler::onEvent(NetlinkEvent *evt) {
	const char *subsys = evt->getSubsystem();

	if (!subsys) {
		ALOGW("No subsystem found in netlink event");
		return;
	}
	//	ALOGD("subsys %s", subsys);
	if (!strcmp(subsys, "display")) {
#ifdef ANDROID_6
		NetlinkEvent::Action action = evt->getAction();
		if (action == NetlinkEvent::Action::kAdd) {
#else
		int action = evt->getAction();
		if (action == evt->NlActionAdd) {
#endif
			const char *iface = evt->findParam("INTERFACE");
			const char *screen = evt->findParam("SCREEN");
			if(iface && screen)
				ALOGW("iface id %s screen is %s", iface, screen);
			notifyInterfaceAdded(iface, screen);
#ifdef ANDROID_6
		} else if (action == NetlinkEvent::Action::kRemove) {
#else
		} else if (action == evt->NlActionRemove) {
#endif
			const char *iface = evt->findParam("INTERFACE");
			const char *screen = evt->findParam("SCREEN");
			if(iface && screen)
				ALOGW("iface id %s screen is %s", iface, screen);
			notifyInterfaceRemoved(iface, screen);
#ifdef ANDROID_6
		} else if (action == NetlinkEvent::Action::kChange) {
#else
		} else if (action == evt->NlActionChange) {
#endif
			//            evt->dump();
			const char *type = "TRIGGER";//evt->findParam("TRIGGER");
			notifyInterfaceChanged(type, true);
		}
	} else if (!strcmp(subsys, "hdmi_hdcp2")) {
		evt->dump();
#ifdef ANDROID_6
		NetlinkEvent::Action action = evt->getAction();
		if (action == NetlinkEvent::Action::kChange) {
#else
		int action = evt->getAction();
		if (action == evt->NlActionChange) {
#endif
			const char *start = evt->findParam("START");
			if (start)
				rk_hdmi_hdcp2_start();
		}
	}
}

void NetlinkHandler::notifyInterfaceAdded(const char *name, const char *screen) {
	char msg[255];
	int display = 0;
	if(screen && !strcmp(screen, "1"))
		display = 1;

	mNm->mDisplayManager->setHDMIEnable(display);
	snprintf(msg, sizeof(msg), "Iface added %s", name);
	ALOGW("Send msg %s", msg);
	//    mNm->getBroadcaster()->sendBroadcast(ResponseCode::InterfaceChange,
	//            msg, false);

#if defined(RK3228) && defined(ANDROID_4_4)
	if (access(HDMI_DEV_PATH, R_OK | W_OK) == 0) {
		HdmicecManage *mHdmicecManage = HdmicecManage::GetInstance();
		mHdmicecManage->cecenumeration();
	}
#endif
}

void NetlinkHandler::notifyInterfaceRemoved(const char *name, const char *screen) {
	char msg[255];
	int display = 0;
	if(screen && !strcmp(screen, "1"))
		display = 1;
	mNm->mDisplayManager->setHDMIDisable(display);
	snprintf(msg, sizeof(msg), "Iface removed %s", name);
	ALOGW("Send msg %s", msg);
	//    mNm->getBroadcaster()->sendBroadcast(ResponseCode::InterfaceChange,
	//            msg, false);
}

void NetlinkHandler::notifyInterfaceChanged(const char *name, bool isUp) {
	char msg[255];
	snprintf(msg, sizeof(msg), "Iface changed triggered by %s", name);
	if(1/*!strcmp(name, "KEY")*/) {
		//		mNm->mDisplayManager->selectNextIface();
		ALOGD("notifyInterfaceChanged ******************************");
		mNm->getBroadcaster()->sendBroadcast(ResponseCode::InterfaceChange,
				msg, false);
	}
	else if(!strcmp(name, "WAKEUP")) {
#if DISPLAY_UNTIL_WAKEUP
		mNm->mDisplayManager->init();
#endif
	}
}
