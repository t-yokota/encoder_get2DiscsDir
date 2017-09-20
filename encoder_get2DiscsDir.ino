// get direction of the rotating disc by encoder

int pin_enc_1 = 6;
int pin_enc_2 = 7;
# define PIN_ENC_1 6
# define PIN_ENC_2 7
# define PIN_BUSY_1 9
# define PIN_BUSY_2 10
# define PIN_LED_1 12
# define PIN_LED_2 13

boolean initBool;

const int initRot = 5; // the amount of init rotation for calibrating encoder.

struct encoderParams{
        int numSlit;
        int slitCount;
        double slitThd;
        boolean encVal_cr;
        boolean encVal_prv;
        unsigned long falseSrt;
        unsigned long falseEnd;
        double falseNrrwAve;
        double falseWidAve;
};

struct encoderParams enc1;
struct encoderParams enc2;

void setup() {
        Serial.begin(2000000);
        pinMode(PIN_ENC_1, INPUT);
        pinMode(PIN_ENC_2, INPUT);
        pinMode(PIN_LED_1, OUTPUT);
        pinMode(PIN_LED_2, OUTPUT);

        initBool = true;

        enc1.numSlit = 6;
        enc2.numSlit = 8;

        enc1.slitCount = 0;
        enc2.slitCount = 0;
        enc1.falseSrt = micros();
        enc2.falseSrt = micros();
        enc1.encVal_prv = digitalRead(PIN_ENC_1);
        enc2.encVal_prv = digitalRead(PIN_ENC_2);
        digitalWrite(PIN_LED_1, LOW);
        digitalWrite(PIN_LED_2, LOW);
}

void loop() {
        // get a BUSY flag from a motor driver.
        // now, we use a stepping motor driven by L6470 and be waiting end of the acceleration.
        while( !( digitalRead(PIN_BUSY_1) && digitalRead(PIN_BUSY_2) ) ){ 
               initBool = true;
               Serial.println("busy_now");
               delay(100);
        }

        // get threshold of the magnification between wide slit and thin slit.
        if( initBool == true ){
                enc1.slitThd = getSlitThreshold( pin_enc_1, &enc1 ,initRot );
                enc2.slitThd = getSlitThreshold( pin_enc_2, &enc2 ,initRot );
                initBool = false;                
        }
        
        // get encoder state and count slit
        if( initBool == false ){
                // get the current states of photo interrupter.
                enc1.encVal_cr = digitalRead(PIN_ENC_1); // in the slit area = false,  except area = true;
                enc2.encVal_cr = digitalRead(PIN_ENC_2); // in the slit area = false,  except area = true;
                
                cntEncSlit( &enc1 );
                cntEncSlit( &enc2 );
                
                enc1.encVal_prv = enc1.encVal_cr;
                enc2.encVal_prv = enc2.encVal_cr;
                Serial.print( enc1.slitCount ); Serial.print( ", " );
                Serial.println( enc2.slitCount );
        }
}
