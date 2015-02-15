#Simple Sniffer#
Simple sniffer is a simple packet sniffer that displays header information from packets on a givern interface.


###Setup###
In order for the simple sniffer to work, you need to install the libpcap library.

On ubuntu this can be done by running:
<code>sudo apt-get install libpcap-dev</code>

On other platforms you can manually download the library from the [TCPDUMP web page](http://www.tcpdump.org/)



###Running Simple Sniffer###

To run the packet sniffer you can use a number of arguments:

* <code>-i \<interfaceName\>  </code> This allows you to specify the interface to listen on. This is a mandatory flag.
* <code>-e \<filterExpresion\> </code> This allows you to specify the type of packet to filter on. Any expression that tcpcap takes will work. An example is <code>"port 80"</code>
* <code>-n \<number of packets\> </code> This allows you to specify the number of packets to listen for. Use a negative -1 to listen forever. The default is 15. 


######Example Commands#####

<code>sudo ./sniff -i eth0</code> Sniffs over the eth0 interface

<code>sudo ./sniff -i eth0 -e "port 80"</code> Sniffs for tcp http packets.

<code>sudo ./sniff -i eth0 -n 100 </code> Sniffs for 100 packets
