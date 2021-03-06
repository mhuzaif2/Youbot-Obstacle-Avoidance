
#include "ros/ros.h"
#include "nav_msgs/Odometry.h"
class Listener
{
public:
 double x;
 double y;
 double thx; double thy; double thz;double thw;

Listener()

	{x=0; y=0; thx=0; thy=0; thz = 0; thw =0;}
void chatterCallback(const nav_msgs::Odometry::ConstPtr& msg)
{
ROS_INFO("Seq: [%d]", msg->header.seq);
  ROS_INFO("Position-> x: [%f], y: [%f], z: [%f]", msg->pose.pose.position.x,msg->pose.pose.position.y, msg->pose.pose.position.z);
  ROS_INFO("Orientation-> x: [%f], y: [%f], z: [%f], w: [%f]", msg->pose.pose.orientation.x, msg->pose.pose.orientation.y, msg->pose.pose.orientation.z, msg->pose.pose.orientation.w);
  ROS_INFO("Vel-> Linear: [%f], Angular: [%f]", msg->twist.twist.linear.x,msg->twist.twist.angular.z);

x = msg->pose.pose.position.x;
y = msg->pose.pose.position.x;
thx = msg->pose.pose.orientation.x;
thy = msg->pose.pose.orientation.y;
thz = msg->pose.pose.orientation.z;
thw = msg->pose.pose.orientation.w;
}

};



int main(int argc, char** argv)
{

  geometry_msgs::Twist vel;
  Listener pose;
  ros::init(argc, argv, "subscriber_node"); 
  ros::NodeHandle n1; 
  ros::init(argc, argv, "publisher_node");  
  ros::NodeHandle n2; 
  ros::Publisher pub = n1.advertise<geometry_msgs::Twist>("cmd_vel",1000);
  ros::Subscriber sub = n1.subscribe("odom",1000,&Listener::chatterCallback,&pose); 
  ros::Rate loop_rate(5);

while(ros::ok())
 { 
  ros::spinOnce();

  vel.linear.x = 1;
  vel.linear.y = 1;
  vel.linear.z = 1;

//  vel.angular.x = 0;
//  vel.angular.y = 0;
//  vel.angular.z = 0;
  
  
  pub.publish(vel);


 loop_rate.sleep();
}
  
 

  return 0;
}




