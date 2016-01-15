#include <ros.h>
#include <std_msgs/String.h>

ros::NodeHandle  nh;

std_msgs::String str_msg;
ros::Publisher statusPublisher("snake_status", &str_msg);
//char snakeStatus[16] = "moving";

//subscriber callback which will be called when the value of topic snake_command changes
void messageCb( const std_msgs::String& command){
  
  String cmd = command.data;
  nh.loginfo(cmd.c_str());
}

ros::Subscriber<std_msgs::String> commandSubscriber("snake_command", messageCb );

void setup()
{
  nh.initNode();
  nh.advertise(statusPublisher);
  nh.subscribe(commandSubscriber);
}

void loop()
{
  str_msg.data = "moving";
  statusPublisher.publish( &str_msg );    //publishes "moving" to the snake_status topic
  nh.spinOnce();
  delay(500);
}
