# Analog Synth with a Lowpass Filter and LFO  



I made an analog synthesiser with three knobs:
 * The first one control the pitch of the oscillator.
 * The second one fine tune the pitch of the oscillator in a smaller range.
 * The third one controls the **LFO rate** of an **lowpass filter**.  

#### Video 1  
 https://user-images.githubusercontent.com/74963879/144049626-6d8c0f99-a2b4-460f-b136-61e2f882a029.mp4

[Referenced tutorial](https://www.youtube.com/watch?v=3tMGNI--ofU) for the lowpass filter. The amount of resistance determines the **cutoff frequency** of the filter. However, instead of using a potentiometer to manually control the cutoff, I used a photoresistor. Therefore it's able to use a fading LED to create an LFO for the cutoff.

#### Video 2  
https://user-images.githubusercontent.com/74963879/144050075-43999b2a-7ed0-4d33-bc83-3e880cf347e3.mp4

The potentiometer and the LED are next to each other:  
![analog](https://github.com/msc-creative-computing/p-comp-jasper-zheng/blob/main/week_08/src/IMG_1501.jpg?raw=true)

![analog](https://github.com/msc-creative-computing/p-comp-jasper-zheng/blob/main/week_08/src/IMG_1499.jpg?raw=true)


[Tinkercad schematic](https://www.tinkercad.com/things/7GDv4Wp5EWa-swanky-bruticus-kas)    
![scjematic](https://github.com/msc-creative-computing/p-comp-jasper-zheng/blob/main/week_08/src/schematic.png?raw=true)  
(The Arduino code is basically just using the potentiometer on the right to control the fading speed of the LED, which I could have done it using the hex inverter as well, however the result always give a square wave and it's just not as smooth as using an Arduino.)  
