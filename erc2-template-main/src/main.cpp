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

//int state = 0;

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
    float time_start = TimeNow();
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    while(((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts) && ((TimeNow() - time_start) <= 5))
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
   
    // if ((TimeNow() - time_start) <= 5)
    // {
    //     state = 0;
    // }
    right_motor.Stop();
    left_motor.Stop();

}


//new with backwards
void blue_button(int percent, int counts)
{
    LCD.Clear();
    LCD.Write("BLUE");

    //turn 180 degrees
    percent = turn_power;
    counts = (full_turn_center/1.75);
    turn_counterclockwise_center(percent, counts);

    //turn to get towards red button (needs CPI*TR*2*pi/(portion of turn))
    percent = turn_power;
    counts = full_turn_about/6;
    //turn_about_left(percent, counts);
    turn_about_right(-percent,counts);

    //drive forward to be aligned on button (CPI*distance)
    percent = turn_power;
    counts = CPI*1;
    move_forward(-percent, counts);

    Sleep(1.0);

    percent = turn_power;
    counts = full_turn_about/6;
    //turn_about_right(percent, counts);
    turn_about_left(-percent, counts);

    counts = (CPI*2);
    move_forward(percent, counts);
    Sleep(1.0);

    percent = drive_power;
    counts = CPI*5;
    //move_forward(percent, counts);
    move_forward(-percent, counts);

    Sleep(1.0);

    counts = (CPI*2);
    move_forward(percent, counts);
    Sleep(1.0);

    percent = drive_power;
    counts = CPI*5;
    //move_forward(percent, counts);
    move_forward(-percent, counts);

    Sleep(1.0);

       //2 60 60 4 blue
    percent = drive_power;
    counts = CPI*2;
    //move_forward(percent, counts);
    move_forward(percent, counts);

     Sleep(1.0);


    //added --> from humidifier button go back to line
    percent = turn_power;
    counts = full_turn_about/6;
    turn_about_left(percent, counts);

     Sleep(1.0);

    percent = turn_power;
    counts = full_turn_about/6;
    turn_about_right(percent,counts);

     Sleep(1.0);

    percent = turn_power;
    counts = CPI*4;
   // move_forward(percent, counts);
    follow_line_counts(percent, counts);

     Sleep(1.0);


}

void red_button(int percent, int counts)
{

    LCD.Clear();
    LCD.Write("RED");

    //turn 180 degrees
    percent = turn_power;
    counts = (full_turn_center/1.75);
    turn_counterclockwise_center(percent, counts);

    //turn to get towards red button (needs CPI*TR*2*pi/(portion of turn))
    percent = turn_power;
    counts = (full_turn_about/6);
    // turn_about_right(percent, counts);
    turn_about_left(-percent, counts);

    //drive forward to be aligned on button (CPI*distance)
    percent = turn_power;
    counts = CPI*1;
    move_forward(-percent, counts);

    percent = turn_power;
    counts = full_turn_about/6;
    // turn_about_left(percent, counts);
    turn_about_right(-percent, counts);

    counts = (CPI*2);
    move_forward(percent, counts);
    Sleep(1.0);

    percent = drive_power;
    counts = CPI*5;
    //move_forward(percent, counts);
    move_forward(-percent, counts);

    Sleep(1.0);

    counts = (CPI*2);
    move_forward(percent, counts);
    Sleep(1.0);

    percent = drive_power;
    counts = CPI*5;
    //move_forward(percent, counts);
    move_forward(-percent, counts);

    Sleep(1.0);

    //2 60 60 4 red
    percent = drive_power;
    counts = CPI*2;
    //move_forward(percent, counts);
    move_forward(percent, counts);

     Sleep(1.0);


    //added --> from humidifier button go back to line
    percent = turn_power;
    counts = full_turn_about/6;
    //turn_about_right(percent, counts);
    turn_about_right(percent, counts);

     Sleep(1.0);

    percent = turn_power;
    counts = full_turn_about/6;
    //turn_about_left(percent, counts);
    turn_about_left(percent,counts);

     Sleep(1.0);

    percent = turn_power;
    counts = CPI*4;
   // move_forward(percent, counts);
    follow_line_counts(percent, counts);

     Sleep(1.0);


}

void cds_check(int percent, int counts)
{

int check = 0;
float value;
percent = turn_power/2;
counts = (CPI*0.5);

//if value is red, add to counter
value = CdS_cell.Value();
if (value < 0.5) {check += 1;}

//change position and take reading again
move_forward(percent, counts);
Sleep(0.75);

value = CdS_cell.Value();
if (value < 0.5) {check += 1;}

move_forward(percent, counts);
Sleep(0.75);

value = CdS_cell.Value();
if (value < 0.5) {check += 1;}

move_forward(percent, counts);
Sleep(0.75);
value = CdS_cell.Value();
if (value < 0.5) {check += 1;}

move_forward(percent, counts);
Sleep(0.75);
value = CdS_cell.Value();
if (value < 0.5) {check += 1;}

move_forward(percent, counts);
Sleep(0.75);

value = CdS_cell.Value();
if (value < 0.5) {check += 1;}

move_forward(percent, counts);
Sleep(0.75);

value = CdS_cell.Value();
if (value < 0.5) {check += 1;}

move_forward(percent, counts);
Sleep(0.75);

value = CdS_cell.Value();
if (value < 0.5) {check += 1;}

if(check != 0)
{red_button(percent, counts);}

else
{blue_button(percent, counts);}


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
counts = (full_turn_center/8);
turn_counterclockwise_center(percent, counts);

counts = (CPI*10.75);
move_forward(percent, counts);

counts = (full_turn_center/3.8);
turn_counterclockwise_center(percent, counts);

counts = (CPI*4);
move_forward(percent, counts);

counts = (full_turn_center/4);
turn_counterclockwise_center(-percent, counts);

counts = (CPI*.5); //changed
move_forward(percent, counts);

counts = (CPI*0.5);
move_forward(percent,counts);

arm_servo.SetDegree(60); //for loop later

Sleep(1.5);

arm_servo.SetDegree(0);

counts = (CPI*3.5);
move_forward(-percent,counts);

Sleep(5.0);

arm_servo.SetDegree(90);

Sleep(1.0);
move_forward(percent,counts);
Sleep(1.0);

arm_servo.SetDegree(60);

Sleep(.5);

//move back
    counts = (CPI*3.5);
    move_forward(-percent,counts);

    arm_servo.SetDegree(0);

    Sleep(1.0);

    //turn counterclockwise
    counts = (full_turn_center/4);
    turn_counterclockwise_center(percent, counts);

    Sleep(1.0);

    // move back to center line
    // move one inch
    counts = (CPI*4);
    move_forward(-percent, counts);

    Sleep(1.0);

    //turn back
    //turn 90 degrees
    counts = (full_turn_center/4);
    turn_counterclockwise_center(-percent, counts);

    Sleep(1.0);

    //move 9 inches to junction
    counts = (CPI*10);
    move_forward(-percent, counts);
    Sleep(1.0);

}

//protocol for navigating to the center lever
void center_lever(int percent, int counts)
{
percent = turn_power;
counts = (full_turn_center/7.8);
turn_counterclockwise_center(percent, counts);

arm_servo.SetDegree(0);
    Sleep(1.0);

counts = (CPI*8.75); //measured
move_forward(percent, counts);

counts = (CPI*4);
move_forward(percent, counts);

counts = (CPI*0.5);
move_forward(percent,counts);

arm_servo.SetDegree(60); //for loop later

Sleep(1.5);

arm_servo.SetDegree(0);

counts = (CPI*3.5);
move_forward(-percent,counts);

Sleep(5.0);

arm_servo.SetDegree(90);

Sleep(1.0);
move_forward(percent,counts);
Sleep(1.0);

arm_servo.SetDegree(60);

Sleep(.5);
    //move back
    counts = (CPI*3.5);
    move_forward(-percent,counts);

    arm_servo.SetDegree(0);

    Sleep(.5);

    //move 9 inches to junction
    counts = (CPI*9.5);
    move_forward(-percent, counts);


    Sleep(.5);
}


//protocol for navigating to the right lever
//potentially reorder to have orienting turns first, then drive
void right_lever(int percent, int counts)
{
    //turn 45 degrees
    percent = turn_power;
    counts = (full_turn_center/8);
    turn_counterclockwise_center(percent, counts);

    //move 5 inches to center line
    counts = (CPI*10.75);
    move_forward(percent, counts);

    //turn 90 degrees
    counts = (full_turn_center/4+5);
    turn_counterclockwise_center(-percent, counts);
    // move one inch
    counts = (CPI*4);
    move_forward(percent, counts);

    //set servo up
    arm_servo.SetDegree(0);
    Sleep(1.0);

    //move 90 degrees the other way
    counts = (full_turn_center/4);
    turn_counterclockwise_center(percent, counts);

counts = (CPI*.5); //changed
move_forward(percent, counts);

counts = (CPI*0.5);
move_forward(percent,counts);

arm_servo.SetDegree(60); //for loop later

Sleep(1.5);

arm_servo.SetDegree(0);

Sleep(.5);

counts = (CPI*3.5);
move_forward(-percent,counts);

Sleep(5.0);

arm_servo.SetDegree(90);

//Sleep(1.0);
move_forward(percent,counts);
//Sleep(1.0);

arm_servo.SetDegree(60);


//move back
    counts = (CPI*3.5);
    move_forward(-percent,counts);

    arm_servo.SetDegree(0);

    Sleep(.5);
    //turn clockwise
    counts = (full_turn_center/4);
    turn_counterclockwise_center(-percent, counts);

    Sleep(.5);

    // move back to center line
    // move one inch
    counts = (CPI*4);
    move_forward(-percent, counts);
    Sleep(.5);

    //turn back
    //turn 90 degrees
    counts = (full_turn_center/4+5);
    turn_counterclockwise_center(percent, counts);
    Sleep(.5);

    //move 9 inches to junction
    counts = (CPI*10);
    move_forward(-percent, counts);
    Sleep(.5);


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

    //reset arm servo
    arm_servo.SetDegree(0);
    LCD.Write("  servo");

    int touch_x,touch_y;

    LCD.WriteLine("Waiting for Final Action...");
    while(!LCD.Touch(&touch_x,&touch_y));
    while(LCD.Touch(&touch_x,&touch_y));
    LCD.Clear();

    //read start light
    read_start();
    LCD.Write(CdS_cell.Value());
    LCD.Write("cds");

    //back into start button
    counts = (CPI*2);
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
    counts = (CPI*13.75);
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
    counts = (full_turn_about/3.8);
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
    counts = (CPI*1.75);
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
    Sleep(2.25);
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
    counts = (CPI*3);
    move_forward(percent, counts);

    //90 degrees clockwise about center
    percent = -turn_power;
    counts = (full_turn_center/3.7);
    turn_counterclockwise_center(percent, counts);

    //drive backward to starting zone
    percent = -drive_power;
    counts = (CPI*25); //test and modify
    move_forward(percent, counts);

    //slow down!
    percent = -(turn_power);
    counts = (CPI*6);
    move_forward(percent, counts);
    Sleep(1.5);


}

void apple_nav (int percent, int counts, int i, int target_angle)
{

    //forward 0.5 in off wall
    percent = turn_power;
    counts = (CPI*0.5);
    move_forward(percent, counts);

    //90 degrees clockwise about right
    percent = turn_power;
    counts = (full_turn_about/3.6);
    turn_about_right(percent, counts);

    //move up ramp and try to catch line
    percent = drive_power-10;
    counts = (CPI*18);
    move_forward(percent, counts);

    Sleep(0.5);

    //follow along line until 90 degree turn
    percent = turn_power;
    counts = (CPI*4); //test and modify
    follow_line_counts(percent, counts);

    // if(state == 0)
    // {
    //     state = 1;
    //     percent = turn_power;
    //     counts = (full_turn_center/24);
    //     turn_counterclockwise_center(-percent, counts);
    //     //follow along line until 90 degree turn
    //     percent = turn_power;
    //     counts = (CPI*5); //test and modify
    //     follow_line_counts(percent, counts);
    // }
    // state = 1;

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
    counts = (CPI*14); //test and modify
    follow_line_counts(percent, counts);
    LCD.Write("caught line");

    //align on crate
    percent = turn_power;
    counts = (CPI*2);
    move_forward(percent, counts);

    // //back out slightly so basket does not hit back of crate
    // percent = -turn_power;
    // counts = (CPI*0.25);
    // move_forward(percent, counts);

    //lower basket into crate (down 30 degrees, get to 80 degrees)
    for(i=0; i<50; i++){
        arm_servo.SetDegree(i);
        Sleep(0.01);
    }
    LCD.Write("arm down");
    Sleep(0.5);

    //back arm out of basket
    percent = -turn_power;
    counts = (CPI*5);
    move_forward(percent, counts);
    Sleep(0.5);

    //raise arm back up to top
    for(i; i>0; i--){
        arm_servo.SetDegree(i);
        Sleep(0.01);
    }
    LCD.Write("raise arm");

    //follow line backwards to turn junction
    percent = -turn_power;
    counts = (CPI*8); //DOUBLE CHECK
    move_forward(percent, counts);


}

void humider_button(int percent, int counts)
{
    //reverse of levers --> put in lever function
    //45 degree turn
    // shaft encode back into wall
    //shaft encode to line
    // line follow by counts
    //read cds cell
    //turn 180 degrees
    //hit either blue or red


    Sleep(.5);
   
    //turn 45 degrees
    percent = turn_power;
    counts = (full_turn_center/8);
    turn_counterclockwise_center(percent, counts);

    counts = (CPI*4.5);
    follow_line_counts(percent, counts);

    Sleep(1.5);

    counts = (CPI*3.5);
    move_forward(percent,counts);

   
    cds_check(percent,counts);

}

void window(int percent, int counts)
{
   

    //rotate 180
    percent = turn_power;
    counts = (full_turn_center/1.75);
    turn_counterclockwise_center(percent, counts);

    Sleep(1.5);

    //shaft encode to wall
    percent = drive_power;
    counts = CPI*11;
    move_forward(-percent,counts);

    Sleep(1.5);

    //move foward a little
    percent = drive_power;
    counts = CPI*1;
    move_forward(percent,counts);

    Sleep(1.5);

    //turn left
    percent = turn_power;
    counts = full_turn_about/6;
    turn_about_left(percent,counts);  

    Sleep(1.5);

    //add a move foward
     percent = drive_power;
    counts = CPI*1;
    move_forward(percent,counts);

    Sleep(1.5);

    //turn right
    percent = turn_power;
    counts = full_turn_about/6;
    turn_about_right(percent,counts);

    Sleep(1.5);

    //move up to window
    percent = drive_power;
    counts = CPI*4;
    follow_line_counts(percent,counts);

    Sleep(1.5);


    // servo down
    window_servo.SetDegree(80);
    Sleep(.45);
    window_servo.Off();

    Sleep(1.5);

    //move window forward
    percent = drive_power;
    counts = CPI*4;
    follow_line_counts(percent,counts);

    Sleep(1.5);
    //move window back
     percent = drive_power;
    counts = CPI*4;
    follow_line_counts(-percent,counts);
    Sleep(1.5);

    //servo up
    window_servo.SetDegree(0);
    Sleep(.45);
    window_servo.Off();

    // move back
    percent = drive_power;
    counts = CPI*15;
    move_forward(-percent,counts);
    Sleep(1.5);

    counts = CPI*1;
    move_forward(percent, counts);
    Sleep(1.5);

    percent = turn_power;
    counts = full_turn_about/4;
    turn_about_left(percent, counts);
    Sleep(1.5);

    counts = CPI *30;
    move_forward(percent, counts);
    Sleep(1.5);
    percent = turn_power;
    counts = full_turn_about/4;
    turn_about_left(percent, counts);
    Sleep(1.5);

    counts = CPI *30;
    move_forward(percent, counts);
    Sleep(1.5);

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

    start_protocol(percent, counts);

    apple_compost(percent, counts, i , target_angle);

    apple_nav(percent, counts, i ,target_angle);

    choose_lever(percent, counts);

    humider_button(percent, counts);

    //make sure correct
    window_servo.SetDegree(80);
    Sleep(.45);

    window_servo.Off();

    counts = CPI*15;
    move_forward(percent, counts);

    percent = turn_power;
    counts = full_turn_about/6;
    turn_about_left(percent, counts);
    Sleep(1.5);

    counts = CPI*10;
    move_forward(percent, counts);

    percent = turn_power;
    counts = full_turn_about/4;
    turn_about_left(percent, counts);
    Sleep(1.5);

    counts = CPI*25;
    move_forward(-percent, counts);
    Sleep(1.5);

    counts = CPI*1;
    move_forward(percent, counts);

    percent = turn_power;
    counts = full_turn_about/4;
    turn_about_left(percent, counts);
    Sleep(1.5);

    counts = CPI*30;
    move_forward(percent, counts);
    Sleep(1.5);

    counts = CPI*1;
    move_forward(-percent, counts);
    Sleep(1.5);

     percent = turn_power;
    counts = full_turn_about/4;
    turn_about_left(percent, counts);
    Sleep(1.5);

    counts = CPI*30;
    move_forward(percent, counts);
    Sleep(1.5);


    //window(percent,counts);

}