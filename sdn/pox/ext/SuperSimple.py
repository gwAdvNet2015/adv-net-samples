# Copyright 2014 Tim Wood and James McCauley
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at:
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""
This is a really simple POX program that just prints info about
any packets it receives.
"""

from pox.core import core
import pox.openflow.libopenflow_01 as of

log = core.getLogger()


class SuperSimple (object):
  """
  A SuperSimple object is created for each switch that connects.
  A Connection object for that switch is passed to the __init__ function.
  """
  def __init__ (self, connection):
    # Keep track of the connection to the switch so that we can
    # send it messages!
    self.connection = connection

    # This binds our PacketIn event listener
    connection.addListeners(self)


  def _handle_PacketIn (self, event):
    """
    Handles packet in messages from the switch.
    """

    packet = event.parsed # This is the parsed packet data.
    if not packet.parsed:
      log.warning("Ignoring incomplete packet")
      return

    packet_in = event.ofp # The actual ofp_packet_in message.

    log.debug("OF packet: %s", packet_in)
    log.debug("Parsed packet: %s", packet)

    ### Ask the switch to flood the packet out. Do not setup a rule
    # msg = of.ofp_packet_out()
    # msg.actions.append(of.ofp_action_output(port = of.OFPP_FLOOD))
    # msg.data = event.ofp
    # msg.in_port = event.port
    # self.connection.send(msg)

    ### Ask the switch to setup a rule so all packets in the flow will be
    ### flooded out
    # msg = of.ofp_flow_mod()
    # msg.match = of.ofp_match.from_packet(packet, event.port)
    # msg.actions.append(of.ofp_action_output(port = of.OFPP_FLOOD))
    # msg.idle_timeout = 10
    # msg.hard_timeout = 30
    # self.connection.send(msg)


def launch ():
  """
  Starts the component. Run when Pox starts.
  """
  def start_switch (event):
    log.debug("Controlling %s" % (event.connection,))
    SuperSimple(event.connection)

  core.openflow.addListenerByName("ConnectionUp", start_switch)
