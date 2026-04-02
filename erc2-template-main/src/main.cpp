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

//Declarations for encoders & motors
//left and right as view from the front 
DigitalEncoder right_encoder(FEHIO::Pin8);
DigitalEncoder left_encoder(FEHIO::Pin9);
FEHMotor right_motor(FEHMotor::Motor0, 9.0);
FEHMotor left_motor(FEHMotor::Motor1, 9.0);
AnalogInputPin CdS_cell(FEHIO::Pin2);
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
    // float right_value, center_value, left_value;

    while(true)
    {
    //black lines >4, all else <4
    // right_value = right_opto.Value();
    // center_value = center_opto.Value();
    // left_value = left_opto.Value();


        if((right_opto.Value() >= 3.5))
        {
        right_motor.Stop();
        left_motor.SetPercent(percent);
        }

        else if ((left_opto.Value() >= 3.5))
        {
        left_motor.Stop();
        right_motor.SetPercent(percent);
        }

        else if(center_opto.Value() > 3.5)
        {
        right_motor.SetPercent(percent);
        left_motor.SetPercent(percent);
        }

        //add cases for two seeing black where it makes a 90 degree turn
        //in the given direction

        else if ((left_opto.Value() <= 3) && (center_opto.Value() <=3) && (right_opto.Value() <=3))
        {
        right_motor.Stop();
        left_motor.Stop();
        Sleep(0.5);
        LCD.Write("STOP: All White");
        break;
        }

        else if (((left_opto.Value() > 4.5) && (center_opto.Value() > 4.5)) || ((right_opto.Value() > 4.5) && (center_opto.Value() > 4.5)))
        {
        right_motor.Stop();
        left_motor.Stop();
        Sleep(0.5);
        LCD.Write("STOP: Expected Turn");
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
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts)
    {
        if((right_opto.Value() >= 3.5))
        {
        right_motor.Stop();
        left_motor.SetPercent(percent);
        }

        else if ((left_opto.Value() >= 3.5))
        {
        left_motor.Stop();
        right_motor.SetPercent(percent);
        }

        else if(center_opto.Value() > 3.5)
        {
        right_motor.SetPercent(percent);
        left_motor.SetPercent(percent);
        }

    }


}

//protocol for navigating to the left lever
void left_lever()
{


}

//protocol for navigating to the center lever
void center_lever()
{

}

//protocol for navigating to the right lever
void right_lever()
{

}

//decision program for navigating to levers
void choose_lever()
{
    int lever_choice = RCS.GetLever();

    if(lever_choice == 0)
    {
        left_lever();
    }

    else if(lever_choice == 1)
    {
        center_lever();
    }

    else if(lever_choice == 2)
    {
        right_lever();
    }

}

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void ERCMain()
{
//   //counts/inch for 3" wheels : 33.74
//   //180 degree is vertical
//   //90 degree is horizontal


//     int counts;
//     int percent;
//     int percent_R, percent_L;

//     arm_servo.SetMin(servo_min);
//     arm_servo.SetMax(servo_max);


//     // arm_servo.SetDegree(0);
//     // LCD.Write("servo");

//     // //read start light
//     // read_start();
//     // LCD.Write("cds");

//     //back into start button
//     counts = (CPI*0.75); //should be 1.5
//     percent = -drive_power/2;
//     move_forward(percent, counts);
//     LCD.Write("motor");

//     Sleep(0.25);
//     LCD.Write("sleep");

//     //drive straight forward and look for line
//     percent = turn_power;
//     counts = (CPI*5); //should be 9
//     move_forward(percent, counts);
//      LCD.Write("drive");
//     // look_for_line(percent);

//     //lower arm to interface w basket
//     int i = 0;
//     for(i=0; i<50; i++){
//         arm_servo.SetDegree(0+i);
//         Sleep(0.01);
//     }

//     //follow line until turn
//     percent = turn_power;
//     counts = (CPI*6); //should be 10 if using counts
//     follow_line(percent); //will break when none on black, AKA at 90 turn
//     //follow_line_counts(percent, counts);

//     //turn to align
//     percent = -turn_power/2;
//     counts = (CPI*0.5);
//     turn_about_right(percent, counts);

//     //drive into basket to grab
//     percent = turn_power;
//     counts = (CPI*3);
//     move_forward(percent, counts);
//     Sleep(1.0);

//     //raise arm 15 degrees
//     for(i; i>35; i--){
//         arm_servo.SetDegree(0+i);
//         Sleep(0.01);
//     }

//     //go back the way it came and follow line out
//     percent = -turn_power;
//     counts = (CPI*2);
//     move_forward(percent, counts);
//     follow_line(percent);

//      //return to starting position
//     percent = turn_power;
//     counts = (CPI*5); //should be 9
//     move_forward(percent, counts);

//     //turn to align w ramp
//     counts = (CPI*2*pi*2*TR/8);
//     percent = turn_power;
//     turn_about_right(percent, counts);

//     //drive up ramp and catch line
//     counts = (CPI*28);
//     percent = turn_power;
//     move_forward(percent, counts);

//     /////////////////////
//     /////UPPER LEVEL/////
//     /////////////////////

//     //follow along line until 90 degree turn
//     percent = turn_power;
//     counts = (CPI*9);
//     follow_line_counts(percent, counts);

//     //make manual 45 degree turn
//     percent = turn_power;
//     counts = (TR*2*pi/8);
//     turn_counterclockwise_center(percent, counts);

//     //drive forward to try and catch line
//     percent = turn_power;
//     look_for_line(percent);

//     //follow line to apple crate
//     percent = turn_power;
//     follow_line(percent);

//     //align on crate
//     percent = turn_power; 
//     counts = (CPI*2);
//     move_forward(percent, counts);

//     percent = -turn_power; //back out slightly so basket does not hit back of crate
//     counts = (CPI*0.25);
//     move_forward(percent, counts);

//     //lower basket into crate (down 30 degrees, get to 65 degrees)
//     for(i; i<65; i++){
//         arm_servo.SetDegree(0+i);
//         Sleep(0.01);
//     }

//     //back arm out of basket
//     percent = -turn_power;
//     counts = (CPI*3);
//     move_forward(percent, counts);

//     //raise arm back up to top
//     for(i; i>0; i--){
//         arm_servo.SetDegree(0+i);
//         Sleep(0.01);
//     }

//     //follow line backwards to turn junction
//     percent = -turn_power;
//     follow_line(percent);

//     ////////////////////////
//     ////STARTING LEVERS/////
//     ///////////////////////

//     //make 45 degree turn to face levers
//     percent = turn_power;
//     counts = (CPI*TR*2*pi/8);
//     turn_counterclockwise_center(percent, counts);

//     //get info from course and navigate to lever
//     choose_lever();










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


//encoder testing loop
    while(true)
    {
        int percent, counts;

        percent = turn_power;
        counts = (CPI*2*TR*2*pi);
        turn_about_left(percent, counts);
        Sleep(2.0);

        percent = turn_power;
        counts = (CPI*2*TR*2*pi);
        turn_about_right(percent, counts);
        Sleep(2.0);

        percent = turn_power;
        counts = (CPI*TR*2*pi);
        turn_counterclockwise_center(percent, counts);
        Sleep(2.0);

        percent = -turn_power;
        counts = (CPI*TR*2*pi);
        turn_counterclockwise_center(percent, counts);
        Sleep(2.0);

        percent = turn_power;
        counts = (CPI*6);
        move_forward(percent, counts);
        Sleep(2.0);

        percent = -turn_power;
        counts = (CPI*6);
        move_forward(percent, counts);
        Sleep(2.0);


    }

}
