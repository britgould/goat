from gpiozero import Servo
from gpiozero import AngularServo
from time import sleep

from gpiozero.pins.pigpio import PiGPIOFactory

factory = PiGPIOFactory()

servo = AngularServo(12, min_pulse_width=0.5/1000, max_pulse_width=2.5/1000, pin_factory=factory)

print("Start in the middle")
servo.mid()
sleep(5)
print("Go to min")
servo.min()
sleep(5)
print("Go to max")
servo.max()
sleep(5)
print("And back to middle")
servo.mid()
sleep(5)

print("Now with angles")
print("Go to -90")
servo.angle = -90
sleep(5)
print("Go to 0")
servo.angle = 0
sleep(5)
print("Go to 90")
servo.angle = 90
sleep(5)
print("And finally, None")
servo.value = None;
