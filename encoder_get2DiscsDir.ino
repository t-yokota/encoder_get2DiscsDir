// get direction of the rotating disc by encoder  

const int pin_enc_2 = 7;
# define PIN_ENC_1 6
# define PIN_ENC_2 7
# define PIN_BUSY 9
# define PIN_LED 13

const int numSlit = 6;

boolean encVal1_cr;
boolean encVal1_prv;
boolean encVal2_cr;
boolean encVal2_prv;

int slitCount;
unsigned long falseSrt;
unsigned long falseEnd;
unsigned long falseTime;

boolean initBool;
const int numElem =  ( numSlit + 1 ) * 10;
unsigned long falseNrrw[ numElem ];
unsigned long falseWid[ numElem ];
float falseNrrwAve;
float falseWidAve;
float slitThd;

void setup() {
        Serial.begin(2000000);
        pinMode(PIN_ENC_1, INPUT);
        pinMode(PIN_ENC_2, INPUT);
        pinMode(PIN_LED, OUTPUT);

        encVal1_prv = digitalRead(PIN_ENC_1);
        encVal2_prv = digitalRead(PIN_ENC_2);

        slitCount = 0;
        initBool = true;
        digitalWrite(PIN_LED, LOW);
}

void loop() {
        // get a BUSY flag from a motor driver.
        // now, we use a stepping motor driven by L6470 and be waiting end of the acceleration.
        while( !digitalRead(PIN_BUSY)){ 
                slitCount = 0;
                initBool = true;       
                falseSrt = micros();   
                for( int i = 0; i < numElem; i++ ){
                        falseNrrw[ i ] = 0;
                        falseWid[ i ] = 0;
                }
        }  

        // get threshold of the magnification between wide slit and thin slit.
        if( initBool == true ){
                slitThd = getSlitThd(pin_enc_2, numSlit);
                initBool = false;              
        }

        // get the current states of photo interrupter.
        encVal1_cr = digitalRead(PIN_ENC_1); // in the slit area = false,  except area = true;
        encVal2_cr = digitalRead(PIN_ENC_2); // in the slit area = false,  except area = true;
        
        // 
        if( initBool == false ){
                if( encVal2_cr != encVal2_prv ){
                        if ( encVal2_cr == false ){
                                falseSrt = micros();
                        }
                        if( encVal2_cr == true ){
                                falseEnd = micros();
                                falseTime = falseEnd - falseSrt;
                                if( falseTime * slitThd > falseWidAve ){
                                        slitCount = 0;        
                                }else{
                                        slitCount += 1;
                                        digitalWrite(PIN_LED, HIGH);
                                        delay(1);
                                        digitalWrite(PIN_LED, LOW);                                        
                                }                        
                                Serial.print( "slit: " ); Serial.print( slitCount ); Serial.print( ", " );
                                Serial.print( "false: " ); Serial.print( falseTime ); Serial.print( ", " );
                        }
                }
        }
        encVal2_prv = encVal2_cr;
}


float getSlitThd( const int pin_enc, const int numSlit ){  
        boolean encVal_cr;
        boolean encVal_prv;
        
        int slitCount;
        unsigned long falseSrt;
        unsigned long falseEnd;
        unsigned long falseTime;
        
        const int numRot = 10;
        const int numElem =  ( numSlit + 1 ) * numRot;
        unsigned long falseNrrw[ numElem ];
        unsigned long falseWid[ numElem ];
        float falseNrrwAve;
        float falseWidAve;
        float slitThd;                
        
        slitCount = 0;
        falseSrt = micros();   
        for( int i = 0; i < numElem; i++ ){
                falseNrrw[ i ] = 0;
                falseWid[ i ] = 0;
        }
        
        while( slitCount <= numElem ){        
                // get the current states of photo interrupter.
                encVal_cr = digitalRead(pin_enc); // in the slit area = false,  except area = true;

                // get threshold of the magnification between wide slit and thin slit.
                if (encVal_cr != encVal_prv){
                        if( encVal_cr ==  false){ // start slit area 
                                falseSrt = micros();
                                }
                        if( encVal_cr == true ){ // end slit area
                                falseEnd = micros();
                                falseTime = falseEnd - falseSrt;
                                Serial.print( "false: " ); Serial.println( falseTime );
                                slitCount += 1;
                                if( slitCount == 1 ){
                                        falseNrrw[0] = falseTime;
                                        falseWid[0] = falseTime;
                                }else if( slitCount <= numElem ){
                                        if ( falseTime > falseWid[ slitCount -1 -1 ] * 0.8){
                                                falseWid[ slitCount -1 ] = falseTime;
                                                falseNrrw[ slitCount -1 ] = falseNrrw[ slitCount -1 - 1 ];                                                
                                        }
                                        if ( falseTime < falseWid[ slitCount -1 -1 ] * 0.8){
                                                falseNrrw[ slitCount -1 ] = falseTime;
                                                falseWid[ slitCount -1 ] = falseWid[ slitCount -1 - 1 ];        
                                        }
                                }else if( slitCount > numElem ){
                                        slitCount = 0;
                                        Serial.println("");
                                        Serial.println( "falseNrrw: "  );
                                        for( int i = 0; i < numElem; i++ ){
                                                Serial.println( falseNrrw[ i ] );
                                        }
                                        Serial.println( "falseWid: " ); 
                                        for( int i = 0; i < numElem; i++ ){
                                                Serial.println( falseWid[ i ] );
                                        }
                                        unsigned long _falseNrrw[ numElem - ( numSlit + 1 ) ]; // except first rotation
                                        unsigned long _falseWid[ numElem - ( numSlit + 1 ) ]; // except first rotation
                                        for( int i = 0; i < numElem - ( numSlit + 1 ); i++ ){
                                                _falseNrrw[ i ] = falseNrrw[ i + ( numSlit + 1 ) ];     
                                                _falseWid[ i ] = falseWid[ i + ( numSlit + 1 ) ];
                                        }
                                        float falseNrrwStd = stdev( _falseNrrw, numElem - ( numSlit + 1 ) ); // standard deviation
                                        float falseWidStd = stdev( _falseWid, numElem - ( numSlit + 1 ) ); // standard deviation
                                        falseNrrwAve = exOutMean( _falseNrrw, falseNrrwStd, numElem - ( numSlit + 1 )); // mean except outliers
                                        falseWidAve = exOutMean( _falseWid, falseWidStd, numElem - ( numSlit + 1 )); // mean except outliers
                                        slitThd = sqrt( falseWidAve / falseNrrwAve );
                                        Serial.println("falseNrrwStd: "); Serial.println( falseNrrwStd );
                                        Serial.println("falseNrrwAve: "); Serial.println( falseNrrwAve );
                                        Serial.println("falseWidStd: "); Serial.println( falseWidStd );                                       
                                        Serial.println("falseWidAve: "); Serial.println( falseWidAve );
                                        Serial.println("threshold of magnification between falseWid and falseNrrw: "); Serial.println( slitThd );
                                 }
                        }
                }                
                encVal2_prv = encVal2_cr;        
        }
        return slitThd;
}

float mean( unsigned long x[], int xlength ){
        unsigned long sum = 0;
        float ave = 0;
        for(int i = 0; i < xlength ; i++ ){
                sum += x[ i ];
        }
        return ave = (float)sum / xlength;
}

float stdev( unsigned long x[], int xlength ){
        float ave = mean( x, xlength );
        float var = 0;
        float std = 0;
        for( int i = 0; i < xlength; i++ ){
                var += ( x[ i ] - ave ) * ( x[ i ] - ave );
        }
        var = var / xlength;
        return std = sqrt(var);
}

float exOutMean ( unsigned long x[], float xstdev, int xlength ){
        // except outliers mean
        unsigned long sum = 0;
        float ave = mean( x, xlength );
        float exOutAve = 0;
        int n = 0;
        for( int i = 0; i < xlength; i++ ){
                if( abs( (float)x[ i ] - ave ) <= 2*xstdev ){
                        sum += x[ i ]; 
                        n += 1;                                                                 
                }
        }
        return ave = (float)sum / n ;
}
