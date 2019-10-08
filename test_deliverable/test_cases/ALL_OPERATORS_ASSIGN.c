int all_ass_fun(){
    int a = 10;
    int b = 15;
    int c = 3;
    int d = 100;

    a -= c;
    b *= c;
    c += d;
    d /= a;

    return a+b+c+d;
}