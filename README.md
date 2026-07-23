# field-oriented motor controller

A smart servo 79 × 35 mm PCB that mounts behind a gimbal-class brushless motor and turns it into a bus-connected robot joint. 

![3D render](pics/front.png)
![back](pics/back.png)

An STM32G431 runs field-oriented control (FOC) which measures each phase's current through 33 mΩ shunts on the driver's per-phase ground returns
(amplified by the MCU's internal op-amps), reads the rotor angle from an AS5047P 14-bit magnetic encoder over SPI, and drives a DRV8313 three-phase
driver with the PWM pattern that produces exactly the commanded torque. Velocity and position loops sit on top of the current loop, and everything is
commanded over CAN with paired daisy-chain connectors, so several joints share one bus and one power feed.

## Board Layout
![layout](pics/3d-model.png) 

## Design notes

**Rev A** -- Jul 22, 2026

Designed from the DRV8313 and STM32G431 datasheets, with ST's B-G431B-ESC1 (UM2516) and mjbots moteus as references for the current-sense
network and CAN servo architecture. Verification was done with `kicad-cli` ERC/DRC plus scripted netlist audits rather than eyeballing. 
Bugs caught before ordering in roughly descending severity:

- MCU VSS pins no-connected (a stray ✕ flag excused all three ground pads)
- The entire GND net silently absorbed into a sense line after a filter cap was deleted during rework
- A UART_TX label dropped onto the reset net, tying PA2 to NRST
- Current-sense networks bypassed on two of three phases
- Thermal vias under the driver's PowerPAD carrying no net. Zones kept clearance from them, which leaves the chip's cooling path open-circuit
- Assorted: swapped charge-pump/LDO bypass caps, a broken VM route, orphan vias, a 3.6 V rail that thought it was 5 V (wrong FB divider)

Every one of these passed visual inspection and most passed ERC.

**Lesson learnt:** Audit the netlist!!!! Not the picture.

## Status

I'm doing this because I want to learn about the mechatronics side of robotics. This will hopefully be developed into a PID project.

**July 23, 2026** 

Rev A ordered from JLCPCB. 4-layer, 79 × 35 mm. Schematic netlist-verified, ERC/DRC clean (0 errors). Awaiting boards;
firmware bring-up starting on ST's B-G431B-ESC1 in the meantime. Bring-up log to follow.
