# GyverLampCpp MQTT

## `/state` information topic

Contains information about current state and effect parameters

## `/set` control topic

Publish to this topic to control lamp

### State commands

Control ON/OFF:

`{'state': 'ON'}`

`{'state': 'OFF'}`

Change effects:

`{'state': 'ON', 'switchTo': 'next'}`

`{'state': 'ON', 'switchTo': 'prev'}`

Change to effect id:

`{'state': 'ON', 'effect': 'fire'}`

### Effect commands

`{'brightness': 100}`

`{'scalle': 100}`

`{'speed': 100}`