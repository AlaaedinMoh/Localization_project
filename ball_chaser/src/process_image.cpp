#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;
bool isMoving = false;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    //("Driving the robot...");

    ball_chaser::DriveToTarget drivingSvc;
    drivingSvc.request.linear_x = lin_x;
    drivingSvc.request.angular_z = ang_z;

    if(!client.call(drivingSvc))
    {
        ROS_ERROR("Failed calling [DriveToTarget] service");
    }
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{
    bool isBallFound = false;
    int white_pixel = 255;
    int midSectionBegin = img.width/3;
    int midSectionEnd = 2 * (img.width/3);
    //std::string info = "Image recieved. Width = " + std::to_string(img.width) + "\tHeight = " + std::to_string(img.height);
    
    //ROS_INFO_STREAM(info);
    //drive_robot(0.1, 0.1);
    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
    
    
    for(int i = 0; i < img.height * img.step; i+=3)
    {
        if(img.data[i] == white_pixel && img.data[i+1] == white_pixel && img.data[i+2] == white_pixel)
        {
            ROS_INFO("Ball is found...");
            int col_idx = i % img.step;
            isBallFound = true;
            if(col_idx <= midSectionBegin)
            {
                ROS_INFO("Driving left");
                drive_robot(0, 0.5);
                break;
            }
            if(col_idx > midSectionBegin && col_idx <= midSectionEnd )
            {
                ROS_INFO("Driving forward");
                drive_robot(0.5, 0);
                break;
            }
            if(col_idx > midSectionEnd)
            {
                ROS_INFO("Driving right");
                drive_robot(0, -0.5);
                break;
            }
        }
    }
    if(!isBallFound)
    {
        ROS_INFO("Ball could not be found...");
        drive_robot(0, 0);
    }
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
