import moos.core
import sys

ii = 123

class ExampleThing(moos.core.MOOSInstrument):
    def iterate(self):
        global ii
        print "I am the walrus!"
        print "Notify: ", self.comms.notify("VesselHeading_Variation", (ii))
        ii += 1
        return True
    def onStartUp(self):
        print "Wissh!"
        print self.comms.community
        self.comms.register("VesselHeading_Variation")
        return True
    def onConnectToServer(self):
        print "woosh!"
        return True
    def onNewMail(self, msgs):
        print "gonna peek?"
        z = msgs.PeekMail("VesselHeading_Variation", False, find_youngest=True)
        print "vessel heading: ", z, z.value
        return True


thing = ExampleThing()

try:
    thing.run("example1", "mission.moos")
except KeyboardInterrupt:
    pass
