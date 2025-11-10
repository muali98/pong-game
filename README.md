# FPGA-Based Pong Game

This project implements a real-time Pong game on an **FPGA** platform using  using **custom VHDL IPs** (VGA, 7-segment display, input) with game logic in **embedded C** on a **MicroBlaze soft-core processor**, demonstrating efficient hardware–software co-design and real-time digital system control.



## Overview

The design combines modular hardware components and an embedded processor to replicate the classic Pong gameplay:

- **Button Driver:** Captures player inputs for paddle control, ensuring smooth and bounded movement.  
- **Seven-Segment Display:** Displays player scores using multiplexed digit control and numeric pattern generation.  
- **VGA Driver:** Generates synchronization signals and renders paddles, ball, and scores on a 640×480 VGA display.  
- **MicroBlaze Controller:** Implements game logic (collision detection, scoring, and game loop) through GPIO-based communication with hardware modules.

<img width="1000" height="750" alt="11" src="https://github.com/user-attachments/assets/7963c8c1-1867-42f3-bca1-0c264f260c20" />



## System Design Highlights

- **Compact Data Transfer:** Due to limited GPIO resources, multiple control and position signals are packed into 32-bit packets for efficient communication between the MicroBlaze and peripheral IPs.  
- **Smooth Paddle Control:** Counters and range-checking logic ensure stable and realistic paddle movement.  
- **Real-Time Rendering:** The VGA module synchronizes video output with hardware timing to produce smooth visuals.  

<img width="1000" height="750" alt="22" src="https://github.com/user-attachments/assets/ec74fdd9-61b0-4af4-ae02-a29d58ba05f2" />



## Demonstration

- **Video Demo:** [youtube.com/shorts/k2eMmJltsRw](https://youtube.com/shorts/k2eMmJltsRw)



## Summary

This FPGA-based Pong game demonstrates effective **hardware–software integration**, **real-time embedded control**, and **modular digital system design**. It highlights skills in **VHDL development**, **embedded programming**, and **FPGA-based system architecture**, aligning closely with research areas in **embedded systems design**.





