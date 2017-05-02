/*=============================================================*/
/*This application module is written for Sybil Attack Detection*/
/*=============================================================*/

#include "NewApplicationModule.h"
#include <vector>
#include <string>
#include <map>

Define_Module(NewApplicationModule);

/*-----------------------App-specific initialization ---------------------*/
/*Assigning App-specific parameters to the class variables*/

void NewApplicationModule::startup()
{
	packet_rate = par("packet_rate");
	recipientAddress = par("nextRecipient").stringValue();
	startupDelay = par("startupDelay");

	packet_spacing = packet_rate > 0 ? 1 / float (packet_rate) : -1;
	dataSN = 0;			// Broadcast packet count
	rsdataSN = 0;		// Report packet count (Report Packets contain RSSI data) 

	if (packet_spacing > 0 && source.compare(SELF_NETWORK_ADDRESS) == 0)
		setTimer(SEND_BROADCAST_PACKET, packet_spacing + startupDelay);   // Set Timer to broadcast packets
	else
		trace() << "Not sending packets";

	/*Declare the required output*/

	declareOutput("Packets received per node");

}



void NewApplicationModule::fromNetworkLayer(ApplicationPacket * rcvPacket,
		const char *source, double rssi, double lqi)
{
	string packetName(rcvPacket->getName());
	int sequenceNumber = rcvPacket->getSequenceNumber();

	std::vector<double> vr(4) ; 				// vector for handling rssi values from member nodes.
	std::vector<double> iterator i;

	// Reception of Broadcasted Hello message from a Sybil Node 
	
	if(!isSink && packetName.compare(BROADCAST_PACKET) == 1)
	{
		string dest = rcvPacket->getAppNetInfoExchange().destination;
		simtime_t tstamp = rcvPacket->getAppNetInfoExchange().timestamp;

		trace() << dest <<"received broadcast packet #" << sequenceNumber << " from node " << source;
		//collectOutput("Packets received per node", atoi(source));
		
		//Create a new Report Packet with RSSI data values 

		MyPacket *newPkt = new MyPacket("Report packet",APPLICATION_PACKET); //can add timestamp at which the broadcasted packet is created 
		newPkt->setTstamp(tstamp);
		newPkt->setData(rssi);
		pkt->setSequenceNumber(rsdataSN);
		newPkt->setName(REPORT_PACKET);
		toNetworkLayer(REPORT_PACKET, SINK_NETWORK_ADDRESS);
		rsdataSN++;
	}
	else{
			if(isSink && packetName.compare(BROADCAST_PACKET) == 1)
			{
			
				double rsdata = rcvPacket->getData();	
				vr.push_back(rsdata);						//RSSI value received by Sink-Node added

			}
			 
			else
			{
				if(isSink && packetName.compare(REPORT_PACKET) == 1)
				{
					double rmdata = rcvPacket->getData();
					vr.push_back(rmdata);				//RSSI value received by other nodes added
					simtime_t tstampdata = rcvPacket->getTstamp();

					//Map of vectors

					std::map<simtime_t, std::vector<double> > promap;

					promap.insert(pair<simtime_t,std::vector<double> >(tstampdata,vr));
					

				}
				else
					trace() << "unknown packet received: [" << packetName << "]";
			}
			
		}


}

// Packet has to be forwarded to the next hop recipient
	} 
	else {
		ApplicationPacket* fwdPacket = rcvPacket->dup();
		// Reset the size of the packet, otherwise the app overhead will keep adding on
		fwdPacket->setByteLength(0);
		toNetworkLayer(fwdPacket, recipientAddress.c_str());
	}
}




void NewApplicationModule::timerFiredCallback(int index)
{
	switch (index) {
		case SEND_BROADCAST_PACKET:{						// First : Sybil Node broadcasts a Hello Message//
			
			
			if (source.compare(SELF_NETWORK_ADDRESS)==0){ //NodeID = 0 is the Sybil Node
			
				trace() << "Broadcasting a Hello packet #" << dataSN;
				ApplicationPacket *newPkt = createGenericDataPacket(0, dataSN);
				newPkt->setName(BROADCAST_PACKET);
				toNetworkLayer(BROADCAST_PACKET, BROADCAST_NETWORK_ADDRESS);
				dataSN++;
				//setTimer(SEND_REPORT_PACKET, packet_spacing);  //set the TIMER so as to broadcast the packets again after sometime 
			else
				trace() << "Error in Broadcasting";

			break;
		}
	}
}





void NewApplicationModule::handleRadioControlMessage(RadioControlMessage *radioMsg)
{
	switch (radioMsg->getRadioControlMessageKind()) {
		case CARRIER_SENSE_INTERRUPT:
			trace() << "CS Interrupt received! current RSSI value is: " << radioModule->readRSSI();
                        break;
	}	
}
