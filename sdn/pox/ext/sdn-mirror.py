#!/usr/bin/python

"""
Traffic mirroring switch
Mirrors traffic from a given IP to a different port
"""

from pox.core import core
import pox.openflow.libopenflow_01 as of
from pox.openflow import *
from utils import *
from BBN_SimpleL2Learning import SimpleL2LearningSwitch

log = core.getLogger()  # Use central logging service


class TrafficMirrorSwitch(SimpleL2LearningSwitch):

    def __init__(self, connection, src_ip, dst_port):
        SimpleL2LearningSwitch.__init__(self, connection, False)
        self._connection = connection
        self._src_ip = src_ip
        self._dst_port = getOpenFlowPort(connection, dst_port)

    def _handle_PacketIn(self, event):
        log.debug("Got a packet : " + str(event.parsed))
        self.packet = event.parsed
        self.event = event
        self.macLearningHandle()

        # get the desired dst port (flood, if needed)
        out_port = self.get_out_port()
        ports = [out_port]
        # get the source ip address (if it exists)
        ip = self.packet.find('ipv4')
        log.debug("packet src ip {}".format(ip.srcip if ip else ""))
        if ip and ip.srcip == self._src_ip:
                # if the packet is from our watched IP,
                # also send it to the mirror port
                ports.append(self._dst_port)

        # send the packet out to everything in ports list
        self.forward_packet(ports)


class MirrorTraffic(object):
    def __init__(self, src_ip, duplicate_port):
        core.openflow.addListeners(self)
        self._duplicate_port = duplicate_port
        self._src_ip = src_ip
        log.debug("TrafficMirror : {} -> {}".format(src_ip, duplicate_port))

    def _handle_ConnectionUp(self, event):
        log.debug("Connection %s" % (event.connection,))
        TrafficMirrorSwitch(event.connection, self._src_ip, self._duplicate_port)


# use 'src_ip' and 'dst_port' as command line params
def launch(src_ip="10.1.1.1", dst_port="eth4"):
    core.registerNew(MirrorTraffic, src_ip, dst_port)
