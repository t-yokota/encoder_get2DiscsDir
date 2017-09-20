double getSlitThreshold( const int pin_enc, const int numSlit ){
        Serial.println("get threshold of the wide slit and narrow slit.");
        boolean encVal_cr;
        boolean encVal_prv;
        unsigned int slitCount;
        unsigned int elemCount;
        const int initRot = 3;
        const int numElem =  ( numSlit + 1 ) * initRot;
        unsigned long falseSrt;
        unsigned long falseEnd;
        unsigned long falseTime;
        unsigned long falseNrrw[ numElem ];
        unsigned long falseWid[ numElem ];
        double falseNrrwStd;
        double falseWidStd;
        double falseNrrwAve;
        double falseWidAve;
        double slitThd;
        
        slitCount = 0;
        elemCount = 0;
        for( int i = 0; i < numElem; i++ ){
                falseNrrw[ i ] = 0;
                falseWid[ i ] = 0;
        }
        
        falseSrt = micros();
        while( slitCount <= numElem + (numSlit+1) ){
                encVal_cr = digitalRead(pin_enc); // in the slit area = false,  except area = true;
                if (encVal_cr != encVal_prv){
                        if( encVal_cr ==  false){ // start slit area
                                falseSrt = micros();
                        }
                        if( encVal_cr == true ){ // end slit area
                                falseEnd = micros();
                                falseTime = falseEnd - falseSrt;
                                Serial.print( "false: " ); Serial.println( falseTime ); 
                                slitCount += 1;
                                if( slitCount <= numSlit+1 ){
                                        // except first rotation 
                                }else if( slitCount == 1 + (numSlit+1) ){
                                        falseNrrw[0] = falseTime;
                                        falseWid[0] = falseTime;
                                }else if( slitCount <= numElem + (numSlit+1) ){
                                        elemCount  += 1;
                                        if ( falseTime > falseWid[ elemCount -1 ] * 0.8){
                                                falseWid[ elemCount ] = falseTime;
                                                falseNrrw[ elemCount ] = falseNrrw[ elemCount - 1 ];                                                
                                        }
                                        if ( falseTime < falseWid[ elemCount -1 ] * 0.8){
                                                falseNrrw[ elemCount ] = falseTime;                                                        
                                                falseWid[ elemCount ] = falseWid[ elemCount - 1 ];        
                                        }
                                }else if( slitCount > numElem + (numSlit+1) ){
                                        Serial.println("");
                                        Serial.println( "falseNrrw: "  );
                                        for( int i = 0; i < numElem; i++ ){
                                                delay(10);
                                                Serial.println( falseNrrw[ i ] );
                                        }
                                        Serial.println( "falseWid: " ); 
                                        for( int i = 0; i < numElem; i++ ){
                                                delay(10);
                                                Serial.println( falseWid[ i ] );
                                        }
                                        falseNrrwStd = stdev( falseNrrw, numElem ); // standard deviation
                                        falseWidStd = stdev( falseWid, numElem ); // standard deviation
                                        falseNrrwAve = exoutMean( falseNrrw, falseNrrwStd, numElem ); // mean except outliers
                                        falseWidAve = exoutMean( falseWid, falseWidStd, numElem ); // mean except outliers
                                        slitThd = sqrt( falseWidAve / falseNrrwAve );
                                        Serial.println("falseNrrwStd: "); Serial.println( falseNrrwStd );
                                        Serial.println("falseNrrwAve: "); Serial.println( falseNrrwAve );
                                        Serial.println("falseWidStd: "); Serial.println( falseWidStd );
                                        Serial.println("falseWidAve: "); Serial.println( falseWidAve );
                                        Serial.println("threshold of magnification between falseWid and falseNrrw: "); Serial.println( slitThd );
                                        Serial.println("");
                                }
                        }
                }
                encVal_prv = encVal_cr;
        }
        return slitThd = falseWidAve / slitThd;
}

int getEncState( struct encState *enc, const double slitThd ){
        if( enc->encVal_cr != enc->encVal_prv ){
                if ( enc->encVal_cr == false ){
                        enc->falseSrt = micros();
                }
                if( enc->encVal_cr == true ){
                        enc->falseEnd = micros();
                        unsigned long falseTime = enc->falseEnd - enc->falseSrt;
                        //Serial.println(falseTime);
                        if( falseTime  > slitThd ){
                                enc->slitCount = 0;
                        }else{
                                enc->slitCount += 1;
                        }
                }
        }
        return 0;
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

double exoutMean ( unsigned long x[], double xstdev, long xlength ){
        // except outliers mean
        double ave = mean( x, xlength );
        unsigned long exoutSum = 0;
        double exoutAve = 0;
        int n = 0;
        for( int i = 0; i < xlength; i++ ){
                if( abs((double)x[ i ] - ave) <= 2*xstdev ){
                        exoutSum += x[ i ];
                        n += 1;                     
                }
        }
        return exoutAve = (double)exoutSum / n;
}
