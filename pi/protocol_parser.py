import sys

class ProtocolParser:
    def parse(self, num):
        s = str(num)
        if len(s) != 7:
            print("Error: cannot parse, bad protocol format")
            sys.exit()

        sid = int(s[0])
        temp = float(s[1:4]) / 10
        humidity = int(s[4:6])
        relay_status = int(s[6])

        return (sid, temp, humidity, relay_status)

    def isEmptyParsed(self, p):
        return p[1] == 0 and p[2] == 0 and p[3] == 0
