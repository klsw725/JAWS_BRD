#-*- coding:utf-8 -*-
import paho.mqtt.client as mqtt
import RPi.GPIO as gpio
import time

pin = 27

gpio.setmode(gpio.BCM) # Reference Pin number with RPi(Broadcom) board
gpio.setup(pin, gpio.OUT)
gpio.output(pin, False)

# callback when receive CONNECT response from server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    #client.subscribe("$SYS/#")
    client.subscribe("jaws/fortress")

# callback when receive 'publish message' from server.
def on_message(client, userdata, msg):
    sig = str(msg.payload.decode("utf-8"))
    print(sig + " toggle")
    #print(msg.topic + "_" + sig)
    #gpio.output(pin, sig)
    if sig == 'open':
        gpio.output(pin, True)
        time.sleep(1.0)
        gpio.output(pin, False)

def get_ipaddress(network):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    return socket.inet_ntoa(fcntl.ioctl(
        s.fileno(),
        0x8915, # SIOCGIFADDR
        struct.pack('256s', network[:15].encode('utf-8'))
        )[20:24])

port = 1883 # Port for MQTT Broker

client = mqtt.Client() # Create client object
client.on_connect = on_connect # Setting callback
client.on_message = on_message # Setting callback

# Connect to MQTT broker. client.connect(host, port, keepalive, bind_address)
# Use "client.connect_async()" for Asynchronous Connection.
client.connect("127.0.0.1", 1883)
client.loop_forever()
