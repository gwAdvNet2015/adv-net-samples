#!/usr/bin/python

from pox.core import core
import pox.openflow.libopenflow_01 as of
from pox.openflow import *
from utils import *
from SimpleL2Learning import SimpleL2LearningSwitch

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
        out_port = self.get_out_port()
        ports = [out_port]
        ip = self.packet.find('ipv4')
        if ip and ip.srcip == self._src_ip:
                ports.append(self._dst_port)
        self.forward_packet(ports)


class MirrorTraffic(object):
    def __init__(self, duplicate_port):
        core.openflow.addListeners(self)
        self._duplicate_port = duplicate_port

    def _handle_ConnectionUp(self, event):
        log.debug("Connection %s" % (event.connection,))
        TrafficMirrorSwitch(event.connection, self._duplicate_port)


def launch(src_ip="10.1.1.1", dst_port="eth4"):
    log.debug("TrafficMirror : {} -> {}".format(src_ip, dst_port))
    core.registerNew(MirrorTraffic, src_ip, dst_port)
