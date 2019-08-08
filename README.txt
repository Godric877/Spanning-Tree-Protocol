A class for bridge has been prepared which has the necessary member vairables and functions defined to implement the desired functionality. The member functions and variables of the implemented class can be looked up in the bridge.h file and their corresponding 
implementations in the bridge.cpp file.

The algorithm for the spanning tree is as follows:-

1) Initially every port considers itself to be the root port and sends messages accordingly.
2) Initially each port of all the bridges is considered to be a designated port.
2) The messages received by each bridge are stored in a buffer. Since the processing time is assumed to be zero, therefore 
   at the same instance each bridge scans through the stored messages and stores the best configuration according to the following rule :-
   a) Smaller root id
   b) Equal root id but lesser distance
   c) Equal root id and equal distance but lesser sender bridge id
3) After finding the root port, we update the root port for the bridge and then scan the messages received for determining the null ports 
   according to the following algorithm :- 
   a) If the sending bridge has lesser distance than the distance of the bridge being considered.
   b) If the distance of the sending bridge is equal to the distance of the bridge being considered but the sending bridge has lower id.
4) After finding the null ports, another case is analysed which is if the current root of the bridge being considered has changed from 
   the previous value. In such a case, their is a possibility that the ports which are currently null ports may become active ports again.
   Therefore, if the root port changes then all the ports connected to the bridge are marked as designated again and the algorithm again reaches an equilibrium.
5) The termination condition ,i.e the point at which the spanning tree converges, is when the root port for each bridge becomes the same 
   and each connected LAN segment has exactly one designated port. At this point the spanning tree has converged and the output is printed.

The algorithm for the learning bridges is as follows :- 

1) When a host sender sends a packet, the packet first reaches the bridges connected to the LAN segment through active ports.
2) Then all the bridges are iterated through and in each iteration, those bridges which had a message to forward at the beginning of the iteration forward messages on the active ports through the LANS connected to them.
3) The bridges after forwarding the messages empty their message queue and never send or receive a message again during the current transmission.
4) At each such iteration, time increases by one.
5) The bridges forward according to the following algorithm :- 
   a) If the entry corresponding to the receiver host is not present in the forwarding table of a bridge then the message is forwarded on all the ports except the one where it was received.
   b) If the entry corresponding to the receiver host is present in the forwarding table of a bridge but it is the same as the lan on which the particular message was received then the packet is dropped.
   c) If the entry corresponding to the receiver host is present in the forwarding table of a bridge and it is not the same as the lan on which the particular message was received then the packet is forwarded on the desired port.
   In all the above cases, the forwarding table is updated with the sending host lan port in the forwarding table before performing other actions.
6) The termination condition for the above algorithm is when no bridge has any message to send.