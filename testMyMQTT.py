# Tone Detection System
# Using mqtt for communication

#!/usr/bin/env python


import paho.mqtt.client as mqtt
import pyaudio
from numpy import zeros,linspace,short,fromstring,hstack,transpose,log
from scipy import fft
from time import sleep
from collections import deque
import requests
import pygame.mixer
from pygame.mixer import Sound

#GPIO

#audio stuff maybe
pygame.mixer.init(32000)
confirm = Sound("Music/OOT_Song_Correct.wav")

#sensitivity
SENSITIVITY= 1.0

#bandwidth
BANDWIDTH = 25

#frequency ranges
D4 = 630
E = 685
F = 755
G = 806
A = 890
B = 1000
D5 = 1175

#max min range
minD4 = D4-50
maxD4 = D4+BANDWIDTH
minE = E-BANDWIDTH
maxE = E+BANDWIDTH
minF = F-40
maxF = F+BANDWIDTH
minG = G-BANDWIDTH
maxG = G+BANDWIDTH
minA = A-BANDWIDTH
maxA = A+55
minB = B-BANDWIDTH
maxB = B+BANDWIDTH
minD5 = D5-BANDWIDTH
maxD5 = D5+BANDWIDTH

# Song note sequences
sun = deque(['A','F','D5','A','F','D5'])
time = deque(['A','D4','F','A','D4','F'])
storm = deque(['D4','F','D5','D4','F','D5'])
forest = deque(['D4','D5','B','A','B','A'])
saria = deque(['F','A','B','F','A','B'])
fire = deque(['F','D4','F','D4','A','F']) #This is just 6 notes, play all 8 if you want ;)
epona = deque(['D5','B','A','D5','B','A'])
zelda = deque(['E','G','D4','E','G','D4'])
heal = deque(['B','A','F','B','A','F'])
test = deque(['D4','F','A','B','D5','D4']) #Not a Zelda song, just nice to make sure everything's working
#heard note sequence deque
notes = deque(['G','G','G','G','G','G'], maxlen=6)

# Show the most intense frequency detected (useful for configuration)
frequencyoutput=True
freqNow = 1.0
freqPast = 1.0

#Set up audio sampler - 
NUM_SAMPLES = 2048
SAMPLING_RATE = 48000 

#init pyaudio
pa = pyaudio.PyAudio()
_stream = pa.open(format=pyaudio.paInt16,
                  channels=1, rate=SAMPLING_RATE,
                  input=True,
                  frames_per_buffer=NUM_SAMPLES)


#MQTT SERVER VARIABLE
mqtt_server = "localhost"
mqtt_port = 1883 

#mqtt callback function
#callback when message received
def on_message(client,userdata,message):
    print("message received " ,str(message.payload.decode("utf-8")))
    print("message topic=",message.topic)
    print("message qos=",message.qos)
    print("message retain flag=",message.retain)

#callback function when client connected to server
def on_connect(client,userdata,flag,rc):
    print("connected to Server")

    #if connection lose, it will reconnect and resubscribe
    client.subscribe("piCrust/home/lampu1")         #subscribe to mqtt topic

def togle():
    flag_output = False #default is false (no signal)
    try:
        while True:
            FAKE_GPIO = input("\nInput(1/0):")
            if FAKE_GPIO == 1:
                flag_output = (not flag_output)
                if flag_output == True:
                    client.publish("piCrust/home/lampu1","ON")
                else:
                    client.publish("piCrust/home/lampu1","OFF")
    except(KeyboardInterrupt):
	    print("cleaning up..")

def listenMic():
    while True: #listening
        while _stream.get_read_available() < NUM_SAMPLES:
            sleep(0.01)
            audio_data = fromstring(_stream.read( _stream.get_read_available()),dtype=short)[-NUM_SAMPLES:]
            
            normalized_data = audio_data / 32768.0
            intensity = abs(fft(normalized_data))[:NUM_SAMPLES/2]
            frequencies = linspace(0.0, float(SAMPLING_RATE)/2, num=NUM_SAMPLES/2)
            if frequencyoutput:
                which = intensity[1:].argmax()+1
                # use quadratic interpolation around the max
                if which != len(intensity)-1:
                    y0,y1,y2 = log(intensity[which-1:which+2:])
                    x1 = (y2 - y0) * .5 / (2 * y1 - y2 - y0)
                    # find the frequency and output it
                    freqPast = freqNow
                    freqNow = (which+x1)*SAMPLING_RATE/NUM_SAMPLES
                else:
                    freqNow = which*SAMPLING_RATE/NUM_SAMPLES
            if minD4 <= freqPast <= maxD5 and abs(freqNow-freqPast) <= 25:
                if minA<=freqPast<=maxA and minA<=freqNow<=maxA and notes[-1]!='A':
                    notes.append('A')
                    print "You played A!"
                elif minF<=freqPast<=maxF and minF<=freqNow<=maxF and notes[-1]!='F':
                    notes.append('F')
                    print "You played F!"
                elif freqPast <= maxD4 and minD4 <= freqNow <= maxD4 and notes[-1]!='D4':
                    notes.append('D4')
                    print "You played D4!"
                elif minD5 <= freqPast <= maxD5 and minD5 <= freqNow <= maxD5 and notes[-1]!='D5':
                    notes.append('D5')
                    print "You played D5!"
                elif minB<=freqPast<=maxB and minB<=freqNow<=maxB and notes[-1]!='B':
                    notes.append('B') 
                    print "You played B!"
                elif minE<=freqPast<=maxE and minE<=freqNow<=maxE and notes[-1]!='E':
                    notes.append('E')
                    print "You played E!"
                elif minG<=freqPast<=maxG and minG<=freqNow<=maxG and notes[-1]!='G':
                    notes.append('G')
                    print "You played G!"
                else:
                    print "What the heck is that?"

def checkNote():
    if notes == sun:
        print "\t\t\t\tSun song!"
        #publish to Bulb
        client.publish("piCrust/home/lampu1","1")
        notes.append('G')#append with 'G' to 'reset' notes, this keeps the song from triggering
    if notes == time:
        print "\t\t\t\tTime song!"
        client.publish("piCrust/home/serveo1","1")
        notes.append('G')






#setup MQTT
client = mqtt.Client("PiCrust")                 # delete "PiCrust" if had problem
client.on_connect = on_connect                  #set callback function
client.on_message = on_message
client.connect(mqtt_server,mqtt_port,300)       #connect to server
client.loop_start()

listenMic()
checkNote()

#togle()

#client.loop_stop()


            




            

