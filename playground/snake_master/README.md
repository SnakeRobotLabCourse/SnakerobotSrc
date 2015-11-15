The objective is the master gets a message through Serial, which could be to multiple slaves,  
this is then parsed and the messages are sent to the respective slaves

Input Example: "8:S:30:500&9:G"
This means, send message to Slave:8, to 'S'et it's angle to 30 degrees in 500 milliseconds and 
send message to Slave:9, to 'G'et it's current angle in degrees.
The char array actually sent to Slave:8 would be "S:30:500" and to Slave:9, "G" which would be
parsed at the slaves and executed

Output: The addressed slaves respond with ACKs
