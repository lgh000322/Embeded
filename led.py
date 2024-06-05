import RPi.GPIO as GPIO
from time import sleep

Data_In=4
CLK_LAT=22
CLK_Sh=23

GPIO.setmode(GPIO.BCM)
GPIO.setup(Data_In,GPIO.OUT)
GPIO.setup(CLK_LAT,GPIO.OUT)
GPIO.setup(CLK_Sh,GPIO.OUT)

signal=[1,0,1,0,1,0,1,0]

try:
    while 1:
        for s in signal:
            GPIO.output(Data_In,s)
            GPIO.output(CLK_Sh,GPIO.HIGH)
            sleep(0.1)
            GPIO.output(CLK_Sh,GPIO.LOW)
            sleep(0.1)
        GPIO.output(CLK_LAT,GPIO.HIGH)
        sleep(0.1)
        GPIO.output(CLK_LAT,GPIO.LOW)
        sleep(0.1)
finally:
    GPIO.cleanup()
