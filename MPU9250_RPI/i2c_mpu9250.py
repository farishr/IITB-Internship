import smbus
import time
from numpy import int16
 
address = 0x68

bus = smbus.SMBus(1)
# ~ bus.open(1)

# ~ start = time.time_ns()
# ~ for x in range(1000):
while True:
	
	ACCEL_X_H, ACCEL_X_L, ACCEL_Y_H, ACCEL_Y_L, ACCEL_Z_H, ACCEL_Z_L = bus.read_i2c_block_data(address, 0x3B, 6)
	GYRO_X_H, GYRO_X_L, GYRO_Y_H, GYRO_Y_L, GYRO_Z_H, GYRO_Z_L  = bus.read_i2c_block_data(address, 0x43, 6)
	
	Ax = ACCEL_X_H << 8 | ACCEL_X_L
	Ay = ACCEL_Y_H << 8 | ACCEL_Y_L
	Az = ACCEL_Z_H << 8 | ACCEL_Z_L
	Gx = GYRO_X_H << 8 | GYRO_X_L
	Gy = GYRO_Y_H << 8 | GYRO_Y_L
	Gz = GYRO_Z_H << 8 | GYRO_Z_L
	
	print (" Ax: {0} ; Ay : {1} ; Az : {2} ; Gx: {3} ; Gy : {4} ; Gz : {5}".format(int16(Ax), int16(Ay), int16(Az), int16(Gx), int16(Gy), int16(Gz)))
	# ~ print ("Gx: {0} ; Gy : {1} ; Gz : {2}".format(int16(Gx), int16(Gy), int16(Gz)))
	# ~ print (" Ax: {0} ; Ay : {1} ; Az : {2}".format(int16(Ax), int16(Ay), int16(Az)))
	
	time.sleep(0.1)

# ~ end = time.time_ns()
# ~ print("Time Taken: ",(end - start)/1000,"us")
