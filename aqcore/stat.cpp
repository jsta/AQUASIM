////////////////////////////////   stat.cpp   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    04.10.99    Peter Reichert
// revisions:   24.12.99    Peter Reichert    mv_uniform_randsamp added
//              04.01.00    Peter Reichert    lognormal_pdf added
//              05.01.00    Peter Reichert    calcfract added
//              13.02.00    Peter Reichert    lognormal_cdf, lognormal_inv_cdf
//                                            lognormal_randnum added
//              04.03.00    Peter Reichert    DISTRIBUTION added
//              16.04.00    Peter Reichert    gamma distribution added
//              01.11.02    Peter Reichert    delta distribution added
//              11.05.03    Peter Reichert    logpdf functions added
//              17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////
//
// "stat" implements statistical functions
// =======================================
//
//////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include "stat.h"
#include "mdi.h"
#include "sort.h"
#include "intpol.h"
#include "fortran.h"
#include "linpack.h"
#include "gamma.h"

//////////////////////////////////////////////////////////////////////////////

const REAL pihalf = 1.570796327;

static CARDINAL primes[] = {   2,   3,   5,   7,  11,  13,  17,  19,  23,  29,
                              31,  37,  41,  43,  47,  53,  59,  61,  67,  71,
                              73,  79,  83,  89,  97, 101, 103, 107, 109, 113,
                             127, 131, 137, 139, 149, 151, 157, 163, 167, 173,
                             179, 181, 191, 193, 197, 199, 211, 223, 227, 229,
                             233, 239, 241, 251, 257, 263, 269, 271, 277, 281,
                             283, 293, 307, 311, 313, 317, 331, 337, 347, 349,
                             353, 359, 367, 373, 379, 383, 389, 397, 401, 409,
                             419, 421, 431, 433, 439, 443, 449, 457, 461, 463,
                             467, 479, 487, 491, 499, 503, 509, 521, 523, 541,
                             547, 557, 563, 569, 571, 577, 587, 593, 599, 601,
                             607, 613, 617, 619, 631, 641, 643, 647, 653, 659,
                             661, 673, 677, 683, 691, 701, 709, 719, 727, 733,
                             739, 743, 751, 757, 761, 769, 773, 787, 797, 809,
                             811, 821, 823, 827, 829, 839, 853, 857, 859, 863,
                             877, 881, 883, 887, 907, 911, 919, 929, 937, 941,
                             947, 953, 967, 971, 977, 983, 991, 997,1009,1013,
                            1019,1021,1031,1033,1039,1049,1051,1061,1063,1069,
                            1087,1091,1093,1097,1103,1109,1117,1123,1129,1151,
                            1153,1163,1171,1181,1187,1193,1201,1213,1217,1223,
                            1229,1231,1237,1249,1259,1277,1279,1283,1289,1291,
                            1297,1301,1303,1307,1319,1321,1327,1361,1367,1373,
                            1381,1399,1409,1423,1427,1429,1433,1439,1447,1451,
                            1453,1459,1471,1481,1483,1487,1489,1493,1499,1511,
                            1523,1531,1543,1549,1553,1559,1567,1571,1579,1583,
                            1597,1601,1607,1609,1613,1619,1621,1627,1637,1657,
                            1663,1667,1669,1693,1697,1699,1709,1721,1723,1733,
                            1741,1747,1753,1759,1777,1783,1787,1789,1801,1811,
                            1823,1831,1847,1861,1867,1871,1873,1877,1879,1889,
                            1901,1907,1913,1931,1933,1949,1951,1973,1979,1987,
                            1993,1997,1999,2003,2011,2017,2027,2029,2039,2053,
                            2063,2069,2081,2083,2087,2089,2099,2111,2113,2129,
                            2131,2137,2141,2143,2153,2161,2179,2203,2207,2213,
                            2221,2237,2239,2243,2251,2267,2269,2273,2281,2287,
                            2293,2297,2309,2311,2333,2339,2341,2347,2351,2357,
                            2371,2377,2381,2383,2389,2393,2399,2411,2417,2423,
                            2437,2441,2447,2459,2467,2473,2477,2503,2521,2531,
                            2539,2543,2549,2551,2557,2579,2591,2593,2609,2617,
                            2621,2633,2647,2657,2659,2663,2671,2677,2683,2687,
                            2689,2693,2699,2707,2711,2713,2719,2729,2731,2741,
                            2749,2753,2767,2777,2789,2791,2797,2801,2803,2819,
                            2833,2837,2843,2851,2857,2861,2879,2887,2897,2903,
                            2909,2917,2927,2939,2953,2957,2963,2969,2971,2999,
                            3001,3011,3019,3023,3037,3041,3049,3061,3067,3079,
                            3083,3089,3109,3119,3121,3137,3163,3167,3169,3181,
                            3187,3191,3203,3209,3217,3221,3229,3251,3253,3257,
                            3259,3271,3299,3301,3307,3313,3319,3323,3329,3331,
                            3343,3347,3359,3361,3371,3373,3389,3391,3407,3413,
                            3433,3449,3457,3461,3463,3467,3469,3491,3499,3511,
                            3517,3527,3529,3533,3539,3541,3547,3557,3559,3571,
                            3581,3583,3593,3607,3613,3617,3623,3631,3637,3643,
                            3659,3671,3673,3677,3691,3697,3701,3709,3719,3727,
                            3733,3739,3761,3767,3769,3779,3793,3797,3803,3821,
                            3823,3833,3847,3851,3853,3863,3877,3881,3889,3907,
                            3911,3917,3919,3923,3929,3931,3943,3947,3967,3989,
                            4001,4003,4007,4013,4019,4021,4027,4049,4051,4057,
                            4073,4079,4091,4093,4099,4111,4127,4129,4133,4139,
                            4153,4157,4159,4177,4201,4211,4217,4219,4229,4231,
                            4241,4243,4253,4259,4261,4271,4273,4283,4289,4297,
                            4327,4337,4339,4349,4357,4363,4373,4391,4397,4409,
                            4421,4423,4441,4447,4451,4457,4463,4481,4483,4493,
                            4507,4513,4517,4519,4523,4547,4549,4561,4567,4583,
                            4591,4597,4603,4621,4637,4639,4643,4649,4651,4657,
                            4663,4673,4679,4691,4703,4721,4723,4729,4733,4751,
                            4759,4783,4787,4789,4793,4799,4801,4813,4817,4831,
                            4861,4871,4877,4889,4903,4909,4919,4931,4933,4937,
                            4943,4951,4957,4967,4969,4973,4987,4993,4999,5003,
                            5009,5011,5021,5023,5039,5051,5059,5077,5081,5087,
                            5099,5101,5107,5113,5119,5147,5153,5167,5171,5179,
                            5189,5197,5209,5227,5231,5233,5237,5261,5273,5279,
                            5281,5297,5303,5309,5323,5333,5347,5351,5381,5387,
                            5393,5399,5407,5413,5417,5419,5431,5437,5441,5443,
                            5449,5471,5477,5479,5483,5501,5503,5507,5519,5521,
                            5527,5531,5557,5563,5569,5573,5581,5591,5623,5639,
                            5641,5647,5651,5653,5657,5659,5669,5683,5689,5693,
                            5701,5711,5717,5737,5741,5743,5749,5779,5783,5791,
                            5801,5807,5813,5821,5827,5839,5843,5849,5851,5857,
                            5861,5867,5869,5879,5881,5897,5903,5923,5927,5939,
                            5953,5981,5987,6007,6011,6029,6037,6043,6047,6053,
                            6067,6073,6079,6089,6091,6101,6113,6121,6131,6133,
                            6143,6151,6163,6173,6197,6199,6203,6211,6217,6221,
                            6229,6247,6257,6263,6269,6271,6277,6287,6299,6301,
                            6311,6317,6323,6329,6337,6343,6353,6359,6361,6367,
                            6373,6379,6389,6397,6421,6427,6449,6451,6469,6473,
                            6481,6491,6521,6529,6547,6551,6553,6563,6569,6571,
                            6577,6581,6599,6607,6619,6637,6653,6659,6661,6673,
                            6679,6689,6691,6701,6703,6709,6719,6733,6737,6761,
                            6763,6779,6781,6791,6793,6803,6823,6827,6829,6833,
                            6841,6857,6863,6869,6871,6883,6899,6907,6911,6917,
                            6947,6949,6959,6961,6967,6971,6977,6983,6991,6997,
                            7001,7013,7019,7027,7039,7043,7057,7069,7079,7103,
                            7109,7121,7127,7129,7151,7159,7177,7187,7193,7207,
                            7211,7213,7219,7229,7237,7243,7247,7253,7283,7297,
                            7307,7309,7321,7331,7333,7349,7351,7369,7393,7411,
                            7417,7433,7451,7457,7459,7477,7481,7487,7489,7499,
                            7507,7517,7523,7529,7537,7541,7547,7549,7559,7561,
                            7573,7577,7583,7589,7591,7603,7607,7621,7639,7643,
                            7649,7669,7673,7681,7687,7691,7699,7703,7717,7723,
                            7727,7741,7753,7757,7759,7789,7793,7817,7823,7829,
                            7841,7853,7867,7873,7877,7879,7883,7901,7907,7919};

//////////////////////////////////////////////////////////////////////////////


DISTRIBUTION::DISTRIBUTION()
{
  CARDINAL i;

   dim = 0;
   mv_type    = MV_UNIFORM;
   varnames   = 0;
   uv_types   = 0;
   uv_numpars = 0;
   uv_pars    = 0;
   corr       = 0;

   num_mv_dist = 5;
   mv_diststrings = new char*[num_mv_dist];
   for ( i=0; i<num_mv_dist; i++ )
   {
      mv_diststrings[i] = new char[15];
      strcpy(mv_diststrings[i],"");
   }
   if ( MV_DELTA     >= num_mv_dist || 
        MV_UNIFORM   >= num_mv_dist || 
        MV_NORMAL    >= num_mv_dist ||
        MV_LOGNORMAL >= num_mv_dist ||
        MV_COPULA    >= num_mv_dist ) exit(1);
   strcpy(mv_diststrings[MV_DELTA]    ,"delta");
   strcpy(mv_diststrings[MV_UNIFORM]  ,"uniform");
   strcpy(mv_diststrings[MV_NORMAL]   ,"normal");
   strcpy(mv_diststrings[MV_LOGNORMAL],"lognormal");
   strcpy(mv_diststrings[MV_COPULA]   ,"copula");
   // increase num_mv_dist, if additional strings are added!

   num_uv_dist = 7;
   uv_diststrings = new char*[num_uv_dist];
   for ( i=0; i<num_uv_dist; i++ )
   {
      uv_diststrings[i] = new char[15];
      strcpy(uv_diststrings[i],"");
   }
   if ( UV_DELTA      >= num_uv_dist || 
        UV_UNIFORM    >= num_uv_dist || 
        UV_NORMAL     >= num_uv_dist ||
        UV_LOGNORMAL  >= num_uv_dist ||
        UV_GAMMA      >= num_uv_dist ||
        UV_TRIANGULAR >= num_uv_dist ||
        UV_EMPIRICAL  >= num_uv_dist ) exit(1);
   strcpy(uv_diststrings[UV_DELTA]     ,"delta");
   strcpy(uv_diststrings[UV_UNIFORM]   ,"uniform");
   strcpy(uv_diststrings[UV_NORMAL]    ,"normal");
   strcpy(uv_diststrings[UV_LOGNORMAL] ,"lognormal");
   strcpy(uv_diststrings[UV_GAMMA]     ,"gamma");
   strcpy(uv_diststrings[UV_TRIANGULAR],"triangular");
   strcpy(uv_diststrings[UV_EMPIRICAL] ,"empirical");
   // increase num_uv_dist, if additional strings are added!
}


DISTRIBUTION::~DISTRIBUTION()
{
   CARDINAL i;
   if ( varnames != 0 )
   {
      for ( i=0; i<dim; i++ )
      {
         if ( varnames[i] != 0 ) delete [] varnames[i];
      }
      delete [] varnames;
   }
   if ( uv_pars != 0 )
   {
      for ( i=0; i<dim; i++ )
      {
         if ( uv_pars[i] != 0 )  delete [] uv_pars[i];
      }
      delete [] uv_pars;
   }
   if ( uv_types != 0 )   delete [] uv_types;
   if ( uv_numpars != 0 ) delete [] uv_numpars;
   if ( corr != 0 )       delete [] corr;
   for ( i=0; i< num_uv_dist; i++ ) delete [] uv_diststrings[i];
   delete [] uv_diststrings;
   for ( i=0; i< num_mv_dist; i++ ) delete [] mv_diststrings[i];
   delete [] mv_diststrings;
}


BOOLEAN DISTRIBUTION::SetType(MV_DISTTYPE type)
{
   if ( dim > 0 ) return FALSE;
   mv_type = type;
   return TRUE;
}


BOOLEAN DISTRIBUTION::SetType(char* nametype)
{
   if ( nametype == 0 )         return FALSE;
   if ( strlen(nametype) == 0 ) return FALSE;
   CARDINAL i;
   MV_DISTTYPE type;
   BOOLEAN found = FALSE;
   for ( i=0; i<num_mv_dist; i++ )
   {
      if ( strcmp(nametype,mv_diststrings[i]) == 0 )
      {
         type = (MV_DISTTYPE)i;
         found = TRUE;
         break;
      }
   }
   if ( found == FALSE ) return FALSE;

   return SetType(type);
}


MV_DISTTYPE DISTRIBUTION::GetType()
{
   return mv_type;
}


CARDINAL DISTRIBUTION::GetDim()
{
   return dim;
}


UV_DISTTYPE* DISTRIBUTION::GetVarDists()
{
   return uv_types;
}


CARDINAL DISTRIBUTION::GetNumPars(MV_DISTTYPE type)
{
   CARDINAL num = 0;
   switch ( type )
   {
      case MV_DELTA:
         num = 1;
         break;
      case MV_UNIFORM:
         num = 2;
         break;
      case MV_NORMAL:
         num = 2;
         break;
      case MV_LOGNORMAL:
         num = 2;
         break;
      default:
         num = 0; // unknown (e.g. MV_COPULA)
   }
   return num;
}


CARDINAL DISTRIBUTION::GetNumPars(UV_DISTTYPE type)
{
   CARDINAL num = 0;
   switch ( type )
   {
      case UV_DELTA:
         num = 1;
         break;
      case UV_UNIFORM:
         num = 2;
         break;
      case UV_NORMAL:
         num = 2;
         break;
      case UV_LOGNORMAL:
         num = 2;
         break;
      case UV_GAMMA:
         num = 2;
         break;
      case UV_TRIANGULAR:
         num = 3;
         break;
      default:
         num = 0; // unknown (e.g. UV_EMPIRICAL)
   }
   return num;
}


char* DISTRIBUTION::GetVarName(CARDINAL i)
{
   return varnames[i];
}


char** DISTRIBUTION::GetVarNames()
{
   return varnames;
}


REAL* DISTRIBUTION::GetVarPars(CARDINAL i)
{
   return uv_pars[i];
}


REAL** DISTRIBUTION::GetVarPars()
{
   return uv_pars;
}


REAL* DISTRIBUTION::GetCorr()
{
   return corr;
}


BOOLEAN DISTRIBUTION::AddVar(char* name, 
                             UV_DISTTYPE type, 
                             CARDINAL numpars, 
                             REAL* pars,
                             REAL* corrs)
{
   CARDINAL i,j;
   if ( name == 0 )                               return FALSE;
   if ( strlen(name) == 0 )                       return FALSE;
   if ( corrs != 0 )
   {
      for ( i=0; i<dim; i++ ) 
      {
         if ( corrs[i] <= -1 )                    return FALSE;
         if ( corrs[i] >= +1 )                    return FALSE;
      }
   }

   char**       new_varnames   = new char*[dim+1];
   UV_DISTTYPE* new_uv_types   = new UV_DISTTYPE[dim+1];
   CARDINAL*    new_uv_numpars = new CARDINAL[dim+1];
   REAL**       new_uv_pars    = new REAL*[dim+1];
   for ( i=0; i<dim; i++ )
   {
      new_varnames[i]   = varnames[i];
      new_uv_types[i]   = uv_types[i];
      new_uv_numpars[i] = uv_numpars[i];
      new_uv_pars[i]    = uv_pars[i];
   }
   new_varnames[dim] = new char[strlen(name)+1]; 
   strcpy(new_varnames[dim],name);
   new_uv_types[dim] = type;
   new_uv_numpars[dim] = GetNumPars(type);
   if ( new_uv_numpars[dim] > 0 )
   {
      new_uv_pars[dim] = new REAL[new_uv_numpars[dim]];
      if ( numpars >= new_uv_numpars[dim] )
      {
         for ( i=0; i<new_uv_numpars[dim]; i++ )
         {
            new_uv_pars[dim][i] = pars[i];
         }
      }
   }
   BOOLEAN pars_ok = TRUE;
   switch ( type )
   {
      case UV_DELTA:
         if ( mv_type!=MV_DELTA && mv_type!=MV_COPULA ) pars_ok = FALSE;
         if ( numpars < new_uv_numpars[dim] )  // set defaults
         {
            new_uv_pars[dim][0] = 0;
         }
         break;
      case UV_UNIFORM:
         if ( mv_type!=MV_UNIFORM && mv_type!=MV_COPULA ) pars_ok = FALSE;
         if ( numpars < new_uv_numpars[dim] )  // set defaults
         {
            new_uv_pars[dim][0] = 0;
            new_uv_pars[dim][1] = 1;
         }
         if ( new_uv_pars[dim][1] <= new_uv_pars[dim][0] ) pars_ok = FALSE;
         break;
      case UV_NORMAL:
         if ( mv_type!=MV_NORMAL && mv_type!=MV_COPULA ) pars_ok = FALSE;
         if ( numpars < new_uv_numpars[dim] )  // set defaults
         {
            new_uv_pars[dim][0] = 0;
            new_uv_pars[dim][1] = 1;
         }
         if ( new_uv_pars[dim][1] <= 0 ) pars_ok = FALSE;
         break;
      case UV_LOGNORMAL:
         if ( mv_type!=MV_LOGNORMAL && mv_type!=MV_COPULA ) pars_ok = FALSE;
         if ( numpars < new_uv_numpars[dim] )  // set defaults
         {
            new_uv_pars[dim][0] = 1;
            new_uv_pars[dim][1] = 1;
         }
         if ( new_uv_pars[dim][0] <= 0 ) pars_ok = FALSE;
         if ( new_uv_pars[dim][1] <= 0 ) pars_ok = FALSE;
         break;
      case UV_GAMMA:
         if ( mv_type!=MV_COPULA ) pars_ok = FALSE;
         if ( numpars < new_uv_numpars[dim] )  // set defaults
         {
            new_uv_pars[dim][0] = 1;
            new_uv_pars[dim][1] = 1;
         }
         if ( new_uv_pars[dim][0] <= 0 ) pars_ok = FALSE;
         if ( new_uv_pars[dim][1] <= 0 ) pars_ok = FALSE;
         break;
      case UV_TRIANGULAR:
         if ( mv_type != MV_COPULA ) pars_ok = FALSE;
         if ( numpars < new_uv_numpars[dim] )  // set defaults
         {
            new_uv_pars[dim][0] = 0.5;
            new_uv_pars[dim][1] = 0.5;
            new_uv_pars[dim][2] = 0.5;
         }
         if ( new_uv_pars[dim][1] <= 0 ) pars_ok = FALSE;
         if ( new_uv_pars[dim][2] <= 0 ) pars_ok = FALSE;
         break;
      case UV_EMPIRICAL:
         if ( mv_type != MV_COPULA ) pars_ok = FALSE;
         if ( numpars == 0 )
         {
            new_uv_numpars[dim] = 5;
            new_uv_pars[dim]    = new REAL[new_uv_numpars[dim]];
            new_uv_pars[dim][0] = 2;
            new_uv_pars[dim][1] = 0; new_uv_pars[dim][2] = 0;
            new_uv_pars[dim][3] = 1; new_uv_pars[dim][4] = 1;
         }
         j = (CARDINAL) (pars[0]+0.1);
         new_uv_numpars[dim] = 2*j+1;
         new_uv_pars[dim] = new REAL[new_uv_numpars[dim]];
         if ( numpars < 5 ) pars_ok = FALSE;
         if ( pars_ok == FALSE ) break;
         if ( pars[1] != 0 ) pars_ok = FALSE;
         if ( pars[2*j-1] != 1 ) pars_ok = FALSE;
         for ( i=1; i<j; i++ )
         {
            if ( pars[2*i+1] <= pars[2*i-1] )   pars_ok = FALSE; // levels
            if ( pars[2*i+2] <= pars[2*i-2+2] ) pars_ok = FALSE; // fractiles
         }
         for ( i=0; i<new_uv_numpars[dim]; i++ )
         {
            new_uv_pars[dim][i] = pars[i];
         }
         break;
      default:
         pars_ok = FALSE;
   }
   REAL* new_corr = new REAL[(dim+1)*(dim+1)];
   for ( i=0; i<dim; i++ )         // copy old corrs
   {
      new_corr[i+i*(dim+1)] = 1;
      for ( j=0; j<i; j++ )
      {
         new_corr[i+j*(dim+1)] = corr[i+j*dim];
         new_corr[j+i*(dim+1)] = corr[i+j*dim];
      }
   }
   new_corr[dim+dim*(dim+1)] = 1;  // set defaults for new corrs
   for ( i=0; i<dim; i++ )
   {
      new_corr[i+dim*(dim+1)] = 0;
      new_corr[dim+i*(dim+1)] = 0;
   }
   if ( corrs != 0 )
   {
      for ( i=0; i<dim; i++ )
      {
         new_corr[i+dim*(dim+1)] = corrs[i];
         new_corr[dim+i*(dim+1)] = corrs[i];
      }
   }
   else // look for additional parameters
   {
      if ( numpars >= new_uv_numpars[dim]+dim )
      {
         for ( i=0; i<dim; i++ )
         {
            new_corr[i+dim*(dim+1)] = pars[new_uv_numpars[dim]+i];
            new_corr[dim+i*(dim+1)] = pars[new_uv_numpars[dim]+i];
         }
      }
   }
   if ( pars_ok == FALSE )
   {
      delete [] new_varnames[dim];
      delete [] new_varnames;
      delete [] new_uv_types;
      delete [] new_uv_numpars;
      delete [] new_uv_pars[dim];
      delete [] new_uv_pars;
      delete [] new_corr;
      return FALSE;
   }
   delete [] varnames;   varnames   = new_varnames;
   delete [] uv_types;   uv_types   = new_uv_types;
   delete [] uv_numpars; uv_numpars = new_uv_numpars;
   delete [] uv_pars;    uv_pars    = new_uv_pars;
   delete [] corr;       corr       = new_corr;
   dim++;
   return TRUE;
}


BOOLEAN DISTRIBUTION::AddVar(char* name, 
                             char* nametype, 
                             CARDINAL numpars, 
                             REAL* pars,
                             REAL* corrs)
{
   CARDINAL i;
   UV_DISTTYPE type;
   switch ( mv_type )
   {
      case MV_DELTA:
         type = UV_DELTA;
         break;
      case MV_UNIFORM:
         type = UV_UNIFORM;
         break;
      case MV_NORMAL:
         type = UV_NORMAL;
         break;
      case MV_LOGNORMAL:
         type = UV_LOGNORMAL;
         break;
      case MV_COPULA:
         {
            if ( nametype == 0 )         return FALSE;
            if ( strlen(nametype) == 0 ) return FALSE;
            BOOLEAN found = FALSE;
            for ( i=0; i<num_uv_dist; i++ )
            {
               if ( strcmp(nametype,uv_diststrings[i]) == 0 )
               {
                  type = (UV_DISTTYPE)i;
                  found = TRUE;
                  break;
               }
            }
            if ( found == FALSE ) return FALSE;
         }
         break;
      default:
         return FALSE;
   }

   return AddVar(name,type,numpars,pars,corrs);
}


void DISTRIBUTION::Write(std::ostream* out)
{
   if ( out == 0 ) return;
   CARDINAL i,j;
   *out << "\n   " << mv_diststrings[mv_type];
   switch ( mv_type )
   {
      case MV_DELTA:
         for ( i=0; i<dim; i++ )
         {
            *out << "\n   " << varnames[i];
            for ( j=0; j<uv_numpars[i]; j++ ) *out << " " << uv_pars[i][j];
         }
         break;
      case MV_UNIFORM:
         for ( i=0; i<dim; i++ )
         {
            *out << "\n   " << varnames[i];
            for ( j=0; j<uv_numpars[i]; j++ ) *out << " " << uv_pars[i][j];
         }
         break;
      case MV_NORMAL:
         for ( i=0; i<dim; i++ )
         {
            *out << "\n   " << varnames[i];
            for ( j=0; j<uv_numpars[i]; j++ ) *out << " " << uv_pars[i][j];
            for ( j=0; j<i; j++ ) *out << " " << corr[i+j*dim];
         }
         break;
      case MV_LOGNORMAL:
         for ( i=0; i<dim; i++ )
         {
            *out << "\n   " << varnames[i];
            for ( j=0; j<uv_numpars[i]; j++ ) *out << " " << uv_pars[i][j];
            for ( j=0; j<i; j++ ) *out << " " << corr[i+j*dim];
         }
         break;
      case MV_COPULA:
         for ( i=0; i<dim; i++ )
         {
            *out << "\n   " << varnames[i];
            *out << " " << uv_diststrings[uv_types[i]];
            for ( j=0; j<uv_numpars[i]; j++ ) *out << " " << uv_pars[i][j];
            for ( j=0; j<i; j++ ) *out << " " << corr[i+j*dim];
         }
         break;
      default:
         break;
   }
   out -> flush();
   return;
}


CARDINAL DISTRIBUTION::GetNumParsCorr()
{
   if ( dim == 0 ) return 0;
   return dim*(dim-1)/2;
}


CARDINAL DISTRIBUTION::GetParValsCorr(REAL* par, CARDINAL dimpar)
{
   CARDINAL numpar = GetNumParsCorr();
   if ( numpar > dimpar ) return 0;
   CARDINAL k = 0;
   for ( CARDINAL i=0; i<dim; i++ )
   {
      for ( CARDINAL j=0; j<i; j++ )
      {
         par[k] = corr[i+j*dim];
         k++;
      }
   }
   if ( k != numpar )
   {
	   std::cout << "DISTRIBUTION::GetParValsCorr: internal error";
      exit(1);
   }
   return numpar;
}


BOOLEAN DISTRIBUTION::SetParValsCorr(REAL* par, CARDINAL num)
{
   CARDINAL numpar = GetNumParsCorr();
   if ( num != numpar ) return FALSE;
   CARDINAL k;
   for ( k=0; k<numpar; k++ )
   {
      if ( par[k] >= 1 || par[k] <= -1 ) return FALSE;
   }
   k = 0;
   for ( CARDINAL i=0; i<dim; i++ )
   {
      for ( CARDINAL j=0; j<i; j++ )
      {
         corr[i+j*dim] = par[k];
         corr[j+i*dim] = par[k];
         k++;
      }
   }
   if ( k != numpar )
   {
      std::cout << "DISTRIBUTION::SetParValsCorr: internal error";
      exit(1);
   }
   return TRUE;
}


CARDINAL DISTRIBUTION::GetNumParsAll()
{
   if ( dim == 0 ) return 0;
   CARDINAL numpar = dim*(dim-1)/2;
   for ( CARDINAL i=0; i<dim; i++ )
   {
      if ( uv_types[i] != UV_EMPIRICAL )
      {
         numpar += GetNumPars(uv_types[i]);
      }
   }
   return numpar;
}


CARDINAL DISTRIBUTION::GetParValsAll(REAL* par, CARDINAL dimpar)
{
   CARDINAL numpar = GetNumParsAll();
   if ( numpar > dimpar ) return 0;
   CARDINAL k = 0;
   CARDINAL i,j;
   for ( i=0; i<dim; i++ )
   {
      switch ( uv_types[i] )
      {
         case UV_DELTA: 
            par[k] = uv_pars[i][0]; k++;
            break;
         case UV_UNIFORM: 
         case UV_NORMAL:
         case UV_LOGNORMAL:
         case UV_GAMMA:
            par[k] = uv_pars[i][0]; k++;
            par[k] = uv_pars[i][1]; k++;
            break;
         case UV_TRIANGULAR:
            par[k] = uv_pars[i][0]; k++;
            par[k] = uv_pars[i][1]; k++;
            par[k] = uv_pars[i][2]; k++;
            break;
         case UV_EMPIRICAL:
            break;
         default:
            std::cout << "DISTRIBUTION::GetParValsAll: internal error";
            exit(1);
      }
   }
   for ( i=0; i<dim; i++ )
   {
      for ( j=0; j<i; j++ )
      {
         par[k] = corr[i+j*dim];
         k++;
      }
   }
   if ( k != numpar )
   {
      std::cout << "DISTRIBUTION::GetParValsAll: internal error";
      exit(1);
   }
   return numpar;
}


BOOLEAN DISTRIBUTION::SetParValsAll(REAL* par, CARDINAL num)
{
   CARDINAL numpar = GetNumParsAll();
   if ( numpar != num ) return FALSE;
   CARDINAL i,j,k;

   // check values:

   k = 0;
   for ( i=0; i<dim; i++ )
   {
      switch ( uv_types[i] )
      {
         case UV_DELTA: 
            k++; // uv_pars[i][0]: value
            break;
         case UV_UNIFORM: 
            k++; // uv_pars[i][0]: min
            k++; // uv_pars[i][0]: max
            break;
         case UV_NORMAL:
            k++;                                  // uv_pars[i][0]: mean
            if ( par[k] <= 0 ) return FALSE; k++; // uv_pars[i][1]: stdev
            break;
         case UV_LOGNORMAL:
            if ( par[k] <= 0 ) return FALSE; k++; // uv_pars[i][0]: mean
            if ( par[k] <= 0 ) return FALSE; k++; // uv_pars[i][1]: stdev
            break;
         case UV_GAMMA:
            if ( par[k] <= 0 ) return FALSE; k++; // uv_pars[i][0]: mean
            if ( par[k] <= 0 ) return FALSE; k++; // uv_pars[i][1]: stdev
            break;
         case UV_TRIANGULAR:
            k++;                                  // uv_pars[i][0]: mode
            if ( par[k] <= 0 ) return FALSE; k++; // uv_pars[i][1]: wileft
            if ( par[k] <= 0 ) return FALSE; k++; // uv_pars[i][1]: wiright
            break;
         case UV_EMPIRICAL:
            break;
         default:
            std::cout << "DISTRIBUTION::GetParValsAll: internal error";
            exit(1);
      }
   }
   CARDINAL offset = k;
   for ( k=offset; k<numpar; k++ )
   {
      if ( par[k] >= 1 || par[k] <= -1 ) return FALSE;
   }

   // copy values:

   k = 0;
   for ( i=0; i<dim; i++ )
   {
      switch ( uv_types[i] )
      {
         case UV_DELTA:
            uv_pars[i][0] = par[k]; k++;
            break;
         case UV_UNIFORM: 
         case UV_NORMAL:
         case UV_LOGNORMAL:
         case UV_GAMMA:
            uv_pars[i][0] = par[k]; k++;
            uv_pars[i][1] = par[k]; k++;
            break;
         case UV_TRIANGULAR:
            uv_pars[i][0] = par[k]; k++;
            uv_pars[i][1] = par[k]; k++;
            uv_pars[i][2] = par[k]; k++;
            break;
         case UV_EMPIRICAL:
            break;
         default:
            std::cout << "DISTRIBUTION::GetParValsAll: internal error";
            exit(1);
      }
   }
   for ( i=0; i<dim; i++ )
   {
      for ( j=0; j<i; j++ )
      {
         corr[i+j*dim] = par[k];
         corr[j+i*dim] = par[k];
         k++;
      }
   }
   if ( k != numpar )
   {
      std::cout << "DISTRIBUTION::SetParValsAll: internal error";
      exit(1);
   }
   return TRUE;
}


BOOLEAN DISTRIBUTION::GetParExInf(CARDINAL ind)
{
   CARDINAL numpar = GetNumParsAll();
   if ( ind >= numpar ) return FALSE;
   CARDINAL i,k;
   k = 0;
   for ( i=0; i<dim; i++ )
   {
      if ( uv_types[i] != UV_EMPIRICAL )
      {
         if ( ind < k+GetNumPars(uv_types[i]) )
         {
            CARDINAL index = k+GetNumPars(uv_types[i]) - ind;
            switch ( uv_types[i] )
            {
               case UV_DELTA: 
               case UV_UNIFORM: 
                  break;
               case UV_NORMAL:
                  if ( index == 1 ) return TRUE;
                  break;
               case UV_LOGNORMAL:
                  return TRUE;
                  break;
               case UV_GAMMA:
                  return TRUE;
                  break;
               case UV_TRIANGULAR:
                  if ( index > 0 ) return TRUE;
                  break;
               case UV_EMPIRICAL:
                  break;
               default:
                  std::cout << "DISTRIBUTION::GetParValsAll: internal error";
                  exit(1);
            }
            return FALSE;
         }
         else
         {
            k += uv_types[i];
         }
      }
   }
   return TRUE;  // corr
}


BOOLEAN DISTRIBUTION::GetParExSup(CARDINAL ind)
{
   CARDINAL numpar = GetNumParsAll();
   if ( ind >= numpar ) return FALSE;
   CARDINAL i,k;
   k = 0;
   for ( i=0; i<dim; i++ )
   {
      if ( uv_types[i] != UV_EMPIRICAL )
      {
         if ( ind < k+GetNumPars(uv_types[i]) )
         {
            return FALSE;
         }
         else
         {
            k += uv_types[i];
         }
      }
   }
   return TRUE;  // corr
}


REAL DISTRIBUTION::GetParInf(CARDINAL ind)
{
   CARDINAL numpar = GetNumParsAll();
   if ( ind >= numpar ) return 0;
   if ( ind >= numpar-GetNumParsCorr() ) return -1;
   return 0;
}


REAL DISTRIBUTION::GetParSup(CARDINAL ind)
{
   CARDINAL numpar = GetNumParsAll();
   if ( ind >= numpar ) return 0;
   if ( ind >= numpar-GetNumParsCorr() ) return 1;
   return 0;
}


//////////////////////////////////////////////////////////////////////////////

REAL machine_random_number()
{
   return ((REAL)rand()) / ((REAL)RAND_MAX);
}
   
const CARDINAL randomization_dim = 97;

class STATINIT
{
   public:

      STATINIT();

      REAL randomization_array[randomization_dim];
      REAL randomization_number;
};

STATINIT::STATINIT()
{
   CARDINAL i;

   // set seed:
   srand(1);

   // initialize random number generator:
   for ( i=0; i<randomization_dim; i++ )
   {
      rand();
   }
   
   // initialize array of random numbers:
   for ( i=0; i<randomization_dim; i++ )
   {
      randomization_array[i] = machine_random_number();
   }

   // initialiye randomization number:
   randomization_number = machine_random_number();
}

STATINIT statinit;

REAL uniform_random_number_01()
{
   CARDINAL i = (CARDINAL) ( ((REAL)randomization_dim) 
                                      * statinit.randomization_number );
   if ( i >= randomization_dim ) i = randomization_dim-1;
   statinit.randomization_number = statinit.randomization_array[i];
   REAL r = statinit.randomization_number;
   statinit.randomization_array[i] = machine_random_number();
   return r;
}

//////////////////////////////////////////////////////////////////////////////

REAL calc_sum(CARDLONG numsets, REAL* x)
{
   CARDLONG i,j;
   CARDLONG auxdim = (CARDLONG)sqrt((REAL)numsets);
   REAL* auxvar = new REAL[auxdim]; 
   for ( i=0; i<auxdim; i++ ) auxvar[i] = 0;
   REAL sum = 0;
   i = 0;
   for ( j=0; j<numsets; j++ )
   {
      auxvar[i] += x[j];
      i++; if ( i >= auxdim ) i = 0;
   }
   for ( i=0; i<auxdim; i++ ) sum += auxvar[i];
   delete [] auxvar;
   
   return sum;
}

//////////////////////////////////////////////////////////////////////////////

REAL calc_mean(CARDLONG numsets, REAL* x, REAL* weights)
{
   if ( numsets==0 || x==0 ) return 0;
   CARDLONG i;

   REAL mean;

   if ( weights == 0 )
   {
      mean = calc_sum(numsets,x)/((REAL)numsets);
   }
   else
   {
      REAL* wx = new REAL[numsets];
      for ( i=0; i<numsets; i++ ) wx[i] = weights[i]*x[i];
      mean = calc_sum(numsets,wx)/calc_sum(numsets,weights);
      delete [] wx;
   }

   return mean;
}

//////////////////////////////////////////////////////////////////////////////

void calc_meancov(CARDLONG numsets, CARDLONG numvar, REAL** sample,
                  REAL* weights,
                  REAL* means, REAL* stdevs, REAL* corr)
{
   CARDLONG i,j,k;
   REAL* auxvar = new REAL[numsets];
   for ( i=0; i<numvar; i++ )
   {
      for ( k=0; k<numsets; k++ ) auxvar[k] = sample[k][i];
      means[i] = calc_mean(numsets,auxvar,weights);
   }
   for ( i=0; i<numvar; i++ )
   {
      for ( j=0; j<=i; j++ )
      {
         for ( k=0; k<numsets; k++ )
         {
            auxvar[k] = (sample[k][i]-means[i])*(sample[k][j]-means[j]);
            corr[i+numvar*j] = calc_mean(numsets,auxvar,weights);
            corr[j+numvar*i] = corr[i+numvar*j];
         }
      }
   }
   for ( i=0; i<numvar; i++ )
   {
      stdevs[i] = sqrt(corr[i+numvar*i]);
   }
   for ( i=0; i<numvar; i++ )
   {
      for ( j=0; j<numvar; j++ )
      {
	 corr[i+numvar*j] = corr[i+numvar*j]/(stdevs[i]*stdevs[j]);
      }
   }
   delete [] auxvar;

   return;
}

//////////////////////////////////////////////////////////////////////////////

void calc_kendallcorr(CARDLONG numsets, CARDLONG numvar, REAL** sample, 
                      REAL* weights, REAL* corr)
{
   CARDLONG i,j,k,l;

   REAL* wi = new REAL[numsets];
   if ( weights == 0 )
   {
      REAL w = 1/((REAL)numsets);
      for ( i=0; i<numsets; i++ )
      {
         wi[i] = w;
      }
   }
   else
   {
      for ( i=0; i<numsets; i++ )
      {
         wi[i] = weights[i];
      }
   }

   REAL sum_wi = calc_sum(numsets,wi);
   REAL* wi2 = new REAL[numsets];
   for ( i=0; i<numsets; i++ ) wi2[i] = wi[i]*wi[i];
   REAL sum_wi2 = calc_sum(numsets,wi2);
   delete [] wi2;
   REAL* wiwj = new REAL[numsets];
   REAL* partsum_wiwj = new REAL[numsets];
   for ( i=0; i<numvar; i++ )
   {
      corr[i+numvar*i] = 1;
      for ( j=0; j<i; j++ )
      {
         for ( k=0; k<numsets; k++ )
         {
            CARDLONG m = 0; 
            for ( l=0; l<k; l++ )
            {
               if ( (sample[k][i]<sample[l][i] && sample[k][j]<sample[l][j])
                    || (sample[k][i]>sample[l][i] && sample[k][j]>sample[l][j]) )
               {
                  wiwj[m] = wi[k]*wi[l];
               }
               else
               {
                  if ( sample[k][i]==sample[l][i] || sample[k][j]==sample[l][j] )
		  {
                     wiwj[m] = 0.5*wi[k]*wi[l];
                  }
                  else
		  {
                     wiwj[m] = 0;
                  }
               }
               m++;
            }
            partsum_wiwj[k] = calc_sum(m,wiwj);
         }
         REAL sum_wiwj = calc_sum(numsets,partsum_wiwj);
         corr[i+numvar*j] = 4*sum_wiwj/(sum_wi*sum_wi)
                            /(1-sum_wi2/(sum_wi*sum_wi)) - 1;
         corr[j+numvar*i] = corr[i+numvar*j];
      }
   }
   delete [] wiwj;
   delete [] wi;

   return;
}

//////////////////////////////////////////////////////////////////////////////

void calc_fract(CARDLONG numsets, REAL* x, REAL* weights,
                CARDLONG numfract, REAL* levels, REAL* fract)
{
   CARDINAL i,j,k;

   // sort x array:

   REAL* wi          = new REAL[numsets];
   CARDLONG* indices = new CARDLONG[numsets];
   REAL* cumweights  = new REAL[numsets];
   if ( weights == 0 )
   {
      REAL w = 1/((REAL)numsets);
      for ( i=0; i<numsets; i++ ) wi[i] = w;
   }
   else
   {
      for ( i=0; i<numsets; i++ ) wi[i] = weights[i];
   }

   sort(numsets,x,indices);

   // calculate cumulative weights for sorted x array:

   j = 0;
   k = 0;
   cumweights[0] = 0;
   REAL auxvar = 0;
   CARDLONG auxdim = (CARDLONG)sqrt((REAL)numsets);
   for ( i=0; i<numsets; i++ )
   {
      auxvar += wi[indices[i]];
      cumweights[i] = cumweights[k]+auxvar;
      j++;
      if ( j > auxdim )
      {
	k = i; auxvar = 0; j = 0;
      }
   }
   REAL cumweightsend = cumweights[numsets-1];
   for ( i=0; i<numsets; i++ ) 
   {
      cumweights[i] = cumweights[i]/cumweightsend;
   }

   // calculate fractiles:

   for ( i=0; i<numfract; i++ )
   {
      CARDLONG index;
      if ( levels[i] <= 0 )
      {
         index = 0;
         if ( wi[indices[index]] == 0 )
         {
            for ( j=1; j<numsets; j++ )
            {
               if ( wi[indices[index+1]]==0 ) index++;
               else                           break;
            }
            CARDLONG ind = index;
            for ( j=0; j<ind; j++ )
            {
               if ( x[indices[index]] == x[indices[index+1]] ) index--;
               else                                            break;
            }
         }
      }
      else
      {
         if ( levels[i] >= 1 )
         {
            index = numsets-1;
            if ( wi[indices[index]] == 0 )
            {
               for ( j=1; j<numsets; j++ )
               {
                  if ( wi[indices[index-1]]==0 ) index--;
                  else                           break;
               }
               CARDLONG ind = numsets-index-1;
               for ( j=0; j<ind; j++ )
               {
                  if ( x[indices[index]] == x[indices[index-1]] ) index++;
                  else                                            break;
               }
            }
         }
         else
         {
            index = (CARDLONG)(levels[i]/((REAL)numsets));
            if ( index == numsets ) index = numsets-1;
            if ( cumweights[index] < levels[i] )
            {
               while ( index+1<numsets && cumweights[index]<levels[i] ) index++;
            }
            else
            {
               while ( index>0 && cumweights[index-1]>=levels[i] ) index--;
            }
         }
      }
      fract[i] = x[indices[index]];
   }

   delete [] wi;
   delete [] indices;
   delete [] cumweights;

   return;
}

//////////////////////////////////////////////////////////////////////////////

void calc_fracts(CARDLONG numsets, CARDLONG numvar, REAL** sample,
                 REAL* weights,
                 CARDLONG numfract, REAL* levels, REAL** fract)
{
   CARDLONG i,j;
   REAL* auxvar = new REAL[numsets];
   for ( i=0; i<numvar; i++ )
   {
      for ( j=0; j<numsets; j++ ) auxvar[j] = sample[j][i];
      calc_fract(numsets,auxvar,weights,numfract,levels,fract[i]);
   }
   delete [] auxvar;
   return;
}

//////////////////////////////////////////////////////////////////////////////

void calc_hpdlevels(CARDLONG numsets, REAL* samppdf, REAL* pdf,
                    CARDLONG numlevels, REAL* problevels, REAL* pdflevels)
{
   CARDINAL i,j,k;

   // sort pdf array:

   CARDLONG* indices = new CARDLONG[numsets];
   sort(numsets,pdf,indices);
   
   // calculate cumulative weights:

   REAL* locsamppdf = new REAL[numsets];
   if ( samppdf == 0 )
   {
      for ( i=0; i<numsets; i++ ) locsamppdf[i] = 1;
   }
   else
   {
      for ( i=0; i<numsets; i++ ) locsamppdf[i] = samppdf[i];
   }
   j = 0;
   k = 0;
   REAL* cumweights  = new REAL[numsets];
   REAL auxvar = 0;
   CARDLONG auxdim = (CARDLONG)sqrt((REAL)numsets);
   cumweights[0] = pdf[indices[0]]/locsamppdf[indices[0]];
   for ( i=1; i<numsets; i++ )
   {
      auxvar += pdf[indices[i]]/locsamppdf[indices[i]];
      cumweights[i] = cumweights[k]+auxvar;
      j++;
      if ( j > auxdim )
      {
         k = i; auxvar = 0; j = 0;
      }
   }
   REAL cumweightsend = cumweights[numsets-1];
   for ( i=0; i<numsets; i++ ) 
   {
      cumweights[i] = cumweights[i]/cumweightsend;
   }

   // eliminate noninceasing parts

   CARDLONG numincsamp = 1;
   REAL* locpdf = new REAL[numsets];
   for ( i=1; i<numsets; i++ )
   {
      if ( cumweights[i] > cumweights[numincsamp-1] )
      {
         cumweights[numincsamp] = cumweights[i];
         locpdf[numincsamp]     = pdf[indices[i]];
         numincsamp++;
      }
   }

   // calculate pdf-levels:

   CARDINAL index = 0;
   for ( i=0; i<numlevels; i++ )
   {
      pdflevels[i] = LinInt(cumweights,locpdf,numincsamp,
                            1-problevels[i],&index);
   }

   delete [] indices;
   delete [] cumweights;
   delete [] locsamppdf;
   delete [] locpdf;

   return;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN delta_pdf(REAL x, REAL* pdf, REAL value,
                  std::ostream* cjob)
{
   if ( x == value ) return FALSE;
   *pdf = 0;
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN delta_logpdf(REAL x, REAL* logpdf, REAL value,
                     std::ostream* cjob)
{
   return FALSE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN delta_cdf(REAL x, REAL* cdf, REAL value,
                  std::ostream* cjob)
{
   if ( x < value )
   {
      *cdf = 0;
   }
   else
   {
      *cdf = 1;
   }
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN delta_inv_cdf(REAL cdf, REAL* x, REAL value,
                      std::ostream* cjob)
{
   *x = value;
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN delta_randnum(REAL* r, REAL value,
                      std::ostream* cjob)
{
   *r = value;
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN uniform_pdf(REAL x, REAL* pdf, REAL lowerbound, REAL upperbound,
                    std::ostream* cjob)
{
   if ( upperbound == lowerbound )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in uniform_pdf: illegal argument\n";
      }
      return FALSE;
   }

   *pdf = 0;
   if ( upperbound > lowerbound )
   {
      if ( x>=lowerbound && x<=upperbound ) *pdf = 1/(upperbound-lowerbound);
   }
   else
   {
      if ( x>=upperbound && x<=lowerbound ) *pdf = 1/(lowerbound-upperbound);
   }

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN uniform_logpdf(REAL x, REAL* logpdf, REAL lowerbound, REAL upperbound,
                       std::ostream* cjob)
{
   if ( upperbound == lowerbound )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in uniform_logpdf: illegal argument\n";
      }
      return FALSE;
   }

   *logpdf = 0;
   if ( upperbound > lowerbound )
   {
      if ( x>=lowerbound && x<=upperbound ) *logpdf = -log(upperbound-lowerbound);
   }
   else
   {
      if ( x>=upperbound && x<=lowerbound ) *logpdf = -log(lowerbound-upperbound);
   }

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN uniform_cdf(REAL x, REAL* cdf, REAL lowerbound, REAL upperbound,
                    std::ostream* cjob)
{
   if ( upperbound == lowerbound )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in uniform_cdf: illegal argument\n";
      }
      return FALSE;
   }

   if ( upperbound >= lowerbound )
   {
      if ( x <= lowerbound )
      {
         *cdf = 0; return TRUE;
      }
      if ( x >= upperbound )
      {
         *cdf = 1; return TRUE;
      }
      *cdf = (x-lowerbound)/(upperbound-lowerbound);
   }
   else
   {
      if ( x <= upperbound )
      {
         *cdf = 0; return TRUE;
      }
      if ( x >= lowerbound )
      {
         *cdf = 1; return TRUE;
      }
      *cdf = (x-upperbound)/(lowerbound-upperbound);
   }

   return TRUE;   
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN uniform_inv_cdf(REAL cdf, REAL* x, REAL lowerbound, REAL upperbound,
                        std::ostream* cjob)
{
   if ( cdf < 0 || cdf > 1 || upperbound == lowerbound )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in uniform_inv_cdf: illegal argument\n";
      }
      return FALSE;
   }

   if ( lowerbound < upperbound )
   {
      *x = lowerbound + cdf*(upperbound-lowerbound);
   }
   else
   {
      *x = upperbound + cdf*(lowerbound-upperbound);
   }

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN uniform_randnum(REAL* r, REAL lowerbound, REAL upperbound,
                        std::ostream* cjob)
{
   if ( upperbound == lowerbound )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in uniform_randnum: illegal argument\n";
      }
      return FALSE;
   }

   *r = lowerbound + (upperbound-lowerbound)*uniform_random_number_01();
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN normal_pdf(REAL x, REAL* pdf, REAL mean, REAL sigma, std::ostream* cjob)
{
   static REAL c = 0.3989422804;

   if ( sigma <= 0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in normal_pdf: illegal argument\n";
      }
      return FALSE;
   }

   *pdf = c/sigma*exp(-(x-mean)*(x-mean)/(2*sigma*sigma));
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN normal_logpdf(REAL x, REAL* logpdf, REAL mean, REAL sigma, std::ostream* cjob)
{
   static REAL c = 0.3989422804;

   if ( sigma <= 0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in normal_logpdf: illegal argument\n";
      }
      return FALSE;
   }

   *logpdf = log(c) - log(sigma) - (x-mean)*(x-mean)/(2*sigma*sigma);
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN normal_cdf(REAL x, REAL* cdf, REAL mean, REAL sigma, std::ostream* cjob)
{
   static REAL b1 =  0.319381530;
   static REAL b2 = -0.356563782;
   static REAL b3 =  1.781477937;
   static REAL b4 = -1.821255978;
   static REAL b5 =  1.330274429;
   static REAL p  =  0.2316419;

   if ( sigma <= 0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in normal_cdf: illegal argument\n";
      }
      return FALSE;
   }

   REAL z = (x-mean)/sigma;
   if( x < mean ) z = -z;
   REAL t = 1.0/(1.0 + p*z);
   *cdf = 1.0 - ((0.39894228040143)*exp( -0.5*z*z ))*(b1*t + b2*pow(t, 2L) + 
          b3*pow(t, 3L) + b4*pow(t, 4L) + b5*pow(t, 5L));
   if( x < mean ) *cdf = 1.0 - *cdf;

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN normal_inv_cdf(REAL cdf, REAL* x, REAL mean, REAL sigma, 
                       std::ostream* cjob)
{
   static REAL p0 = -0.322232431088;
   static REAL p1 = -1.0;
   static REAL p2 = -0.342242088547;
   static REAL p3 = -0.204231210245e-1;
   static REAL p4 = -0.453642210148e-4;
   static REAL q0 =  0.993484626060e-1;
   static REAL q1 =  0.588581570495;
   static REAL q2 =  0.531103462366;
   static REAL q3 =  0.103537752850;
   static REAL q4 =  0.38560700634e-2;

   if ( cdf <= 0 || cdf >= 1 || sigma <= 0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in normal_inv_cdf: illegal argument\n";
      }
      return FALSE;
   }

   *x = 0;
   if( cdf != 0.5 )
   {
      REAL r = cdf;
      if( cdf > 0.5 ) r = 1.0 - r;
      REAL t = sqrt( -2.0*log( r ) );
      REAL anum = (((t*p4 + p3)*t + p2)*t + p1)*t + p0;
      REAL aden = (((t*q4 + q3)*t + q2)*t + q1)*t + q0;
      *x = t + (anum/aden);
      if ( cdf < 0.5 ) *x = -*x;
   }
   *x = *x*sigma + mean;

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN normal_randnum(REAL* r, REAL mean, REAL sigma, std::ostream* cjob)
{
   if ( sigma <= 0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in normal_randnum: illegal argument\n";
      }
      return FALSE;
   }

   REAL uniform;
   do 
   { 
      if ( uniform_randnum(&uniform,0,1,cjob) == FALSE )  return FALSE;
   } 
   while ( uniform==0 || uniform==1 );
   return normal_inv_cdf(uniform,r,mean,sigma,cjob);
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN lognormal_pdf(REAL x, REAL* pdf, REAL mean, REAL sigma, std::ostream* cjob)
{
   static REAL c = 0.3989422804;

   if ( mean<=0 || sigma<=0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in lognormal_pdf: illegal argument\n";
      }
      return FALSE;
   }

   if ( x <= 0 )
   {
      *pdf = 0;
   }
   else
   {
      REAL s = sqrt(log(1+sigma*sigma/(mean*mean)));
      *pdf = c/(x*s)*exp(- (log(x)-log(mean)+0.5*s*s)
                          *(log(x)-log(mean)+0.5*s*s)
                          /(2*s*s)                   );
   }
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN lognormal_logpdf(REAL x, REAL* logpdf, REAL mean, REAL sigma, std::ostream* cjob)
{
   static REAL c = 0.3989422804;

   if ( mean<=0 || sigma<=0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in lognormal_logpdf: illegal argument\n";
      }
      return FALSE;
   }

   if ( x <= 0 )
   {
      *logpdf = 0;
      return FALSE;
   }
   else
   {
      REAL s = sqrt(log(1+sigma*sigma/(mean*mean)));
      *logpdf = log(c) - log(x*s) - (log(x)-log(mean)+0.5*s*s)
                                   *(log(x)-log(mean)+0.5*s*s)
                                   /(2*s*s);
   }
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN lognormal_cdf(REAL x, REAL* cdf, REAL mean, REAL sigma, std::ostream* cjob)
{
   if ( mean<=0 || sigma<=0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in lognormal_cdf: illegal argument\n";
      }
      return FALSE;
   }

   if ( x <= 0 )
   {
      *cdf = 0;
   }
   else
   {
      REAL x_normal = log(x);
      REAL sigma_normal = sqrt(log(1+sigma*sigma/(mean*mean)));
      REAL mean_normal  = log(mean)-0.5*sigma_normal*sigma_normal;
      if ( normal_cdf(x_normal,cdf,mean_normal,sigma_normal,cjob) == FALSE )
         return FALSE;
   }

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN lognormal_inv_cdf(REAL cdf, REAL* x, REAL mean, REAL sigma, 
                          std::ostream* cjob)
{
   if ( mean<=0 || sigma<=0 || cdf<0 || cdf>=1 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in lognormal_inv_cdf: illegal argument\n";
      }
      return FALSE;
   }

   if ( cdf == 0 ) 
   {
      *x = 0;
   }
   else
   {
      REAL sigma_normal = sqrt(log(1+sigma*sigma/(mean*mean)));
      REAL mean_normal  = log(mean)-0.5*sigma_normal*sigma_normal;
      if ( normal_inv_cdf(cdf,x,mean_normal,sigma_normal,cjob) == FALSE )
         return FALSE;
      *x = exp(*x);
   }

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN lognormal_randnum(REAL* r, REAL mean, REAL sigma, std::ostream* cjob)
{
   if ( mean<=0 || sigma<=0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in lognormal_randnum: illegal argument\n";
      }
      return FALSE;
   }

   REAL sigma_normal = sqrt(log(1+sigma*sigma/(mean*mean)));
   REAL mean_normal  = log(mean)-0.5*sigma_normal*sigma_normal;
   if ( normal_randnum(r,mean_normal,sigma_normal,cjob) == FALSE )
      return FALSE;
   *r = exp(*r);

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN gamma_pdf(REAL x, REAL* pdf, REAL mean, REAL sigma, std::ostream* cjob)
{
   if ( mean<=0 || sigma<=0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in gamma_pdf: illegal argument\n";
      }
      return FALSE;
   }

   if ( x <= 0 )
   {
      *pdf = 0;
   }
   else
   {
      REAL alpha = mean*mean/(sigma*sigma);
      REAL beta  = mean/(sigma*sigma);
      *pdf = pow(beta,alpha)/gamma(alpha)*pow(x,alpha-1)*exp(-beta*x);
   }
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN gamma_logpdf(REAL x, REAL* logpdf, REAL mean, REAL sigma, std::ostream* cjob)
{
   if ( mean<=0 || sigma<=0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in gamma_logpdf: illegal argument\n";
      }
      return FALSE;
   }

   if ( x <= 0 )
   {
      *logpdf = 0;
      return FALSE;
   }
   else
   {
      REAL alpha = mean*mean/(sigma*sigma);
      REAL beta  = mean/(sigma*sigma);
      *logpdf = log(pow(beta,alpha)) - log(gamma(alpha))
                + log(pow(x,alpha-1)) -beta*x;
   }
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN gamma_cdf(REAL x, REAL* cdf, REAL mean, REAL sigma, std::ostream* cjob)
{
   if ( mean<=0 || sigma<=0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in gamma_cdf: illegal argument\n";
      }
      return FALSE;
   }

   if ( x <= 0 )
   {
      *cdf = 0;
   }
   else
   {
      REAL alpha = mean*mean/(sigma*sigma);
      REAL beta  = mean/(sigma*sigma);
      if ( gamcdf(beta*x,alpha,cdf) == FALSE ) return FALSE;
   }

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN gamma_inv_cdf(REAL cdf, REAL* x, REAL mean, REAL sigma, 
                      std::ostream* cjob)
{
   if ( mean<=0 || sigma<=0 || cdf<0 || cdf>=1 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in gamma_inv_cdf: illegal argument\n";
      }
      return FALSE;
   }

   if ( cdf == 0 ) 
   {
      *x = 0;
   }
   else
   {
      REAL alpha = mean*mean/(sigma*sigma);
      REAL beta  = mean/(sigma*sigma);
      if ( gamppf(cdf,alpha,x) == FALSE ) return FALSE;
      *x = *x/beta;
   }

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN gamma_randnum(REAL* r, REAL mean, REAL sigma, std::ostream* cjob)
{
   if ( mean<=0 || sigma<=0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in gamma_randnum: illegal argument\n";
      }
      return FALSE;
   }

   REAL uniform;
   do 
   { 
      if ( uniform_randnum(&uniform,0,1,cjob) == FALSE ) return FALSE;
   } 
   while ( uniform==0 || uniform==1 );
   return gamma_inv_cdf(uniform,r,mean,sigma,cjob);

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN triangular_pdf(REAL x, REAL* pdf, 
                       REAL mode, REAL leftwidth, REAL rightwidth,
                       std::ostream* cjob)
{
   if ( leftwidth<=0 || rightwidth<=0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in triangular_pdf: illegal argument\n";
      }
      return FALSE;
   }

   REAL pdfmode = 2.0/(leftwidth+rightwidth);
   if ( x>=mode-leftwidth && x<=mode ) 
   {
      *pdf = (x-mode+leftwidth)/leftwidth*pdfmode;
   }
   else
   {
      if ( x>=mode && x<=mode+rightwidth )
      {
         *pdf = (mode+rightwidth-x)/rightwidth*pdfmode;
      }
      else
      {
         *pdf = 0;
      }
   }
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN triangular_logpdf(REAL x, REAL* logpdf, 
                          REAL mode, REAL leftwidth, REAL rightwidth,
                          std::ostream* cjob)
{
   if ( leftwidth<=0 || rightwidth<=0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in triangular_logpdf: illegal argument\n";
      }
      return FALSE;
   }

   REAL pdfmode = 2.0/(leftwidth+rightwidth);
   if ( x>=mode-leftwidth && x<=mode ) 
   {
      *logpdf = log(x-mode+leftwidth) - log(leftwidth) + log(pdfmode);
   }
   else
   {
      if ( x>=mode && x<=mode+rightwidth )
      {
         *logpdf = log(mode+rightwidth-x) - log(rightwidth) + log(pdfmode);
      }
      else
      {
         *logpdf = 0;
         return FALSE;
      }
   }
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN triangular_cdf(REAL x, REAL* cdf, 
                       REAL mode, REAL leftwidth, REAL rightwidth,
                       std::ostream* cjob)
{
   if ( leftwidth<=0 || rightwidth<=0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in triangular_cdf: illegal argument\n";
      }
      return FALSE;
   }

   REAL pdfmode = 2.0/(leftwidth+rightwidth);
   REAL cdfmode = 0.5*pdfmode*leftwidth;
   if ( x>=mode-leftwidth && x<=mode ) 
   {
      *cdf = 0.5*pdfmode/leftwidth * (x-mode+leftwidth)*(x-mode+leftwidth);
   }
   else
   {
      if ( x>=mode && x<=mode+rightwidth )
      {
         *cdf = cdfmode
                + 0.5*pdfmode/rightwidth
                  *( rightwidth*rightwidth
                     - (mode+rightwidth-x)*(mode+rightwidth-x) );
      }
      else
      {
         if ( x <= mode-leftwidth ) *cdf = 0;
         else                       *cdf = 1;
      }
   }
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN triangular_inv_cdf(REAL cdf, REAL* x,
                           REAL mode, REAL leftwidth, REAL rightwidth,
                           std::ostream* cjob)
{
   if ( leftwidth<=0 || rightwidth<=0 || cdf<0 || cdf>1 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in triangular_inv_cdf: illegal argument\n";
      }
      return FALSE;
   }

   REAL pdfmode = 2.0/(leftwidth+rightwidth);
   REAL cdfmode = 0.5*pdfmode*leftwidth;

   if ( cdf>=0 && cdf<=cdfmode ) 
   {
      *x = mode-leftwidth + sqrt(2*cdf/pdfmode*leftwidth);
   }
   else
   {
      if ( cdf>=cdfmode && cdf<=1 )
      {
         REAL dx2 = rightwidth*rightwidth 
                    - 2*(cdf-cdfmode)/pdfmode*rightwidth;
         if ( dx2 <= 0 ) 
         {
            *x = mode+rightwidth;
         }
         else
         {
            *x = mode+rightwidth - sqrt(dx2);
         }
      }
      else
      {
         if ( cdf <= 0 ) *x = mode-leftwidth;
         else            *x = mode+rightwidth;
      }
   }
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN triangular_randnum(REAL* r, 
                           REAL mode, REAL leftwidth, REAL rightwidth,
                           std::ostream* cjob)
{
   if ( leftwidth<=0 || rightwidth<=0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in triangular_randnum: illegal argument\n";
      }
      return FALSE;
   }

   REAL uniform;
   if ( uniform_randnum(&uniform,0,1,cjob) == FALSE )  return FALSE;
   return triangular_inv_cdf(uniform,r,mode,leftwidth,rightwidth,cjob);
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN empirical_pdf(REAL x, REAL* pdf, 
                      CARDLONG numfract, REAL* levels, REAL* fract,
                      std::ostream* cjob)
{
   CARDLONG i;
   if ( numfract<2 || levels==0 || fract==0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in empirical_pdf: illegal argument\n";
      }
      return FALSE;
   }
   if ( levels[0]!=0 || levels[numfract-1]!=1 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in empirical_pdf: "
               << "levels must strictly increase from 0 to 1\n";
      }
      return FALSE;
   }
   for ( i=1; i<numfract; i++ )
   {
      if ( levels[i] <= levels[i-1] )
      {
         if ( cjob != 0 )
         {
            *cjob << "\n*** ERROR in empirical_pdf: "
                  << "levels must strictly increase from 0 to 1\n";
         }
         return FALSE;
      }
      if ( fract[i] <= fract[i-1] )
      {
         if ( cjob != 0 )
         {
            *cjob << "\n*** ERROR in empirical_pdf: "
                  << "fractile values must strictly increase\n";
         }
         return FALSE;
      }
   }

   if ( x<=fract[0] || x>=fract[numfract-1] ) 
   {
      *pdf = 0;
   }
   else
   {
      CARDLONG index = (CARDLONG)((x-fract[0])/(fract[numfract-1]-fract[0]));
      if ( index+1 >= numfract ) index = numfract-2;
      if ( x < fract[index] )
      {
         while ( index>0 && x<fract[index] ) index--;
      }
      else
      {
         while ( index+2<numfract && x>fract[index+1] ) index++;
      }
      *pdf = (levels[index+1]-levels[index])/(fract[index+1]-fract[index]);
   }

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN empirical_logpdf(REAL x, REAL* logpdf, 
                         CARDLONG numfract, REAL* levels, REAL* fract,
                         std::ostream* cjob)
{
   CARDLONG i;
   if ( numfract<2 || levels==0 || fract==0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in empirical_logpdf: illegal argument\n";
      }
      return FALSE;
   }
   if ( levels[0]!=0 || levels[numfract-1]!=1 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in empirical_logpdf: "
               << "levels must strictly increase from 0 to 1\n";
      }
      return FALSE;
   }
   for ( i=1; i<numfract; i++ )
   {
      if ( levels[i] <= levels[i-1] )
      {
         if ( cjob != 0 )
         {
            *cjob << "\n*** ERROR in empirical_logpdf: "
                  << "levels must strictly increase from 0 to 1\n";
         }
         return FALSE;
      }
      if ( fract[i] <= fract[i-1] )
      {
         if ( cjob != 0 )
         {
            *cjob << "\n*** ERROR in empirical_logpdf: "
                  << "fractile values must strictly increase\n";
         }
         return FALSE;
      }
   }

   if ( x<=fract[0] || x>=fract[numfract-1] ) 
   {
      *logpdf = 0;
      return FALSE;
   }
   else
   {
      CARDLONG index = (CARDLONG)((x-fract[0])/(fract[numfract-1]-fract[0]));
      if ( index+1 >= numfract ) index = numfract-2;
      if ( x < fract[index] )
      {
         while ( index>0 && x<fract[index] ) index--;
      }
      else
      {
         while ( index+2<numfract && x>fract[index+1] ) index++;
      }
      *logpdf = log(levels[index+1]-levels[index])
                - log((fract[index+1]-fract[index]));
   }

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN empirical_cdf(REAL x, REAL* cdf, 
                      CARDLONG numfract, REAL* levels, REAL* fract,
                      std::ostream* cjob)
{
   CARDLONG i;
   if ( numfract<2 || levels==0 || fract==0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in empirical_cdf: illegal argument\n";
      }
      return FALSE;
   }
   if ( levels[0]!=0 || levels[numfract-1]!=1 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in empirical_cdf: "
               << "levels must strictly increase from 0 to 1\n";
      }
      return FALSE;
   }
   for ( i=1; i<numfract; i++ )
   {
      if ( levels[i] <= levels[i-1] )
      {
         if ( cjob != 0 )
         {
            *cjob << "\n*** ERROR in empirical_cdf: "
                  << "levels must strictly increase from 0 to 1\n";
         }
         return FALSE;
      }
      if ( fract[i] <= fract[i-1] )
      {
         if ( cjob != 0 )
         {
            *cjob << "\n*** ERROR in empirical_cdf: "
                  << "fractile values must strictly increase\n";
         }
         return FALSE;
      }
   }

   *cdf = LinInt(fract,levels,numfract,x);

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN empirical_inv_cdf(REAL cdf, REAL* x,
                          CARDLONG numfract, REAL* levels, REAL* fract,
                          std::ostream* cjob)
{
   CARDLONG i;
   if ( numfract<2 || levels==0 || fract==0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in empirical_inv_cdf: illegal argument\n";
      }
      return FALSE;
   }
   if ( levels[0]!=0 || levels[numfract-1]!=1 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in empirical_inv_cdf: "
               << "levels must strictly increase from 0 to 1\n";
      }
      return FALSE;
   }
   for ( i=1; i<numfract; i++ )
   {
      if ( levels[i] <= levels[i-1] )
      {
         if ( cjob != 0 )
         {
            *cjob << "\n*** ERROR in empirical_inv_cdf: "
                  << "levels must strictly increase from 0 to 1\n";
         }
         return FALSE;
      }
      if ( fract[i] <= fract[i-1] )
      {
         if ( cjob != 0 )
         {
            *cjob << "\n*** ERROR in empirical_inv_cdf: "
                  << "fractile values must strictly increase\n";
         }
         return FALSE;
      }
   }

   *x = LinInt(levels,fract,numfract,cdf);

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN empirical_randnum(REAL* r, 
                          CARDLONG numfract, REAL* levels, REAL* fract,
                          std::ostream* cjob)
{
   CARDLONG i;
   if ( numfract<2 || levels==0 || fract==0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in empirical_inv_cdf: illegal argument\n";
      }
      return FALSE;
   }
   if ( levels[0]!=0 || levels[numfract-1]!=1 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in empirical_inv_cdf: "
               << "levels must strictly increase from 0 to 1\n";
      }
      return FALSE;
   }
   for ( i=1; i<numfract; i++ )
   {
      if ( levels[i] <= levels[i-1] )
      {
         if ( cjob != 0 )
         {
            *cjob << "\n*** ERROR in empirical_inv_cdf: "
                  << "levels must strictly increase from 0 to 1\n";
         }
         return FALSE;
      }
      if ( fract[i] <= fract[i-1] )
      {
         if ( cjob != 0 )
         {
            *cjob << "\n*** ERROR in empirical_inv_cdf: "
                  << "fractile values must strictly increase\n";
         }
         return FALSE;
      }
   }

   REAL uniform;
   if ( uniform_randnum(&uniform,0,1,cjob) == FALSE )  return FALSE;
   return empirical_inv_cdf(uniform,r,numfract,levels,fract,cjob);
}

//////////////////////////////////////////////////////////////////////////////

static REAL phi(CARDINAL p, CARDLONG k)
{
   CARDLONG p1  = p;
   CARDLONG k1  = k;
   REAL     phi = 0;
   while ( k1 > 0 )
   {
      CARDLONG b = (CARDLONG)(k1/p);
      CARDLONG a = k1 - b*p;
      phi += ((REAL)a)/((REAL)p1);
      k1 = (CARDLONG)(k1/p);
      p1 = p1*p;
   }
   return phi;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN mv_delta_pdf(CARDLONG dim, REAL* x, REAL* pdf,
                     REAL* values, std::ostream* cjob)
{
   if ( dim==0 || x==0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in mv_delta_pdf: illegal argument\n";
      }
      return FALSE;
   }
   CARDINAL i;
   if ( values==0 )
   {
      for ( i=0; i<dim; i++ )
      {
         if ( x[i] == 0 ) return FALSE;
      }
   }
   else
   {
      for ( i=0; i<dim; i++ )
      {
         if ( x[i] == values[i] ) return FALSE;
      }
   }
   *pdf = 0;
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////


BOOLEAN mv_delta_logpdf(CARDLONG dim, REAL* x, REAL* logpdf,
                        REAL* values, std::ostream* cjob)
{
   if ( dim==0 || x==0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in mv_delta_logpdf: illegal argument\n";
      }
      return FALSE;
   }
   return FALSE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN mv_delta_cdf(CARDLONG dim, REAL* x, REAL* cdf,
                     REAL* values, std::ostream* cjob)
{
   if ( dim==0 || x==0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in mv_delta_cdf: illegal argument\n";
      }
      return FALSE;
   }
   CARDINAL i;

   *cdf = 1; 

   if ( values==0 )
   {
      for ( i=0; i<dim; i++ )
      {
         if ( x[i] == 0 ) return FALSE;
         if ( x[i] < 0 )
         {
            *cdf = 0; break;
         }
      }
   }
   else
   {
      for ( i=0; i<dim; i++ )
      {
         if ( x[i] == values[i] ) return FALSE;
         if ( x[i] < values[i] )
         {
            *cdf = 0; break;
         }
      }
   }
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

CARDLONG mv_delta_randsamp(CARDLONG dim, CARDLONG maxsize, SAMPTECH samptech,
                           REAL** sample, 
                           REAL* values,
                           std::ostream* cjob)
{
   CARDLONG i,j;

   // input consistency checks:

   if ( dim==0 || maxsize==0 || sample== 0 || dim>1000 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in mv_uniform_randsamp: illegal argument\n";
      }
      return 0;
   }
   for ( j=0; j<maxsize; j++ )
   {
      if ( sample[j] == 0 )
      {
         if ( cjob != 0 )
         {
            *cjob << "\n*** ERROR in mv_uniform_randsamp: illegal argument\n";
         }
         return 0;
      }
   }

   // calculated sample in unit cube:

   CARDLONG sampsize = maxsize;
   for ( j=0; j<sampsize; j++ )
   {
      for ( i=0; i<dim; i++ )
      {
         sample[j][i] = values[i];
	  }
   }

   return sampsize;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN mv_uniform_pdf(CARDLONG dim, REAL* x, REAL* pdf,
                       REAL* lowerbounds, REAL* upperbounds, std::ostream* cjob)
{
   if ( dim==0 || x==0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in mv_uniform_pdf: illegal argument\n";
      }
      return FALSE;
   }
   CARDINAL i;
   if ( lowerbounds==0 && upperbounds==0 )
   {
      for ( i=0; i<dim; i++ )
      {
         if ( x[i] < 0 )
         {
            *pdf = 0; return TRUE;
         }
         if ( x[i] > 1 )
         {
            *pdf = 0; return TRUE;
         }
      }
      *pdf = 1; return TRUE;
   }
   
   *pdf = 1;
   for ( i=0; i<dim; i++ )
   {
      if ( lowerbounds!=0 && upperbounds!=0 )
      {
         if ( lowerbounds[i] == upperbounds[i] )
         {
            if ( cjob != 0 )
            {
               *cjob << "\n*** ERROR in mv_uniform_pdf: upperbound equal to lowerbound\n";
            }
            return FALSE;
         }
         if ( x[i] < fmin(lowerbounds[i],upperbounds[i]) )
         {
            *pdf = 0; return TRUE;
         }
         if ( x[i] > fmax(lowerbounds[i],upperbounds[i]) ) 
         {
            *pdf = 0; return TRUE;
         }
         *pdf = *pdf/fabs(upperbounds[i]-lowerbounds[i]);
      } 
      else
      {
         if ( lowerbounds != 0 )
         {
            if ( lowerbounds[i] == 1 )
            {
               if ( cjob != 0 )
               {
                  *cjob << "\n*** ERROR in mv_uniform_pdf: upperbound equal to lowerbound\n";
               }
               return FALSE;
            }
            if ( x[i] < fmin(lowerbounds[i],1.0) )
            {
               *pdf = 0; return TRUE;
            }
            if ( x[i] > fmax(lowerbounds[i],1.0) )
            {
               *pdf = 0; return TRUE;
            }
            *pdf = *pdf/fabs(1-lowerbounds[i]);
         } 
         else
         {
            if ( upperbounds[i] == 0 )
            {
               if ( cjob != 0 )
               {
                  *cjob << "\n*** ERROR in mv_uniform_pdf: upperbound equal to lowerbound\n";
               }
               return FALSE;
            }
            if ( x[i] < fmin(0.0,upperbounds[i]) )
            {
               *pdf = 0; return TRUE;
            }
            if ( x[i] > fmax(0.0,upperbounds[i]) )
            {
               *pdf = 0; return TRUE;
            }
            *pdf = *pdf/fabs(upperbounds[i]-0);
         } 
      }
   }
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////


BOOLEAN mv_uniform_logpdf(CARDLONG dim, REAL* x, REAL* logpdf,
                          REAL* lowerbounds, REAL* upperbounds, std::ostream* cjob)
{
   if ( dim==0 || x==0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in mv_uniform_logpdf: illegal argument\n";
      }
      return FALSE;
   }
   CARDINAL i;
   if ( lowerbounds==0 && upperbounds==0 )
   {
      for ( i=0; i<dim; i++ )
      {
         if ( x[i] < 0 )
         {
            *logpdf = 0; return FALSE;
         }
         if ( x[i] > 1 )
         {
            *logpdf = 0; return FALSE;
         }
      }
      *logpdf = 0; return TRUE;
   }
   
   *logpdf = 0;
   for ( i=0; i<dim; i++ )
   {
      if ( lowerbounds!=0 && upperbounds!=0 )
      {
         if ( lowerbounds[i] == upperbounds[i] )
         {
            if ( cjob != 0 )
            {
               *cjob << "\n*** ERROR in mv_uniform_logpdf: upperbound equal to lowerbound\n";
            }
            return FALSE;
         }
         if ( x[i] < fmin(lowerbounds[i],upperbounds[i]) )
         {
            return FALSE;
         }
         if ( x[i] > fmax(lowerbounds[i],upperbounds[i]) ) 
         {
            return FALSE;
         }
         *logpdf = *logpdf - log(fabs(upperbounds[i]-lowerbounds[i]));
      } 
      else
      {
         if ( lowerbounds != 0 )
         {
            if ( lowerbounds[i] == 1 )
            {
               if ( cjob != 0 )
               {
                  *cjob << "\n*** ERROR in mv_uniform_logpdf: upperbound equal to lowerbound\n";
               }
               return FALSE;
            }
            if ( x[i] < fmin(lowerbounds[i],1.0) )
            {
               return FALSE;
            }
            if ( x[i] > fmax(lowerbounds[i],1.0) )
            {
               return FALSE;
            }
            *logpdf = *logpdf - log(fabs(1-lowerbounds[i]));
         } 
         else
         {
            if ( upperbounds[i] == 0 )
            {
               if ( cjob != 0 )
               {
                  *cjob << "\n*** ERROR in mv_uniform_logpdf: upperbound equal to lowerbound\n";
               }
               return FALSE;
            }
            if ( x[i] < fmin(0.0,upperbounds[i]) )
            {
               return FALSE;
            }
            if ( x[i] > fmax(0.0,upperbounds[i]) )
            {
               return FALSE;
            }
            *logpdf = *logpdf - log(fabs(upperbounds[i]-0));
         } 
      }
   }
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN mv_uniform_cdf(CARDLONG dim, REAL* x, REAL* cdf,
                       REAL* lowerbounds, REAL* upperbounds, std::ostream* cjob)
{
   if ( dim==0 || x==0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in mv_uniform_cdf: illegal argument\n";
      }
      return FALSE;
   }
   CARDINAL i;

   *cdf = 1; 

   if ( lowerbounds==0 && upperbounds==0 )
   {
      for ( i=0; i<dim; i++ )
      {
         if ( x[i] < 0 )
         {
            *cdf = 0; return TRUE;
         }
         if ( x[i] < 1 )
         {
            *cdf = *cdf*x[i];
         }
      }
      return TRUE;
   }

   for ( i=0; i<dim; i++ )
   {
      if ( lowerbounds!=0 && upperbounds!=0 )
      {
         if ( lowerbounds[i] == upperbounds[i] )
         {
            if ( cjob != 0 )
            {
               *cjob << "\n*** ERROR in mv_uniform_cdf: upperbound equal to lowerbound\n";
            }
            return FALSE;
         }
         if ( x[i] < fmin(lowerbounds[i],upperbounds[i]) )
         {
            *cdf = 0; return TRUE;
         }
         if ( x[i] < fmax(lowerbounds[i],upperbounds[i]) ) 
         {
            *cdf = *cdf*(x[i]-fmin(lowerbounds[i],upperbounds[i]))
                              /fabs(upperbounds[i]-lowerbounds[i]);
         }
      } 
      else
      {
         if ( lowerbounds != 0 )
         {
            if ( lowerbounds[i] == 1 )
            {
               if ( cjob != 0 )
               {
                  *cjob << "\n*** ERROR in mv_uniform_cdf: upperbound equal to lowerbound\n";
               }
               return FALSE;
            }
            if ( x[i] < fmin(lowerbounds[i],1.0) )
            {
               *cdf = 0; return TRUE;
            }
            if ( x[i] < fmax(lowerbounds[i],1.0) )
            {
               *cdf = *cdf*(x[i]-fmin(lowerbounds[i],1.0))
                                 /fabs(1.0-lowerbounds[i]);
            }
         } 
         else
         {
            if ( upperbounds[i] == 0 )
            {
               if ( cjob != 0 )
               {
                  *cjob << "\n*** ERROR in mv_uniform_cdf: upperbound equal to lowerbound\n";
               }
               return FALSE;
            }
            if ( x[i] < fmin(0.0,upperbounds[i]) )
            {
               *cdf = 0; return TRUE;
            }
            if ( x[i] < fmax(0.0,upperbounds[i]) )
            {
               *cdf = *cdf*(x[i]-fmin(0.0,upperbounds[i]))
                                 /fabs(upperbounds[i]-0.0);
            }
         } 
      }
   }
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

CARDLONG mv_uniform_randsamp(CARDLONG dim, CARDLONG maxsize, SAMPTECH samptech,
                             REAL** sample, 
                             REAL* lowerbounds, REAL* upperbounds,
                             std::ostream* cjob)
{
   CARDLONG i,j;

   // input consistency checks:

   if ( dim==0 || maxsize==0 || sample== 0 || dim>1000 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in mv_uniform_randsamp: illegal argument\n";
      }
      return 0;
   }
   for ( j=0; j<maxsize; j++ )
   {
      if ( sample[j] == 0 )
      {
         if ( cjob != 0 )
         {
            *cjob << "\n*** ERROR in mv_uniform_randsamp: illegal argument\n";
         }
         return 0;
      }
   }
   CARDLONG sampsize = 0;

   // calculated sample in unit cube:

   switch ( samptech )
   {
      case GRID:
         {
            CARDLONG numintervals = (CARDLONG)(exp(log((REAL)maxsize)/(REAL)dim)+0.5);
            MultiDimIndex ind(dim,numintervals);
            sampsize = ind.OneDimSize(); if ( sampsize > maxsize ) return 0;
            ind.ResetIndex();
            j = 0;
            REAL delta = 1/((REAL)numintervals);
            do
            {
               for ( i=0; i<dim; i++ )
               {
                  sample[j][i] = ((REAL)ind.CurrentIndex(i)-0.5)*delta;
               }
               j++;
            } while ( ind.IncrementIndex() == TRUE );
         }
         break;
      case RANDOM:
         {
            sampsize = maxsize;
            for ( j=0; j<sampsize; j++ )
            {
               for ( i=0; i<dim; i++ )
               {
                  do
		  {
                     if ( uniform_randnum(&sample[j][i],0,1,cjob) == FALSE)
                        return 0;
                  } while ( sample[j][i]==0 || sample[j][i]==1 );
               }
            }
         }
         break;
      case LHS:
         {
            sampsize = maxsize;
            REAL delta = 1/((REAL)sampsize);
            for ( j=0; j<sampsize; j++ )
            {
               for ( i=0; i<dim; i++ )
               {
                  sample[j][i] = ((REAL)j+0.5)*delta;
               }
            }
            for ( i=1; i<dim; i++ )
            {
               for ( j=0; j+1<sampsize; j++ )
               {
                  REAL r;
                  if ( uniform_randnum(&r,0,1,cjob) == FALSE ) return 0;
                  CARDLONG k = j + (CARDLONG)((sampsize-j)*r);
                  if ( k >= sampsize ) k = sampsize-1;
                  if ( k > j )
                  {
                     r = sample[k][i];
                     sample[k][i] = sample[j][i];
                     sample[j][i] = r;
                  }
               }
            }
         }
         break;
      case HAMMERSLEY:
         {
            sampsize = maxsize;
            REAL delta = 1/(REAL)(sampsize+1);
            for ( j=0; j<sampsize; j++ )
            {
               sample[j][0] = ((REAL)(j+1))*delta;
               for ( i=1; i<dim; i++ )
               {
                  sample[j][i] = phi(primes[i-1],j+1);
               }
            }
         }
         break;
      case HALTON:
         {
            sampsize = maxsize;
            for ( j=0; j<sampsize; j++ )
            {
               for ( i=0; i<dim; i++ )
               {
                  sample[j][i] = phi(primes[i],j+1);
               }
            }
         }
         break;
      default:
         if ( cjob != 0 )
         {
            *cjob << "\n*** ERROR in mv_uniform_randsamp: unknown sampling technique\n";
         }
         return 0;
   }

   // rescaling to intervals given in lowerbounds and upperbounds:

   if ( lowerbounds!=0 || upperbounds!=0 )
   {
      if ( lowerbounds!=0 && upperbounds!=0 )
      {
         for ( j=0; j<sampsize; j++ )
         {
            for ( i=0; i<dim; i++ )
            {
               sample[j][i] 
                  = lowerbounds[i]
                    + (upperbounds[i]-lowerbounds[i])*sample[j][i];
            }
         }
      }
      else
      {
         if ( lowerbounds != 0 )
         {
            for ( j=0; j<sampsize; j++ )
            {
               for ( i=0; i<dim; i++ )
               {
                  sample[j][i] 
                     = lowerbounds[i]
                       + (1-lowerbounds[i])*sample[j][i];
               }
            }
         }
         else
         {
            for ( j=0; j<sampsize; j++ )
            {
               for ( i=0; i<dim; i++ )
               {
                  sample[j][i] 
                     = upperbounds[i]*sample[j][i];
               }
            }
         }
      }
   } 
   for ( j=sampsize; j<maxsize; j++ )
   {
      for ( i=0; i<dim; i++ ) sample[j][i] = 0;
   }

   return sampsize;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN mv_normal_pdf(CARDLONG dim, REAL* x, REAL* pdf,
                      REAL* means, REAL* sigmas, REAL* corr,
                      std::ostream* cjob) 
{
   CARDLONG i,j;
   if ( dim==0 || x==0 || pdf==0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in mv_normal_pdf: illegal argument\n";
      }
      return FALSE;
   }
   if ( sigmas != 0 )
   {
      for ( i=0; i<dim; i++ )
      {
         if ( sigmas[i] <= 0 )
         {
            if ( cjob != 0 )
            {
               *cjob << "\n*** ERROR in mv_normal_pdf: illegal argument\n";
            }
            return FALSE;
         }
      }
   }

   REAL* mu      = new REAL[dim];
   REAL* sig     = new REAL[dim];
   REAL* var     = new REAL[dim*dim];
   REAL* work    = new REAL[dim];
   INTEGER* ipvt = new INTEGER[dim];

   if ( means == 0 )
   {
      for ( i=0; i<dim; i++ ) mu[i] = 0;
   }
   else
   {
      for ( i=0; i<dim; i++ ) mu[i] = means[i];
   }
   if ( sigmas == 0 )
   {
      for ( i=0; i<dim; i++ ) sig[i] = 1;
   }
   else
   {
      for ( i=0; i<dim; i++ ) sig[i] = sigmas[i];
   }
   if ( corr == 0 )
   {
      for ( i=0; i<dim*dim; i++ ) var[i] = 0;
      for ( i=0; i<dim; i++ )     var[i+dim*i] = sig[i]*sig[i];
   }
   else
   {
      for ( i=0; i<dim; i++ )
      {
         for ( j=0; j<dim; j++ )
         {
            var[i+dim*j] = corr[i+dim*j]*sig[i]*sig[j];
         }
      }
   }

   const REAL sqrtwopiinv = 0.398942280;
   *pdf = 1;
   for ( i=0; i<dim; i++ ) *pdf = *pdf*sqrtwopiinv;
   REAL det[2]; REAL rcond;
   dgeco(var,dim,dim,ipvt,&rcond,work);
   dgedi(var,dim,dim,ipvt,det,work,11);
   if ( 1+rcond == 1 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in mv_normal_pdf: covariance matrix is singular";
      }
      return FALSE;
   }
   const REAL ten = 10.0;
   *pdf = *pdf/sqrt(det[0]*pow(ten,det[1]));
   REAL sum = 0;
   for ( i=0; i<dim; i++ )
   {
      for ( j=0; j<dim; j++ )
      {
         sum += var[i+dim*j]*(x[i]-mu[i])*(x[j]-mu[j]);
      }
   }
   *pdf = *pdf*exp(-0.5*sum);

   delete [] mu;
   delete [] sig;
   delete [] var;
   delete [] work;
   delete [] ipvt;

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN mv_normal_logpdf(CARDLONG dim, REAL* x, REAL* logpdf,
                         REAL* means, REAL* sigmas, REAL* corr,
                         std::ostream* cjob) 
{
   CARDLONG i,j;
   if ( dim==0 || x==0 || logpdf==0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in mv_normal_logpdf: illegal argument\n";
      }
      return FALSE;
   }
   if ( sigmas != 0 )
   {
      for ( i=0; i<dim; i++ )
      {
         if ( sigmas[i] <= 0 )
         {
            if ( cjob != 0 )
            {
               *cjob << "\n*** ERROR in mv_normal_logpdf: illegal argument\n";
            }
            return FALSE;
         }
      }
   }

   REAL* mu      = new REAL[dim];
   REAL* sig     = new REAL[dim];
   REAL* var     = new REAL[dim*dim];
   REAL* work    = new REAL[dim];
   INTEGER* ipvt = new INTEGER[dim];

   if ( means == 0 )
   {
      for ( i=0; i<dim; i++ ) mu[i] = 0;
   }
   else
   {
      for ( i=0; i<dim; i++ ) mu[i] = means[i];
   }
   if ( sigmas == 0 )
   {
      for ( i=0; i<dim; i++ ) sig[i] = 1;
   }
   else
   {
      for ( i=0; i<dim; i++ ) sig[i] = sigmas[i];
   }
   if ( corr == 0 )
   {
      for ( i=0; i<dim*dim; i++ ) var[i] = 0;
      for ( i=0; i<dim; i++ )     var[i+dim*i] = sig[i]*sig[i];
   }
   else
   {
      for ( i=0; i<dim; i++ )
      {
         for ( j=0; j<dim; j++ )
         {
            var[i+dim*j] = corr[i+dim*j]*sig[i]*sig[j];
         }
      }
   }

   const REAL sqrtwopiinv = 0.398942280;
   *logpdf = 0;
   for ( i=0; i<dim; i++ ) *logpdf = *logpdf + log(sqrtwopiinv);
   REAL det[2]; REAL rcond;
   dgeco(var,dim,dim,ipvt,&rcond,work);
   dgedi(var,dim,dim,ipvt,det,work,11);
   if ( 1+rcond == 1 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in mv_normal_logpdf: covariance matrix is singular";
      }
      return FALSE;
   }
   *logpdf = *logpdf - 0.5*(log(det[0]) + det[1]*log(10.0));
   REAL sum = 0;
   for ( i=0; i<dim; i++ )
   {
      for ( j=0; j<dim; j++ )
      {
         sum += var[i+dim*j]*(x[i]-mu[i])*(x[j]-mu[j]);
      }
   }
   *logpdf = *logpdf - 0.5*sum;

   delete [] mu;
   delete [] sig;
   delete [] var;
   delete [] work;
   delete [] ipvt;

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

CARDLONG mv_normal_randsamp(CARDLONG dim, CARDLONG maxsize, SAMPTECH samptech,
                            REAL** sample, 
                            REAL* means, REAL* sigmas, REAL* corr,
                            std::ostream* cjob)
{
   CARDLONG i,j;
   if ( dim==0 || maxsize==0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in mv_normal_randsamp: illegal argument\n";
      }
      return 0;
   }
   if ( sigmas != 0 )
   {
      for ( i=0; i<dim; i++ )
      {
         if ( sigmas[i] <= 0 )
         {
            if ( cjob != 0 )
            {
               *cjob << "\n*** ERROR in mv_normal_randsamp: illegal argument\n";
            }
            return 0;
         }
      }
   }

   REAL* mu      = new REAL[dim];
   REAL* sig     = new REAL[dim];
   REAL* a       = new REAL[dim*dim];
   REAL* work    = new REAL[dim];
   INTEGER* ipvt = new INTEGER[dim];

   // calculate means, sigmas and covariance matrix

   if ( means == 0 )
   {
      for ( i=0; i<dim; i++ ) mu[i] = 0;
   }
   else
   {
      for ( i=0; i<dim; i++ ) mu[i] = means[i];
   }
   if ( sigmas == 0 )
   {
      for ( i=0; i<dim; i++ ) sig[i] = 1;
   }
   else
   {
      for ( i=0; i<dim; i++ ) sig[i] = sigmas[i];
   }
   if ( corr == 0 )
   {
      for ( i=0; i<dim*dim; i++ ) a[i] = 0;
      for ( i=0; i<dim; i++ )     a[i+dim*i] = sig[i]*sig[i];
   }
   else
   {
      for ( i=0; i<dim; i++ )
      {
         for ( j=0; j<dim; j++ )
         {
            a[i+dim*j] = corr[i+dim*j]*sig[i]*sig[j];
         }
      }
   }

   // calculate Cholesky factor of covariance matrix

   INTEGER info;
   dchdc(a,dim,dim,work,ipvt,0,&info);

   if ( info < dim )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in mv_normal_randsamp: covariance matrix not positive definite\n";
      }
      return 0;
   }
   else
   {
      for ( i=0; i<dim; i++ )
      {
         for ( j=0; j<i; j++ ) a[i+dim*j] = 0;
      }
   }

   // generate multivariate uniform sample:

   CARDLONG sampsize;
   sampsize = mv_uniform_randsamp(dim,maxsize,samptech,
                                  sample,0,0,cjob);
   if ( sampsize == 0 ) 
   {
      delete [] mu;
      delete [] sig;
      delete [] a;
      delete [] work;
      delete [] ipvt;
      return 0;
   }

   // transform to multivariate standard normal sample:

   for ( j=0; j<sampsize; j++ )
   {
      for ( i=0; i<dim; i++ )
      {
         REAL oldsamp = sample[j][i];
         if ( normal_inv_cdf(oldsamp,&sample[j][i],0,1) == FALSE )
         {
            if ( cjob != 0 )
            {
               *cjob << "\n*** ERROR in mv_normal_randsamp:"
                     << " problem transforming uniform to normal\n";
            }
            return 0;
         }
      }
   }

   // transform to multivariate normal sample with given means and covariance:

   REAL* oldsamp = new REAL[dim];
   for ( j=0; j<sampsize; j++ )
   {
      for ( i=0; i<dim; i++ ) oldsamp[i] = sample[j][i];
      for ( i=0; i<dim; i++ ) sample[j][i] = mu[i];
      for ( i=0; i<dim; i++ )
      {
         for ( CARDLONG k=0; k<dim; k++ )
         {
            sample[j][i] += a[k+dim*i]*oldsamp[k]; // use transpose of a!!
         }
      }
   }

   delete [] mu;
   delete [] sig;
   delete [] a;
   delete [] work;
   delete [] ipvt;
   delete [] oldsamp;

   return sampsize;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN mv_lognormal_pdf(CARDLONG dim, REAL* x, REAL* pdf,
                         REAL* means, REAL* sigmas, REAL* corr,
                         std::ostream* cjob) 
{
   CARDLONG i,j;
   if ( dim==0 || x==0 || pdf==0 || means==0 || sigmas==0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in mv_lognormal_pdf: illegal argument\n";
      }
      return FALSE;
   }
   for ( i=0; i<dim; i++ )
   {
      if ( means[i]<=0 || sigmas[i] <= 0 )
      {
         if ( cjob != 0 )
         {
            *cjob << "\n*** ERROR in mv_lognormal_pdf: illegal argument\n";
         }
         return FALSE;
      }
   }
   for ( i=0; i<dim; i++ ) 
   {
      if ( x[i] <= 0 )
      {
         *pdf = 0; return TRUE;
      }
   }

   REAL* x_normal      = new REAL[dim];
   REAL* means_normal  = new REAL[dim];
   REAL* sigmas_normal = new REAL[dim];
   REAL* corr_normal   = new REAL[dim*dim];

   for ( i=0; i<dim; i++ )
   {
      x_normal[i] = log(x[i]);
      sigmas_normal[i] = sqrt(log(1+sigmas[i]*sigmas[i]/(means[i]*means[i])));
      means_normal[i]  = log(means[i])-0.5*sigmas_normal[i]*sigmas_normal[i];
   }
   if ( corr == 0 )
   {
      delete [] corr_normal; corr_normal = 0;
   }
   else
   {
      for ( i=0; i<dim; i++ )
      {
         for ( j=0; j<dim; j++ )
         {
            if ( i == j )
            {
               corr_normal[i+j*dim] = 1;
            }
            else
            {
               corr_normal[i+j*dim] 
                  = 1/(sigmas_normal[i]*sigmas_normal[j])
                     *log(corr[i+j*dim]
                          *sqrt(exp(sigmas_normal[i]*sigmas_normal[i])-1)
                          *sqrt(exp(sigmas_normal[j]*sigmas_normal[j])-1) + 1);
               if ( fabs(corr_normal[i+j*dim]) >= 1.0 )
               {
                  if ( cjob != 0 )
                  {
                     *cjob << "\n*** ERROR in mv_lognormal_pdf: correlation"
                           << " too strong\n";
                  }
                  return FALSE;
               }
            }
         }
      }
   }
   if ( mv_normal_pdf(dim,x_normal,pdf,means_normal,
                      sigmas_normal,corr_normal,cjob) == FALSE )
      return FALSE;
   for ( i=0; i<dim; i++ ) *pdf = *pdf/x[i];

   delete [] x_normal;
   delete [] means_normal;
   delete [] sigmas_normal;
   delete [] corr_normal;

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN mv_lognormal_logpdf(CARDLONG dim, REAL* x, REAL* logpdf,
                            REAL* means, REAL* sigmas, REAL* corr,
                            std::ostream* cjob) 
{
   CARDLONG i,j;
   if ( dim==0 || x==0 || logpdf==0 || means==0 || sigmas==0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in mv_lognormal_logpdf: illegal argument\n";
      }
      return FALSE;
   }
   for ( i=0; i<dim; i++ )
   {
      if ( means[i]<=0 || sigmas[i] <= 0 )
      {
         if ( cjob != 0 )
         {
            *cjob << "\n*** ERROR in mv_lognormal_logpdf: illegal argument\n";
         }
         return FALSE;
      }
   }
   for ( i=0; i<dim; i++ ) 
   {
      if ( x[i] <= 0 )
      {
         *logpdf = 0; return FALSE;
      }
   }

   REAL* x_normal      = new REAL[dim];
   REAL* means_normal  = new REAL[dim];
   REAL* sigmas_normal = new REAL[dim];
   REAL* corr_normal   = new REAL[dim*dim];

   for ( i=0; i<dim; i++ )
   {
      x_normal[i] = log(x[i]);
      sigmas_normal[i] = sqrt(log(1+sigmas[i]*sigmas[i]/(means[i]*means[i])));
      means_normal[i]  = log(means[i])-0.5*sigmas_normal[i]*sigmas_normal[i];
   }
   if ( corr == 0 )
   {
      delete [] corr_normal; corr_normal = 0;
   }
   else
   {
      for ( i=0; i<dim; i++ )
      {
         for ( j=0; j<dim; j++ )
         {
            if ( i == j )
            {
               corr_normal[i+j*dim] = 1;
            }
            else
            {
               corr_normal[i+j*dim] 
                  = 1/(sigmas_normal[i]*sigmas_normal[j])
                     *log(corr[i+j*dim]
                          *sqrt(exp(sigmas_normal[i]*sigmas_normal[i])-1)
                          *sqrt(exp(sigmas_normal[j]*sigmas_normal[j])-1) + 1);
               if ( fabs(corr_normal[i+j*dim]) >= 1.0 )
               {
                  if ( cjob != 0 )
                  {
                     *cjob << "\n*** ERROR in mv_lognormal_logpdf: correlation"
                           << " too strong\n";
                  }
                  return FALSE;
               }
            }
         }
      }
   }
   if ( mv_normal_logpdf(dim,x_normal,logpdf,means_normal,
                         sigmas_normal,corr_normal,cjob) == FALSE )
      return FALSE;
   for ( i=0; i<dim; i++ ) *logpdf = *logpdf - log(x[i]);

   delete [] x_normal;
   delete [] means_normal;
   delete [] sigmas_normal;
   delete [] corr_normal;

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

CARDLONG mv_lognormal_randsamp(CARDLONG dim, CARDLONG maxsize, SAMPTECH samptech,
                               REAL** sample, 
                               REAL* means, REAL* sigmas, REAL* corr,
                               std::ostream* cjob)
{
   CARDLONG i,j;
   if ( dim==0 || means==0 || sigmas==0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in mv_lognormal_randsamp: illegal argument\n";
      }
      return 0;
   }
   for ( i=0; i<dim; i++ )
   {
      if ( means[i]<=0 || sigmas[i]<=0 )
      {
         if ( cjob != 0 )
         {
            *cjob << "\n*** ERROR in mv_lognormal_randsamp: illegal argument\n";
         }
         return 0;
      }
   }

   REAL* means_normal  = new REAL[dim];
   REAL* sigmas_normal = new REAL[dim];
   REAL* corr_normal   = new REAL[dim*dim];

   for ( i=0; i<dim; i++ )
   {
      sigmas_normal[i] = sqrt(log(1+sigmas[i]*sigmas[i]/(means[i]*means[i])));
      means_normal[i]  = log(means[i])-0.5*sigmas_normal[i]*sigmas_normal[i];
   }
   if ( corr == 0 )
   {
      delete [] corr_normal; corr_normal = 0;
   }
   else
   {
      for ( i=0; i<dim; i++ )
      {
         for ( j=0; j<dim; j++ )
         {
            if ( i == j )
            {
               corr_normal[i+j*dim] = 1;
            }
            else
            {
               corr_normal[i+j*dim] 
                  = 1/(sigmas_normal[i]*sigmas_normal[j])
                     *log(corr[i+j*dim]
                          *sqrt(exp(sigmas_normal[i]*sigmas_normal[i])-1)
                          *sqrt(exp(sigmas_normal[j]*sigmas_normal[j])-1) + 1);
               if ( fabs(corr_normal[i+j*dim]) >= 1.0 )
               {
                  if ( cjob != 0 )
                  {
                     *cjob << "\n*** ERROR in mv_lognormal_randsamp: correlation"
                           << " too strong\n";
                  }
                  return 0;
               }
            }
         }
      }
   }
   CARDLONG sampsize 
      = mv_normal_randsamp(dim,maxsize,samptech,sample,
                           means_normal,sigmas_normal,corr_normal,cjob);
   for ( j=0; j<sampsize; j++ )
   {
      for ( i=0; i<dim; i++ )
      {
         sample[j][i] = exp(sample[j][i]);
      }
   }

   delete [] means_normal;
   delete [] sigmas_normal;
   delete [] corr_normal;

   return sampsize;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN mv_copula_pdf(CARDLONG dim, REAL* x, REAL* pdf,
                      UV_DISTTYPE* marginals, REAL** pars, REAL* corr,
                      std::ostream* cjob)
{
   CARDLONG i,j;
   if ( dim==0 || x==0 || pdf==0 || marginals==0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in mv_copula_pdf: illegal argument\n";
      }
      return FALSE;
   }

   REAL* R       = new REAL[dim*dim];
   REAL* y       = new REAL[dim];
   REAL* work    = new REAL[dim];
   INTEGER* ipvt = new INTEGER[dim];

   if ( corr == 0 )
   {
      for ( i=0; i<dim*dim; i++ ) R[i] = 0;
      for ( i=0; i<dim; i++ )     R[i+dim*i] = 1;
   }
   else
   {
      for ( i=0; i<dim; i++ )
      {
         for ( j=0; j<dim; j++ )
         {
            R[i+dim*j] = sin(pihalf*corr[i+dim*j]);
         }
      }
   }

   REAL* pdfmarg = new REAL[dim];
   REAL* cdfmarg = new REAL[dim];
   for ( i=0; i<dim; i++ )
   {
      switch ( marginals[i] )
      {
         case UV_DELTA:
            {
               REAL value = 0;
               if ( pars != 0 )
               {
                  if ( pars[i] != 0 )
                  {
                     value = pars[i][0];
                  }
               }
               if ( delta_pdf(x[i],&pdfmarg[i],value,cjob)
                    == FALSE ) return FALSE;
               if ( delta_cdf(x[i],&cdfmarg[i],value,cjob)
                    == FALSE ) return FALSE;
            }
            break;
         case UV_UNIFORM:
            {
               REAL lowerbound = 0;
               REAL upperbound = 1;
               if ( pars != 0 )
               {
                  if ( pars[i] != 0 )
                  {
                     lowerbound = pars[i][0];
                     upperbound = pars[i][1];
                  }
               }
               if ( uniform_pdf(x[i],&pdfmarg[i],lowerbound,upperbound,cjob)
                    == FALSE ) return FALSE;
               if ( uniform_cdf(x[i],&cdfmarg[i],lowerbound,upperbound,cjob)
                    == FALSE ) return FALSE;
            }
            break;
         case UV_NORMAL:
            {
               REAL mean = 0;
               REAL sigma = 1;
               if ( pars != 0 )
               {
                  if ( pars[i] != 0 )
                  {
                     mean = pars[i][0];
                     sigma = pars[i][1];
                  }
               }
               if ( normal_pdf(x[i],&pdfmarg[i],mean,sigma,cjob)
                    == FALSE ) return FALSE;
               if ( normal_cdf(x[i],&cdfmarg[i],mean,sigma,cjob)
                    == FALSE ) return FALSE;
            }
            break;
         case UV_LOGNORMAL:
            {
               REAL mean = 1;
               REAL sigma = 1;
               if ( pars != 0 )
               {
                  if ( pars[i] != 0 )
                  {
                     mean = pars[i][0];
                     sigma = pars[i][1];
                  }
               }
               if ( lognormal_pdf(x[i],&pdfmarg[i],mean,sigma,cjob)
                    == FALSE ) return FALSE;
               if ( lognormal_cdf(x[i],&cdfmarg[i],mean,sigma,cjob)
                    == FALSE ) return FALSE;
            }
            break;
         case UV_GAMMA:
            {
               REAL mean = 1;
               REAL sigma = 1;
               if ( pars != 0 )
               {
                  if ( pars[i] != 0 )
                  {
                     mean = pars[i][0];
                     sigma = pars[i][1];
                  }
               }
               if ( gamma_pdf(x[i],&pdfmarg[i],mean,sigma,cjob)
                    == FALSE ) return FALSE;
               if ( gamma_cdf(x[i],&cdfmarg[i],mean,sigma,cjob)
                    == FALSE ) return FALSE;
            }
            break;
         case UV_TRIANGULAR:
            {
               REAL mode  = 0.0;
               REAL left  = 0.5;
               REAL right = 0.5;
               if ( pars != 0 )
               {
                  if ( pars[i] != 0 )
                  {
                     mode  = pars[i][0];
                     left  = pars[i][1];
                     right = pars[i][2];
                  }
               }
               if ( triangular_pdf(x[i],&pdfmarg[i],mode,left,right,cjob)
                    == FALSE ) return FALSE;
               if ( triangular_cdf(x[i],&cdfmarg[i],mode,left,right,cjob)
                    == FALSE ) return FALSE;
            }
            break;
         case UV_EMPIRICAL:
            {
               if ( pars==0 || pars[i]==0 )
               {
                  if ( cjob != 0 )
                  {
                     *cjob << "\n*** ERROR in mv_copula_pdf: parameters not found\n";
                  }
                  return FALSE;
               }
               CARDLONG numfract = (CARDLONG) (pars[i][0]+0.1);
               REAL* levels = new REAL[numfract];
               REAL* fract  = new REAL[numfract];
               for ( j=0; j<numfract; j++ )
               {
                  levels[j] = pars[i][2*j+1];
                  fract[j]  = pars[i][2*j+2];
               }
               if ( empirical_pdf(x[i],&pdfmarg[i],numfract,levels,fract,cjob)
                    == FALSE ) return FALSE;
               if ( empirical_cdf(x[i],&cdfmarg[i],numfract,levels,fract,cjob)
                    == FALSE ) return FALSE;
               delete [] levels;
               delete [] fract;
            }
            break;
         default:
            if ( cjob != 0 )
            {
               *cjob << "\n*** ERROR in mv_copula_pdf: unknown marginal distribution type\n";
            }
            return FALSE;
      }
   }
   for ( i=0; i<dim; i++ )
   {
      if ( cdfmarg[i]<=0 || cdfmarg[i]>=1 )
      {
         *pdf = 0;
         delete [] R;
         delete [] y;
         delete [] pdfmarg;
         delete [] cdfmarg;
         delete [] work;
         delete [] ipvt;
         return TRUE;
      }
   }
   *pdf = 1;
   for ( i=0; i<dim; i++ ) *pdf = *pdf*pdfmarg[i];
   REAL det[2]; REAL rcond;
   dgeco(R,dim,dim,ipvt,&rcond,work);
   dgedi(R,dim,dim,ipvt,det,work,11);
   if ( 1+rcond == 1 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in mv_copula_pdf: correlation matrix is singular\n";
      }
      return FALSE;
   }
   const REAL ten = 10.0;
   *pdf = *pdf/sqrt(det[0]*pow(ten,det[1]));
   for ( i=0; i<dim; i++ ) R[i+dim*i] -= 1.0;
   for ( i=0; i<dim; i++ )
   {
      if ( normal_inv_cdf(cdfmarg[i],&y[i],0,1,cjob) == FALSE ) return FALSE;
   }
   for ( i=0; i<dim; i++ )
   {
      work[i] = 0;
      for ( j=0; j<dim; j++ )
      {
         work[i] += R[i+dim*j]*y[j];
      }
   }
   REAL sum = 0;
   for ( i=0; i<dim; i++ ) sum += y[i]*work[i];
   *pdf = *pdf*exp(-0.5*sum);

   delete [] R;
   delete [] y;
   delete [] pdfmarg;
   delete [] cdfmarg;
   delete [] work;
   delete [] ipvt;

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////


BOOLEAN mv_copula_logpdf(CARDLONG dim, REAL* x, REAL* logpdf,
                         UV_DISTTYPE* marginals, REAL** pars, REAL* corr,
                         std::ostream* cjob)
{
   CARDLONG i,j;
   if ( dim==0 || x==0 || logpdf==0 || marginals==0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in mv_copula_logpdf: illegal argument\n";
      }
      return FALSE;
   }

   REAL* R       = new REAL[dim*dim];
   REAL* y       = new REAL[dim];
   REAL* work    = new REAL[dim];
   INTEGER* ipvt = new INTEGER[dim];

   if ( corr == 0 )
   {
      for ( i=0; i<dim*dim; i++ ) R[i] = 0;
      for ( i=0; i<dim; i++ )     R[i+dim*i] = 1;
   }
   else
   {
      for ( i=0; i<dim; i++ )
      {
         for ( j=0; j<dim; j++ )
         {
            R[i+dim*j] = sin(pihalf*corr[i+dim*j]);
         }
      }
   }

   REAL* logpdfmarg = new REAL[dim];
   REAL* cdfmarg    = new REAL[dim];
   for ( i=0; i<dim; i++ )
   {
      switch ( marginals[i] )
      {
         case UV_DELTA:
            {
               REAL value = 0;
               if ( pars != 0 )
               {
                  if ( pars[i] != 0 )
                  {
                     value = pars[i][0];
                  }
               }
               if ( delta_logpdf(x[i],&logpdfmarg[i],value,cjob)
                    == FALSE ) return FALSE;
               if ( delta_cdf(x[i],&cdfmarg[i],value,cjob)
                    == FALSE ) return FALSE;
            }
            break;
         case UV_UNIFORM:
            {
               REAL lowerbound = 0;
               REAL upperbound = 1;
               if ( pars != 0 )
               {
                  if ( pars[i] != 0 )
                  {
                     lowerbound = pars[i][0];
                     upperbound = pars[i][1];
                  }
               }
               if ( uniform_logpdf(x[i],&logpdfmarg[i],lowerbound,upperbound,cjob)
                    == FALSE ) return FALSE;
               if ( uniform_cdf(x[i],&cdfmarg[i],lowerbound,upperbound,cjob)
                    == FALSE ) return FALSE;
            }
            break;
         case UV_NORMAL:
            {
               REAL mean = 0;
               REAL sigma = 1;
               if ( pars != 0 )
               {
                  if ( pars[i] != 0 )
                  {
                     mean = pars[i][0];
                     sigma = pars[i][1];
                  }
               }
               if ( normal_logpdf(x[i],&logpdfmarg[i],mean,sigma,cjob)
                    == FALSE ) return FALSE;
               if ( normal_cdf(x[i],&cdfmarg[i],mean,sigma,cjob)
                    == FALSE ) return FALSE;
            }
            break;
         case UV_LOGNORMAL:
            {
               REAL mean = 1;
               REAL sigma = 1;
               if ( pars != 0 )
               {
                  if ( pars[i] != 0 )
                  {
                     mean = pars[i][0];
                     sigma = pars[i][1];
                  }
               }
               if ( lognormal_logpdf(x[i],&logpdfmarg[i],mean,sigma,cjob)
                    == FALSE ) return FALSE;
               if ( lognormal_cdf(x[i],&cdfmarg[i],mean,sigma,cjob)
                    == FALSE ) return FALSE;
            }
            break;
         case UV_GAMMA:
            {
               REAL mean = 1;
               REAL sigma = 1;
               if ( pars != 0 )
               {
                  if ( pars[i] != 0 )
                  {
                     mean = pars[i][0];
                     sigma = pars[i][1];
                  }
               }
               if ( gamma_logpdf(x[i],&logpdfmarg[i],mean,sigma,cjob)
                    == FALSE ) return FALSE;
               if ( gamma_cdf(x[i],&cdfmarg[i],mean,sigma,cjob)
                    == FALSE ) return FALSE;
            }
            break;
         case UV_TRIANGULAR:
            {
               REAL mode  = 0.0;
               REAL left  = 0.5;
               REAL right = 0.5;
               if ( pars != 0 )
               {
                  if ( pars[i] != 0 )
                  {
                     mode  = pars[i][0];
                     left  = pars[i][1];
                     right = pars[i][2];
                  }
               }
               if ( triangular_logpdf(x[i],&logpdfmarg[i],mode,left,right,cjob)
                    == FALSE ) return FALSE;
               if ( triangular_cdf(x[i],&cdfmarg[i],mode,left,right,cjob)
                    == FALSE ) return FALSE;
            }
            break;
         case UV_EMPIRICAL:
            {
               if ( pars==0 || pars[i]==0 )
               {
                  if ( cjob != 0 )
                  {
                     *cjob << "\n*** ERROR in mv_copula_logpdf: parameters not found\n";
                  }
                  return FALSE;
               }
               CARDLONG numfract = (CARDLONG) (pars[i][0]+0.1);
               REAL* levels = new REAL[numfract];
               REAL* fract  = new REAL[numfract];
               for ( j=0; j<numfract; j++ )
               {
                  levels[j] = pars[i][2*j+1];
                  fract[j]  = pars[i][2*j+2];
               }
               if ( empirical_logpdf(x[i],&logpdfmarg[i],numfract,levels,fract,cjob)
                    == FALSE ) return FALSE;
               if ( empirical_cdf(x[i],&cdfmarg[i],numfract,levels,fract,cjob)
                    == FALSE ) return FALSE;
               delete [] levels;
               delete [] fract;
            }
            break;
         default:
            if ( cjob != 0 )
            {
               *cjob << "\n*** ERROR in mv_copula_logpdf: unknown marginal distribution type\n";
            }
            return FALSE;
      }
   }
   for ( i=0; i<dim; i++ )
   {
      if ( cdfmarg[i]<=0 || cdfmarg[i]>=1 )
      {
         *logpdf = 0;
         delete [] R;
         delete [] y;
         delete [] logpdfmarg;
         delete [] cdfmarg;
         delete [] work;
         delete [] ipvt;
         return FALSE;
      }
   }
   *logpdf = 0;
   for ( i=0; i<dim; i++ ) *logpdf = *logpdf + logpdfmarg[i];
   REAL det[2]; REAL rcond;
   dgeco(R,dim,dim,ipvt,&rcond,work);
   dgedi(R,dim,dim,ipvt,det,work,11);
   if ( 1+rcond == 1 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in mv_copula_logpdf: correlation matrix is singular\n";
      }
      return FALSE;
   }
   *logpdf = *logpdf - 0.5*(log(det[0]) + det[1]*log(10.0));
   for ( i=0; i<dim; i++ ) R[i+dim*i] -= 1.0;
   for ( i=0; i<dim; i++ )
   {
      if ( normal_inv_cdf(cdfmarg[i],&y[i],0,1,cjob) == FALSE ) return FALSE;
   }
   for ( i=0; i<dim; i++ )
   {
      work[i] = 0;
      for ( j=0; j<dim; j++ )
      {
         work[i] += R[i+dim*j]*y[j];
      }
   }
   REAL sum = 0;
   for ( i=0; i<dim; i++ ) sum += y[i]*work[i];
   *logpdf = *logpdf - 0.5*sum;

   delete [] R;
   delete [] y;
   delete [] logpdfmarg;
   delete [] cdfmarg;
   delete [] work;
   delete [] ipvt;

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

CARDLONG mv_copula_randsamp(CARDLONG dim, CARDLONG maxsize, SAMPTECH samptech,
                            REAL** sample, 
                            UV_DISTTYPE* marginals, REAL** pars, REAL* corr,
                            std::ostream* cjob)
{
   CARDLONG i,j;
   if ( dim==0 || maxsize==0 || marginals==0 )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in mv_copula_randsamp: illegal argument\n";
      }
      return 0;
   }

   REAL* a       = new REAL[dim*dim];
   REAL* work    = new REAL[dim];
   INTEGER* ipvt = new INTEGER[dim];

   if ( corr == 0 )
   {
      for ( i=0; i<dim*dim; i++ ) a[i] = 0;
      for ( i=0; i<dim; i++ )     a[i+dim*i] = 1.0;
   }
   else
   {
      for ( i=0; i<dim; i++ )
      {
         for ( j=0; j<dim; j++ )
         {
            a[i+dim*j] = sin(pihalf*corr[i+dim*j]);
         }
      }
   }

   // calculate Cholesky factor of correlation matrix

   INTEGER info;
   dchdc(a,dim,dim,work,ipvt,0,&info);

   if ( info < dim )
   {
      if ( cjob != 0 )
      {
         *cjob << "\n*** ERROR in mv_copula_randsamp: correlation matrix not positive definite\n";
      }
      return 0;
   }
   else
   {
      for ( i=0; i<dim; i++ )
      {
         for ( j=0; j<i; j++ ) a[i+dim*j] = 0;
      }
   }

   // generate multivariate uniform sample:

   CARDLONG sampsize;
   sampsize = mv_uniform_randsamp(dim,maxsize,samptech,
                                  sample,0,0,cjob);
   if ( sampsize == 0 ) 
   {
      delete [] a;
      delete [] work;
      delete [] ipvt;
      return 0;
   }

   // transform to multivariate standard normal sample:

   for ( j=0; j<sampsize; j++ )
   {
      for ( i=0; i<dim; i++ )
      {
         REAL oldsamp = sample[j][i];
         if ( normal_inv_cdf(oldsamp,&sample[j][i],0,1) == FALSE )
         {
            if ( cjob != 0 )
            {
               *cjob << "\n*** ERROR in mv_copula_randsamp:"
                     << " problem transforming uniform to normal\n";
            }
            return 0;
         }
      }
   }

   // transform to multivariate uniform sample with given rank correlation 
   // coefficients:

   REAL* oldsamp = new REAL[dim];
   for ( j=0; j<sampsize; j++ )
   {
      for ( i=0; i<dim; i++ ) oldsamp[i] = sample[j][i];
      for ( i=0; i<dim; i++ )
      {
	 sample[j][i] = 0;
         for ( CARDLONG k=0; k<dim; k++ )
         {
            sample[j][i] += a[k+dim*i]*oldsamp[k]; // use transpose of a!!
         }
         REAL sampnorm = sample[j][i];
         if ( normal_cdf(sampnorm,&sample[j][i],0,1,cjob) == FALSE ) 
            return FALSE;
      }
   }

   // transform to given marginals:

   for ( i=0; i<dim; i++ )
   {
      switch ( marginals[i] )
      {
         case UV_DELTA:
            {
               REAL value = 0;
               if ( pars != 0 )
               {
                  if ( pars[i] != 0 )
                  {
                     value = pars[i][0];
                  }
               }
               for ( j=0; j<sampsize; j++ )
			   {
		          REAL sampuniform = sample[j][i];
                  if ( delta_inv_cdf(sampuniform,&sample[j][i],
                                     value,cjob)
                       == FALSE ) return 0;
               }
            }
            break;
         case UV_UNIFORM:
            {
               REAL lowerbound = 0;
               REAL upperbound = 1;
               if ( pars != 0 )
               {
                  if ( pars[i] != 0 )
                  {
                     lowerbound = pars[i][0];
                     upperbound = pars[i][1];
                  }
               }
               for ( j=0; j<sampsize; j++ )
	       {
		       REAL sampuniform = sample[j][i];
                  if ( uniform_inv_cdf(sampuniform,&sample[j][i],
                                       lowerbound,upperbound,cjob)
                       == FALSE ) return 0;
               }
            }
            break;
         case UV_NORMAL:
            {
               REAL mean = 0;
               REAL sigma = 1;
               if ( pars != 0 )
               {
                  if ( pars[i] != 0 )
                  {
                     mean = pars[i][0];
                     sigma = pars[i][1];
                  }
               }
               for ( j=0; j<sampsize; j++ )
	       {
		  REAL sampuniform = sample[j][i];
                  if ( normal_inv_cdf(sampuniform,&sample[j][i],
                                      mean,sigma,cjob)
                       == FALSE ) return 0;
               }
            }
            break;
         case UV_LOGNORMAL:
            {
               REAL mean = 1;
               REAL sigma = 1;
               if ( pars != 0 )
               {
                  if ( pars[i] != 0 )
                  {
                     mean = pars[i][0];
                     sigma = pars[i][1];
                  }
               }
               for ( j=0; j<sampsize; j++ )
	       {
		  REAL sampuniform = sample[j][i];
                  if ( lognormal_inv_cdf(sampuniform,&sample[j][i],
                                         mean,sigma,cjob)
                       == FALSE ) return 0;
               }
            }
            break;
         case UV_GAMMA:
            {
               REAL mean = 1;
               REAL sigma = 1;
               if ( pars != 0 )
               {
                  if ( pars[i] != 0 )
                  {
                     mean = pars[i][0];
                     sigma = pars[i][1];
                  }
               }
               for ( j=0; j<sampsize; j++ )
	       {
		  REAL sampuniform = sample[j][i];
                  if ( gamma_inv_cdf(sampuniform,&sample[j][i],
                                     mean,sigma,cjob)
                       == FALSE ) return 0;
               }
            }
            break;
         case UV_TRIANGULAR:
            {
               REAL mode  = 0.5;
               REAL left  = 0.5;
               REAL right = 0.5;
               if ( pars != 0 )
               {
                  if ( pars[i] != 0 )
                  {
                     mode  = pars[i][0];
                     left  = pars[i][1];
                     right = pars[i][2];
                  }
               }
               for ( j=0; j<sampsize; j++ )
	       {
		  REAL sampuniform = sample[j][i];
                  if ( triangular_inv_cdf(sampuniform,&sample[j][i],
                                          mode,left,right,cjob)
                       == FALSE ) return 0;
               }
            }
            break;
         case UV_EMPIRICAL:
            {
               if ( pars==0 || pars[i]==0 )
               {
                  if ( cjob != 0 )
                  {
                     *cjob << "\n*** ERROR in mv_copula_randsamp: "
                           << "missing parameter for empirical marginal\n";
                  }
               }
               CARDLONG numfract = (CARDINAL)(pars[i][0]+0.1);
               REAL* levels = new REAL[numfract];
               REAL* fract  = new REAL[numfract];
               for ( j=0; j<numfract; j++ )
               {
                  levels[j] = pars[i][2*j+1];
                  fract[j]  = pars[i][2*j+2];
               }               
               for ( j=0; j<sampsize; j++ )
	       {
		  REAL sampuniform = sample[j][i];
                  if ( empirical_inv_cdf(sampuniform,&sample[j][i],
                                         numfract,levels,fract,cjob)
                       == FALSE ) return 0;
               }
               delete [] levels;
               delete [] fract;
            }
            break;
         default:
            if ( cjob != 0 )
            {
               *cjob << "\n*** ERROR in mv_copula_randsamp: unknown marginal distribution type\n";
            }
	    return FALSE;
            break;
      }   
   }

   delete [] a;
   delete [] work;
   delete [] ipvt;
   delete [] oldsamp;

   return sampsize;
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN mv_pdf(DISTRIBUTION& dist, REAL* x, REAL* pdf,
               std::ostream* cjob)
{
   if ( x == 0 ) return FALSE;
   CARDINAL dim = dist.GetDim();
   if ( dim == 0 ) return FALSE;
   CARDINAL i;
   switch ( dist.GetType() )
   {
      case MV_DELTA:
         {
            REAL* varval = new REAL[dim];
            for ( i=0; i<dim; i++ )
            {
               varval[i] = dist.GetVarPars(i)[0];
            }
            if ( mv_delta_pdf(dim,x,pdf,
                              varval,cjob) == FALSE )
               return FALSE;
            delete [] varval;
         }
         break;
      case MV_UNIFORM:
         {
            REAL* varmin = new REAL[dim];
            REAL* varmax = new REAL[dim];
            for ( i=0; i<dim; i++ )
            {
               varmin[i] = dist.GetVarPars(i)[0];
               varmax[i] = dist.GetVarPars(i)[1];
            }
            if ( mv_uniform_pdf(dim,x,pdf,
                                varmin,varmax,cjob) == FALSE )
               return FALSE;
            delete [] varmin;
            delete [] varmax;
         }
         break;
      case MV_NORMAL:
         {
            REAL* means  = new REAL[dim];
            REAL* sigmas = new REAL[dim];
            REAL* corr = dist.GetCorr();
            for ( i=0; i<dim; i++ )
            {
               means[i]  = dist.GetVarPars(i)[0];
               sigmas[i] = dist.GetVarPars(i)[1];
            }
            if ( mv_normal_pdf(dim,x,pdf,
                               means,sigmas,corr,cjob) == FALSE )
               return FALSE;
            delete [] means;
            delete [] sigmas;
         }
         break;
      case MV_LOGNORMAL:
         {
            REAL* means  = new REAL[dim];
            REAL* sigmas = new REAL[dim];
            REAL* corr = dist.GetCorr();
            for ( i=0; i<dim; i++ )
            {
               means[i]  = dist.GetVarPars(i)[0];
               sigmas[i] = dist.GetVarPars(i)[1];
            }
            if ( mv_lognormal_pdf(dim,x,pdf,
                                  means,sigmas,corr,cjob) == FALSE )
               return FALSE;
            delete [] means;
            delete [] sigmas;
         }
         break;
      case MV_COPULA:
         {
            REAL* corr = dist.GetCorr();
            REAL** parvals = dist.GetVarPars();
            UV_DISTTYPE* marginals = dist.GetVarDists();
            if ( mv_copula_pdf(dim,x,pdf,
                               marginals,parvals,corr,cjob) == FALSE )
               return FALSE;
         }
         break;
      default:
         return FALSE;
   }
   return TRUE;
}

BOOLEAN mv_logpdf(DISTRIBUTION& dist, REAL* x, REAL* logpdf,
                  std::ostream* cjob)
{
   if ( x == 0 ) return FALSE;
   CARDINAL dim = dist.GetDim();
   if ( dim == 0 ) return FALSE;
   CARDINAL i;
   switch ( dist.GetType() )
   {
      case MV_DELTA:
         {
            REAL* varval = new REAL[dim];
            for ( i=0; i<dim; i++ )
            {
               varval[i] = dist.GetVarPars(i)[0];
            }
            if ( mv_delta_logpdf(dim,x,logpdf,
                                 varval,cjob) == FALSE )
               return FALSE;
            delete [] varval;
         }
         break;
      case MV_UNIFORM:
         {
            REAL* varmin = new REAL[dim];
            REAL* varmax = new REAL[dim];
            for ( i=0; i<dim; i++ )
            {
               varmin[i] = dist.GetVarPars(i)[0];
               varmax[i] = dist.GetVarPars(i)[1];
            }
            if ( mv_uniform_logpdf(dim,x,logpdf,
                                   varmin,varmax,cjob) == FALSE )
               return FALSE;
            delete [] varmin;
            delete [] varmax;
         }
         break;
      case MV_NORMAL:
         {
            REAL* means  = new REAL[dim];
            REAL* sigmas = new REAL[dim];
            REAL* corr = dist.GetCorr();
            for ( i=0; i<dim; i++ )
            {
               means[i]  = dist.GetVarPars(i)[0];
               sigmas[i] = dist.GetVarPars(i)[1];
            }
            if ( mv_normal_logpdf(dim,x,logpdf,
                                  means,sigmas,corr,cjob) == FALSE )
               return FALSE;
            delete [] means;
            delete [] sigmas;
         }
         break;
      case MV_LOGNORMAL:
         {
            REAL* means  = new REAL[dim];
            REAL* sigmas = new REAL[dim];
            REAL* corr = dist.GetCorr();
            for ( i=0; i<dim; i++ )
            {
               means[i]  = dist.GetVarPars(i)[0];
               sigmas[i] = dist.GetVarPars(i)[1];
            }
            if ( mv_lognormal_logpdf(dim,x,logpdf,
                                     means,sigmas,corr,cjob) == FALSE )
               return FALSE;
            delete [] means;
            delete [] sigmas;
         }
         break;
      case MV_COPULA:
         {
            REAL* corr = dist.GetCorr();
            REAL** parvals = dist.GetVarPars();
            UV_DISTTYPE* marginals = dist.GetVarDists();
            if ( mv_copula_logpdf(dim,x,logpdf,
                                  marginals,parvals,corr,cjob) == FALSE )
               return FALSE;
         }
         break;
      default:
         return FALSE;
   }
   return TRUE;
}

CARDLONG mv_randsamp(DISTRIBUTION& dist, 
                     CARDLONG maxsize, SAMPTECH samptech, REAL** sample, 
                     std::ostream* cjob)
{
   CARDINAL dim = dist.GetDim();
   if ( dim == 0 ) return FALSE;
   CARDINAL i;
   CARDLONG sampsize = 0;
   switch ( dist.GetType() )
   {
      case MV_DELTA:
         {
            REAL* varval = new REAL[dim];
            for ( i=0; i<dim; i++ )
            {
               varval[i] = dist.GetVarPars(i)[0];
            }
            sampsize = mv_delta_randsamp(dim,maxsize,samptech,sample,
                                         varval,cjob);
            delete [] varval;
         }
         break;
      case MV_UNIFORM:
         {
            REAL* varmin = new REAL[dim];
            REAL* varmax = new REAL[dim];
            for ( i=0; i<dim; i++ )
            {
               varmin[i] = dist.GetVarPars(i)[0];
               varmax[i] = dist.GetVarPars(i)[1];
            }
            sampsize = mv_uniform_randsamp(dim,maxsize,samptech,sample,
                                           varmin,varmax,cjob);
            delete [] varmin;
            delete [] varmax;
         }
         break;
      case MV_NORMAL:
         {
            REAL* means  = new REAL[dim];
            REAL* sigmas = new REAL[dim];
            REAL* corr = dist.GetCorr();
            for ( i=0; i<dim; i++ )
            {
               means[i]  = dist.GetVarPars(i)[0];
               sigmas[i] = dist.GetVarPars(i)[1];
            }
            sampsize = mv_normal_randsamp(dim,maxsize,samptech,sample,
                                          means,sigmas,corr,cjob);
            delete [] means;
            delete [] sigmas;
         }
         break;
      case MV_LOGNORMAL:
         {
            REAL* means  = new REAL[dim];
            REAL* sigmas = new REAL[dim];
            REAL* corr = dist.GetCorr();
            for ( i=0; i<dim; i++ )
            {
               means[i]  = dist.GetVarPars(i)[0];
               sigmas[i] = dist.GetVarPars(i)[1];
            }
            sampsize = mv_lognormal_randsamp(dim,maxsize,samptech,sample,
                                             means,sigmas,corr,cjob);
            delete [] means;
            delete [] sigmas;
         }
         break;
      case MV_COPULA:
         {
            REAL* corr = dist.GetCorr();
            REAL** parvals = dist.GetVarPars();
            UV_DISTTYPE* marginals = dist.GetVarDists();
            sampsize = mv_copula_randsamp(dim,maxsize,samptech,sample,
                                          marginals,parvals,corr,cjob);
         }
         break;
      default:
         return FALSE;
   }
   return sampsize;
}

//////////////////////////////////////////////////////////////////////////////
