# Sensor Sleep Mode Example.
# This example demonstrates the sensor sleep mode. The sleep mode saves around
# 40mA when enabled and it's automatically cleared when calling sensor reset().

import sensor, image, time, machine, pyb
from pyb import LED

def whoAreU(line) :
    pass

led = LED(1) # red led

######## Configure external wakeup pin for sleep ######
extint = pyb.ExtInt("P0", pyb.ExtInt.IRQ_RISING, pyb.Pin.PULL_DOWN, whoAreU)
#######################################################

while True:
    sensor.reset()
	sensor.set_pixformat(sensor.RGB565)
	sensor.set_framesize(sensor.QVGA)
	led.on()
	sensor.skip_frames(time = 3000) # You may write down previous settings related to auto-gain and white balance and re-apply them to skip this on wakeup.
	img = sensor.snapshot()
	led.off()
	pyb.stop()
