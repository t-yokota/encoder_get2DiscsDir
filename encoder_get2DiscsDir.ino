// get direction of the rotating disc by encoder  


//const int pin_enc_2 = 7;
const int pin_enc_1 = 6;
const int pin_enc_2 = 7;
# define PIN_ENC_1 6
# define PIN_ENC_2 7
# define PIN_BUSY_1 9
# define PIN_BUSY_2 10
# define PIN_LED 13


const int numSlit1 = 6;
const int numSlit2 = 6;

boolean encVal1_cr;
boolean encVal1_prv;
boolean encVal2_cr;
boolean encVal2_prv;

int slitCount1;
unsigned long falseSrt1;
unsigned long falseEnd1;
unsigned long falseTime1;
int slitCount2;
unsigned long falseSrt2;
unsigned long falseEnd2;
unsigned long falseTime2;


boolean initBool;

const int numElem1 =  ( numSlit1 + 1 ) * 5;
unsigned long falseNrrw1[ numElem1 ];
unsigned long falseWid1[ numElem1 ];
double falseNrrwAve1;
double falseWidAve1;

const int numElem2 =  ( numSlit2 + 1 ) * 5;
unsigned long falseNrrw2[ numElem2 ];
unsigned long falseWid2[ numElem2 ];
double falseNrrwAve2;
double falseWidAve2;

double slitThd1;
double slitThd2;

void setup() {
        Serial.begin(2000000);
        pinMode(PIN_ENC_1, INPUT);
        pinMode(PIN_ENC_2, INPUT);
        pinMode(PIN_LED, OUTPUT);

        encVal1_prv = digitalRead(PIN_ENC_1);
        encVal2_prv = digitalRead(PIN_ENC_2);

        slitCount1 = 0;
        slitCount2 = 0;

        initBool = true;
        digitalWrite(PIN_LED, LOW);
}

void loop() {
        // get a BUSY flag from a motor driver.
        // now, we use a stepping motor driven by L6470 and be waiting end of the acceleration.
        while( !( digitalRead(PIN_BUSY_1) && digitalRead(PIN_BUSY_2)) ){ 
                slitCount1 = 0;
                slitCount2 = 0;
                initBool = true;       
                falseSrt1 = micros();
                falseSrt2 = micros();
                
                for( int i = 0; i < numElem1; i++ ){
                        falseNrrw1[ i ] = 0;
                        falseWid1[ i ] = 0;
                }
                for( int i = 0; i < numElem2; i++ ){
                        falseNrrw2[ i ] = 0;
                        falseWid2[ i ] = 0;
                }
        }  

        // get threshold of the magnification between wide slit and thin slit.
        if( initBool == true ){
                //slitThd = getSlitThd(7, numSlit);

                // モータ①のキャリブレーション
                while( slitCount1 <= numElem1 ){        
                        // get the current states of photo interrupter.
                        encVal1_cr = digitalRead(pin_enc_1); // in the slit area = false,  except area = true;
        
                        // get threshold of the magnification between wide slit and thin slit.
                        if (encVal1_cr != encVal1_prv){
                                if( encVal1_cr ==  false){ // start slit area 
                                        falseSrt1 = micros();
                                        }
                                if( encVal1_cr == true ){ // end slit area
                                        falseEnd1 = micros();
                                        falseTime1 = falseEnd1 - falseSrt1;                                       
                                        Serial.print( "false: " ); Serial.println( falseTime1 );
                                        slitCount1 += 1;
                                        if( slitCount1 == 1 ){
                                                falseNrrw1[0] = falseTime1;
                                                falseWid1[0] = falseTime1;
                                        }else if( slitCount1 <= numElem1 ){
                                                if ( falseTime1 > falseWid1[ slitCount1 -1 -1 ] * 0.8){
                                                        falseWid1[ slitCount1 -1 ] = falseTime1;
                                                        falseNrrw1[ slitCount1 -1 ] = falseNrrw1[ slitCount1 -1 - 1 ];                                                
                                                }
                                                if ( falseTime1 < falseWid1[ slitCount1 -1 -1 ] * 0.8){
                                                        falseNrrw1[ slitCount1 -1 ] = falseTime1;
                                                        falseWid1[ slitCount1 -1 ] = falseWid1[ slitCount1 -1 - 1 ];        
                                                }
                                        }else if( slitCount1 > numElem1 ){
                                               Serial.println(""); 
                                                /*
                                                Serial.println( "falseNrrw1: "  );
                                                for( int i = 0; i < numElem1; i++ ){
                                                        Serial.println( falseNrrw1[ i ] );
                                                }
                                                Serial.println( "falseWid1: " ); 
                                                for( int i = 0; i < numElem1; i++ ){
                                                        Serial.println( falseWid1[ i ] );
                                                }
                                                */
                                                unsigned long _falseNrrw1[ numElem1 - ( numSlit1 + 1 ) ]; // except first rotation
                                                unsigned long _falseWid1[ numElem1 - ( numSlit1 + 1 ) ]; // except first rotation
                                                for( int i = 0; i < numElem1 - ( numSlit1 + 1 ); i++ ){
                                                        _falseNrrw1[ i ] = falseNrrw1[ i + ( numSlit1 + 1 ) ];     
                                                        _falseWid1[ i ] = falseWid1[ i + ( numSlit1 + 1 ) ];
                                                }
                                                double falseNrrwStd1 = stdev( _falseNrrw1, numElem1 - ( numSlit1 + 1 ) ); // standard deviation
                                                double falseWidStd1 = stdev( _falseWid1, numElem1 - ( numSlit1 + 1 ) ); // standard deviation
                                                falseNrrwAve1 = exoutMean( _falseNrrw1, falseNrrwStd1, numElem1 - ( numSlit1 + 1 )); // mean except outliers
                                                falseWidAve1 = exoutMean( _falseWid1, falseWidStd1, numElem1 - ( numSlit1 + 1 )); // mean except outliers
                                                slitThd1 = sqrt( falseWidAve1 / falseNrrwAve1 );
                                                Serial.println("falseNrrwStd1: "); Serial.println( falseNrrwStd1 );
                                                Serial.println("falseNrrwAve1: "); Serial.println( falseNrrwAve1 );
                                                Serial.println("falseWidStd1: "); Serial.println( falseWidStd1 );                                       
                                                Serial.println("falseWidAve1: "); Serial.println( falseWidAve1 );
                                                Serial.println("threshold of magnification between falseWid1 and falseNrrw1: "); Serial.println( slitThd1 );
                                         }
                                }
                        }                
                        encVal1_prv = encVal1_cr;        
                }
                slitThd1 = falseWidAve1 / slitThd1;
                slitCount1 = 0;
                Serial.println("");
                
                // モータ②のキャリブレーション
                while( slitCount2 <= numElem2 ){        
                        // get the current states of photo interrupter.
                        encVal2_cr = digitalRead(pin_enc_2); // in the slit area = false,  except area = true;
        
                        // get threshold of the magnification between wide slit and thin slit.
                        if (encVal2_cr != encVal2_prv){
                                if( encVal2_cr ==  false){ // start slit area 
                                        falseSrt2 = micros();
                                        }
                                if( encVal2_cr == true ){ // end slit area
                                        falseEnd2 = micros();
                                        falseTime2 = falseEnd2 - falseSrt2;
                                        Serial.print( "false: " ); Serial.println( falseTime2 );
                                        slitCount2 += 1;
                                        if( slitCount2 == 1 ){
                                                falseNrrw2[0] = falseTime2;
                                                falseWid2[0] = falseTime2;
                                        }else if( slitCount2 <= numElem2 ){
                                                if ( falseTime2 > falseWid2[ slitCount2 -1 -1 ] * 0.8){
                                                        falseWid2[ slitCount2 -1 ] = falseTime2;
                                                        falseNrrw2[ slitCount2 -1 ] = falseNrrw2[ slitCount2 -1 - 1 ];                                                
                                                }
                                                if ( falseTime2 < falseWid2[ slitCount2 -1 -1 ] * 0.8){
                                                        falseNrrw2[ slitCount2 -1 ] = falseTime2;
                                                        falseWid2[ slitCount2 -1 ] = falseWid2[ slitCount2 -1 - 1 ];        
                                                }
                                        }else if( slitCount2 > numElem2 ){
                                                Serial.println("");
                                                /*
                                                Serial.println( "falseNrrw2: "  );
                                                for( int i = 0; i < numElem2; i++ ){
                                                        Serial.println( falseNrrw2[ i ] );
                                                }
                                                Serial.println( "falseWid2: " ); 
                                                for( int i = 0; i < numElem2; i++ ){
                                                        Serial.println( falseWid2[ i ] );
                                                }
                                                */
                                                unsigned long _falseNrrw2[ numElem2 - ( numSlit2 + 1 ) ]; // except first rotation
                                                unsigned long _falseWid2[ numElem2 - ( numSlit2 + 1 ) ]; // except first rotation
                                                for( int i = 0; i < numElem2 - ( numSlit2 + 1 ); i++ ){
                                                        _falseNrrw2[ i ] = falseNrrw2[ i + ( numSlit2 + 1 ) ];     
                                                        _falseWid2[ i ] = falseWid2[ i + ( numSlit2 + 1 ) ];
                                                }
                                                double falseNrrwStd2 = stdev( _falseNrrw2, numElem2 - ( numSlit2 + 1 ) ); // standard deviation
                                                double falseWidStd2 = stdev( _falseWid2, numElem2 - ( numSlit2 + 1 ) ); // standard deviation
                                                falseNrrwAve2 = exoutMean( _falseNrrw2, falseNrrwStd2, numElem2 - ( numSlit2 + 1 )); // mean except outliers
                                                falseWidAve2 = exoutMean( _falseWid2, falseWidStd2, numElem2 - ( numSlit2 + 1 )); // mean except outliers
                                                slitThd2 = sqrt( falseWidAve2 / falseNrrwAve2 );
                                                Serial.println("falseNrrwStd2: "); Serial.println( falseNrrwStd2 );
                                                Serial.println("falseNrrwAve2: "); Serial.println( falseNrrwAve2 );
                                                Serial.println("falseWidStd2: "); Serial.println( falseWidStd2 );                                       
                                                Serial.println("falseWidAve2: "); Serial.println( falseWidAve2 );
                                                Serial.println("threshold of magnification between falseWid2 and falseNrrw2: "); Serial.println( slitThd2 );
                                         }
                                }
                        }                
                        encVal2_prv = encVal2_cr;        
                }
                slitThd2 = falseWidAve2 / slitThd2;
                slitCount2 = 0;

                initBool = false;
        }

        // get the current states of photo interrupter.
        encVal1_cr = digitalRead(PIN_ENC_1); // in the slit area = false,  except area = true;
        encVal2_cr = digitalRead(PIN_ENC_2); // in the slit area = false,  except area = true;
        
        // 
        if( initBool == false ){
                if( encVal1_cr != encVal1_prv ){
                        if ( encVal1_cr == false ){
                                falseSrt1 = micros();
                        }
                        if( encVal1_cr == true ){
                                falseEnd1 = micros();
                                falseTime1 = falseEnd1 - falseSrt1;
                                if( falseTime1  > slitThd1 ){
                                        slitCount1 = 0;        
                                }else{
                                        slitCount1 += 1;
                                }                        
                        }
                }

                if( encVal2_cr != encVal2_prv ){
                        if ( encVal2_cr == false ){
                                falseSrt2 = micros();
                        }
                        if( encVal2_cr == true ){
                                falseEnd2 = micros();
                                falseTime2 = falseEnd2 - falseSrt2;
                                if( falseTime2  > slitThd2 ){
                                        slitCount2 = 0;        
                                }else{
                                        slitCount2 += 1;
                                }                        
                        }
                }
        }
        
        Serial.print( "slit1: " ); Serial.print( slitCount1 ); Serial.print( ", " );
        Serial.print( "slit2: " ); Serial.println( slitCount2 );
        encVal1_prv = encVal1_cr;
        encVal2_prv = encVal2_cr;
}

double getSlitThd( const int pin_enc, const int numSlit ){ 
        boolean encVal_cr;
        boolean encVal_prv;
        
        int slitCount;
        unsigned long falseSrt;
        unsigned long falseEnd;
        unsigned long falseTime;
        
        const int numRot = 3;
        const int numElem =  ( numSlit + 1 ) * numRot;
        unsigned long falseNrrw[ numElem ];
        unsigned long falseWid[ numElem ];
        double falseNrrwAve;
        double falseWidAve;
        double slitThd;                
        
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
                                        double falseNrrwStd = stdev( _falseNrrw, numElem - ( numSlit + 1 ) ); // standard deviation
                                        double falseWidStd = stdev( _falseWid, numElem - ( numSlit + 1 ) ); // standard deviation
                                        falseNrrwAve = exoutMean( _falseNrrw, falseNrrwStd, numElem - ( numSlit + 1 )); // mean except outliers
                                        falseWidAve = exoutMean( _falseWid, falseWidStd, numElem - ( numSlit + 1 )); // mean except outliers
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
        slitCount = 0;
        return slitThd = falseWidAve / slitThd;
}

double mean( unsigned long x[], int xlength ){
        unsigned long sum = 0;
        double ave = 0;
        for(int i = 0; i < xlength ; i++ ){
                sum += x[ i ];
        }
        return ave = (double)sum / xlength;
}

double stdev( unsigned long x[], int xlength ){
        double ave = mean( x, xlength );
        double var = 0;
        double std = 0;
        for( int i = 0; i < xlength; i++ ){
                var += ( x[ i ] - ave ) * ( x[ i ] - ave );
        }
        var = var / xlength;
        return std = sqrt(var);
}

double exoutMean ( unsigned long x[], double xstdev, int xlength ){
        // except outliers mean
        unsigned long sum = 0;
        double ave = mean( x, xlength );
        // double exoutAve = 0;
        int n = 0;
        for( int i = 0; i < xlength; i++ ){
                if( abs( (double)x[ i ] - ave ) <= 2*xstdev ){
                        sum += x[ i ];
                        n += 1;                     
                }
        }
        return ave = (double)sum / n;
}
