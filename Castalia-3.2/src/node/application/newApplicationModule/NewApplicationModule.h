/*=============================================================*/
/*This application module is written for Sybil Attack Detection*/
/*=============================================================*/


#ifndef _NEWAPPLICATIONMODULE_H_
#define _NEWAPPLICATIONMODULE_H_

#include "VirtualApplication.h"
#include "MyPacket_m.h"

using namespace std;

//Timers

enum NewApplicationModuleTimers {
	SEND_BROADCAST_PACKET = 1
	SEND_REPORT_PACKET = 2
};

class NewApplicationModule: public VirtualApplication {
 private:
	double packet_rate;
	string recipientAddress;
	double startupDelay;

	float packet_spacing;
	int dataSN;

 protected:
	void startup();
	void fromNetworkLayer(ApplicationPacket * rcvPacket,const char *source, double rssi, double lqi);
	void handleRadioControlMessage(RadioControlMessage *);
	void timerFiredCallback(int);
};

#endif				// _NEWAPPLICATIONMODULE_APPLICATIONMODULE_H_
