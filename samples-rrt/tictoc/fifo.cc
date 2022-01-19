#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <vector>

using namespace omnetpp;
using namespace std;

#include "myPacket_m.h"

// S&W state machine
const short idle = 0;
const short sending = 1;
const short waitingAck = 2;

class Fifo : public cSimpleModule
{
    private:
        short state = -1;
        long int numQueuedPackets = -1;
        long int numSeqLink = -1;
    protected:
        cQueue queue;
        cMessage *lastPacketSent;
        cMessage *txFinishedMsg = new cMessage("Packet finished its transmission");
        cMessage *timeoutMsg = new cMessage("Timeout!");
        double timeout;
        virtual void forwardMessage(cMessage *msg);
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void refreshDisplay() const override;
};

Define_Module(Fifo);

void Fifo::initialize()
{
    state = idle;
    numQueuedPackets = 0;
    numSeqLink = 0;
    timeout = par("timeout");
}

void Fifo::handleMessage(cMessage *msg)
{
    int arrivalGateId = msg->getArrivalGateId();

    if(msg->isSelfMessage()) {
        //EV << "SELF MESSAGE\n";
        if(msg == txFinishedMsg) {
            //EV << "SELF MESSAGE - TX FINISHED\n";
            // Last data packet sent finished its transmission
            state = waitingAck;
            // Set timeout
            simtime_t timeoutExpireTime = simTime() + timeout;
            scheduleAt(timeoutExpireTime, timeoutMsg);
        } else if(msg == timeoutMsg) {
            //EV << "SELF MESSAGE - TIMEOUT\n";
            // Timeout expired waiting for ACK, resend last packet
            bubble("Timeout expired!");
            state = sending;
            forwardMessage(lastPacketSent);
        }


    } else if(arrivalGateId == gate("gateNode")->getId()) {
        // Data packet arrived

        MyPacket *pkt = check_and_cast<MyPacket *>(msg);
        pkt->setLinkSeqNum(numSeqLink);
        numSeqLink++;

        if(gate("gateLink$o")->getTransmissionChannel()->isBusy() || state != idle) {
            // If channel is busy or state is not idle, add it to queue
            queue.insert(pkt);
            numQueuedPackets++;
        } else {
            // If channel is not busy and state is idle, send it
            state = sending;
            forwardMessage(pkt);
        }
    } else if(arrivalGateId == gate("gateLink$i")->getId()) {
        // ACK arrived
        delete msg;
        if(state == waitingAck) {
            cancelEvent(timeoutMsg);
            if(!queue.isEmpty()) {
                cMessage *nextMsg = check_and_cast<cMessage *>(queue.pop());
                numQueuedPackets--;
                state = sending;
                forwardMessage(nextMsg);
            } else {
                state = idle;
            }
        }
    }
}

void Fifo::forwardMessage(cMessage *msg)
{
    send(msg, "gateLink$o");
    lastPacketSent = msg->dup();
    //EV << "Packet sent\n";
    simtime_t finishTime = gate("gateLink$o")->getTransmissionChannel()->getTransmissionFinishTime();
    scheduleAt(finishTime, txFinishedMsg);
    //EV << "Packet will finish its transmission at " << finishTime << "\n";
}

void Fifo::refreshDisplay() const
{
    char buf[400];
    char stateString[20];
    switch(state) {
        case idle:
            sprintf(stateString, "idle");
            break;
        case sending:
            sprintf(stateString, "sending packet");
            break;
        case waitingAck:
            sprintf(stateString, "waiting ACK");
            break;
        default:
            sprintf(stateString, "unknown");
    }
    sprintf(buf, "state %s\ncontains %ld packets", stateString, numQueuedPackets);
    getDisplayString().setTagArg("t", 0, buf);
}
