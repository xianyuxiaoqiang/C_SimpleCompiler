int gi1;
FUNC2(){
    int d;
    int f;
    int g;
    int h;
    h = -123;
    h = h * 2;
    if( h > 0 ){
        h = 9999;
    }
    if( h < 0 ){
        h = h * (-1);
    }
    d = 1;
    f = d;
    g = f+f+f+f+f+f;
    if( g == g ){
        g = 1024;
    }
    if( g != g ){
        g = 9999;
    }
    if( g > 1023 ){
        g = 1023;
    }
    if( g < 100 ){
        g = 9999;
    }
    if( g >= g ){
        g = 1022;
    }
    if( g <= g - 1 ){
        g = 9999;
    }
    for( f = 0; f < 10; f = f + 1 ){
        d = d + 1;
    }
    while( f ){
        d = d + 1;
        if( d == 100 ){
            f = 0;
        }
    }
}
int gi2;
ff(){
    int a;
    int b;
    int c;
    a = 1;
    b = 2;
    c = a + b * b / (b - a);
    for( a = 0; a < 10; a = a + 1 ){
        c = c+a;
   }
    a = 1;
    while( a <= 10 ){
        c = c + a;
        a = a + 1;
    }
    if( a >= 10 ){
        if( b == 2 ){
            if( c > 0 ){
                c = ( c + a * (c - b) );
            }
        }
    }
    FUNC2();
}
int gi3;
main() {
    gi1 = 1;
    gi2 = gi1 + 1;
    gi3 = gi2 * 2;
    ff();
    if( gi2 == ( gi2 - gi1 * gi3 ) ){
        gi2= 1;
    }
    gi3 = (( gi2 ) * (( gi1 )) + gi3 / gi1-gi2)*gi1 + 100;
}
