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
AnalogInputPin CdS_cell(FEHIO::Pin4);
FEHServo arm_servo(FEHServo::Servo0);
AnalogInputPin right_opto(FEHIO::Pin5);
AnalogInputPin center_opto(FEHIO::Pin6);
AnalogInputPin left_opto(FEHIO::Pin7);

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


// void follow_line (int percent)
// {
//     float right_value, center_value, left_value;

//     while(true)
//     {
//     //black lines >4, all else <4
//     // right_value = right_opto.Value();
//     // center_value = center_opto.Value();
//     // left_value = left_opto.Value();
//         if(center_opto.Value() > 4)
//         {
//         right_motor.SetPercent(percent);
//         left_motor.SetPercent(percent);
//         }

//         else if(right_opto.Value() <= 4)
//         {
//         right_motor.Stop();
//         left_motor.SetPercent(percent);
//         }

//         else if (left_opto.Value() <= 4)
//         {
//         left_motor.Stop();
//         right_motor.SetPercent(percent);
//         }

//         else if ((left_opto.Value() <= 4) && (center_opto.Value() <=4) && (right_opto.Value() <=4))
//         {
//         right_motor.Stop();
//         left_motor.Stop();
//         }

//     }


// }



void ERCMain()
{
  //counts/inch for 3" wheels : 33.74
    int counts;
    int percent;

    arm_servo.SetMin(servo_min);
    arm_servo.SetMax(servo_max);

    arm_servo.SetDegree(180.);

    // //read start light
    // read_start();

    //back into start button
    counts = (CPI*1.5);
    percent = -drive_power/2;
    move_forward(percent, counts);

    Sleep(0.25);

    //turn to align w ramp
    counts = (CPI*2*pi*2*TR/8);
    percent = turn_power;
    turn_about_right(percent, counts);

    //drive forward to reach 90 degree turn
    counts = (CPI*39.5); //modify to end at start of line
    percent = drive_power;
    move_forward(percent, counts);

    //align parallel to buttons to readjust w wall
    counts = (CPI*2*pi*TR*11/36);
    percent = -turn_power;
    turn_counterclockwise_center(percent, counts);

    //back into wall
    counts = (CPI*5); //troubleshoot
    percent = turn_power;
    move_forward(percent, counts);

    //move forward to catch line
    counts = (CPI*6); //change with testing
    percent = -turn_power;
    move_forward(percent, counts);

    // //follow until reaches end of line
    // percent = 15;
    // follow_line(percent);

    // replace line following with shaft encoding
    counts = (CPI*18.425); //measurement needs to be tested
    percent = turn_power;
    move_forward(percent, counts);

    //10 in away from buttons
    counts = (CPI*10); //fill in distance til window is fully open
    percent = turn_power;
    move_forward(percent, counts);

    //lower hook arm
    arm_servo.SetDegree(90.);

    //4.5 in towards buttons to open window
    counts = (CPI*4.5); //fill in distance til window is fully open
    percent = -turn_power;
    move_forward(percent, counts);

    //raise hook arm
    arm_servo.SetDegree(180.);

    //1 in to get around handle
    counts = (CPI*1); //fill in width of window handle
    percent = turn_power;
    move_forward(percent, counts);

    //lower hook arm
    arm_servo.SetDegree(90.);

    //4.5 in to close window (w/ added buffer)
    counts = (CPI*5); //fill in distance til window is fully closed
    percent = -turn_power;
    move_forward(percent, counts);


    // counts = (CPI*2*pi*TR*11/36);
    // percent = turn_power;
    // turn_counterclockwise_center(percent, counts);

    // counts = (CPI*8);
    // percent = -turn_power;
    // move_forward(percent, counts);

    // counts = (CPI*18.425);
    // percent = turn_power;
    // move_forward(percent, counts);

    // percent = turn_power;
    // read_color(percent, counts);

    // counts = (CPI*2*pi*TR*19/36);
    // percent = turn_power;
    // turn_counterclockwise_center(percent, counts);

    // counts = (CPI*13.625);
    // percent = drive_power;
    // move_forward(percent, counts);

    // counts = (CPI*2*pi*TR/4);
    // percent = -turn_power;
    // turn_counterclockwise_center(percent, counts);

    // counts = (CPI*40);
    // percent = drive_power;
    // move_forward(percent, counts);

    // counts = (CPI*30);
    // percent = drive_power;
    // move_forward(percent, counts);







}
