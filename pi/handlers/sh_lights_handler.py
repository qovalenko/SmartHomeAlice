from sh_module_handler_interface import SHModuleHandler

class SHLightsHandler(SHModuleHandler):
    def run(self, sid, relay_status):
        print("Switch relay to " + str(relay_status - 1) + " on the station #" + str(sid))

        self.transmitter.updateRelayStatusWC(sid, relay_status)
