#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRCS.h>
#include <FEHSD.h>

#define CPI 33.74
#define TR 3.5
#define pi 3.14159
#define turn_power 25
#define drive_power 40

//Declarations for encoders & motors
DigitalEncoder right_encoder(FEHIO::Pin8);
DigitalEncoder left_encoder(FEHIO::Pin9);
FEHMotor right_motor(FEHMotor::Motor0, 9.0);
FEHMotor left_motor(FEHMotor::Motor1, 9.0);
AnalogInputPin CdS_cell(FEHIO::Pin0);

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
    if((value <= ###) && (value >= ###)){
    break;
    }

}
}


void read_color ()
{
float value;
float time_start = TimeNow();
while((TimeNow() - time_start) <= 30)
{
    value = CdS_cell.Value();
    if((value <= ###) && (value >= ###)){
    break;
    }

}


}


void ERCMain()
{
    //counts/inch for 3" wheels : 33.74
    int counts;
    int percent;

    read_start();

    counts = (CPI*2);
    percent = -drive_power;
    move_forward(percent, counts);

    counts = (CPI*2);
    percent = drive_power;
    move_forward(percent, counts);

    counts = (CPI*2*pi*TR/8);
    percent = turn_power;
    turn_about_right(percent, counts);

    counts = (CPI*37);
    percent = drive_power;
    move_forward(percent, counts);

    counts = (CPI*2*pi*TR/4);
    percent = turn_power;
    turn_counterclockwise_center(percent, counts);

    counts = (CPI*13.625);
    percent = drive_power;
    move_forward(percent, counts);



   









}