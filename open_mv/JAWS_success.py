# Sensor Sleep Mode Example.
# This example demonstrates the sensor sleep mode. The sleep mode saves around
# 40mA when enabled and it's automatically cleared when calling sensor reset().

import sensor, image, time, machine, pyb, network, usocket, ubinascii
from pyb import LED

def whoAreU(line):
    pass

led = LED(1)  # red led for running check

######## Configure external wakeup pin for sleep ######
extint = pyb.ExtInt("P0", pyb.ExtInt.IRQ_RISING, pyb.Pin.PULL_DOWN, whoAreU)
#######################################################

while True:
    sensor.reset()
    sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.VGA)
    led.on()
    sensor.skip_frames(time=7000)  # You may write down previous settings related to auto-gain and white balance and re-apply them to skip this on wakeup.

    # AP info
    SSID = 'EZ2MAX-5s'  # Network SSID
    KEY = 'EZ2Connect'  # Network key

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
        # Set timeout to 1s
        client.settimeout(1.0)
        timer = time.ticks()
        while True:
            # Send HTTP request and recv response
            img = sensor.snapshot()         # Take a picture and return the image.
            frame = img.compress(30     )
            encodeframe = ubinascii.b2a_base64(frame)
            #print(frame.size())
            msg = bytes("image{:07}".format(len(encodeframe)), "ascii")
            client.send(msg)
            client.send(ubinascii.b2a_base64(frame))
            # client.send("GET / HTTP/1.0\r\n\r\n")
            # Close socket
            except Exception as e :
                print(e)
                client.close()
            led.off()
            pyb.stop()
