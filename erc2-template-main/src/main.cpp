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
#define full_turn_center  (CPI*TR*2*pi)
#define full_turn_about  (CPI*2*TR*2*pi)

//Declarations for encoders & motors
//left and right as view from the front 
DigitalEncoder right_encoder(FEHIO::Pin8);
DigitalEncoder left_encoder(FEHIO::Pin9);
FEHMotor right_motor(FEHMotor::Motor0, 9.0);
FEHMotor left_motor(FEHMotor::Motor1, 9.0);
AnalogInputPin CdS_cell(FEHIO::Pin0);
FEHServo arm_servo(FEHServo::Servo0);
FEHServo wheel_servo(FEHServo::Servo1);
FEHServo window_servo(FEHServo::Servo2);
AnalogInputPin left_opto(FEHIO::Pin4);
AnalogInputPin center_opto(FEHIO::Pin6);
AnalogInputPin right_opto(FEHIO::Pin7);

void move_forward(int percent, int counts) //using encoders
{
    float time_start = TimeNow();
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent);

    // While the average of the left and right encoder are less than counts,
    // keep running motors
    while(((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts) && ((TimeNow() - time_start) <= 5));

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
    counts = full_turn_about/4;
    turn_about_left(percent, counts);

    //drive forward to be aligned on button (CPI*distance)
    percent = turn_power;
    counts = CPI*1;
    move_forward(percent, counts);

    percent = turn_power;
    counts = full_turn_about/4;
    turn_about_right(percent, counts);

    percent = turn_power;
    counts = CPI*0.5;
    move_forward(percent, counts);

    percent = drive_power;
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
    counts = (full_turn_about/4);
    turn_about_right(percent, counts);

    //drive forward to be aligned on button (CPI*distance)
    percent = turn_power;
    counts = CPI*1;
    move_forward(percent, counts);

    percent = turn_power;
    counts = full_turn_about/4;
    turn_about_left(percent, counts);

    percent = turn_power;
    counts = CPI*0.5;
    move_forward(percent, counts);

    percent = drive_power;
    counts = CPI*0.5;
    move_forward(percent, counts);

    percent = -drive_power;
    counts = CPI*6;
    move_forward(percent, counts);


}

void read_color(int percent, int counts) //times out after 10 seconds
{
float value;
float time_start = TimeNow();

value = CdS_cell.Value();
LCD.Clear();
LCD.Write(value);

while((TimeNow() - time_start) <= 10)
{
    //read for color
    value = CdS_cell.Value();
    if(value <= 0.5)
    {
        red_button(percent, counts);
        break;
    }

    else 
    {
        blue_button(percent, counts);
        break;
    }


}
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
        if ((left_opto.Value() <= 2.5) && (center_opto.Value() <=2.5) && (right_opto.Value() < 2.5))
        {
        right_motor.Stop();
        left_motor.Stop();
        Sleep(0.5);
        LCD.Write("  STOP: All White");
        break;
        }

        else if ((left_opto.Value() > 4.2) && (center_opto.Value() > 4.2) && (right_opto.Value() > 4.2))
        {
        right_motor.Stop();
        left_motor.Stop();
        Sleep(0.5);
        LCD.Write("  STOP: All Black");
        break;
        }

        else if((right_opto.Value() >= 4.2))
        {
        right_motor.SetPercent(-(percent-5));
        left_motor.SetPercent(percent);
        }

        else if ((left_opto.Value() >= 4.2))
        {
        left_motor.SetPercent(-(percent-5));
        right_motor.SetPercent(percent);
        }

        else if(center_opto.Value() > 4.2)
        {
        right_motor.SetPercent(percent);
        left_motor.SetPercent(percent);
        }

        
    }
    
    right_motor.Stop();
    left_motor.Stop();

}

//after apple basket drop back at turn junction
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
counts = (CPI*TR*2*pi/8);
turn_counterclockwise_center(percent, counts);

counts = (CPI*11);
move_forward(percent, counts);
counts = (CPI*TR*2*pi/4);
turn_counterclockwise_center(percent, counts);
counts = (CPI*5);
move_forward(percent, counts);
counts = (CPI*TR*2*pi/4);
turn_counterclockwise_center(-percent, counts);
follow_line(percent);


arm_servo.SetDegree(0);
Sleep(1.0);

counts = (CPI*.5);
move_forward(-percent, counts);
arm_servo.SetDegree(60);
move_forward(percent,counts);
arm_servo.SetDegree(0);

Sleep(5.0);

    arm_servo.SetDegree(0);
    move_forward(percent,counts);
    arm_servo.SetDegree(60);

}

//protocol for navigating to the center lever
void center_lever(int percent, int counts)
{
percent = turn_power;
counts = (CPI*TR*2*pi/8);
turn_counterclockwise_center(percent, counts);

arm_servo.SetDegree(0);
    Sleep(1.0);

counts = (CPI*15); //measured
move_forward(percent, counts);
follow_line(percent);

counts = (CPI*.5);
move_forward(-percent, counts);
arm_servo.SetDegree(60);
move_forward(percent,counts);
arm_servo.SetDegree(0);

Sleep(5.0);

arm_servo.SetDegree(0);
move_forward(percent,counts);
arm_servo.SetDegree(60);



}


//protocol for navigating to the right lever
void right_lever(int percent, int counts)
{
    //turn 45 degrees
    percent = percent;
    counts = (CPI*TR*2*pi/8);
    turn_counterclockwise_center(percent, counts);

    //move 5 inches to center line
    counts = (CPI*11);
    move_forward(percent, counts);

    //turn 90 degrees
    counts = (CPI*TR*2*pi/4);
    turn_counterclockwise_center(-percent, counts);
    // move one inch
    counts = (CPI*5);
    move_forward(percent, counts);

    //set servo up
    arm_servo.SetDegree(0);
    Sleep(1.0);

    //move 90 degrees the other way
    counts = (CPI*TR*2*pi/4);
    turn_counterclockwise_center(percent, counts);
    follow_line(percent);

   

    //move half an inch back
    counts = (CPI*.5);
    move_forward(-percent, counts);
    arm_servo.SetDegree(60);
    move_forward(percent,counts);
    arm_servo.SetDegree(0);

    Sleep(5.0);

    arm_servo.SetDegree(0);
    move_forward(percent,counts);
    arm_servo.SetDegree(60);


}


//decision program for navigating to levers
void choose_lever(int percent, int counts)
{
    int lever_choice = RCS.GetLever();

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

void start_protocol (int percent, int counts)
{
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

}

void apple_compost(int percent, int counts, int i, int target_angle)
{

    //drive straight forward and look for line
    percent = turn_power;
    counts = (CPI*14);
    move_forward(percent, counts);
    LCD.Write("  drive");

    //lower arm to interface w basket
    for(i=0; i<55; i++){
        arm_servo.SetDegree(i);
        Sleep(0.01);
    }
     LCD.Write("arm down");

    //45 degree about right
    percent = turn_power;
    counts = (full_turn_about/8);
    turn_about_right(percent, counts);
    LCD.Write("  45 deg");

    //90 degrees about left
    percent = turn_power;
    counts = (full_turn_about/3.9);
    turn_about_left(percent, counts);
    LCD.Write("  90 deg");

    // //backwards to align
    // percent = -turn_power;
    // counts = (CPI*3);
    // move_forward(percent, counts);

    // //follow line until turn
    // percent = turn_power;
    // counts = (CPI*3); 
    // move_forward(percent, counts); //changed to manual
    // LCD.Write("  follow");

    //drive into basket to grab
    percent = turn_power;
    counts = (CPI*1.25);
    move_forward(percent, counts);
    LCD.Write("  grab basket");
    Sleep(1.0);

    //raise arm 20 degrees
    for(i; i>0; i--){
        arm_servo.SetDegree(i);
        Sleep(0.01);
    }
    LCD.Write("raise arm");
    Sleep(1.5);

    //back up from bin to make turn
    percent = -turn_power;
    counts = (CPI*1); //test and modify
    move_forward(percent, counts);
    LCD.Write("  back up");

    //90 degree center turn to start toward compost bin
    percent = turn_power;
    counts = (full_turn_center/4);
    turn_counterclockwise_center(percent, counts);
    LCD.Write("  90 deg");

    //forward to get past apple basket 
    percent = turn_power;
    counts = (CPI*1); //test and modify
    move_forward(percent, counts);

    //67ish degree turn about right
    percent = turn_power;
    counts = (full_turn_about/5.5);
    turn_about_right(percent, counts);

    //forward to get past apple basket 
    percent = turn_power;
    counts = (CPI*2.5); //test and modify
    move_forward(percent, counts);

    //67ish degree turn about left
    percent = turn_power;
    counts = (full_turn_about/5.5);
    turn_about_left(percent, counts);

    //drive forward into robot and keep motors moving
    percent = turn_power/2;
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent);

    //turn on wheel servo forward
    wheel_servo.SetDegree(70);
    Sleep(3.0);
    LCD.Write("  bin forward");

    //pause for a bit
    wheel_servo.SetDegree(95);
    Sleep(0.5); 
    LCD.Write("  bin stop");

    //turn wheel servo backwards
    wheel_servo.SetDegree(110);
    LCD.Write("  bin backward");
    Sleep(2.0);

    //stop wheel servo
    wheel_servo.Off();

    //stop motors and run drive sequence backwards
    right_motor.Stop();
    left_motor.Stop();

    //function to break encoder counts?

    //drive backwards from bin to make turn
    percent = -turn_power;
    counts = (CPI*1);
    move_forward(percent, counts);

    //90 degrees clockwise about center
    percent = turn_power;
    counts = (full_turn_center/3.5);
    turn_counterclockwise_center(percent, counts);

    //drive forward to starting zone
    percent = drive_power;
    counts = (CPI*22.5); //test and modify
    move_forward(percent, counts);


}

void apple_nav (int percent, int counts, int i, int target_angle)
{

    //90 degrees counterclockwise about center
    percent = turn_power;
    counts = (full_turn_center/3.7);
    turn_counterclockwise_center(percent, counts);

    //move up ramp and try to catch line
    percent = drive_power-10;
    counts = (CPI*24);
    move_forward(percent, counts);

    //follow along line until 90 degree turn
    percent = turn_power;
    counts = (CPI*5); //test and modify
    follow_line_counts(percent, counts); 

    //90 degrees counterclockwise (manual)
    percent = turn_power;
    counts = (full_turn_center/8);
    turn_counterclockwise_center(percent, counts);

    //move along line until line
    percent = turn_power;
    counts = (CPI*7); //test and modify
    move_forward(percent, counts); 

    //45 degrees clockwise (manual)
    percent = -turn_power;
    counts = (full_turn_center/8);
    turn_counterclockwise_center(percent, counts);

    //follow line to apple crate
    percent = turn_power;
    counts = (CPI*13); //test and modify
    follow_line_counts(percent, counts);
    LCD.Write("caught line");

    //align on crate
    percent = turn_power; 
    counts = (CPI*4);
    move_forward(percent, counts);

    // //back out slightly so basket does not hit back of crate
    // percent = -turn_power; 
    // counts = (CPI*0.25);
    // move_forward(percent, counts);

    //lower basket into crate (down 30 degrees, get to 80 degrees)
    for(i=0; i<75; i++){
        arm_servo.SetDegree(i);
        Sleep(0.01);
    }
    LCD.Write("arm down");

    //back arm out of basket
    percent = -turn_power;
    counts = (CPI*1.5);
    move_forward(percent, counts);

    //raise arm back up to top
    for(i; i>0; i--){
        arm_servo.SetDegree(i);
        Sleep(0.01);
    }
    LCD.Write("raise arm");

    //move backwards to turn junction
    percent = -turn_power;
    counts = (CPI*14); //DOUBLE CHECK
    move_forward(percent, counts);


}

void buttons (int percent, int counts)
{

    //90 degrees counterclockwise
    percent = turn_power;
    counts = (full_turn_center/4);
    turn_counterclockwise_center(percent, counts);

    //follow w counts til end of line
    percent = turn_power;
    counts = (CPI*9); //test and modify
    follow_line_counts(percent, counts);

    //pause for a second to make me feel better about this
    Sleep(0.5);

    //move forward just enough to be over CdS cell
    percent = turn_power/2;
    counts = (CPI*1); //test and modify
    move_forward(percent, counts);

    //read color and continue on needed path
    read_color(percent, counts);



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
  //95 is stop
  //less than 95 is forwards
  //greater than 95 is backwards
  //the further from 95, the faster
  //sweet spots -> 75 for for, 105 for back

    int counts;
    int percent;
    int percent_R, percent_L;
    int target_angle;
    int i;

    arm_servo.SetMin(servo_min);
    arm_servo.SetMax(servo_max);

    wheel_servo.SetMin(1200);
    wheel_servo.SetMax(1650);

    // //initialize the RCS
    // RCS.InitializeTouchMenu("1130D8HDL");

    // //read start light
    // read_start();
    // LCD.Write(CdS_cell.Value());
    // LCD.Write("cds");

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

    apple_compost(percent, counts, i , target_angle);

    apple_nav(percent, counts, i ,target_angle);






}
