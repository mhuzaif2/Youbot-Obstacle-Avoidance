#include <CGAL/Random.h>
#include "ros/ros.h"
#include "nav_msgs/Odometry.h"
#include <iostream>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/squared_distance_2.h>
#include <math.h>
#include <CGAL/Cartesian_matrix.h>
#include <iterator>
#include "cmatrix"
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

typedef CGAL::Polygon_2<K> Polygon_2;
typedef CGAL::Point_2<K> Point_2;
typedef CGAL::Segment_2<K> Segment_2;
typedef CGAL::Line_2<K> Line_2;
typedef CGAL::Vector_2<K> Vector_2;
typedef CGAL::Vector_3<K> Vector_3;
typedef techsoft::matrix<double> Matrix;

double crpdt(Point_2 a, Point_2 b);
double absolute(Point_2 a);
double dst_pt_lineseg(Segment_2 l, Point_2 pt, Point_2 &pt_cl);
Vector_3 rep_potential_field(double dist_rep,Point_2 p1,Point_2 p2,Point_2 p3,Point_2 p4,Point_2 p5, Point_2 ctrl_pt, Point_2 jac);

Vector_2 att_potential_field(double dist_att, Point_2 goal, Point_2 ctrl_pt);

void config_force(Matrix wksp_force, Matrix jac, Matrix &u);

class Listener
{
public:
 double x;
 double y;
 double thx; double thy; double thz; double thw; double th;
 double x_fl; double y_fl; 
 double x_bl; double y_bl; 
 double x_fr; double y_fr; 
 double x_br; double y_br; 
 ~Listener();
Listener()

	{x=0; y=0; thx=0; thy=0; thz = 0; thw =0;  th=0;
	x_fr = x + 0.29; // Front right Corner
y_fr = y - 0.19;

x_br = x - 0.29; // Back right Corner
y_br = y - 0.19;

x_fl = x + 0.29; // Front left Corner
y_fl = y + 0.19;

x_bl = x - 0.29; // Back left Corner
y_bl = y + 0.19; }

void chatterCallback(const nav_msgs::Odometry::ConstPtr& msg)
{


x = msg->pose.pose.position.x;
y = msg->pose.pose.position.x;
th = msg->twist.twist.angular.z;
// Control points for the robot
// We have chosen the corner points.. Remember the dimensions of youbot are:
// Total Length: 580 mm, Total Width: 380 mm

x_fr = x -0.29*sin(th)+0.19*cos(th); // Front right Corner
y_fr = y +0.29*cos(th)+0.19*sin(th);;

x_br = x +0.29*sin(th)+0.19*cos(th); // Back right Corner
y_br = y -0.29*cos(th)+0.19*sin(th);

x_fl = x -0.29*sin(th)-0.19*cos(th); // Front left Corner
y_fl = y +0.29*cos(th)-0.19*sin(th);

x_bl = x +0.29*sin(th)-0.19*cos(th); // Back left Corner
y_bl = y -0.29*cos(th)-0.19*sin(th);
}

Point_2 jacobian_fl()
{

std::cout<<"came to jac"<<std::endl;
double r11, r21;
double j1, j2;
r11 = 0.29;
r21 = 0.19;

j1 = -r11*sin(th)-r21*cos(th);
j2 = r11*cos(th)-r21*sin(th);

Point_2 result(j1,j2);

return result;
}

Point_2 jacobian_fr()
{

std::cout<<"came to jac"<<std::endl;
double r11, r21;
double j1, j2;
r11 = 0.29;
r21 = -0.19;

j1 = -r11*sin(th)-r21*cos(th);
j2 = r11*cos(th)-r21*sin(th);

Point_2 result(j1,j2);

return result;


}

Point_2 jacobian_bl()
{
std::cout<<"came to jac"<<std::endl;
double r11, r21;
double j1, j2;
r11 = -0.29;
r21 = 0.19;


j1 = -r11*sin(th)-r21*cos(th);
j2 = r11*cos(th)-r21*sin(th);

Point_2 result(j1,j2);

return result;
}

Point_2 jacobian_br()
{
std::cout<<"came to jac"<<std::endl;
double r11, r21;
double j1, j2;
r11= -0.29;
r21 = -0.19;


j1 = -r11*sin(th)-r21*cos(th);
j2 = r11*cos(th)-r21*sin(th);

Point_2 result(j1,j2);

return result;

}

};


// Projection on a line and a line segment

double dst_pt_lineseg (Segment_2 s, Point_2 pt, Point_2 &pt_cl)

{
double d1, d2, d, distance;
Point_2 proj_pt;
Line_2 l = s.supporting_line();
proj_pt = l.projection(pt); 


if (s.has_on(proj_pt))
{

pt_cl = proj_pt;
distance = (double) sqrt(squared_distance (proj_pt, pt));
}
else
{
d1 = squared_distance(proj_pt,s.vertex(0));
d2 = squared_distance(proj_pt,s.vertex(1));
d = std::min(d1,d2);

	if (d == d1){

		distance = (double) sqrt(squared_distance(s.vertex(0),pt));
		pt_cl = s.vertex(0);

		    }
	else
		{

		pt_cl = s.vertex(1);
		distance = (double) sqrt(squared_distance(s.vertex(1),pt));
		}
	
}

return distance;
}

double dst_pt_pgn(Polygon_2 pgn, Point_2 pt, Point_2 &pt_cl)
{
double distance;
double distance1, distance2, distance3, distance4;
double min1, min2;
Point_2 p1, p2, p3, p4;

Segment_2 s1, s2, s3, s4;
s1 = pgn.edge(0);
s2 = pgn.edge(1);s3 = pgn.edge(2);s4 = pgn.edge(3);
distance1 = dst_pt_lineseg(s1,pt,p1);
distance2 = dst_pt_lineseg(s2,pt,p2);
distance3 = dst_pt_lineseg(s3,pt,p3);
distance4 = dst_pt_lineseg(s4,pt,p4);
min1 = std::min(distance1, distance2);
min2 = std::min(distance3, distance4);
distance = std::min(min1,min2);
if (distance == min1)
{
	if (min1 == distance1)
		pt_cl = p1;
	else
		pt_cl = p2;
}
else
{
	if (min2 == distance3)
		pt_cl = p3;
	else
		pt_cl = p4;
}


return distance;
}


double dst_from_obs(Point_2 p_ctrl, Polygon_2 pgn_1, Polygon_2 pgn_2, Polygon_2 pgn_3, Polygon_2 pgn_4, Polygon_2 pgn_5, Point_2 &p1, Point_2 &p2, Point_2 &p3, Point_2 &p4, Point_2 &p5, Point_2 &pt_cl)
{
// Gives distance to the nearest obstacle as well as the position of the nearest point on the obstacle
double distance;
double d1, d2, d3, d4, d5;
double min1, min2;
//Point_2 p1, p2, p3, p4, p5;

d1 = dst_pt_pgn(pgn_1, p_ctrl, p1);
d2 = dst_pt_pgn(pgn_2, p_ctrl, p2);
d3 = dst_pt_pgn(pgn_3, p_ctrl, p3);
d4 = dst_pt_pgn(pgn_4, p_ctrl, p4);
d5 = dst_pt_pgn(pgn_5, p_ctrl, p5);

min1 = std::min(d1,d2);
min2 = std::min(d3,d4);

// Getting Minimum distance of all the obstacles
distance = std::min(min1,min2);
distance = std::min(d5,distance);
// Getting Closest Point of all the obstacles
if (distance == min1)
{
	if (min1 == d1)
		pt_cl = p1;
	else
		pt_cl = p2;
}
else if (distance == min2)
{
	if (min2 == d3)
		pt_cl = p3;
	else
		pt_cl = p4;
}
else 
  		pt_cl = p5;



//std::cout<<distance<<std::endl;
//std::cout<<pt_cl<<std::endl;
return distance;
}

Vector_3 planner(Listener &p, Polygon_2 pgn_1, Polygon_2 pgn_2, Polygon_2 pgn_3, Polygon_2 pgn_4, Polygon_2 pgn_5, Point_2 goal)
{

  double dist_rep;
  double att_gain = 2;
  double att_thresh = 0.5;
  double dist_att_fl,dist_att_fr,dist_att_bl,dist_att_br;
  double dist_rep_fl,dist_rep_fr,dist_rep_bl,dist_rep_br;
  Point_2 nearest_fl, nearest_fr, nearest_bl,nearest_br;

  Point_2 fl(p.x_fl, p.y_fl);
  Point_2 fr(p.x_fr, p.y_fr);
  Point_2 bl(p.x_bl, p.y_bl);
  Point_2 br(p.x_br, p.y_br);
  
  //std::cout<<fl<<"\t"<<fr<<"\t"<<bl<<"\t"<<br<<std::endl;
   Point_2 p1_fl,p2_fl,p3_fl,p4_fl, p5_fl;
   Point_2 p1_fr,p2_fr,p3_fr,p4_fr, p5_fr;
   Point_2 p1_bl,p2_bl,p3_bl,p4_bl, p5_bl;
   Point_2 p1_br,p2_br,p3_br,p4_br, p5_br;
  dist_rep_fl = dst_from_obs(fl, pgn_1,pgn_2,pgn_3,pgn_4, pgn_5, p1_fl,p2_fl,p3_fl,p4_fl, p5_fl, nearest_fl);
  dist_rep_fr = dst_from_obs(fr, pgn_1,pgn_2,pgn_3,pgn_4, pgn_5, p1_fr,p2_fr,p3_fr,p4_fr, p5_fr, nearest_fr);
  dist_rep_bl = dst_from_obs(bl, pgn_1,pgn_2,pgn_3,pgn_4, pgn_5, p1_bl,p2_bl,p3_bl,p4_bl, p5_bl, nearest_bl);
  dist_rep_br = dst_from_obs(br, pgn_1,pgn_2,pgn_3,pgn_4, pgn_5, p1_br,p2_br,p3_br,p4_br, p5_br, nearest_br);
  //std::cout<<nearest_fl<<"\t"<<nearest_fr<<"\t"<<nearest_bl<<"\t"<<nearest_br<<std::endl;
  dist_att_fl = (double) sqrt(squared_distance(fl, goal));
  dist_att_fr = (double) sqrt(squared_distance(fr, goal));
  dist_att_bl = (double) sqrt(squared_distance(bl, goal));
  dist_att_br = (double) sqrt(squared_distance(br, goal));
 
//  dist_rep_fl = (double) sqrt(squared_distance(fl, nearest_fl));
//  dist_rep_fr = (double) sqrt(squared_distance(fr, nearest_fr));
//  dist_rep_bl = (double) sqrt(squared_distance(bl, nearest_bl));
//  dist_rep_br = (double) sqrt(squared_distance(br, nearest_br));
 // Attractive Potential Field  
//  OR
 // Forces in Workspace


   Vector_2 dU_att_fl(1,1), dU_att_fr(1,1), dU_att_bl(1,1), dU_att_br(1,1);

	dU_att_fl = att_potential_field(dist_att_fl,goal, fl);
        dU_att_fr = att_potential_field(dist_att_fr,goal, fr);
	dU_att_bl = att_potential_field(dist_att_bl,goal, bl);
	dU_att_br = att_potential_field(dist_att_br,goal, br);

   //     std::cout<<dU_att_fl<<"\t"<<dU_att_fr<<"\t"<<dU_att_bl<<"\t"<<dU_att_br<<std::endl;
      //  std::cout<<dU_att_fl.x()<<"\t"<<dU_att_fl.y()<<std::endl;
 // Repulsive Potential Field


Point_2 j1, j2, j3, j4;     
    j1 = p.jacobian_fl();
    j2 = p.jacobian_fr();
    j3 = p.jacobian_bl();
    j4 = p.jacobian_br();
   
  Vector_3 dU_rep_fl(1,1,1), dU_rep_fr(1,1,1), dU_rep_bl(1,1,1), dU_rep_br(1,1,1);
    dU_rep_fl = rep_potential_field(dist_rep_fl,p1_fl,p2_fl,p3_fl,p4_fl, p5_fl, fl, j1);
    dU_rep_fr = rep_potential_field(dist_rep_fr,p1_fr,p2_fr,p3_fr,p4_fr, p5_fr, fr, j2);
    dU_rep_bl = rep_potential_field(dist_rep_bl,p1_bl,p2_bl,p3_bl,p4_bl, p5_bl, bl, j3);
    dU_rep_br = rep_potential_field(dist_rep_br,p1_br,p2_br,p3_br,p4_br, p5_br, br, j4);

//   std::cout<<dU_rep_fl<<"\t"<<dU_rep_fr<<"\t"<<dU_rep_bl<<"\t"<<dU_rep_br<<std::endl;
 // Forces in ConfigSpace

    
//std::cout<<j1<<"\t"<<j2<<"\t"<<j3<<"\t"<<j4<<std::endl;
    double u1, u2, u3;
    u1 = dU_att_fl.x() + dU_att_fr.x() + dU_att_bl.x() + dU_att_br.x();
    u2 = dU_att_fl.y() + dU_att_fr.y() + dU_att_bl.y() + dU_att_br.y();
    u3 = (j1.x()*dU_att_fl.x()+ j1.y()*dU_att_fl.y()) + (j2.x()*dU_att_fr.x()+ j2.y()*dU_att_fr.y()) + (j3.x()*dU_att_bl.x()+ j3.y()*dU_att_bl.y()) + (j4.x()*dU_att_br.x()+ j4.y()*dU_att_br.y());
    Vector_3 u_att(u1,u2,u3);

//std::cout<<u1<<"\t"<<u2<<"\t"<<u3<<std::endl;
   
    u1 = dU_rep_fl.x() + dU_rep_fr.x() + dU_rep_bl.x() + dU_rep_br.x();
    u2 = dU_rep_fl.y() + dU_rep_fr.y() + dU_rep_bl.y() + dU_rep_br.y();
    u3 = dU_rep_fl.z() + dU_rep_fr.z() + dU_rep_bl.z() + dU_rep_br.z();
   Vector_3 u_rep(u1,u2,u3);
//   std::cout<<u_rep<<std::endl;
    std::cout<<"passed config space"<<std::endl;

    Vector_3 u(u_att.x()+u_rep.y(),u_att.y()+u_rep.z(),u_att.z()+u_rep.z());
  // std::cout<<u<<std::endl;
return u;
}



Vector_2 att_potential_field(double dist_att, Point_2 goal, Point_2 ctrl_pt)
{

  double att_gain = 0.1;
  double att_thresh = 0.5;
  double U_att;

  
  double a, b;
 
 
  a= -ctrl_pt.x() + goal.x();
  b= -ctrl_pt.y() + goal.y();


 Point_2 q_qgoal(a,b); 

 if (dist_att < att_thresh) 
		{
   U_att =  0.5 * att_gain * pow(dist_att,2);

   a =(double) (q_qgoal.x() *att_gain);
   b =(double) (q_qgoal.y() *att_gain);

 		}
  else
		{
   U_att = att_thresh * att_gain * dist_att - 0.5 * att_gain * pow(att_thresh,2);
   a = (q_qgoal.x() *att_gain*att_thresh)/dist_att;
   b =  (q_qgoal.y() *att_gain*att_thresh)/dist_att;

		}
   Vector_2 dU_att(a,b);
//  std::cout<<dU_att<<std::endl;
return dU_att;

}

Vector_3 rep_potential_field(double dist_rep,Point_2 p1, Point_2 p2, Point_2 p3, Point_2 p4, Point_2 p5, Point_2 ctrl_pt, Point_2 j)
{
  double rep_gain = 2;
  double rep_thresh = 1;

  double a, b,c;

  a= ctrl_pt.x() - p1.x();
  b= ctrl_pt.y() - p1.y();


  if (squared_distance(ctrl_pt,p1)<rep_thresh)
	{  a=(ctrl_pt.x() - p1.x())/squared_distance(ctrl_pt,p1);b = (ctrl_pt.y() - p1.y())/squared_distance(ctrl_pt,p1);}
  else
	 { a=0;b=0;}

  Vector_2 Del_dq1(a,b);
 
  a= ctrl_pt.x() - p2.x();
  b= ctrl_pt.y() - p2.y();

  if (squared_distance(ctrl_pt,p2)<rep_thresh)
	{  a=(ctrl_pt.x() - p1.x())/squared_distance(ctrl_pt,p1);b = (ctrl_pt.y() - p1.y())/squared_distance(ctrl_pt,p1);}
  else
	 { a=0;b=0;}
  Vector_2 Del_dq2(a,b);
	  a= ctrl_pt.x() - p3.x();
	  b= ctrl_pt.y() - p3.y();

    if (squared_distance(ctrl_pt,p3)<rep_thresh)
	{  a=(ctrl_pt.x() - p1.x())/squared_distance(ctrl_pt,p1);b = (ctrl_pt.y() - p1.y())/squared_distance(ctrl_pt,p1);}
  else
	 { a=0;b=0;}
  Vector_2 Del_dq3(a,b);
	  a= ctrl_pt.x() - p4.x();
	  b= ctrl_pt.y() - p4.y();

  if (squared_distance(ctrl_pt,p4)<rep_thresh)
	{  a=(ctrl_pt.x() - p1.x())/squared_distance(ctrl_pt,p1);b = (ctrl_pt.y() - p1.y())/squared_distance(ctrl_pt,p1);}
  else
	 { a=0;b=0;}
  Vector_2 Del_dq4(a,b);
	  a= ctrl_pt.x() - p5.x();
	  b= ctrl_pt.y() - p5.y();
  Vector_2 dq5(a,b);
    if (squared_distance(ctrl_pt,p5)<rep_thresh)
	{  a=(ctrl_pt.x() - p1.x())/squared_distance(ctrl_pt,p1);b = (ctrl_pt.y() - p1.y())/squared_distance(ctrl_pt,p1);}
  else
	 { a=0;b=0;}
      Vector_2 Del_dq5(0,0);        
     

	a = Del_dq1.x() + Del_dq2.x() + Del_dq3.x() + Del_dq4.x() + Del_dq5.x();
	b = Del_dq1.y() + Del_dq2.y() + Del_dq3.y() + Del_dq4.y() + Del_dq5.y();
	c = (j.x()*Del_dq1.x() + j.y()*Del_dq1.y()) + (j.x()*Del_dq2.x() + j.y()*Del_dq2.y()) + (j.x()*Del_dq3.x() + j.y()*Del_dq3.y()) + (j.x()*Del_dq4.x() + j.y()*Del_dq4.y()) + (j.x()*Del_dq5.x() + j.y()*Del_dq5.y());

 Vector_3 u_rep(a,b,c);
std::cout<<u_rep<<std::endl;
return u_rep;
}

int main(int argc, char** argv)
{
  double dist_rep, dist_att;
  Point_2 goal(5,5);
  geometry_msgs::Twist vel;
  Listener pose;
  Vector_3 command;
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
   Point_2 obs_1[] = {o11,o12,o13,o14};
   Polygon_2 pgn_1(obs_1, obs_1+4);

   Point_2 o21(4.625,0.625), o22(5.375,0.625), o23(5.375,1.375), o24(4.625,1.375); // obs_2 green in color
   Point_2 obs_2[] = {o21,o22,o23,o24};
   Polygon_2 pgn_2(obs_2, obs_2+4);

   Point_2 o31(2.75,4.75), o32(3.25,4.75), o33(3.25,5.25), o34(2.75,5.25); // obs_3 blue in color
   Point_2 obs_3[] = {o31,o32,o33,o34};
   Polygon_2 pgn_3(obs_3, obs_3+4);

   Point_2 o41(-0.125,2.875), o42(0.125,2.875), o43(0.125,3.125), o44(-0.125,3.125); // obs_4 purple in color
   Point_2 obs_4[] = {o41,o42,o43,o44};
   Polygon_2 pgn_4(obs_4, obs_4+4);

   Point_2 o51(-1.0,6.0), o52(6.0,6.0), o53(6.0,-1.0), o54(-1.0,-1.0); // obs_4 purple in color
   Point_2 obs_5[] = {o51,o52,o53,o54};
   Polygon_2 pgn_5(obs_5, obs_5+4);



   Point_2 nearest;
while(ros::ok())
 { 
  ros::spinOnce();

  pub.publish(vel);
  command=planner(pose,pgn_1,pgn_2,pgn_3,pgn_4, pgn_5, goal);
  std::cout<<"came out safely"<<std::endl;
//  std::cout<<command<<std::endl;
/*  vel.linear.x = 0.5;
  vel.linear.y = 0.5;
  vel.linear.z = 0;
 */
  vel.linear.x = command.x();
  vel.linear.y = command.y();
  vel.angular.z = command.z();
loop_rate.sleep();
}
  
  

  return 0;
}

Listener::~Listener() {
   // Deallocate the memory that was previously reserved
   //  for this string.
  
}


