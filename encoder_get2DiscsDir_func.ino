double getSlitThreshold( const int pin_enc, struct encoderValues *enc , const int initRot){
        Serial.println("get threshold of the wide slit and narrow slit.");
        unsigned int elemCount;
        const int numElem =  ( enc->numSlit + 1 ) * initRot;
        unsigned long falseNrrw[ numElem ];
        unsigned long falseWid[ numElem ];
        double falseNrrwStd;
        double falseWidStd;
       
        enc->slitCount = 0;
        elemCount = 0;
        for( int i = 0; i < numElem; i++ ){
                falseNrrw[ i ] = 0;
                falseWid[ i ] = 0;
        }
        
        enc->falseSrt = micros();
        while( enc->slitCount <= numElem + (enc->numSlit+1) ){
                enc->encVal_cr = digitalRead(pin_enc); // in the slit area = false,  except area = true;
                if (enc->encVal_cr != enc->encVal_prv){
                        if( enc->encVal_cr ==  false){ // start slit area
                                enc->falseSrt = micros();
                        }
                        if( enc->encVal_cr == true ){ // end slit area
                                enc->falseEnd = micros();
                                unsigned long falseTime = enc->falseEnd - enc->falseSrt;
                                Serial.print( "false: " ); Serial.println( falseTime ); 
                                enc->slitCount += 1;
                                if( enc->slitCount <= enc->numSlit+1 ){
                                        // except first rotation 
                                }else if( enc->slitCount == 1 + (enc->numSlit+1) ){
                                        falseNrrw[0] = falseTime;
                                        falseWid[0] = falseTime;
                                }else if( enc->slitCount <= numElem + (enc->numSlit+1) ){
                                        elemCount  += 1;
                                        if ( falseTime > falseWid[ elemCount -1 ] * 0.8){
                                                falseWid[ elemCount ] = falseTime;
                                                falseNrrw[ elemCount ] = falseNrrw[ elemCount - 1 ];                                                
                                        }
                                        if ( falseTime < falseWid[ elemCount -1 ] * 0.8){
                                                falseNrrw[ elemCount ] = falseTime;                                                        
                                                falseWid[ elemCount ] = falseWid[ elemCount - 1 ];        
                                        }
                                }else if( enc->slitCount > numElem + (enc->numSlit+1) ){
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
                                        enc->falseNrrwAve = exoutMean( falseNrrw, falseNrrwStd, numElem ); // mean except outliers
                                        enc->falseWidAve = exoutMean( falseWid, falseWidStd, numElem ); // mean except outliers
                                        enc->slitThd = sqrt( enc->falseWidAve / enc->falseNrrwAve );
                                        Serial.println("falseNrrwStd: "); Serial.println( falseNrrwStd );
                                        Serial.println("falseNrrwAve: "); Serial.println( enc->falseNrrwAve );
                                        Serial.println("falseWidStd: "); Serial.println( falseWidStd );
                                        Serial.println("falseWidAve: "); Serial.println( enc->falseWidAve );
                                        Serial.println("threshold of magnification between falseWid and falseNrrw: "); Serial.println( enc->slitThd );
                                        Serial.println("");
                                }
                        }
                }
                enc->encVal_prv = enc->encVal_cr;
        }
        enc->slitCount = 0;
        return enc->slitThd = enc->falseWidAve / enc->slitThd;
}

void getEncState( struct encoderValues *enc ){
        if( enc->encVal_cr != enc->encVal_prv ){
                if ( enc->encVal_cr == false ){
                        enc->falseSrt = micros();
                }
                if( enc->encVal_cr == true ){
                        enc->falseEnd = micros();
                        unsigned long falseTime = enc->falseEnd - enc->falseSrt;
                        //Serial.println(falseTime);
                        if( falseTime  > enc->slitThd ){
                                enc->slitCount = 0;
                        }else{
                                enc->slitCount += 1;
                        }
                }
        }
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
