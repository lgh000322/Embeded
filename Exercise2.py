import RPi.GPIO as GPIO
import time
SWITCH_PIN1=4
SWITCH_PIN2=22
SWITCH_PIN3=23
SWITCH_PIN4=24
flag=True

def my_handler(channel):
    GPIO.output(SWITCH_PIN2,GPIO.LOW)
    GPIO.output(SWITCH_PIN3,GPIO.HIGH)
    
def my_handler2(channel):
    if flag==True:
        GPIO.remove_event_detect(SWITCH_PIN1)
        GPIO.add_event_detect(SWITCH_PIN1,GPIO.RISING,callback=my_handler3,bouncetime=100)
        flag=False
    else:
        GPIO.remove_event_detect(SWITCH_PIN1)
        GPIO.add_event_detect(SWITCH_PIN1,GPIO.RISING,callback=my_handler,bouncetime=100)
        flag=True

def my_handler3(channel):
    GPIO.output(SWITCH_PIN2,GPIO.HIGH)
    GPIO.output(SWITCH_PIN3,GPIO.LOW
)
def init():
    GPIO.output(SWITCH_PIN2,GPIO.HIGH)
    GPIO.output(SWITCH_PIN3,GPIO.LOW)

GPIO.setmode(GPIO.BCM)
GPIO.setup(SWITCH_PIN1,GPIO.IN)
GPIO.setup(SWITCH_PIN2,GPIO.OUT)
GPIO.setup(SWITCH_PIN3,GPIO.OUT)
GPIO.setup(SWITCH_PIN4,GPIO.IN)

GPIO.add_event_detect(SWITCH_PIN1,GPIO.RISING, callback=my_handler,bouncetime=100)
GPIO.add_event_detect(SWITCH_PIN4,GPIO.RISING, callback=my_handler2,bouncetime=100)

try:
    while 1:
        init()
        print("프로그램 실행중")        

finally:
    GPIO.cleanup()
