void initializeSteppers();
void writeDirPin(int axis,int pos);
void mov1(int axis, int pos);
void mov0(int axis, int deg);
bool checkDeg(int axis,int deg);
bool checkPos(int axis,int pos);
void mov2(int axis1,int pos1,int axis2,int pos2);
void mov3(int axis1, int pos1, int axis2, int pos2,int axis3,int pos3);