# SPI Control
#
# This example shows how to use the SPI bus on your OpenMV Cam to directly
# the LCD shield without using the built-in lcd shield driver. You will need
# the LCD shield to run this example.

import sensor, image, time
from pyb import Pin, SPI

TARGET_CMD = 0xA0   # Target Temp Command
SENSOR_CMD = 0xA1   # Ambient Temp Command

T_high_byte = 0
T_low_byte = 0
TARGET_TEMP = 0

cs  = Pin("P3", Pin.OUT_OD)
miso = Pin("P7", Pin.IN)
mosi = Pin("P8", Pin.OUT_PP)
sck  = Pin("P8", Pin.OUT_PP)
# The hardware SPI bus for your OpenMV Cam is always SPI bus 2.

spi = SPI(2, SPI.MASTER, baudrate=9600, polarity=0, phase=0)

def send_command(cCMD):
    cs.low()
    time.sleep(0.01)
    SPI.send(cCMD)
    time.sleep(0.01)
    T_low_byte = SPI.send_recv(0x22)
    time.sleep(0.01)
    T_high_byte = SPI.send_recv(0x22)
    cs.high()
    return (T_high_byte<<8 | T_low_byte)

while(True):
    TARGET_TEMP = (send_command(TARGET_CMD)/100)
    time.sleep(0.05)
    print("Target Temp : ", TARGET_TEMP)
