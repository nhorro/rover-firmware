import serial, time
from threading import Thread

# Connection Settings
DEFAULT_SERIAL_PORT = '/dev/serial0'
DEFAULT_BAUDRATE = 115200

# Protocol Settings
PACKET_SYNC_0_CHAR = 'P';
PACKET_SYNC_1_CHAR = 'K';
PACKET_SYNC_2_CHAR = 'T';
PACKET_SYNC_3_CHAR = '!';
PACKET_TERMINATOR_CHAR = '\n';
PACKET_HEADER=[  ord(PACKET_SYNC_0_CHAR),  ord(PACKET_SYNC_1_CHAR), ord(PACKET_SYNC_2_CHAR), ord(PACKET_SYNC_3_CHAR) ]
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
    # FIXME Convert to integer list
    pkt_as_ints = []
    for x in pkt:
        pkt_as_ints.append(ord(x))
    pkt = pkt_as_ints

    tmp = []
    if ( len(pkt) > (HEADER_SIZE) ):
        if ( (pkt[0] == PACKET_HEADER[0]) and
             (pkt[1] == PACKET_HEADER[1]) and
             (pkt[2] == PACKET_HEADER[2]) and
             (pkt[3] == PACKET_HEADER[3]) ):
                #print("Valid header")
                length = pkt[4]
                expected_packet_length = (HEADER_SIZE+length+TRAILER_SIZE)
                #print("Length:", pkt[4])                
                if len(pkt) >= expected_packet_length:
                    for x in range(length):
                        tmp.append(pkt[HEADER_SIZE+x])
                else:
                    print("Invalid packet length: ", length, ". Expected: ", expected_packet_length)
                    tmp = "Received: "
                    for x in pkt:
                        tmp+="%02X " % x
                    print(tmp)
        else:
            print("Header mismatch. Received packet:", pkt) # TODO: log
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