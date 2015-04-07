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
import pox.openflow.libopenflow_01 as of
from pox.openflow import *
from utils import *
from SimpleL2Learning import SimpleL2LearningSwitch

log = core.getLogger() # Use central logging service

class DuplicateTrafficSwitch(SimpleL2LearningSwitch):

    def __init__(self, connection, duplicate_port):
        SimpleL2LearningSwitch.__init__(self, connection, False)
        self._connection = connection;
        self._duplicate_port=duplicate_port 
        self._of_duplicate_port=getOpenFlowPort(connection, duplicate_port)

    def _handle_PacketIn(self, event):
        log.debug("Got a packet : " + str(event.parsed))
        self.packet = event.parsed
        self.event = event
        self.macLearningHandle()
        out_port = self.get_out_port()
        self.forward_packet([out_port, self._of_duplicate_port])


class DuplicateTraffic(object):
    def __init__(self, duplicate_port):
        core.openflow.addListeners(self)
        self._duplicate_port= duplicate_port

    def _handle_ConnectionUp(self, event):
        log.debug("Connection %s" % (event.connection,))
        DuplicateTrafficSwitch(event.connection, self._duplicate_port)


def launch(duplicate_port="eth4"):
    log.debug("DuplicateTraffic" + duplicate_port);
    core.registerNew(DuplicateTraffic, duplicate_port)

