#include "menu.h"


void mainMenu(int i) {
    if(i==1)    {
        menuFunc=menu1;
    }
    else if(i==2)   {
menuFunc=menu2;
    }
    else if(i==3)   {
menuFunc=menu3;
    }
}
void menu1(int i)   {

}
void menu2(int i)   {

}
void menu3(int i)   {

}


void initializeMenuOptions()    {
    menuFunc=mainMenu;
}
