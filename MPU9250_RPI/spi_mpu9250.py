import spidev
import time
from numpy import int16

# REGISTERS
REGISTER_INT_PIN_CFG            = 0x37
REGISTER_VALUE_BYPASS_EN        = 0x02

REGISTER_PWR_MGMT_1             = 0x6B
REGISTER_VALUE_RESET            = 0x80

REGISTER_ACCEL_CONFIG           = 0x1C
REGISTER_ACCEL_OUT              = 0x3B

# VARIABLES

csPin = 5
useSPI = True
xRaw, yRaw, zRaw = 0, 0, 0
rawData = [0,0,0,0,0,0]

# SPI SETUP
spi = spidev.SpiDev()
spi.open(0 , 0)
spi.max_speed_hz = 1 * 1000

# FUNCTIONS

def writeMPU9250Register(reg, val):
	# ~ log1 = spi.xfer([reg])
	# ~ log2 = spi.xfer([val])
	spi.writebytes([reg])
	spi.writebytes([val])
	

def readMPU9250Register3x16(reg, buf):
	reg |= 0x80;
	spi.writebytes([reg])
	for x in range(6):
		buf[x] = spi.readbytes(1)[0]
		
# PROGRAM CODE

writeMPU9250Register(REGISTER_PWR_MGMT_1, REGISTER_VALUE_RESET)
time.sleep(0.1)

writeMPU9250Register(REGISTER_INT_PIN_CFG, REGISTER_VALUE_BYPASS_EN)
time.sleep(0.1)

while True:
	readMPU9250Register3x16(REGISTER_ACCEL_OUT, rawData)
	xRaw = ((rawData[0] << 8) | rawData[1])
	yRaw = ((rawData[2] << 8) | rawData[3])
	zRaw = ((rawData[4] << 8) | rawData[5])
	print (" Ax: {0} ; Ay : {1} ; Az : {2}".format(xRaw, yRaw, zRaw))
	time.sleep(0.5)
