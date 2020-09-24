/*******************************************************************************
* mb_odometry.c
*
* TODO: Implement these functions to add odometry and dead rekoning 
*
*******************************************************************************/

#include "../mobilebot/mobilebot.h"
#include "mb_defs.h"
#include <math.h>

#define PI 3.14159265358979323846

/*******************************************************************************
* mb_initialize_odometry() 
*
* TODO: initialize odometry
* NOTE: you should initialize from Optitrack data if available
*
*******************************************************************************/
void mb_initialize_odometry(mb_odometry_t* mb_odometry, float x, float y, float theta){
    mb_odometry->x = x;
    mb_odometry->y = y;
    mb_odometry->theta = theta;
}


/*******************************************************************************
* mb_update_odometry() 
*
* TODO: calculate odometry from internal variables
*       publish new odometry to lcm ODOMETRY_CHANNEL
*
*******************************************************************************/
void mb_update_odometry(mb_odometry_t* mb_odometry, mb_state_t* mb_state){
    
    
    // mb_odometry->x += mb_state->distance_delta * cos(mb_clamp_radians(mb_state->last_yaw + mb_state->yaw_delta/2));
    // mb_odometry->y += mb_state->distance_delta * sin(mb_clamp_radians(mb_staste->last_yaw + mb_state->yaw_delta/2));
    // mb_odometry->x += mb_state->distance_delta * cos(mb_clamp_radians(mb_odometry->theta + mb_state->yaw_delta/2));
    // mb_odometry->y += mb_state->distance_delta * sin(mb_clamp_radians(mb_odometry->theta + mb_state->yaw_delta/2));
    // mb_odometry->theta = mb_clamp_radians(mb_odometry->theta + mb_state->yaw_delta);

    float diff = mb_state->yaw_delta - mb_state->gyro_heading_delta;
    
    if (abs(diff) > 0.1 && (mb_state->left_velocity != 0 || mb_state->right_velocity != 0)) {
        mb_state->picked_delta = mb_state->gyro_heading_delta;
    } else {
        mb_state->picked_delta = mb_state->yaw_delta;
    }

    mb_odometry->x += mb_state->distance_delta * cos(mb_clamp_radians(mb_odometry->theta + mb_state->picked_delta/2));
    mb_odometry->y += mb_state->distance_delta * sin(mb_clamp_radians(mb_odometry->theta + mb_state->picked_delta/2));
    mb_odometry->theta = mb_clamp_radians(mb_odometry->theta + mb_state->picked_delta);
}


/*******************************************************************************
* mb_clamp_radians() 
* clamp an angle from -PI to PI
*******************************************************************************/
float mb_clamp_radians(float angle){

    if(angle < -PI)
    {
        for(; angle < -PI; angle += 2.0*PI);
    }
    else if(angle > PI)
    {
        for(; angle > PI; angle -= 2.0*PI);
    }

    return angle;
}


/*******************************************************************************
* mb_angle_diff_radians() 
* computes difference between 2 angles and wraps from -PI to PI
*******************************************************************************/
float mb_angle_diff_radians(float angle1, float angle2){
    float diff = angle2 - angle1;
    while(diff < -PI) diff+=2.0*PI;
    while(diff > PI) diff-=2.0*PI;
    return diff;
}