# Copyright 2011 James McCauley
#
# This file is part of POX.
#
# POX is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# POX is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with POX.  If not, see <http://www.gnu.org/licenses/>.

"""
An L2 learning switch.

It is derived from one written live for an SDN crash course.
It is somwhat similar to NOX's pyswitch in that it installs
exact-match rules for each flow.
"""

from pox.core import core
import pox.openflow.libopenflow_01 as of
from pox.lib.util import dpid_to_str
from pox.lib.util import str_to_bool
import time

log = core.getLogger()

# We don't want to flood immediately when a switch connects.
# Can be overriden on commandline.
_flood_delay = 0

class SimpleL2LearningSwitch(object):
  """
  The learning switch "brain" associated with a single OpenFlow switch.

  When we see a packet, we'd like to output it on a port which will
  eventually lead to the destination.  To accomplish this, we build a
  table that maps addresses to ports.

  We populate the table by observing traffic.  When we see a packet
  from some source coming from some port, we know that source is out
  that port.

  When we want to forward traffic, we look up the desintation in our
  table.  If we don't know the port, we simply send the message out
  all ports except the one it came in on.  (In the presence of loops,
  this is bad!).

  In short, our algorithm looks like this:

  For each packet from the switch:
  1) Use source address and switch port to update address/port table
  2) Is transparent = False and either Ethertype is LLDP or the packet's
     destination address is a Bridge Filtered address?
     Yes:
        2a) Drop packet -- don't forward link-local traffic (LLDP, 802.1x)
            DONE
  3) Is destination multicast?
     Yes:
        3a) Flood the packet
            DONE
  4) Port for destination address in our address/port table?
     No:
        4a) Flood the packet
            DONE
  5) Is output port the same as input port?
     Yes:
        5a) Drop packet and similar ones for a while
  6) Install flow table entry in the switch so that this
     flow goes out the appopriate port
     6a) Send the packet out appropriate port
  """
  def __init__ (self, connection, transparent):
    # Switch we'll be adding L2 learning switch capabilities to
    self.connection = connection
    self.transparent = transparent

    # Our table
    self.macToPort = {}

    # We want to hear PacketIn messages, so we listen
    # to the connection
    connection.addListeners(self)

    # We just use this to know when to log a helpful message
    self.hold_down_expired = _flood_delay == 0

    #log.debug("Initializing LearningSwitch, transparent=%s",
    #          str(self.transparent))

  def _handle_PacketIn (self, event):
    """
    Handle packet in messages from the switch to implement above algorithm.
    """

    self.packet = event.parsed
    self.event = event
    self.macLearningHandle()
    outport = self.get_out_port()
    self.forward_packet([outport])

  def macLearningHandle(self) :
    self.macToPort[self.packet.src] = self.event.port # 1


  def forward_packet(self, port_list) :

    if 0 in port_list :
      self._drop(10)
    else :
      if of.OFPP_FLOOD in port_list:
        self._flood()
      else : 
      # 6
        log.debug("installing flow for %s.%i -> %s.%s" %
                  (self.packet.src, self.event.port, self.packet.dst, str(port_list)))
        msg = of.ofp_flow_mod()
        msg.match = of.ofp_match.from_packet(self.packet, self.event.port)
        msg.idle_timeout = 10
        msg.hard_timeout = 30
        for p in port_list:
          msg.actions.append(of.ofp_action_output(port = p))
        msg.data = self.event.ofp # 6a
        self.connection.send(msg)

  def get_out_port(self)   :
    # By default drop the packet
    outport = 0 

    if not self.transparent: # 2
      if self.packet.type == self.packet.LLDP_TYPE or self.packet.dst.isBridgeFiltered():
        outport = 0

    if self.packet.dst.is_multicast:
      outport = of.OFPP_FLOOD # 3a
      msg = ""
    else:
      if self.packet.dst not in self.macToPort: # 4
        outport = of.OFPP_FLOOD # 3a
        msg = "Port for %s unknown -- flooding" % (self.packet.dst,) # 4a
      else:
        outport = self.macToPort[self.packet.dst]
        if outport == self.event.port: # 5
          # 5a
          log.warning("Same port for packet from %s -> %s on %s.%s.  Drop."
              % (self.packet.src, self.packet.dst, dpid_to_str(self.event.dpid), port))
          outport = 0
    return outport
	

  def _flood (self, message = None):
      """ Floods the packet """
      msg = of.ofp_packet_out()
      if time.time() - self.connection.connect_time >= _flood_delay:
        # Only flood if we've been connected for a little while...

        if self.hold_down_expired is False:
          # Oh yes it is!
          self.hold_down_expired = True
          log.info("%s: Flood hold-down expired -- flooding",
              dpid_to_str(self.event.dpid))

        if message is not None: log.debug(message)
        #log.debug("%i: flood %s -> %s", self.event.dpid,self.packet.src,self.packet.dst)
        # OFPP_FLOOD is optional; on some switches you may need to change
        # this to OFPP_ALL.
        msg.actions.append(of.ofp_action_output(port = of.OFPP_FLOOD))
      else:
        pass
        #log.info("Holding down flood for %s", dpid_to_str(self.event.dpid))
      msg.data = self.event.ofp
      msg.in_port = self.event.port
      self.connection.send(msg)

  def _drop (self, duration = None):
      """
      Drops this packet and optionally installs a flow to continue
      dropping similar ones for a while
      """
      if duration is not None:
        if not isinstance(duration, tuple):
          duration = (duration,duration)
        msg = of.ofp_flow_mod()
        msg.match = of.ofp_match.from_packet(self.packet)
        msg.idle_timeout = duration[0]
        msg.hard_timeout = duration[1]
        msg.buffer_id = self.event.ofp.buffer_id
        self.connection.send(msg)
      elif self.event.ofp.buffer_id is not None:
        msg = of.ofp_packet_out()
        msg.buffer_id = self.event.ofp.buffer_id
        msg.in_port = self.event.port
        self.connection.send(msg)

    
class SimpleL2Learning(object):
  """
  Waits for OpenFlow switches to connect and makes them learning switches.
  """
  def __init__ (self, transparent):
    core.openflow.addListeners(self)
    self.transparent = transparent

  def _handle_ConnectionUp (self, event):
    log.debug("Connection %s" % (event.connection,))
    SimpleL2LearningSwitch(event.connection, self.transparent)


def launch (transparent=False, hold_down=_flood_delay):
  """
  Starts an L2 learning switch.
  """
  try:
    global _flood_delay
    _flood_delay = int(str(hold_down), 10)
    assert _flood_delay >= 0
  except:
    raise RuntimeError("Expected hold-down to be a number")

  core.registerNew(SimpleL2Learning, str_to_bool(transparent))
