import serial, time
from threading import Thread

# Connection Settings
DEFAULT_SERIAL_PORT = '/dev/ttyACM0'
DEFAULT_BAUDRATE = 115200

# Protocol Settings
PACKET_HEADER=[ ord('P'), ord('K'), ord('T'), ord('!')]
HEADER_SIZE = len(PACKET_HEADER)+1
TRAILER_SIZE = 3 

def build_packet(payload):
    pkt = bytearray( PACKET_HEADER )
    pkt.append(len(payload))
    for x in payload:
        pkt.append(x)
    crc = crc16(payload)
    pkt.append( (crc>>8) & 0xFF )
    pkt.append( crc&0xFF) 
    pkt.append(ord('\n'))
    return pkt

def crc16(buf):    
    crc = 0xFFFF
    for bi in buf:
        x = crc >> 8 ^ bi
        x ^= x>>4
        crc = ((crc << 8)&0xFFFF) ^ ((x << 12)&0xFFFF) ^ ((x <<5)&0xFFFF) ^ (x&0xFFFF)
    return crc

def get_packet_payload(pkt):
    tmp = []
    if ( len(pkt) > (HEADER_SIZE) ):
        if ( (pkt[0] == PACKET_HEADER[0]) and
             (pkt[1] == PACKET_HEADER[1]) and
             (pkt[2] == PACKET_HEADER[2]) and
             (pkt[3] == PACKET_HEADER[3]) ):
                #print("Valid header")
                length = pkt[4]
                #print("Length:", pkt[4])                
                if len(pkt) >= (HEADER_SIZE+length+TRAILER_SIZE):
                    for x in range(length):
                        tmp.append(pkt[HEADER_SIZE+x])
                else:
                    raise ValueError("Invalid Size")
        else:
            print("Header does not match:", pkt)
    return tmp

class Connection(Thread):
    def __init__(self, port=DEFAULT_SERIAL_PORT, baudrate=DEFAULT_BAUDRATE, 
            on_packet_callback = None ):
        Thread.__init__(self)
        self.port = port
        self.baudrate=baudrate
        self.conn = None

        if on_packet_callback != None:
            self.on_packet_callback = on_packet_callback
        else:
            self.on_packet_callback = self.default_on_packet_callback
                
    def connect(self):
        self.conn = serial.Serial( self.port, self.baudrate,timeout=1)
        self.conn.readline() # FIXME: do proper flush
        self.start()

    def send_packet(self,pkt):
        self.conn.write(pkt)
        
    def disconnect(self):
        self.keep_running = False
        self.join()
        self.conn.close()

    def default_on_packet_callback(self, payload):
        tmp = ""
        for x in payload:
            tmp+="%02X " % x
        print(tmp)

    def run(self):
       self.keep_running = True
       while self.keep_running:
            line = self.conn.readline()
            if len(line)>1:
                payload = get_packet_payload(line)
                if len(payload) > 1:
                    self.on_packet_callback(payload)



