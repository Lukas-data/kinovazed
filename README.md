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

| issue:          | args:       |
| --------------- | ----------- |
| Initialize      | -           |
| Unfold          | -           |
| Retract         | -           |
| Freeze          | -           |
| Unfreeze        | -           |
| SetJoystickMode | \<mode name> |
| MoveJoystick    | \<js_X> : <js_Y> : <js_Z> |
| RunObjective    | \<objective name> |
| EStop           | -           |
| QuitEStop       | -           |

__events:__

Events have no arguments and are always sent __from__ kinovaZED __to__ roboRIO.

| issue:          |
| --------------- |
| Initialized     |
| JoystickModeSet |
| ObjectiveDone   |
| Unfolded        |
| Retracted       |
| Freezed         |
| Unfreezed       |
| Accepted        |
| Rejected        |

__command-event-mapping:__

Each _command_ has a positive reply _event_.
A reoccurring _command_ is answered with its positive reply if still valid.

| command         | positive event |
|---------------- | -------------- |
| Initialize      | Initialized    |
| Unfold          | Unfolded       |
| Retract         | Retracted      |
| Freeze          | Freezed        |
| Unfreeze        | Unfreezed      |
| SetJoystickMode | JoystickModeSet|
| MoveJoystick    | Accepted       |
| RunObjective    | ObjectiveDone  |
| EStop           | Accepted       |
| QuitEStop       | Accepted       |       

__heartbeat:__

    Heartbeat:<bitset>\r\n

Where the _bitset_ has 8bit and is transfered as an unsigned integer.
Each bit reflects an important state.

| bit (LSB first): | state of |
| ---------------- | -------- |
| 0 | isConnected |
| 1 | hasEmergencyStop |
| 2 | isInitialized |
| 3 | isFreezed   |
| 4 | isIdle    |
| 5 | isRunningObjective |
| 6 | isInJoystickMode   |
| 7 | isRetracted    |

# Joystick Modes

currently supported are:

| modes    | description |
| -------- | ----------- |
| NoMode   | robot is initialized, but doesn't accept Joystick inputs |
| XYZ      | robot moves linear on TCP axes |
| Rotation | robot does rotate around TPC axes |
| Axis1to3 | direct input for axis motors 1, 2 and 3 |
| Axis4to6 | direct input for axis motors 4, 5 and 6 |

# Objectives

current objectives:

| objective | description |
| --------- | ----------- |
| Current   | no movement / fictitious objective |
| Home      | custom home position |
| Handle    | end position near door Handle |
| OpenDoor  | operate door handle and pull a crack |
| PullDoor  | pull the door open and move into gap |
| SuckDoor  | bring suction cup into position | 
| Antenna   | position to pull door into lock behind wheelchair |

# Transitions

Transitions are changing state of the KinovaZED software by sending a command.
All valid transitions can be found in the the transition table of the state machine [here](https://iltserver.hsr.ch:9090/hsr-enhanced/kinovazed/blob/dev/include/control/CoreStateMachine.h).
The following transitions can be requested by the commander before an event is received, such called __interrupt command__.

| issued command | waiting for event | interrupt command |
| -------------- | ----------------- | ----------------- |
| RunObjective   | ObjectiveDone     | SetMode           |
| _any_          |                   | EStop             |

All unvalid transitions are answered with the _event_ __Rejected__.

# Usage

In the system _ZEDe_ the application is deployed on a raspberryPI 3+ running Ubuntu 18.04.
The Jaco Arm is connected to the raspberryPI by USB.
The raspberryPI is connected to the main controller roboRIO by Ethernet TCP.

# Installation

* Clone the source code into ~/Code/kinovazed
* Change the working directory into the new clone
* Configure the build environment:
    *  `mkdir build`
    *  `cd build`
    *  `cmake .. -DCMAKE_BUILD_TYPE=Release`
    *  `cd ..`
* Build the project: `cmake --build build --target all -- -j4`
* Install the result: `sudo cmake --build build --target install`

# Enable Disable Autostart

Start: autamatically if service enabled, else: `systemctl --user start kinovazed.service`

Stop: `systemctl --user stop kinovazed.service`

Enable autostart: `systemctl --user enable kinovazed.service`

Disable autostart: `systemctl --user disable kinovazed.service`

# Change Objectives

* open ssh: `ssh 192.168.77.2 -l kinova`
* stop the service (command above) - if nothing helps use `killall -9 kinovaZED`
* start the program with `kinovaZED`
* connect with telnet from your local machine: `telnet 192.168.77.2 51717`
* Initialize the arm, hit enter after every line: 
```
EStop
QuitEStop
Initialize
```
* Select the Objective you want to change: `SetActiveObjective:<Objective Name>`
* Use the Kinova Joystick to move the arm to target position
* read the Position relative to Objective origin: `GetObjectivePosition`
* kinovaZED reports Position in ssh shell log output as JSON
* If you want to determine an origin use the absolute position: `GetAbsolutePosition`
* you can now copy the JSON line from the log into the Objectives file and save it at: `~/Code/kinovazed/support/objectives/Cybathlon.json`
* install the project again (see above)
* restart the program and reconnect with telnet
* Initialize the arm, hit enter after every line: 
```
EStop
QuitEStop
Initialize
RunObjective:<Objective Name>
```
* if you are satisfied with the changes commit and push the changes!