## The master rules!

The objective is the Master gets a message through ROS publish/subscribe. The Master processes the received messages and sends the appropriate commands to the respective slaves via I2C. At the moment it is only possible to send one message at a time (this can be changed, if desired).

### Message Definition

Messages could look like follows: <slaveID>:<command>:<value>

Input Example: "9:S:900"
This means, send message to Slave 9, to 'S'et it's angle to 900 degrees.
Supported commands are:
- **S** for **S**et the angle
- **G** for **G**et the current angle
- **C** for **C**alibrate the servo (find min and max angles)

Output: The addressed slaves respond with ACKs

The code doesn't use the 'String' library and instead uses standard C functions and char arrays
on purpose to minimize dynamic memory usage and increase speed. The used ROS datatype is std_msgs/String

### ROS communication

Current setup:
- start roscore: `$ roscore`
- start the serial forwarder: `$ rosrun rosserial_python serial_node.py /dev/<usbport_of_the_connected_arduino>`	(port could be ttyUSB0)
- upload master code to arduino
- publish value from command line (pc to master) like this: `$ rostopic pub /snake_command std_msgs/String "data: '9:S:900'"`

The screenshot rqt\_graph\_for_rosconnection shows the rqt graph (all ROS nodes and their connections). You can show the current connections by using `$ rqt_graph` when some ROS nodes are running.

### Problems

The connection by serial is still a problem.. If the publisher is started from command line, the serial_node.py loses connection, tries to restart, but cant reconnect.

### Future Work

1. Set up a publisher for the pc, in order to publish useful values to the master (subscriber)
2. Connect the publisher from 1 to the movement model


