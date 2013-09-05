rfproxy-for-nox13
=================
1. Environment: Ubuntu 12.04(64), openvswitch-1.10.0, nox13oflib

2. The component of NOX is compiled to be .so for running. RouteFlow in https://github.com/CPqD/RouteFlow provide us a rflib.a. I dont't know how to use .a in .so. So I just unpicked rflib.a. Using rflib.a is T.B.D.

3. To use mongodb with NOX13, a) configure NOX using: configure --enable-ndebug
                              b) In pch.h add #undef VERSION

4. Rfproxy for nox13 is a little different with one in https://github.com/CPqD/RouteFlow. I have made an expansion to perceive the up and down of a VM.

5. Any problem, contect me. E-mail:liximing1125@gmail.com
