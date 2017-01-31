import sys
import pi_switch
from pi_switch import RCSwitchReceiver

class Transmitter:
    def __init__(self):
        self.sender = pi_switch.RCSwitchSender()
        self.sender.enableTransmit(0)

        self.receiver = RCSwitchReceiver()
        self.receiver.enableReceive(2)

    def sendRelayStatus(self, sid, relay_status):
        code = str(sid) + "000" + "00" + str(relay_status)
        code_encoded = int(code)
        self.sender.sendDecimal(code_encoded, 24)

    def updateRelayStatusWC(self, sid, relay_status):
        if relay_status != 1 and relay_status != 2:
            print("Error: wrong relay status on sending")
            sys.exit()

        self.sendRelayStatus(sid, relay_status)
