#include <FEH.h>
#include <Arduino.h>


// Declare things like Motors, Servos, etc. here
// For example:
// FEHMotor leftMotor(FEHMotor::Motor0, 6.0);
// FEHServo servo(FEHServo::Servo0);

AnalogInputPin CdS_Cell(FEHIO::Pin0);

void ERCMain()
{
    while(true)
    {
        float value = CdS_Cell.Value();
        LCD.Write(value);
        LCD.Clear();

    }

}