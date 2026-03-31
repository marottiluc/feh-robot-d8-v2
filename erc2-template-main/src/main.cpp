#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRCS.h>
#include <FEHSD.h>
#include <FEHservo.h>

#define CPI 33.74
#define TR 3.5
#define pi 3.14159
#define turn_power -25
#define drive_power -40
#define servo_min 1322
#define servo_max 2279

//Declarations for encoders & motors
//left and right as view from the front 
DigitalEncoder right_encoder(FEHIO::Pin8);
DigitalEncoder left_encoder(FEHIO::Pin9);
FEHMotor right_motor(FEHMotor::Motor0, 9.0);
FEHMotor left_motor(FEHMotor::Motor1, 9.0);
AnalogInputPin CdS_cell(FEHIO::Pin3);
FEHServo arm_servo(FEHServo::Servo0);
AnalogInputPin left_opto(FEHIO::Pin4);
AnalogInputPin center_opto(FEHIO::Pin6);
AnalogInputPin right_opto(FEHIO::Pin7);

void move_forward(int percent, int counts) //using encoders
{
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent);

    // While the average of the left and right encoder are less than counts,
    // keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

void move_forward_var(int percent_R, int percent_L, int counts) //using encoders
{
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // Set both motors to desired percent
    right_motor.SetPercent(percent_R);
    left_motor.SetPercent(percent_L);

    // While the average of the left and right encoder are less than counts,
    // keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

/*
 * Turn counterclockwise using shaft encoders where percent is the motor percent and counts is the distance to travel
 */
void turn_counterclockwise_center(int percent, int counts)
{
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(-percent);

    // While the average of the left and right encoder are less than counts,
    // keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}


void turn_about_right(int percent, int counts)
{
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // Set both motors to desired percent
    right_motor.SetPercent(0);
    left_motor.SetPercent(percent);

    // While the left encoder is  less than counts,
    // keep running motors
    while((left_encoder.Counts()) < counts);

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}


void turn_about_left(int percent, int counts)
{
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(0);

    // While the right encoder is less than counts,
    // keep running motors
    while((right_encoder.Counts()) < counts);

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}



void read_start ()
{
// Initialize start time
float value;
float time_start = TimeNow();

// looks for light constantly for 30 seconds (timeout given by course)
while((TimeNow() - time_start) <= 30)
{
    value = CdS_cell.Value();
    //continue to rest of main when the light is sensed
    if((value <= 2)){
    break;
    }

}
}

void blue_button(int percent, int counts)
{
    LCD.Clear();
    LCD.Write("BLUE");

    //turn to get towards red button (needs CPI*TR*2*pi/(portion of turn))
    percent = turn_power;
    counts = CPI*TR*2*pi/4;
    turn_about_left(percent, counts);

    //drive forward to be aligned on button (CPI*distance)
    percent = turn_power;
    counts = CPI*1;
    move_forward(percent, counts);

    percent = turn_power;
    counts = CPI*TR*2*pi/4;
    turn_about_right(percent, counts);

    percent = turn_power;
    counts = CPI*0.5;
    move_forward(percent, counts);

    percent = -60;
    counts = CPI*0.5;
    move_forward(percent, counts);

    percent = -drive_power;
    counts = CPI*6;
    move_forward(percent, counts);


}

void red_button(int percent, int counts)
{

    LCD.Clear();
    LCD.Write("RED");

    //turn to get towards red button (needs CPI*TR*2*pi/(portion of turn))
    percent = turn_power;
    counts = CPI*TR*2*pi/4;
    turn_about_right(percent, counts);

    //drive forward to be aligned on button (CPI*distance)
    percent = turn_power;
    counts = CPI*1;
    move_forward(percent, counts);

    percent = turn_power;
    counts = CPI*TR*2*pi/4;
    turn_about_left(percent, counts);

    percent = turn_power;
    counts = CPI*0.5;
    move_forward(percent, counts);

    percent = -60;
    counts = CPI*0.5;
    move_forward(percent, counts);

    percent = -drive_power;
    counts = CPI*6;
    move_forward(percent, counts);


}

void read_color(int percent, int counts)
{
float value;
float time_start = TimeNow();
while((TimeNow() - time_start) <= 30)
{

    
    //read for color
    value = CdS_cell.Value();
    if(value <= 0.5){
        red_button(percent, counts);
        break;
    }

    else {
        blue_button(percent, counts);
        break;
    }

    value = CdS_cell.Value();
    LCD.Write(value);
    LCD.Clear();
    Sleep(0.05);

}


}

void look_for_line (int percent)
{
 // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent);

    // While the optosensors see a color that isn't white,
    // keep running motors
    while((left_opto.Value() > 3) && (left_opto.Value() > 3) && (left_opto.Value() > 3));

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();

}


void follow_line (int percent)
{
    // float right_value, center_value, left_value;

    while(true)
    {
    //black lines >4, all else <4
    // right_value = right_opto.Value();
    // center_value = center_opto.Value();
    // left_value = left_opto.Value();


        if((right_opto.Value() >= 4))
        {
        right_motor.Stop();
        left_motor.SetPercent(percent);
        }

        else if ((left_opto.Value() >= 4))
        {
        left_motor.Stop();
        right_motor.SetPercent(percent);
        }

        else if(center_opto.Value() > 4)
        {
        right_motor.SetPercent(percent);
        left_motor.SetPercent(percent);
        }

        //add cases for two seeing black where it makes a 90 degree turn
        //in the given direction

        else if ((left_opto.Value() <= 4.8) && (center_opto.Value() <=4.8) && (right_opto.Value() <=4.8))
        {
        right_motor.Stop();
        left_motor.Stop();
        Sleep(0.5);
        break;
        }


       

    }


}

//these are used to control the speed of the servo arm so we don't break anything

void lower_arm(int deg_from_180)
{
    int i = 0;
    for(i=0; i<deg_from_180; i++){
        arm_servo.SetDegree(180-i);
        Sleep(0.01);
    }

}

void raise_arm(int deg_from_90)
{
    int i = 0;
    for(i=0; i<deg_from_90; i++){
        arm_servo.SetDegree(90+i);
        Sleep(0.01);
    }

}



void ERCMain()
{

  //counts/inch for 3" wheels : 33.74
  //180 degree is vertical
  //90 degree is horizontal


    int counts;
    int percent;
    int percent_R, percent_L;
    int deg_from_180, deg_from_90;

    arm_servo.SetMin(servo_min);
    arm_servo.SetMax(servo_max);

    arm_servo.SetDegree(180.);

    //read start light
    read_start();

    //back into start button
    counts = (CPI*1.5);
    percent = -drive_power/2;
    move_forward(percent, counts);

    Sleep(0.25);

    //drive straight forward and look for line
    percent = drive_power;
    look_for_line(percent);

    //follow line until turn
    percent = turn_power;
    follow_line(percent); //will break when none on black, AKA at 90 turn

    //lower arm to interface with apple basket
    //deg_from_180 = value;
    //lower_arm(deg_from_180);

    //raise arm slightly to let basket fall back
    //deg_from_90 = value;
    //raise_arm(deg_from_90);

    //drive backwards to allow for turn
    percent = turn_power;
    //counts = (CPI*value); //small value to allow for turn
    move_forward(percent, counts);

    //90 degree turn
    percent = turn_power;
    counts = (TR*2*pi/4);
    turn_counterclockwise_center(percent, counts);

    //drive forwards to get center behind ramp
    percent = turn_power;
    //counts = (CPI*value); //small value to align w ramp
    move_forward(percent, counts);

    //90 degree turn
    percent = turn_power;
    counts = (TR*2*pi/4);
    turn_counterclockwise_center(percent, counts);

    //drive forward to be center aligned to climb ramp
    percent = turn_power;
    //counts = (CPI*value); //distance from last turn to ramp
    move_forward(percent, counts);

    //drive up ramp and look for line
    percent = turn_power;
    look_for_line(percent);

    //follow line to crate
    //add case for making the 90 degree turn when both center and right see black
    //loop will break and stop when all sensors see white
    percent = turn_power;
    follow_line(percent);

    //lower arm to deposit in crate
    //deg_from_180 = value;
    //lower_arm(deg_from_180);





    // testing protocols for optosensors
       // while(true){
    //     LCD.Write(right_opto.Value());
    //     LCD.Write(center_opto.Value());
    //     LCD.Write(left_opto.Value());

    //     Sleep(0.5);
    //     LCD.Clear();

        
    // }

    // percent = turn_power;
    // follow_line(percent);

}
