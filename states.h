bool STATE=1;
String returnState()  {
    if(STATE) return "OPERATIONAL";
    else return "ERROR";
}