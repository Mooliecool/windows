/*=============================================================================
**
** Source: test1.c
**
** Purpose: Test to ensure that srand provide random
**          number to rand. Also make sure that rand result from a 
**          srand with seed 1 and no call to srand are the same.
**
** Dependencies: PAL_Initialize
**               PAL_Terminate
**				 Fail
**               srand()
** 
** 
**  Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
** 
**  The use and distribution terms for this software are contained in the file
**  named license.txt, which can be found in the root of this distribution.
**  By using this software in any fashion, you are agreeing to be bound by the
**  terms of this license.
** 
**  You must not remove this notice, or any other, from this software.
** 
**
**===========================================================================*/

#include <palsuite.h>


int __cdecl main(int argc, char **argv)
{   
    int RandNumber[10];
    int TempRandNumber;
    int i;
    int SRAND_SEED;
    int SRAND_REINIT  =  1;

    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if (PAL_Initialize(argc, argv))
    {
       return FAIL;
    }
    
    SRAND_SEED = time(NULL);
    
    /* does not initialize srand and call rand. */
    for (i=0; i<10; i++)
    {
        /* keep the value in a array            */
        RandNumber[i]=rand();
        if (RandNumber[i] < 0 || RandNumber[i] > RAND_MAX)
        {
            Fail("1) ERROR: random generated an invalid value: %d", RandNumber[i]);
        }
    }


    /*   initialize random generator            */
    srand(SRAND_SEED);


    /* choose 10 numbers with a different seed. 
       the numbers should be different than
       those the previously generated one       */
    for(i = 0; i < 10; i++)
    {
        TempRandNumber=rand();      
        if(RandNumber[i]==TempRandNumber)
        {
          Fail("ERROR: rand should return the different value when srand "
               "is initialized with different value");
        } 
        if (TempRandNumber < 0 || TempRandNumber > RAND_MAX)
        {
            Fail("2) ERROR: random generated an invalid value: %d", TempRandNumber);
        }
    }
    


    /* renitialize the srand with 1 */
    srand(SRAND_REINIT);



    /* choose 10 numbers with seed 1,
       the number should be the same as those we kept in the array. */
    for( i = 0;   i < 10;i++ )
    {
        /* pick the random number*/
        TempRandNumber=rand();
        /* test if it is the same number generated in the first sequences*/
        if(RandNumber[i]!=TempRandNumber)
        {
            Fail ("ERROR: rand should return the same value when srand "
                  "is initialized with 1 or not initialized at all");
        } 
        if (TempRandNumber < 0 || TempRandNumber > RAND_MAX)
        {
            Fail("3) ERROR: random generated an invalid value: %d", TempRandNumber);
        }
    }


    PAL_Terminate();
    return PASS;
}
