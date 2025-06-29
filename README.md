# eater-8bit-cpu-programmer
Programmer for Ben Eater 8-bit computer

The programmer has 4 sample programs that can be selected with a button. It also allows programming via a serial terminal. it's used cheap CH32X033F8P6 microcontroller.

Data, Address and Write directly connected to DIP switches and Write button. For programming, all DIP switches must be set to the **up position** (If it's not the LED will light up) and the CPU must be in programming mode. Then press the left button.

Pre-programmed samples:
- Fibonacci
- Double
- FizzBuzz prints each number 1 to 255, printing 0 if the number is divisible by 3 and printing 255 if the number is divisible by 5 (required [additional instructions](https://github.com/procks/eater-8bit-cpu-programmer/tree/main/software/microcode-eeprom-programmer))
- Counts up to 255 and then counts down to 0

Also, a prime number generation program can be added with a 6-step modification, see [reddit post](https://www.reddit.com/r/beneater/comments/1lnd37j/primes_on_the_8bit_cpu/)

![programmer](https://github.com/procks/eater-8bit-cpu-programmer/blob/main/resources/programmer.jpg)
<img src="https://github.com/user-attachments/assets/5d55bb16-5db1-4cf6-8fc0-9f7c38e01131" width="250"><img src="https://github.com/procks/eater-8bit-cpu-programmer/blob/main/resources/cpu-back.jpg" width="250">

