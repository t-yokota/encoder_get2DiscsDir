// get direction of the rotating disc by encoder  

int pin_enc_1 = 5;
int pin_enc_2 = 7;
# define PIN_ENC_1 5
# define PIN_ENC_2 7
# define PIN_BUSY_1 9
# define PIN_BUSY_2 10
# define PIN_LED 13

boolean initBool;

const int numSlit1 = 6;
const int numSlit2 = 8;

double slitThd1;
double slitThd2;

struct encState{
        int slitCount;
        boolean encVal_cr;
        boolean encVal_prv;
        unsigned long falseSrt;        
        unsigned long falseEnd;                   
};

struct encState enc1;
struct encState enc2;

void setup() {
        Serial.begin(2000000);
        pinMode(PIN_ENC_1, INPUT);
        pinMode(PIN_ENC_2, INPUT);
        pinMode(PIN_LED, OUTPUT);

        initBool = false;
        
        enc1.slitCount = 0;
        enc2.slitCount = 0;
        enc1.falseSrt = micros();
        enc2.falseSrt = micros();
        enc1.encVal_prv = digitalRead(PIN_ENC_1);
        enc2.encVal_prv = digitalRead(PIN_ENC_2);
        digitalWrite(PIN_LED, LOW);
}

void loop() {
        // get a BUSY flag from a motor driver.
        while( !( digitalRead(PIN_BUSY_1) && digitalRead(PIN_BUSY_2) ) ){ 
               // now, we use a stepping motor driven by L6470 and be waiting end of the acceleration.
               initBool = true;
               Serial.println("busy_now");
               delay(100);
        }

        // get threshold of the magnification between wide slit and thin slit.
        if( initBool == true ){
                slitThd1 = getSlitThreshold( pin_enc_1, numSlit1 );
                slitThd2 = getSlitThreshold( pin_enc_2, numSlit2 );
                initBool = false;                
        }
        
        // get encoder state and count slit
        if( initBool == false ){
                // get the current states of photo interrupter.
                enc1.encVal_cr = digitalRead(PIN_ENC_1); // in the slit area = false,  except area = true;
                enc2.encVal_cr = digitalRead(PIN_ENC_2); // in the slit area = false,  except area = true;
                
                getEncState( &enc1, slitThd1 );
                getEncState( &enc2, slitThd2 );
                
                enc1.encVal_prv = enc1.encVal_cr;
                enc2.encVal_prv = enc2.encVal_cr;
                Serial.print( enc1.slitCount ); Serial.print( ", " );
                Serial.println( enc2.slitCount );
        }
}
