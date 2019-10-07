# Sensor Sleep Mode Example.
# This example demonstrates the sensor sleep mode. The sleep mode saves around
# 40mA when enabled and it's automatically cleared when calling sensor reset().

import sensor, image, time, pyb
from pyb import LED

led = LED(1)  # red led for running check

def test():
	sensor.sleep(False)
	led.on()
	sensor.reset()                      # Reset and initialize the sensor.
	sensor.set_pixformat(sensor.RGB565) # Set pixel format to RGB565 (or GRAYSCALE)
	sensor.set_framesize(sensor.QVGA)   # Set frame size to QVGA (320x240)
	sensor.skip_frames(time = 3000)     # Capture frames for 3000ms.
	sensor.sleep(True)                  # Enable sensor sleep mode (saves about 40mA)
	led.off()

######## Configure external wakeup pin for sleep ######
extint = pyb.ExtInt("P7", pyb.ExtInt.IRQ_RISING, pyb.Pin.PULL_DOWN,  test)
#######################################################

test()
while True:
	time.sleep(100)
	pass
