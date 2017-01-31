import sys

class SHProtocolParser:
    def parse(self, num):
        s = str(num)
        if len(s) != 7:
            print("")
            sys.exit()

        sid = int(num[0])
        temp = float(int(num[1:3]) / 10)
        humidity = int(num[3:5])
        relay_status = int(num[6])

        return (sid, temp, humidity, relay_status)

    def isEmpty(self, p):
        return p[1] == 0 and p[2] == 0 and p[3] == 0
