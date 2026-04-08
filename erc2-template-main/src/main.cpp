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
#define servo_min 500
#define servo_max 1561
#define wheel_speed = 100;

//Declarations for encoders & motors
//left and right as view from the front 
DigitalEncoder right_encoder(FEHIO::Pin8);
DigitalEncoder left_encoder(FEHIO::Pin9);
FEHMotor right_motor(FEHMotor::Motor0, 9.0);
FEHMotor left_motor(FEHMotor::Motor1, 9.0);
AnalogInputPin CdS_cell(FEHIO::Pin0);
FEHServo arm_servo(FEHServo::Servo0);
FEHServo wheel_servo(FEHServo::Servo1);
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


void turn_counterclockwise_center(int percent, int counts) //turn radius = TR = 3.5
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


void turn_about_right(int percent, int counts) //turn radius = 2*TR = 7
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


void turn_about_left(int percent, int counts) //turn radius = 2*TR = 7
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



void read_start () //times out after 30 seconds
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

void read_color(int percent, int counts) //times out after 30 seconds
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

void look_for_line (int percent) //times out after 30 seconds
{
    float time_start = TimeNow();
 // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent);

    // While the optosensors see a color that isn't white, keep running motors
    while((left_opto.Value() > 3) && (left_opto.Value() > 3) && (left_opto.Value() > 3) && ((TimeNow() - time_start) <= 30));

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();

    
}


void follow_line (int percent)
{
    while(true)
    {
        if((right_opto.Value() >= 4.2))
        {
        right_motor.SetPercent(-(percent+5));
        left_motor.SetPercent(percent);
        }

        else if ((left_opto.Value() >= 4.2))
        {
        left_motor.SetPercent(-(percent+5));
        right_motor.SetPercent(percent);
        }

        else if(center_opto.Value() > 4.2)
        {
        right_motor.SetPercent(percent);
        left_motor.SetPercent(percent);
        }

        else if ((left_opto.Value() <= 2.5) && (center_opto.Value() <=2.5) && (right_opto.Value() < 2.5))
        {
        right_motor.Stop();
        left_motor.Stop();
        Sleep(0.5);
        LCD.Write("STOP: All White");
        break;
        }

        // LCD.Write(right_opto.Value());
        // LCD.Write(center_opto.Value());
        // LCD.Write(left_opto.Value());

        // Sleep(0.5);
        // LCD.Clear();



       

    }


}

void follow_line_counts (int percent, int counts)
{
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts)
    {
       if((right_opto.Value() >= 4.2))
        {
        right_motor.SetPercent(-(percent+5));
        left_motor.SetPercent(percent);
        }

        else if ((left_opto.Value() >= 4.2))
        {
        left_motor.SetPercent(-(percent+5));
        right_motor.SetPercent(percent);
        }

        else if(center_opto.Value() > 4.2)
        {
        right_motor.SetPercent(percent);
        left_motor.SetPercent(percent);
        }

        else if ((left_opto.Value() <= 2.5) && (center_opto.Value() <=2.5) && (right_opto.Value() < 2.5))
        {
        right_motor.Stop();
        left_motor.Stop();
        Sleep(0.5);
        LCD.Write("STOP: All White");
        break;
        }
    }
    
    right_motor.Stop();
    left_motor.Stop();

}

void left_lever(int percent, int counts)
{

//rotate 45 degrees
//move to left line
//move forward
//flip lever up
//move back
//move lever arm
//move up
//put lever down

percent = turn_power;
counts = (TR*2*pi/8);
turn_counterclockwise_center(percent, counts);

counts = (CPI*5);
move_forward(percent, counts);
counts = (TR*2*pi/4);
turn_counterclockwise_center(percent, counts);
counts = (CPI*1);
move_forward(percent, counts);
counts = (TR*2*pi/4);
turn_counterclockwise_center(-percent, counts);
follow_line(percent);


arm_servo.SetDegree(0);
Sleep(1.0);

counts = (CPI*.5);
move_forward(-percent, counts);
arm_servo.SetDegree(60);
move_forward(percent,counts);
arm_servo.SetDegree(0);

}

//protocol for navigating to the center lever
void center_lever(int percent, int counts)
{
percent = turn_power;
counts = (TR*2*pi/8);
turn_counterclockwise_center(percent, counts);

counts = (CPI*15); //measured
move_forward(percent, counts);
follow_line(percent);

arm_servo.SetDegree(75);
Sleep(1.0);

counts = (CPI*.5);
move_forward(-percent, counts);
arm_servo.SetDegree(90);
move_forward(percent,counts);
arm_servo.SetDegree(0);



}

//protocol for navigating to the right lever
void right_lever(int percent, int counts)
{
percent = turn_power;
counts = (TR*2*pi/8);
turn_counterclockwise_center(percent, counts);


counts = (CPI*5);
move_forward(percent, counts);
counts = (TR*2*pi/4);
turn_counterclockwise_center(-percent, counts);
counts = (CPI*1);
move_forward(percent, counts);
counts = (TR*2*pi/4);
turn_counterclockwise_center(percent, counts);
follow_line(percent);

counts = (CPI*.5);
move_forward(-percent, counts);
arm_servo.SetDegree(60);
move_forward(percent,counts);
arm_servo.SetDegree(0);

}


//decision program for navigating to levers
void choose_lever()
{
    int lever_choice = RCS.GetLever();
    int percent, counts;

    if(lever_choice == 0)
    {
        left_lever(percent, counts);
    }

    else if(lever_choice == 1)
    {
        center_lever(percent, counts);
    }

    else if(lever_choice == 2)
    {
        right_lever(percent, counts);
    }

}

void arm_servo_up (int i, int target_angle) //for raising the arm 
{
  for(i; i<target_angle; i++){
        arm_servo.SetDegree(i);
        Sleep(0.01);
    }

}

void arm_servo_down (int i, int target_angle) //for lowering the arm 
{
  for(i; i>target_angle; i--){
        arm_servo.SetDegree(i);
        Sleep(0.01);
    }

}


  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void ERCMain()
{
  //counts/inch for 3" wheels : 33.74

  //for arm servo
  //180 degree is vertical
  //90 degree is horizontal

  //for wheel servo
  //90 is stop
  //less than 90 is forwards
  //greater than 90 is backwards
  //the further from 90, the faster
  //sweet spots -> 75 for for, 105 for back


    int counts;
    int percent;
    int percent_R, percent_L;
    int target_angle;
    int i;

    arm_servo.SetMin(servo_min);
    arm_servo.SetMax(servo_max);

    //initialize the RCS
    RCS.InitializeTouchMenu("1130D8HDL");

    //read start light
    read_start();
    LCD.Write(CdS_cell.Value());
    LCD.Write("cds");

    //reset arm servo
    arm_servo.SetDegree(0);
    LCD.Write("  servo");

    //back into start button
    counts = (CPI*1.5);
    percent = -drive_power/2;
    move_forward(percent, counts);
    LCD.Write("  start button");

    Sleep(0.25);
    LCD.Write("  sleep");

    //drive forward for 9
    percent = turn_power;
    counts = (CPI*10);
    move_forward(percent, counts);

    //turn 45 about left wheel
    percent = turn_power;
    counts = (CPI*2*TR*2*pi/8);
    turn_about_left(percent, counts);

    //drive forward into wall
    percent = turn_power;
    counts = (CPI*14);
    move_forward(percent, counts);

    //drive backwards 1 to prep for turn
    percent = -drive_power;
    counts = (CPI*1);
    move_forward(percent, counts);

    //90 degrees counterclockwise about center
    percent = turn_power;
    counts = (CPI*TR*2*pi/3.75);
    turn_counterclockwise_center(percent, counts);

    //drive forward into robot and keep motors moving
    percent = turn_power/2;
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent);

    //turn on wheel servo forward
    wheel_servo.SetDegree(70);
    Sleep(1.5);

    //pause for a bit
    wheel_servo.SetDegree(90);
    Sleep(1.5); 

    //turn wheel servo backwards
    wheel_servo.SetDegree(105);
    Sleep(1.5);

    //stop wheel servo
    wheel_servo.SetDegree(90);

    //stop motors and run drive sequence backwards
    right_motor.Stop();
    left_motor.Stop();

    //drive backwards from bin to make turn
    percent = turn_power;
    counts = (CPI*3);
    move_forward(percent, counts);

    //90 degrees clockwise about center
    percent = -turn_power;
    counts = (CPI*TR*2*pi/4);
    turn_counterclockwise_center(percent, counts);

    //drive forwards into wall to align
    percent = turn_power;
    counts = (CPI*4);
    move_forward(percent, counts);

    //drive backward to starting zone
    percent = -turn_power;
    counts = (CPI*21);
    move_forward(percent, counts);

     //turn backwards 45 about left wheel
    percent = -turn_power;
    counts = (CPI*2*TR*2*pi/8);
    turn_about_left(percent, counts);

    //drive backwards into start button
    percent = -turn_power;
    counts = (CPI*12);
    move_forward(percent, counts);






}
