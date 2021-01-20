#define rctrl 5
#define grnctrl 3
#define bluectrl 6

const int szArr=9;
int redval[szArr]={0,187,255,0,146,255,182,255,11};
int grnval[szArr]={0,255,113,255,255,194,105,0,231};
int blueval[szArr]={255,245,20,0,71,13,255,0,255};


int arrVal=0;

int rcalc;
int grncalc;
int bcalc;
int rdiff;
int bdiff;
int grndiff;
int stepincr;

double tchng=5.0;
int nstep=85;
double tstep;
int ndiv;
int ndivcount=0;
int tflag=0;

//PWM stats
int rval=0;
int bval=0;
int gval=0;

unsigned int b;



void setup(){
  //Time calculation, for each increment step
  tstep=tchng/((double)nstep);
  
  //Timer1 setup
  b=(65536.0-62500.0*tstep);//calc delay. MUST be unsigned
  
  TCCR1A = 0x00;    //Upper timer1 control bits
  TCCR1B = 0b00000100;    //Lower timer1 control bits, ps=256
  TCNT1=b;//for 1sec
  TIMSK1 = 0x01;//comp0 disabled

  //Initial calcs
  rdiff=redval[1]-redval[0];
  bdiff=blueval[1]-blueval[0];
  grndiff=grnval[1]-grnval[0];
}

void loop(){
  if(tflag==1){
    tflag=0;
    nextStepPlan();
  }
}

void nextStepPlan(){
    //Calculate current rgb values as a linear progression between 2 colors
    rcalc=redval[arrVal]+(int)((double)rdiff*((double)stepincr/(double)nstep));
    grncalc=grnval[arrVal]+(int)((double)grndiff*((double)stepincr/(double)nstep));
    bcalc=blueval[arrVal]+(int)((double)bdiff*((double)stepincr/(double)nstep));

    //Now write these colors to ADC
    analogWrite(rctrl,rcalc);
    analogWrite(grnctrl,grncalc);
    analogWrite(bluectrl,bcalc);
    stepincr++;
    
  //resetting increments when necessary (after color loop has been played)
  if(stepincr>nstep){
    stepincr=0;
    arrVal++;

    if(arrVal==(szArr)){
      arrVal=0;
    }
    if(arrVal!=(szArr-1)){
      rdiff=redval[arrVal+1]-redval[arrVal];
      bdiff=blueval[arrVal+1]-blueval[arrVal];
      grndiff=grnval[arrVal+1]-grnval[arrVal];
    }
    if(arrVal==(szArr-1)){
      rdiff=redval[0]-redval[arrVal];
      bdiff=blueval[0]-blueval[arrVal];
      grndiff=grnval[0]-grnval[arrVal];
    }
  }
}

ISR(TIMER1_OVF_vect){
  TCNT1=b;
  tflag=1;
}
