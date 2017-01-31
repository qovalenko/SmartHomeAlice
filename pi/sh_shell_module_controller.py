#!/usr/bin/python

# forwards calls to module_controller

import sys
from sh_module_controller import *

if __name__ == "__main__":
    args = sys.argv[1:]

    if len(args) < 2:
        print("./sh_shell_module_controller <module> <station_id> [<relay_status>]")
        sys.exit()

    module       = args[0]
    station_id   = int(args[1])
    
    if len(args) == 3:
        relay_status = int(args[2])
    else:
        relay_status = 0

    print(call(module, station_id, relay_status))
