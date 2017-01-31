
# calls appropriate module handler

import sys, importlib

def cap(str):
    return str[:1].upper() + str[1:]

av_modules  = ("lights", "poll")
av_stations = ("2")

def call(handler_name, sid, relay_status):
    if handler_name not in av_modules:
        print("Error: no such a module")
        sys.exit()

    if str(sid) not in av_stations:
        print("Error: no such a station")
        sys.exit()

    relay_status += 1
    if relay_status < 1 or relay_status > 2:
        print("Error: relay status is not binary")
        sys.exit()

    module = importlib.import_module("handlers.sh_" + handler_name + "_handler")
    _class = getattr(module, "SH" + cap(handler_name) + "Handler")
    handler = _class()

    handler.run(sid, relay_status)
    return handler.fetch(sid)
