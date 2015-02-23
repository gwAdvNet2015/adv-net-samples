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

import ConfigParser
import os
import sys

from pox.lib.packet.tcp import tcp
from pox.lib.packet.arp import arp
from pox.lib.packet.ipv4 import ipv4
from pox.lib.packet.ethernet import ethernet
from pox.lib.packet.ethernet import ETHER_BROADCAST

import pox.openflow.libopenflow_01 as of
from pox.lib.addresses import IPAddr

def getOpenFlowPort(connection, port_name) :
  phy_port = connection.ports[port_name]
  if phy_port is not None:
    return phy_port.port_no
  return -1

def readConfigFile(filename, logger) :
  config = None
  logger.debug("Looking for configuration file %s" % filename)
  filename = os.path.expanduser(filename) 
  if not os.path.exists(filename):
    logger.error("Configuration file %s does not exist! Exit!" %(filename)) 
    sys.exit(-1)
  logger.debug("Using configuration file %s" % filename)

  confparser = ConfigParser.RawConfigParser()
  try:
    confparser.read(filename)
  except ConfigParser.Error as exc:
    logger.warning("Config file %s could not be parsed: %s" 
                     % (filename, str(exc)))
        
  # Create a dictionary from the configuration
  # - each section is a key in the dictionary that it's value
  # is a dictionary with (key, value) pairs of configuration 
  # parameters
  config = {}
  for sec in confparser.sections():
    config[sec] = {}
    for (key,val) in confparser.items(sec):
      config[sec][key] = val

  logger.debug(config)
  return config

def packetIsIP(packet, logger) :
  if isinstance(packet, ethernet): 
    return isinstance(packet.next, ipv4)
  return False

def packetIsARP(packet, logger) :
  if isinstance(packet, ethernet): 
    return isinstance(packet.next, arp)
  return False

def packetIsRequestARP(packet, logger) : 
  if packetIsARP(packet, logger) :
     if packet.next.opcode == arp.REQUEST:
       return True
  return False

def packetIsReplyARP(packet, logger) : 
  if packetIsARP(packet, logger) :
     if packet.next.opcode == arp.REPLY:
       return True
  return False

def packetArpDstIp(packet, dstip, logger):
  if packetIsARP(packet, logger) :
     if packet.next.protodst == dstip:
       return True
  return False

def packetArpSrcIp(packet, srcip, logger):
  if packetIsARP(packet, logger) :
     if packet.next.protosrc == srcip:
       return True
  return False

def packetIsTCP(packet, logger) :
  if packetIsIP(packet, logger):
      return isinstance(packet.next.next, tcp)
  return False

def packetDstIp(packet, ipaddr, logger) :
  if packetIsIP(packet, logger):
    if not cmp(packet.next.dstip, ipaddr):
       return True
  return False
 
def packetSrcIp(packet, ipaddr, logger) :
  if packetIsIP(packet, logger):
    if not cmp(packet.next.srcip, ipaddr):
       return True
  return False
 
def packetDstTCPPort(packet, tcpport, logger) :
  if packetIsTCP(packet, logger):
    dsttcpportstr = packet.next.next.dstport
    if dsttcpportstr == tcpport :
       return True
  return False

def packetSrcTCPPort(packet, tcpport, logger) :
  if packetIsTCP(packet, logger):
    srctcpportstr = packet.next.next.srcport
    if srctcpportstr == tcpport :
       return True
  return False

def createOFAction(action_type, arg, logger) : 

  if action_type == of.OFPAT_OUTPUT : 
  # XXX Check if arg is a list
    logger.debug("Creating output action to %d" % arg)
    
    return of.ofp_action_output(port = arg)
  if action_type == of.OFPAT_SET_DL_SRC : 
    return of.ofp_action_dl_addr.set_src(arg)
  if action_type == of.OFPAT_SET_DL_DST : 
    return of.ofp_action_dl_addr.set_dst(arg)
  if action_type == of.OFPAT_SET_NW_SRC : 
    return of.ofp_action_nw_addr.set_src(arg)
  if action_type == of.OFPAT_SET_NW_DST : 
    return of.ofp_action_nw_addr.set_dst(arg)
  if action_type == of.OFPAT_SET_TP_SRC : 
    return of.ofp_action_tp_port.set_src(arg)
  if action_type == of.OFPAT_SET_TP_DST : 
    return of.ofp_action_tp_port.set_dst(arg)

  logger.warn("Type %d not supported" % action_type)
  return None
   

def getFullMatch(packet, inport) :
   return of.ofp_match.from_packet(packet, inport)

def createFlowMod(match, actions, hard_timeout, idle_timeout, buffid=None) :
    msg = of.ofp_flow_mod(command=of.OFPFC_ADD,
                          idle_timeout=idle_timeout,
                          hard_timeout=hard_timeout,
                          buffer_id=buffid,
                          actions=actions,
                          match=match)
    return msg



def createArpRequest(packet, ip, logger):
  if not packetIsARP(packet, logger):
     logger.warn("Packet is not ARP")
     return
  origarp = packet.next 
  arppkt = arp()
  arppkt.hwsrc      = origarp.hwsrc
  arppkt.hwdst      = origarp.hwdst
  arppkt.hwlen      = origarp.hwlen
  arppkt.opcode     = arp.REQUEST
  arppkt.protolen   = origarp.protolen
  arppkt.protosrc   = origarp.protosrc
  arppkt.protodst   = IPAddr(ip)
  pkt = ethernet()
  pkt.set_payload(arppkt)
  pkt.type = ethernet.ARP_TYPE
  pkt.src = arppkt.hwsrc
  pkt.dst = ETHER_BROADCAST
  return pkt

def createArpReply(packet, ip, logger):
  if not packetIsARP(packet, logger):
     logger.warn("Packet is not ARP")
     return
  origarp = packet.next 
  arppkt = arp()
  arppkt.hwsrc      = origarp.hwsrc
  arppkt.hwdst      = origarp.hwdst
  arppkt.hwlen      = origarp.hwlen
  arppkt.opcode     = arp.REPLY
  arppkt.protolen   = origarp.protolen
  arppkt.protosrc   = IPAddr(ip)
  arppkt.protodst   = origarp.protodst
  pkt = ethernet()
  pkt.set_payload(arppkt)
  pkt.type = ethernet.ARP_TYPE
  pkt.src = arppkt.hwsrc
  pkt.dst = arppkt.hwdst
  return pkt


