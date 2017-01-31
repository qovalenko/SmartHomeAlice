from sh_module_handler_interface import SHModuleHandler
import time

current_milli_time = lambda: int(round(time.time() * 1000))

class SHPollHandler(SHModuleHandler):
    def fetch(self, sid):
        #
        # Getting the response

        iterations   = 0
        temp         = 0
        humidity     = 0
        relay_status = 0

        # try 5 times to fetch the response
        while iterations < 5:
            self.transmitter.sendRelayStatus(sid, 0)
            time.sleep(0.1)

            # wait for 5s after requesting the data
            start_time = current_milli_time()
            while current_milli_time() - start_time < 5000:
                if self.transmitter.receiver.available():
                    received_value = self.transmitter.receiver.getReceivedValue()
                    parsed_value = self.protocol_parser.parse(received_value)

                    print(received_value)

                    if parsed_value[0] == sid and not self.protocol_parser.isEmptyParsed(parsed_value):
                        temp         = parsed_value[1]
                        humidity     = parsed_value[2]
                        relay_status = parsed_value[3]
                        break

                    self.transmitter.receiver.resetAvailable()

            if temp != 0 or humidity != 0 or relay_status != 0:
                break

        return (temp, humidity, relay_status)
