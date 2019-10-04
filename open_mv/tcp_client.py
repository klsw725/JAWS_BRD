# TCP Client Example
#
# This example shows how to send and receive TCP traffic with the WiFi shield.

import sensor, image, time, network, usocket, sys

# AP info
SSID='Dmotion' # Network SSID
KEY='11270726'  # Network key

# Reset sensor
sensor.reset()

# Set sensor settings
sensor.set_contrast(1)
sensor.set_brightness(1)
sensor.set_saturation(1)
sensor.set_gainceiling(16)
sensor.set_framesize(sensor.QQVGA)
sensor.set_pixformat(sensor.GRAYSCALE)

# Init wlan module and connect to network
print("Trying to connect... (may take a while)...")

wlan = network.WINC()
wlan.connect(SSID, key=KEY, security=wlan.WPA_PSK)

# We should have a valid IP now via DHCP
print(wlan.ifconfig())

# Get addr info via DNS
#addr = usocket.getaddrinfo("www.google.com", 80)[0][4]
addr = usocket.getaddrinfo("www.ddotmotion.kr", 8880)[0][4]

# Create a new socket and connect to addr
client = usocket.socket(usocket.AF_INET, usocket.SOCK_STREAM)
client.connect(addr)

# Set timeout to 1s
client.settimeout(1.0)
clock = time.clock()

while (True):
    clock.tick() # Track elapsed milliseconds between snapshots().
    frame = sensor.snapshot()
    cframe = frame.compressed(quality=35)
    #client.send("I"+str(cframe.size()))
    client.send(cframe)
    print(clock.fps())

# Send HTTP request and recv response
#client.send("GET / HTTP/1.0\r\n\r\n")
#print(client.recv(1024))

# Close socket
client.close()
