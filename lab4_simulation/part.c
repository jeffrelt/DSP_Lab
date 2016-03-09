void getProfile(const Coef* coef, Coef* mean, Matrix* cov){
    
    // Find the mean
    float sum  = 0;
    for (i = 0; i < word_count; ++i)
        sum += coefs[i];
    
    mean = sum/word_count;
    
    // Find covariance
    Matrix* cov_sum = 0;
    for number of words do
        cov_sum += (coef[i] - mean)* transpose of (coef[i] - mean);
        
    cov = cov_sum/word_count;   
}
