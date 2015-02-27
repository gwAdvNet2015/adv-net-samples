#!/usr/bin/python

#----------------------------------------------------------------------
# Copyright (c) 2013 Raytheon BBN Technologies
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and/or hardware specification (the "Work") to
# deal in the Work without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Work, and to permit persons to whom the Work
# is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Work.
#
# THE WORK IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE WORK OR THE USE OR OTHER DEALINGS
# IN THE WORK.
#----------------------------------------------------------------------

from pox.core import core
from pox.openflow import *
import string
import time
import threading
import pdb
from utils import *
from SimpleL2Learning import SimpleL2LearningSwitch
from pox.lib.packet.ethernet import ethernet
from pox.lib.packet.vlan import vlan
from pox.lib.packet.ipv4 import ipv4
from pox.lib.packet.arp import arp
from pox.lib.packet.tcp import tcp

log = core.getLogger() # Use central logging service

SCRIPT_PATH = os.path.dirname(os.path.abspath(__file__))

FLOW_HARD_TIMEOUT = 30
FLOW_IDLE_TIMEOUT = 10

class PortForwardingSwitch(SimpleL2LearningSwitch):

    def __init__(self, connection, config):
        SimpleL2LearningSwitch.__init__(self, connection, False)
        self._connection = connection;
        self._serverip = config['server_ip']
        self._serverport = int(config['server_port'])
        self._proxyip = config['proxy_ip']
        self._proxyport = int(config['proxy_port'])

    def _handle_PacketIn(self, event):
        log.debug("Got a packet : " + str(event.parsed))
        self.packet = event.parsed
        self.event = event
        self.macLearningHandle()

        if packetIsARP(self.packet, log) :
          self._handle_PacketInARP(event)
          return

        if packetIsTCP(self.packet, log) :
          self._handle_PacketInTCP(event)
          return
        SimpleL2LearningSwitch._handle_PacketIn(self, event)

    def _handle_PacketInARP(self, event) :
        inport = event.port
        arppkt = None

        # If this an ARP Packet srcd at the server, 
        # Then we drop it not to confuse the MAC learning
        # At the hosts
        if packetArpSrcIp(self.packet, self._serverip, log):
           log.info("DROP ARP Packet From Server!")
           return

        # XXX If this is an ARP Request for the server iP
        # create new ARP request and save it in arppkt
        if packetIsRequestARP(self.packet, log) : 
           log.debug("Packet is an ARP Request")
           if packetArpDstIp(self.packet, self._serverip, log):
             arppkt = createArpRequest(self.packet, self._proxyip, log)

        # XXX If this is an ARP Reply from the proxy
        # create new ARP reply  and save it in arppkt
        if packetIsReplyARP(self.packet, log) : 
           log.debug("Packet is an ARP Reply")
           if packetArpSrcIp(self.packet, self._proxyip, log):
             arppkt = createArpReply(self.packet, self._serverip, log)

        # If we haven't created a new arp packet, send the one we 
        # received
        if arppkt is None :
          SimpleL2LearningSwitch._handle_PacketIn(self, event)
          return

        # Send a packet out with the ARP
        msg = of.ofp_packet_out() 
        msg.actions.append(of.ofp_action_output(port = of.OFPP_FLOOD))
        msg.data = arppkt.pack() 
        msg.in_port = inport
        self.connection.send(msg)
            

    def _handle_PacketInTCP(self, event) :
        inport = event.port
        actions = []
        out_port = self.get_out_port()

        # XXX If packet is destined to serverip:server port
        # make the appropriate rewrite
        if packetDstIp( self.packet, self._serverip, log ): 
            if packetDstTCPPort(self.packet, self._serverport, log) :
              log.debug("Packet is TCP destined to %s:%d" % (self._serverip, self._serverport))
              newaction = createOFAction(of.OFPAT_SET_TP_DST, self._proxyport, log)
              actions.append(newaction)
              newaction = createOFAction(of.OFPAT_SET_NW_DST, self._proxyip, log)
              actions.append(newaction)

        # XXX If packet is sourced at proxyip:proxy port
        # make the appropriate rewrite
        if packetSrcIp(self.packet, self._proxyip, log ): 
          if packetSrcTCPPort(self.packet, self._proxyport, log) :
            log.debug("Packet is TCP sourced at %s:%d" % (self._proxyip, self._proxyport))
            newaction = createOFAction(of.OFPAT_SET_TP_SRC, self._serverport, log)
            actions.append(newaction)
            newaction = createOFAction(of.OFPAT_SET_NW_SRC, self._serverip, log)
            actions.append(newaction)
              
        # XXX Create the flow mod message in a variable
        # called msg
        newaction = createOFAction(of.OFPAT_OUTPUT, out_port, log)
        actions.append(newaction)
        
        match = getFullMatch(self.packet, inport)
        msg = createFlowMod(match, actions, FLOW_HARD_TIMEOUT, 
                                  FLOW_IDLE_TIMEOUT, event.ofp.buffer_id)
        event.connection.send(msg.pack())
        		
class PortForwarding(object):
    def __init__(self, config):
        core.openflow.addListeners(self)
        self._config=config 

    def _handle_ConnectionUp(self, event):
        log.debug("Connection %s" % (event.connection,))
        PortForwardingSwitch(event.connection, self._config)


def launch(config_file=os.path.join(SCRIPT_PATH, "proxy.config")):
    log.debug("Proxy " + config_file);
    config = readConfigFile(config_file, log)
    core.registerNew(PortForwarding, config["general"])

