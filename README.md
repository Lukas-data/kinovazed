# Description

This Project is part of the Robot Arm Module of the powered wheelchair _ZED evolution_, short _ZEDe_.
KinovaZED is the bridge application between the wheelchair's main controller and the commercial 6DOF robot arm "Jaco" from Kinova.
The main purpose of the application is to translate, run and handle commands from the main controller into robot arm specific tasks and routines. 

## Interface to roboRIO

KinovaZED communicates with the main controller by a human readable protocol.
The Messages are designed as follows:

    <issue>:<arg 1>:<arg 2>:...:<arg n>\r\n


Where the placeholders are defined as:

- _issue_: the name of the issue
- _args_ : issue specific arguments

The Protocol is strictly ASCII in content.

1. all fields, even numbers are ASCII characters.
2. field separation is `:`, (colon)
3. message termination is `CRLF`, CR (carriage return) followed by a LF (linefeed), or `\r\n`

The interface is designed with three categories of messages:

__commands:__

Commands have __n arguments__ and are always sent __from__ roboRIO __to__ kinovaZED.

| issue:       | args:       |
| ----------   | ----------- |
| Initialize   | -           |
| Unfold       | -           |
| Retract      | -           |
| SetMode      | \<mode name> |
| MoveJoystick | \<js_X> : <js_Y> : <js_Z> |
| RunObjective | \<objective name> |
| EStop        | -           |
| QuitEStop    | -           |

__events:__

Events have no arguments and are always sent __from__ kinovaZED __to__ roboRIO.

| issue:        |
| ------------- |
| ModeSet       |
| ObjectiveDone |
| Unfolded      |
| Retracted     |

__heartbeat:__

    Heartbeat:<bitset>;

Where the _bitset_ has 8bit and is transfered as an unsigned integer.
Each bit reflects an important state.

| bit (LSB first): | state of |
| ---------------- | -------- |
| 0 | isConnected |
| 1 | hasEmergencyStop |
| 2 | isInitialized |
| 3 | isFreezed   |
| 4 | _unused_    |
| 5 | _unused_    |
| 6 | _unused_    |
| 7 | _unused_    |

# Modes

currently supported are:

| modes    | description |
| -------- | ----------- |
| NoMode   | robot is initialized, but doesn't accept Joystick inputs |
| XYZ      | robot moves linear on TCP axes |
| Rotation | robot does rotate around TPC axes |
| Axis1to3 | direct input for axis motors 1, 2 and 3 |
| Axis4to6 | direct input for axis motors 4, 5 and 6 |
| Freeze   | robot is initialized and freezed at a position* |

\* Freeze can only be entered and exited at defined positions

# Objectives

current objectives:

| modes    | description |
| -------- | ----------- |
| Current  | no movement / fictitious objective |
| Home     | custom home position |
| Handle   | end position near door Handle |
| OpenDoor | operate door handle and pull a crack |
| PullDoor | pull the door open and move into gap |
| SuckDoor | bring suction cup into position | 
| Antenna  | position to pull door into lock behind wheelchair |

# Usage

In the system _ZEDe_ the application is deployed on a raspberryPI 3+ running Ubuntu 18.04.
The Jaco Arm is connected to the raspberryPI by USB.
The raspberryPI is connected to the main controller roboRIO by Ethernet TCP.

# Installation