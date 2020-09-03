#include "menu.h"

void mainMenu(int i)
{
    Serial.println("Welcome in JarvisOS ! ");
    Serial.println("Choose program mode : ");
    Serial.println("1. Commands mode");
    Serial.println("2. Joystick mode");
    if (i == 1)
    {
        //Serial.println("dzaaaaaaaaaaaaaaaaaaa");
        menuFunc = menu1;
    }
    else if (i == 2)
    {
        menuFunc = menu2;
        //Serial.println("dzaaaaaaaaaaaaaaaaaaa2");
    }
    else if (i == 3)
    {
        menuFunc = menu3;
    }
}
void menu1(int i)
{
    Serial.println("Moving by commands ! ");
    Serial.println("Type 'exit' to back");
    String str;
    while (1)
    {
        while (Serial.available()>0)
        {
            str = Serial.readString();
        }
        if (str.length() > 2)
        {
            
            char *input = new char[40];
            str.toCharArray(input, str.length());
            Serial.println(str);
            if(str.equals("exit")) break;
            if (interpreter(input) == -1)
                Serial.println("Null error or invalid command");
            str = "";
        }
    }
}
void menu2(int i)
{
    Serial.println("Bluetooth control mode ");
    Serial.println("Type '`' to back ");
    Serial.write(250);
    int deg = 1;
    int axis = 0;
    char a = 'x';
    bool lastGripper=1;
    while (1)
    {
        
        while (Serial.available() > 0)
        {
            a = Serial.read();
        }
        if(a=='`') break;
        else if (a != 'x')
        {
            //char *input = new char[40];
            // str.toCharArray(input, str.length());
            // Serial.println(str);
            // if (interpreter(input) == -1)
            //     Serial.println("Null error or invalid command");
            if (a == 200)
            {
                a='x';
                //deg control mode
                while (1)
                {
                    while (Serial.available() > 0)
                    {
                        a = Serial.read();
                    }
                    if (a != 'x')   {
                        deg=a;
                        a='x';
                        break;
                    }
                }
            }
            else if (a ==0 || a ==1 || a ==2 || a ==3 || a ==4 || a ==5 )
            {
                if(a==5 && lastGripper) {
                    procedureG10();
                    lastGripper=!lastGripper;
                }
                else if(a==5 && !lastGripper) {
                    procedureG11();
                    lastGripper=!lastGripper;
                }
                else    {
                    axis=a;
                }
            }
            else if (a =='q' || a == 'w' || a=='e')//up
            {
                mov0(axis,deg);
            }
            else if (a =='r' || a == 't' || a=='y')//down 
            {
                mov0(axis,-deg);
            }
            

            a = 'x';
        }
    }
}
void menu3(int i)
{
}

void initializeMenuOptions()
{
    menuFunc = mainMenu;
}
