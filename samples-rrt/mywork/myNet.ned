//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple Tic9
{
    parameters:
        @display("i=block/routing");
    gates:
        input in;
        output out;
}

simple Toc9
{
    parameters:
        @display("i=block/process");
    gates:
        input in;
        output out;
}

//
// Same as Tictoc8.
//
network myNet
{
    @display("bgb=210,108");
    submodules:
        tic: Tic9 {
            parameters:
                @display("i=,cyan;p=72.8,60.48");
        }
        toc: Toc9 {
            parameters:
                @display("i=,gold;p=185,61");
        }
    connections:
        tic.out --> {  delay = 100ms; } --> toc.in;
        tic.in <-- {  delay = 100ms; } <-- toc.out;
}
