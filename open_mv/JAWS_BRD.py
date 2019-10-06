# TCP Client Example
#
# This example shows how to send and receive TCP traffic with the WiFi shield.

import network, usocket, sensor, image, time, machine, pyb
from pyb import LED

sensor.reset()                      # Reset and initialize the sensor.
sensor.set_pixformat(sensor.RGB565) # Set pixel format to RGB565 (or GRAYSCALE)
sensor.set_framesize(sensor.VGA)   # Set frame size to QVGA (320x240)
sensor.skip_frames(time = 2000)     # Wait for settings take effect.
clock = time.clock()                # Create a clock object to track the FPS.

def whoAreU(line) :
    pass

led = LED(1) # red led

######## Configure external wakeup pin for sleep ######
extint = pyb.ExtInt("P0", pyb.ExtInt.IRQ_RISING, pyb.Pin.PULL_DOWN, whoAreU)
#######################################################

while True:
    # AP info
	SSID='Dmotion' # Network SSID
	KEY='11270726'  # Network key

    # Init wlan module and connect to network
	print("Trying to connect... (may take a while)...")

    wlan = network.WINC()
	wlan.connect(SSID, key=KEY, security=wlan.WPA_PSK)

    # We should have a valid IP now via DHCP
	print(wlan.ifconfig())

    # Get addr info via DNS
	addr = usocket.getaddrinfo("www.ddotmotion.kr", 8880)[0][4]

    # Create a new socket and connect to addr
	client = usocket.socket(usocket.AF_INET, usocket.SOCK_STREAM)
	try:
	    conn = client.connect(addr)
		#Set timeout to 1s
		client.settimeout(1.0)

        led.on()	# for debug

        while True:
		# Send HTTP request and recv response
		    img = sensor.snapshot()         # Take a picture and return the image.
			frame = img.compress(70)
			#print(frame.size())
			msg = bytes("image{:07}".format(frame.size()), "ascii")
			print(msg)
			client.send(msg)
			print(frame)
			client.send(frame)
			#client.send("GET / HTTP/1.0\r\n\r\n")

            led.off()
			pyb.stop()	##### Deep Sleep #####

            # Close socket
	except Exception as e:
	    print(e)
		client.close()
