
#include "ros/ros.h"
#include "nav_msgs/Odometry.h"
#include <iostream>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/squared_distance_2.h>
#include <math.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Polygon_2<K> Polygon_2;
typedef CGAL::Point_2<K> Point_2;
typedef CGAL::Segment_2<K> Segment_2;
typedef CGAL::Line_2<K> Line_2;


double crpdt(Point_2 a, Point_2 b);
double absolute(Point_2 a);
double dst_pt_lineseg(Segment_2 l, Point_2 pt);

class Listener
{
public:
 double x;
 double y;
 double thx; double thy; double thz; double thw;
 double x_fl; double y_fl; 
 double x_bl; double y_bl; 
 double x_fr; double y_fr; 
 double x_br; double y_br; 
Listener()

	{x=0; y=0; thx=0; thy=0; thz = 0; thw =0;}
void chatterCallback(const nav_msgs::Odometry::ConstPtr& msg)
{
//ROS_INFO("Seq: [%d]", msg->header.seq);
/*  ROS_INFO("Position-> x: [%f], y: [%f], z: [%f]", msg->pose.pose.position.x,msg->pose.pose.position.y, msg->pose.pose.position.z);
  ROS_INFO("Orientation-> x: [%f], y: [%f], z: [%f], w: [%f]", msg->pose.pose.orientation.x, msg->pose.pose.orientation.y, msg->pose.pose.orientation.z, msg->pose.pose.orientation.w);
  ROS_INFO("Vel-> Linear: [%f], Angular: [%f]", msg->twist.twist.linear.x,msg->twist.twist.angular.z);
*/
x = msg->pose.pose.position.x;
y = msg->pose.pose.position.x;
thx = msg->pose.pose.orientation.x;
thy = msg->pose.pose.orientation.y;
thz = msg->pose.pose.orientation.z;
thw = msg->pose.pose.orientation.w;
// Control points for the robot
// We have chosen the corner points.. Remember the dimensions of youbot are:
// Total Length: 580 mm, Total Width: 380 mm

x_fr = x + 0.29; // Front right Corner
y_fr = y - 0.19;

x_br = x - 0.29; // Back right Corner
y_br = y - 0.19;

x_fl = x + 0.29; // Front left Corner
y_fl = y + 0.19;

x_bl = x - 0.29; // Back left Corner
y_bl = y + 0.19;
}

};


// Projection on a line and a line segment

double dst_pt_lineseg (Segment_2 s, Point_2 pt)

{
double d1, d2, d, distance;
Point_2 proj_pt;
Line_2 l = s.supporting_line();
proj_pt = l.projection(pt); 


if (s.has_on(proj_pt)){
std::cout<<proj_pt.x()<<"\t"<<proj_pt.y()<<std::endl;
distance = (double) sqrt(squared_distance (proj_pt, pt));
}
else
{
d1 = squared_distance(proj_pt,s.vertex(0));
d2 = squared_distance(proj_pt,s.vertex(1));
d = std::min(d1,d2);

	if (d == d1){
		std::cout<<s.vertex(0)<<std::endl;
		distance = (double) sqrt(squared_distance(s.vertex(0),pt));
//		std::cout<<distance<<std::endl;
		    }
	else
		{
		std::cout<<s.vertex(1)<<std::endl;
		distance = (double) sqrt(squared_distance(s.vertex(1),pt));
		}
	
}
//std::cout<<distance<<std::endl;
return distance;
}

double dst_pt_pgn(Polygon_2 pgn, Point_2 pt)
{
double distance;
double distance1, distance2, distance3, distance4;
double min1, min2;

Segment_2 s1, s2, s3, s4;
s1 = pgn.edge(0);
s2 = pgn.edge(1);s3 = pgn.edge(2);s4 = pgn.edge(3);
distance1 = dst_pt_lineseg(s1,pt);
distance2 = dst_pt_lineseg(s2,pt);
distance3 = dst_pt_lineseg(s3,pt);
distance4 = dst_pt_lineseg(s4,pt);
min1 = std::min(distance1, distance2);
min2 = std::min(distance3, distance4);
distance = std::min(min1,min2);
//std::cout<<distance1<<"\t"<<distance2<<"\t"<<distance3<<"\t"<<distance4<<std::endl;

return distance;
}

int main(int argc, char** argv)
{
  double distance;
  geometry_msgs::Twist vel;
  Listener pose;
  ros::init(argc, argv, "subscriber_node"); 
  ros::NodeHandle n1; 
  ros::init(argc, argv, "publisher_node");  
  ros::NodeHandle n2; 
  ros::Publisher pub = n1.advertise<geometry_msgs::Twist>("cmd_vel",1000);
  ros::Subscriber sub = n1.subscribe("odom",1000,&Listener::chatterCallback,&pose); 
  ros::Rate loop_rate(5);


/* 
Need to save the obstacles in the form of polygons here
*/
  Point_2 o11(2,2), o12(3,2), o13(3,3), o14(2,3); // obs_1 red in color
   Point_2 obs_1[]={o11,o12,o13,o14};
   Polygon_2 pgn_1(obs_1, obs_1+4);

   Point_2 o21(4.625,0.625), o22(5.375,0.625), o23(5.375,1.375), o24(5.375,1.375); // obs_2 green in color
   Point_2 obs_2[]={o21,o22,o23,o24};
   Polygon_2 pgn_2(obs_2, obs_2+4);

   Point_2 o31(2.75,4.75), o32(3.25,4.75), o33(3.25,5.25), o34(2.75,5.25); // obs_3 blue in color
   Point_2 obs_3[]={o31,o32,o33,o34};
   Polygon_2 pgn_3(obs_3, obs_3+4);

   Point_2 o41(-0.125,2.875), o42(0.125,2.875), o43(0.125,3.125), o44(-0.125,3.125); // obs_4 purple in color
   Point_2 obs_4[]={o41,o42,o43,o44};
   Polygon_2 pgn_4(obs_4, obs_4+4);

// Distance from the edges of an obstacle


while(ros::ok())
 { 
  ros::spinOnce();

  vel.linear.x = 0.5;
  vel.linear.y = 0.5;
  vel.linear.z = 0;

  pub.publish(vel);

//Point_2 m(pose.x_fl,pose.y_fl);
Point_2 m(-1,-1);
//Segment_2 s(o11,o12);

//distance = dst_pt_lineseg(s, m);
distance = dst_pt_pgn(pgn_1, m);
std::cout<<distance<<std::endl;

loop_rate.sleep();
}
  
  vel.linear.x = 0.0;
  vel.linear.y = 0.0;
  vel.linear.z = 0;

 

  return 0;
}


// Distance from a line -- Wont Need probably

double dst_pt_line(Point_2 p1, Point_2 p2,Point_2 pt )
{

double distance;
Point_2 intermed1(p2.x() - p1.x(), p2.y() - p1.y());
Point_2 intermed2(pt.x() - p1.x(), pt.y() - p1.y());
distance =  fabs(crpdt(intermed1,intermed2));
distance = (double) distance/absolute(intermed1);
//std::cout<<distance<<std::endl;
//Segment_2 s(p1,p2);
//distance = squared_distance(s,pt);
return distance;
}

double crpdt(Point_2 a,Point_2 b )
{ double result;
 result = a.x()*b.y() - a.y()*b.x();
//std::cout<<result<<std::endl;
   return result;

}

 double absolute(Point_2 a)
{
double result = (double)pow((pow(a.x(),2) + pow(a.y(),2)) , 0.5);

return result;
}

