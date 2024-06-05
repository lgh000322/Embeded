import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)
GPIO.setup(22,GPIO.IN)

try:
    while 1:
        switch=GPIO.input(22)
        if switch == 0:
            print('pressed')
        time.sleep(1)
finally:
    GPIO.cleanup()
