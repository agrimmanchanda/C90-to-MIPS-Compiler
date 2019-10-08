int rec_fun(int x){
    
    if (x > 1){
        return x*rec_fun(x-1);
    }
    else{
        return x;
    }
}