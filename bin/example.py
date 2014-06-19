#!/usr/bin/python

import moos.core
import sys

class HInfinityController(moos.core.MOOSInstrument):
    def __init__(self):
        self.yaw = 0
        self.pitch = 0
        self.roll = 0
        self.gyro_x = 0
        self.gyro_y = 0
        self.gyro_z = 0
        self.acc_x = 0
        self.acc_y = 0
        self.acc_z = 0
        self.heading = 0
        self.desired_heading = 0
        self.depth = 0
        self.desired_depth = 0

    def iterate(self):
        # RUI! Code HERE please!
        # print "Notify: ", self.comms.notify("GYRO_SUM", (s_gyro))
        return True

    def onStartUp(self):
        print "We're starting now!"
        print "Our MOOS Community name: ", self.comms.community
        self.comms.register("YAW")
        self.comms.register("ROLL")
        self.comms.register("PITCH")
        self.comms.register("GYRO_X")
        self.comms.register("GYRO_Y")
        self.comms.register("GYRO_Z")
        self.comms.register("ACC_X")
        self.comms.register("ACC_Y")
        self.comms.register("ACC_Z")
        self.comms.register("HEADING")
        self.comms.register("DESIRED_HEADING")
        self.comms.register("DEPTH")
        self.comms.register("DESIRED_DEPTH")
        return True

    def onConnectToServer(self):
        print "Connected to server."
        return True

    def onNewMail(self, msgs):
        self.gyro_x = msgs.PeekMail("GYRO_X", False, find_youngest=True).getDouble()
        self.gyro_y = msgs.PeekMail("GYRO_Y", False, find_youngest=True).getDouble()
        self.gyro_z = msgs.PeekMail("GYRO_Z", False, find_youngest=True).getDouble()
        self.acc_x  = msgs.PeekMail("ACC_X", False, find_youngest=True).getDouble()
        self.acc_y  = msgs.PeekMail("ACC_Y", False, find_youngest=True).getDouble()
        self.acc_z  = msgs.PeekMail("ACC_Z", False, find_youngest=True).getDouble()
        self.yaw    = msgs.PeekMail("YAW", False, find_youngest=True).getDouble()
        self.roll   = msgs.PeekMail("ROLL", False, find_youngest=True).getDouble()
        self.pitch  = msgs.PeekMail("PITCH", False, find_youngest=True).getDouble()
        self.heading = msgs.PeekMail("HEADING", False, find_youngest=True).getDouble()
        self.desired_heading = msgs.PeekMail("DESIRED_HEADING", False, find_youngest=True).getDouble()
        self.depth  = msgs.PeekMail("DEPTH", False, find_youngest=True).getDouble()
        self.desired_depth = msgs.PeekMail("DESIRED_DEPTH", False, find_youngest=True).getDouble()
        return True

thing = HInfinityController()

try:
    thing.run("example_rui", "mission.moos")
except KeyboardInterrupt:
    pass
