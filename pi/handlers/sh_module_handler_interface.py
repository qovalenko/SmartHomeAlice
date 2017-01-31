from abc import ABCMeta, abstractmethod
from transmitter import Transmitter
from protocol_parser import ProtocolParser

class SHModuleHandler:
    def __init__(self):
        self.transmitter = Transmitter()
        self.protocol_parser = ProtocolParser()

    @abstractmethod
    def run(self, sid, relay_status):
        pass

    @abstractmethod
    def fetch(self, sid):
        pass
