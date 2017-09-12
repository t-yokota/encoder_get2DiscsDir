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
