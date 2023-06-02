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

# ACCEL_XOUT_H = 0x3B
# ACCEL_XOUT_L = 0x3C
# ACCEL_YOUT_H = 0x3D
# ACCEL_YOUT_L = 0x3E
# ACCEL_ZOUT_H = 0x3F
# ACCEL_ZOUT_L = 0x40

# GYRO_XOUT_H = 0x43
# GYRO_XOUT_L = 0x44
# GYRO_YOUT_H = 0x45
# GYRO_YOUT_L = 0x46
# GYRO_ZOUT_H = 0x47
# GYRO_ZOUT_L = 0x48
ACC_REG = [0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40]
GYRO_REG = [0x43, 0x44, 0x45, 0x46, 0x47, 0x48]

# VARIABLES

AxRaw, AyRaw, AzRaw, GxRaw, GyRaw, GzRaw = 0, 0, 0, 0, 0, 0
AccRawData = []
GyrRawData = []

# SPI SETUP
spi = spidev.SpiDev()
spi.open(0 , 0)
# ~ spi.max_speed_hz = 200*1000000

# FUNCTIONS

def writeMPU9250Register(reg, val):
	# ~ log1 = spi.xfer([reg])
	# ~ log2 = spi.xfer([val])
	spi.writebytes([reg])
	spi.writebytes([val])

		
# PROGRAM CODE

writeMPU9250Register(REGISTER_PWR_MGMT_1, REGISTER_VALUE_RESET)	# RESET MPU
time.sleep(0.1)

writeMPU9250Register(REGISTER_INT_PIN_CFG, REGISTER_VALUE_BYPASS_EN) # Bypass Enable FOR DIRECT ACCESS TO auxiliary I2C BUS
time.sleep(0.1)

for x in range(6):
	GYRO_REG[x] |= 0x80
	ACC_REG[x] |= 0x80

start = time.time_ns()
for x in range(1000):
# ~ while True:
	# ~ rawData = spi.xfer([ACC_REG[0], ACC_REG[1], ACC_REG[2], ACC_REG[3], ACC_REG[4], ACC_REG[5], GYRO_REG[0], GYRO_REG[1], GYRO_REG[2], GYRO_REG[3], GYRO_REG[4], GYRO_REG[5]])
	GyrRawData = spi.xfer([GYRO_REG[0], GYRO_REG[1], GYRO_REG[2], GYRO_REG[3], GYRO_REG[4], GYRO_REG[5]])
	AccRawData = spi.xfer([ACC_REG[0], ACC_REG[1], ACC_REG[2], ACC_REG[3], ACC_REG[4], ACC_REG[5]])
	
	AxRaw = int16( (AccRawData[1] << 8) | AccRawData[0] ) # type: ignore
	AyRaw = int16( (AccRawData[3] << 8) | AccRawData[2] ) # type: ignore
	AzRaw = int16( (AccRawData[5] << 8) | AccRawData[4] ) # type: ignore
	
	GxRaw = int16( (GyrRawData[1] << 8) | GyrRawData[0] ) # type: ignore
	GyRaw = int16( (GyrRawData[3] << 8) | GyrRawData[2] ) # type: ignore
	GzRaw = int16( (GyrRawData[5] << 8) | GyrRawData[4] ) # type: ignore
	# ~ print(xRaw)
	# ~ print("Gx: {0} ; Gy : {1} ; Gz : {2}".format(xRaw, yRaw, zRaw))

end = time.time_ns()
print("Time Taken: ",(end - start)/1000,"us")

spi.close()
