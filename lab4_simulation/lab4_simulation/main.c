#include "dsk6713_aic23.h"

#define DSK6713_AIC23_INPUT_MIC 0x0015
#define DSK6713_AIC23_INPUT_LINEIN 0x0011

#include <stdio.h>
#include <stdlib.h>

Uint32 fs = DSK6713_AIC23_FREQ_8KHZ; // 1
Uint16 inputsource = DSK6713_AIC23_INPUT_LINEIN; // 0x011

#define SIMULATION


#define ALPHA .02f
#define A1 .98f

#define WORD_COUNT 5
#define SAMPLE_RATE 8000.0f

#define MAG_THRESHOLD 200
#define TIME_THRESHOLD 2000

#define P 3
/*
 
 P=2 accuracy 86%
 p=3 88.8%
 
 */

#define NUM_USERS 3

struct coef{
    float val[P];
};
typedef struct coef Coef;

struct r{
    long val[P];
};
typedef struct r r_struct;

struct R{
    long val[P][P];
};
typedef struct R R_struct;

struct matrix{
    float val[P][P];
};
typedef struct matrix Matrix;

int abs(int x){
    if( x < 0 )
        return -x;
    return x;
}


void init_rR(r_struct* r, R_struct* R){
    int i,j;
    for(i = 0; i < P; ++i){
        r->val[i] = 0;
        for(j = 0; j < P; ++j){
            R->val[i][j] = 0;
        }
    }
}

void summ_rR(r_struct* r, R_struct* R, short* x){
    int i,j;
    for(i = 0; i < P; ++i){
        r->val[i] += x[0]*x[i+1];
        for(j = 0; j < P; ++j){
            R->val[i][j] += x[i+1]*x[j+1];
        }
    }
}

void Matrix_inverse(Matrix* m, Matrix* inv) {
    
    int i, j, k;
    float temp;
    
    for(i = 0; i < P; ++i)
        for(j = 0; j < P; ++j)
            if(i == j)
                inv->val[i][j] = 1;
            else
                inv->val[i][j] = 0;
    
    for(k = 0; k < P; ++k)
    {
        temp = m->val[k][k];
        for(j = 0; j < P; j++)
        {
            m->val[k][j] /= temp;
            inv->val[k][j] /= temp;
        }
        for(i = 0; i < P; i++)
        {
            temp = m->val[i][k];
            for(j = 0; j < P; j++)
            {
                if(i == k)
                    break;
                m->val[i][j] -= m->val[k][j] * temp;
                inv->val[i][j] -= inv->val[k][j] * temp;
            }
        }
    }
    
}

void matrix_mul_coef(const Matrix* m, const Coef* c, Coef* dest)
{
    int i,j;
    for(i = 0; i < P; ++i)
    {
        dest->val[i] = 0;
        for(j = 0; j < P; ++j)
        {
            dest->val[i] += m->val[i][j] * c->val[j];
        }
    }
}

void findCoeff(Coef* coef, const r_struct* r, const R_struct* R){
    Matrix R_matrix, inverse;
    Coef r_coef;
    int i,j;
    // convert the int based structures into float based
    for(i = 0; i < P; ++i)
    {
        r_coef.val[i] = r->val[i];
        for(j = 0; j < P; ++j)
        {
            R_matrix.val[i][j] = R->val[i][j];
            inverse.val[i][j] = 0;
        }
    }
    // get the inverse
    Matrix_inverse(&R_matrix, &inverse);
    // multiply
    matrix_mul_coef(&inverse, &r_coef, coef);
}

void getCoeffs(Coef* coefs, int count)
{
    unsigned long sample_count = 0;
    short x[P+1];
    float moving_average = 0;
    short state = 0;
    unsigned long start;
    int word_count = 0;
    r_struct r;
    R_struct R;
    
    fflush(stdout);
    
    while(1) {
        
        short sample = input_left_sample();
        int i; // shift our x's
        for(i = P; i >= 0; --i)
            x[i] = x[i-1];
        x[0] = sample; //save current
        
        // exponential moving average with a very small alpha
        moving_average = abs(sample)*ALPHA + moving_average*A1;
        
        // state machine
        switch(state){
            case 0:{
                if( moving_average >= MAG_THRESHOLD ){
                    // our average value is greater than the threshold, set starting time, change state
                    state = 1;
                    start = sample_count;
                    
                    // initialize our r and R, then start accumulating them
                    init_rR(&r,&R);
                    summ_rR(&r, &R, x);
                }
                    break;
            }
            default:{
                // if exceding the threshold accumulate our R and r
                if( moving_average >= MAG_THRESHOLD ){
                    // just accumulate our rR's
                    summ_rR(&r, &R, x);
                    
                    break;
                }
                
                // if not change state and find duration
                state = 0;
                int duration = sample_count - start;
                
                // check if this was long enough to be considered a word
                if( duration > TIME_THRESHOLD ){
                    
                    // if so save it
                    int ms = duration*1000/SAMPLE_RATE;
                    printf("Word duration in samples: %i, in time: %ims. Coefs: ", duration,ms);
                    
                    
                    findCoeff(coefs+word_count, &r, &R);
                    int i;
                    for(i = 0; i< P; ++i)
                        printf("%f ",coefs[word_count].val[i]);
                    
                    printf("\n");
                    fflush(stdout);
 
                    if(++word_count == count){
                        printf("Got Coeffs for %d words!\n",count);
                        fflush(stdout);
                        return;
                    }
                } //if( duration > TIME_THRESHOLD ){
                
            } // default:{
        }// switch(state)

        ++sample_count;
    } // while(1)
} //void getCoeffs(Coef* coefs, int count)


void getProfile(const Coef* words, Coef* mean, Matrix* cov, int word_count){
    int i,j,count;
    Coef temp;
    for(i = 0; i < P; ++i){
        mean->val[i] = 0;
        for(j = 0; j < P; ++j){
            cov->val[i][j] = 0;
        }
    }
    for(count = 0; count < word_count; ++count)
    {
        for(i = 0; i < P; ++i){
            mean->val[i] += words[count].val[i];
        }
    }
    printf("Mean: \n");
    for(i = 0; i < P; ++i){
        mean->val[i] /= word_count;
        printf("%f\t", mean->val[i]);
    }
    
    for(count = 0; count < word_count; ++count)
    {
        for(i = 0; i < P; ++i){
            temp.val[i] = words[count].val[i] - mean->val[i];
        }
        for(i = 0; i < P; ++i){
            for(j = 0; j < P; ++j){
                cov->val[i][j] += temp.val[i]*temp.val[j];
            }
        }
    }
    printf("\nCov: \n");
    for(i = 0; i < P; ++i){
        for(j = 0; j < P; ++j){
            cov->val[i][j] /= word_count;
            printf("%f\t", cov->val[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

float maholanobis_distance(const Coef* word, const Coef* mean, const Matrix* cov){
    Coef mid, temp;
    Matrix cov_temp, inverse;
    float ans = 0;
    int i,j;
    for(i = 0; i < P; ++i){
        mid.val[i] = word->val[i] - mean->val[i];
        for(j = 0; j < P; ++j){
            inverse.val[i][j] = 0;
            cov_temp.val[i][j] = cov->val[i][j];
        }
    }
    Matrix_inverse(&cov_temp, &inverse);
    
    for(i = 0; i < P; ++i){
        temp.val[i] = 0;
        for(j = 0; j < P; ++j){
            temp.val[i] += mid.val[j]*inverse.val[i][j];
        }
    }
    
    for(i = 0; i < P; ++i){
        ans += temp.val[i]*mid.val[i];
    }
    
    return ans;
}

int findUser(const Coef* word, const Coef* means, const Matrix* covs){
    float best_dist = 99999999999;
    int best_user = 0;
    int i;
    for(i = 0;i < NUM_USERS; ++i){
        float result = maholanobis_distance(word, means+i, covs+i);
        if(result < best_dist){
            best_dist = result;
            best_user = i;
        }
    }
    return best_user;
}


#define TEST_SIZE 15

void main()
{
    comm_poll();
    
    Coef words[TEST_SIZE];
    Coef means[NUM_USERS];
    Matrix cov[NUM_USERS];
    
    
    while(1) {
        
        printf("******* SPEAKER RECOGNITION *******\n");
        printf("1 - Training\n");
        printf("2 - Testing\n");
        printf("Please enter your choice:\n");
        
        int choice = 0;
        
        scanf("%d",&choice);
        
        if(choice == 1) {
            
            int user = 0;
            printf("For which user do you want to train (1-%d): \n",NUM_USERS);
            scanf("%d",&user);
#ifdef SIMULATION
            switch(user){
                case 1: load("user1_train_8k.txt"); break;
                case 2: load("user2_train_8k.txt"); break;
                default: load("user3_train_8k.txt");
            }
#else
            int ready = 0;
            do {
                printf("Please provide the training sound, enter 1 when it is ready \n");
                scanf("%d",&ready);
            } while(ready != 1);
#endif
            printf("Training sound is sampling...\n");
            
            getCoeffs(words, TEST_SIZE);
            getProfile(words, means+user-1, cov+user-1, TEST_SIZE);
            
        } else if(choice == 2) {

            int user = 0;
            printf("Which user is speaking? (1-%d): ",NUM_USERS);
            scanf("%d",&user);
#ifdef SIMULATION
            switch(user){
                case 1: load("user1_train_8k.txt"); break;
                case 2: load("user2_train_8k.txt"); break;
                default: load("user3_train_8k.txt");
            }
#else
            int ready = 0;
            do {
                printf("Please provide the test sound, enter 1 when it is ready \n");
                scanf("%d",&ready);
            } while(ready != 1);
#endif
            printf("Test sound is sampling... \n");
            fflush(stdout);
            
            getCoeffs(words, TEST_SIZE);
            
            printf("Finding user... \n");
            
            int i;
            int correct = 0;
            for(i = 0; i < TEST_SIZE; ++i){
                int result = findUser(words+i,means,cov)+1;
                printf("Word %d: user: %d\n",i,result);
                if(result == user)
                    ++correct;
            }
            printf("\nCorrect %d out of %d\n", correct, TEST_SIZE);
        }
    }
    
}


