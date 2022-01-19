#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <vector>

using namespace omnetpp;
using namespace std;

#include "myPacket_m.h"

class Fuente : public cSimpleModule
{
  private:
    long numSent;
  protected:
    virtual MyPacket *generatePacket(int flujoSeqNum);
    virtual void forwardMessage(MyPacket *msg);
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;
};

Define_Module(Fuente);

void Fuente::initialize()
{
    numSent = 0;
    MyPacket *msg = generatePacket(0);
    scheduleAt(0.0, msg);
}

MyPacket *Fuente::generatePacket(int flujoSeqNum)
{
    double packetLength = par("packetLength");
    int idFlujo;
    char msgname[20];

    if(getIndex() == 0) {
        idFlujo = 1;
    } else if(getIndex() == 1) {
        idFlujo = 2;
    } else {
        idFlujo = 5;
    }

    sprintf(msgname, "pkt-%d-%d",idFlujo, flujoSeqNum);

    MyPacket *msg = new MyPacket(msgname);

    msg->setBitLength(packetLength);
    msg->setFlowId(idFlujo);
    msg->setFlowSeqNum(flujoSeqNum);

    return msg;
}

void Fuente::handleMessage(cMessage *msg)
{
    MyPacket *srcmsg = check_and_cast<MyPacket *>(msg);

    if (srcmsg->isSelfMessage()) {
        MyPacket *scheduledMsg = generatePacket(srcmsg->getFlowSeqNum()+1);
        simtime_t genInterval = par("packetGenInterval");

        scheduleAt(simTime()+genInterval, scheduledMsg);
        //EV << "Scheduled new message at " << simTime()+genInterval << "\n";

        srcmsg->setInitTime(simTime());
    }
    forwardMessage(srcmsg);
}

void Fuente::forwardMessage(MyPacket *msg)
{
    //EV << "Forwarding message " << msg << "\n";
    numSent++;
    send(msg, "out");
}

void Fuente::refreshDisplay() const
{
    char buf[200];
    sprintf(buf, "sent: %ld", numSent);
    getDisplayString().setTagArg("t", 0, buf);
}
