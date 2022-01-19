//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

simple Nodo
{
    parameters:
        volatile double pOutputLink1;
        @display("i=block/routing");
    gates:
        input gateSource;
        output gateQueue[];
        inout gateLink[];
}

simple Fuente
{
    parameters:
        volatile double packetGenInterval @unit(s);
        volatile double packetLength;
        @display("i=block/source");
    gates:
        output out;
}

simple Fifo
{
    parameters:
        volatile double timeout @unit(s);
        @display("i=block/queue");
    gates:
        input gateNode;
        inout gateLink;
}

network red
{
    @display("bgb=717,313");
    types:
        channel Channel extends ned.DatarateChannel
        {
            delay = 5ms; // tiempo de propagacion
            datarate = 9600bps;
            //per = 1e-3;
        }
    submodules:
        node[5]: Nodo;
        source[3]: Fuente;
        queue[6]: Fifo;
        
    connections allowunconnected:
        source[0].out --> node[0].gateSource;
        source[1].out --> node[1].gateSource;
        source[2].out --> node[4].gateSource;
        
        node[0].gateQueue++ --> queue[0].gateNode;
        queue[0].gateLink <--> Channel <--> node[1].gateLink++;
        node[0].gateQueue++ --> queue[1].gateNode;
        queue[1].gateLink <--> Channel <--> node[4].gateLink++;
        
        node[1].gateQueue++ --> queue[2].gateNode;
        queue[2].gateLink <--> Channel <--> node[2].gateLink++;
        node[1].gateQueue++ --> queue[3].gateNode;
        queue[3].gateLink <--> Channel <--> node[3].gateLink++;
        
        node[4].gateQueue++ --> queue[4].gateNode;
        queue[4].gateLink <--> Channel <--> node[1].gateLink++;
        node[4].gateQueue++ --> queue[5].gateNode;
        
        queue[5].gateLink <--> Channel <--> node[3].gateLink++;
}
