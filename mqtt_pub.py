#-*- coding:utf-8 -*-
import paho.mqtt.client as mqtt

mqtt = mqtt.Client("python_pub") # Create MQTT Client object
mqtt.connect("192.168.0.52", 1883) # Connect to MQTT server

# Publish topics & messages
'''
mqtt.publish("nodemcu", "led")
mqtt.publish("nodemcu", "led off")
'''

mqtt.publish("jaws/fortress", "open") 

mqtt.loop(2) # Timeout 2 sec.
