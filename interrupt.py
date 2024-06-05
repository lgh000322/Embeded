import RPi.GPIO as GPIO
import time

def my_handler(channel):
    print('pressed!')

GPIO.setmode(GPIO.BCM)
GPIO.setup(22,GPIO.IN)
GPIO.add_event_detect(22,GPIO.RISING, callback=my_handler, bouncetime=100)

try:
    while 1:
        print('do something')
        time.sleep(1)
finally:
    GPIO.cleanup()
