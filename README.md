# Description

This Project is part of the Robot Arm Module of the powered wheelchair _ZED evolution_, short _ZEDe_.
KinovaZED is the bridge application between the wheelchair's main controller and the commercial 6DOF robot arm "Jaco" from Kinova.
The main purpose of the application is to translate, run and handle commands from the main controller into robot arm specific tasks and routines. 

## Interface to roboRIO

KinovaZED communicates with the main controller by a human readable protocol.
The Messages are designed as follows:

    <issue>:<arg 1>:<arg 2>:...:<arg n>;

Where the placeholders are defined as:

- _issue_: the name of the issue
- _args_ : issue specific arguments

The structure is mainly given by separation of message content with a `:` (colon) and a `;` semicolon for message termination.

The interface is designed with three categories of messages:

__commands:__

Commands have __n arguments__ and are always sent __from__ roboRIO __to__ kinovaZED.

| issue:     | args:       |
| ---------- | ----------- |
| Initialize | -           |
| NoMode     | -           |
| SetMode    | \<mode name> |
| Move       | \<js_X> : <js_Y> : <js_Z> |
| GoTo       | \<objective> |
| EStop      | -           |
| QuitEStop  | -           |

__events:__

Events have no arguments and are always sent __from__ kinovaZED __to__ roboRIO.

| issue:       |
| ------------ |
| Initialized  |
| ModeSet      |
| SequenceDone |

__heartbeat:__

    Heartbeat:<bitset>;

Where the _bitset_ has 8bit and is transfered as an unsigned integer.
Each bit reflects an important state.

| bit (LSB first): | state of |
| ---------------- | -------- |
| 0 | is connected       |
| 1 | has Emergency Stop |
| 2 | is initialized     |
| 3 | _unused_ |
| 4 | _unused_ |
| 5 | _unused_ |
| 6 | _unused_ |
| 7 | _unused_ |

# Usage

In the system _ZEDe_ the application is deployed on a raspberryPI 3+ running Ubuntu 18.04.
The Jaco Arm is connected to the raspberryPI by USB.
The raspberryPI is connected to the main controller roboRIO by Ethernet TCP.

# Installation