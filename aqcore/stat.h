/////////////////////////////////   stat.h   /////////////////////////////////
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

#ifndef STAT_H
#define STAT_H

#ifndef USE_TYPES_H
#include "all.h"
#else
#include "types.h"
#endif

//////////////////////////////////////////////////////////////////////////////

enum SAMPTECH { GRID, RANDOM, LHS, HAMMERSLEY, HALTON };

enum UV_DISTTYPE { UV_DELTA,
	               UV_UNIFORM, 
                   UV_NORMAL, 
                   UV_LOGNORMAL, 
                   UV_GAMMA, 
                   UV_TRIANGULAR, 
                   UV_EMPIRICAL };

enum MV_DISTTYPE { MV_DELTA,
	               MV_UNIFORM, 
                   MV_NORMAL, 
                   MV_LOGNORMAL, 
                   MV_COPULA };

//////////////////////////////////////////////////////////////////////////////


class DISTRIBUTION
{
   public:

      DISTRIBUTION();
      ~DISTRIBUTION();

      BOOLEAN      SetType(MV_DISTTYPE type);
      BOOLEAN      SetType(char* nametype);
      MV_DISTTYPE  GetType();
      CARDINAL     GetDim();
      CARDINAL     GetNumPars(MV_DISTTYPE type);  // returns 0 if unknown (copula)
      CARDINAL     GetNumPars(UV_DISTTYPE type);  // returns 0 if unknown (empirical)
      char*        GetVarName(CARDINAL i);
      char**       GetVarNames();
      REAL*        GetVarPars(CARDINAL i);
      REAL**       GetVarPars();
      UV_DISTTYPE* GetVarDists();
      REAL*        GetCorr();
      BOOLEAN      AddVar(char* name, UV_DISTTYPE type, 
                          CARDINAL numpars=0, REAL* pars=0, 
                          REAL* corrs=0);
      BOOLEAN      AddVar(char* name, char* nametype=0, 
                          CARDINAL numpars=0, REAL* pars=0, 
                          REAL* corrs=0);
	  void         Write(std::ostream* out);

      CARDINAL     GetNumParsCorr();
      CARDINAL     GetParValsCorr(REAL* par, CARDINAL dimpar);
      BOOLEAN      SetParValsCorr(REAL* par, CARDINAL num);
      CARDINAL     GetNumParsAll();
      CARDINAL     GetParValsAll(REAL* par, CARDINAL dimpar);
      BOOLEAN      SetParValsAll(REAL* par, CARDINAL num);
      BOOLEAN      GetParExInf(CARDINAL ind);
      BOOLEAN      GetParExSup(CARDINAL ind);
      REAL         GetParInf(CARDINAL ind);
      REAL         GetParSup(CARDINAL ind);

   protected:

      CARDINAL     dim;
      MV_DISTTYPE  mv_type;
      char**       varnames;
      UV_DISTTYPE* uv_types;
      CARDINAL*    uv_numpars;
      REAL**       uv_pars;
      REAL*        corr;

      CARDINAL     num_mv_dist;
      char**       mv_diststrings;
      CARDINAL     num_uv_dist;
      char**       uv_diststrings;
};


//////////////////////////////////////////////////////////////////////////////


// auxiliary functions:
// --------------------


REAL calc_sum(CARDLONG numsets, REAL* x);

     // returns the sum of values

     // numsets:     number of data points (input)
     // x:           array of data points (input)


REAL calc_mean(CARDLONG numsets, REAL* x, REAL* weights=0);

     // returns the mean of values with given weights

     // numsets:     number of data points (input)
     // x:           array of data points (input)
     // weights:     array of weights of data points (optional input)

     // weights need not be normalized
     // if weights=0, weights[i]=1/numsets is used


void calc_meancov(CARDLONG numsets, CARDLONG numvar, REAL** sample, 
                  REAL* weights,
                  REAL* means, REAL* stdevs, REAL* corr);

     // returns the means, standard deviations and correlation coefficients
     // of a multivariate sample

     // numsets:     number of data sets (input)
     // numvar:      number of variables (input)
     // sample:      array of numsets pointers to arrays of numpar
     //              sample values (input)
     // weights:     array of weights of data sets (input)
     // means:       array of calculated means (output)
     // stdevs:      array of calculated standard deviations (output)
     // corr:        array of calculated correlation coefficients
     //              corr(i,j) = corr[i+numvar*j]

     // weights need not be normalized
     // if weights=0, weights[i]=1/numsets is used


void calc_kendallcorr(CARDLONG numsets, CARDLONG numvar, REAL** sample, 
                      REAL* weights, REAL* corr);

     // returns the Kendall correlation coefficients
     // of a multivariate sample

     // numsets:     number of data sets (input)
     // numvar:      number of variables (input)
     // sample:      array of numsets pointers to arrays of numpar
     //              sample values (input)
     // weights:     array of weights of data sets (input)
     // corr:        array of calculated Kendall correlation coefficients
     //              corr(i,j) = corr[i+numvar*j]

     // weights need not be normalized
     // if weights=0, weights[i]=1/numsets is used


void calc_fract(CARDLONG numsets, REAL* x, REAL* weights,
                CARDLONG numfract, REAL* levels, REAL* fract);

     // returns the fractiles of an array of values with given weights

     // numsets:     number of data points (input)
     // x:           array of data points (input)
     // weights:     array of weights of data points (input)
     // numfract:    number of fractiles to be calculated (input)
     // levels:      array of levels of fractiles to be calculated (input)
     // fract:       array of calculated fractiles (output)

     // all levels must be in the interval [0,1]
     // weights need not be normalized
     // if weights=0, weights[i]=1/numsets is used


void calc_fracts(CARDLONG numsets, CARDLONG numvar, REAL** sample,
                 REAL* weights,
                 CARDLONG numfract, REAL* levels, REAL** fract);
                    
     // returns the fractiles of an array of values with given weights

     // numsets:     number of data sets (input)
     // numvar:      number of variables (input)
     // sample:      array of numsets pointers to arrays of numpar
     //              sample values (input)
     // weights:     array of weights of data sets (input)
     // numfract:    number of fractiles to be calculated (input)
     // levels:      array of levels of fractiles to be calculated (input)
     // fract:       array of numvar arrays of calculated fractiles (output)

     // all levels must be in the interval [0,1]
     // weights need not be normalized
     // if weights=0, weights[i]=1/numsets is used


void calc_hpdlevels(CARDLONG numsets, REAL* samppdf, REAL* pdf,
                    CARDLONG numlevels, REAL* problevels, REAL* pdflevels);
                    
     // returns the pdf-values for HPD-volumes of given probability

     // numsets:     number of data sets (input)
     // samppdf:     array pdf-values used for drawing the sample if 0,
     //              uniform sample distribution assumed (input)
     // pdf:         array of pdf-values at the sample points (input)
     // norm:        TRUE if both samppdf and pdf are normalized, else FALSE
     //              (input)
     // numlevels    number of probability for which pdf levels are to be 
     //              calculated (input)
     // proplevels:  array of probability levels for which pdf levels are to
     //              be calculated (input)
     // pdflevels:   array of calculated pdf levels (output)

     // all levels must be in the interval [0,1]


//////////////////////////////////////////////////////////////////////////////


// univariate delta distribution:
// ------------------------------


BOOLEAN delta_pdf(REAL x, REAL* pdf, REAL value=0,
				  std::ostream* cjob=0);

     // returns the value of the probability density function of a 
     // delta distribution at the location x (FALSE if x == value)

     // x:           location where the value of the probability density is
     //              to be calculated (input)
     // pdf:         calculated probability density (output)    
     // value:       location of the distribution (optional input)
     // cjob:        pointer to job log file (optional input)


BOOLEAN delta_logpdf(REAL x, REAL* logpdf, REAL value=0,
                     std::ostream* cjob=0);

     // returns the value of the logarithm of the probability density function  
     // of a delta distribution at the location x (FALSE if x == value)

     // x:           location where the value of the probability density is
     //              to be calculated (input)
     // logpdf:      calculated logarithm of the probability density (output)    
     // value:       location of the distribution (optional input)
     // cjob:        pointer to job log file (optional input)


BOOLEAN delta_cdf(REAL x, REAL* cdf, REAL value=0,
                  std::ostream* cjob=0);

     // returns the value of the cumulative distribution function of a 
     // delta distribution at the location x

     // x:           location where the value of the cumulative distribution
     //              function is to be calculated (input)
     // cdf:         calculated cumulative distribution function (output)    
     // value:       location of the distribution (optional input)
     // cjob:        pointer to job log file (optional input)


BOOLEAN delta_inv_cdf(REAL cdf, REAL* x, REAL valaue=0,
                      std::ostream* cjob=0);

     // returns the value of the inverse cumulative distribution function of  
     // a delta distribution

     // cdf:         given value of the cumulative distribution function
     //              (input)    
     // x:           calcuated location where the value of the cumulative 
     //              distribution function is equal to cdf (output)
     // value:       location of the distribution (optional input)
     // cjob:        pointer to job log file (optional input)


BOOLEAN delta_randnum(REAL* r, REAL value=0,
                      std::ostream* cjob=0);

     // returns a single value from a random sample (that can be obtained
     // by succeeding calls) from a delta distribution

     // r:           calculated random value (output)
     // value:       location of the distribution (optional input)
     // cjob:        pointer to job log file (optional input)


//////////////////////////////////////////////////////////////////////////////


// univariate uniform distribution:
// --------------------------------


BOOLEAN uniform_pdf(REAL x, REAL* pdf, REAL lowerbound=0, REAL upperbound=1,
                    std::ostream* cjob=0);

     // returns the probability densitiy of a univariate uniform distribution
     // at the location x

     // x:           location where the value of the probability density is
     //              to be calculated (input)
     // pdf:         calculated probability density (output)    
     // lowerbound:  lower bound of probability distribution (optional input)
     // upperbound:  upper bound of probability distribution (optional input)
     // cjob:        pointer to job log file (optional input)

     // upperbound must not be equal to lowerbound


BOOLEAN uniform_logpdf(REAL x, REAL* logpdf, REAL lowerbound=0, REAL upperbound=1,
                       std::ostream* cjob=0);

     // returns the logarthm of the probability densitiy of a univariate 
     // uniform distribution at the location x

     // x:           location where the value of the probability density is
     //              to be calculated (input)
     // logpdf:      calculated logarthm of the probability density (output)    
     // lowerbound:  lower bound of probability distribution (optional input)
     // upperbound:  upper bound of probability distribution (optional input)
     // cjob:        pointer to job log file (optional input)

     // upperbound must not be equal to lowerbound


BOOLEAN uniform_cdf(REAL x, REAL* cdf, REAL lowerbound=0, REAL upperbound=1,
                    std::ostream* cjob=0);

     // returns the value of the cumulative distribution function of a 
     // univariate uniform distribution at the location x

     // x:           location where the value of the cumulative distribution
     //              function is to be calculated (input)
     // cdf:         calculated cumulative distribution function (output)    
     // lowerbound:  lower bound of probability distribution (optional input)
     // upperbound:  upper bound of probability distribution (optional input)
     // cjob:        pointer to job log file (optional input)

     // upperbound must not be equal to lowerbound


BOOLEAN uniform_inv_cdf(REAL cdf, REAL* x,
                        REAL lowerbound=0, REAL upperbound=1, std::ostream* cjob=0);

     // returns the value of the  inverse cumulative distribution function of a 
     // univariate uniform distribution

     // cdf:         given value of the cumulative distribution function
     //              (input)    
     // x:           calcuated location where the value of the cumulative 
     //              distribution function is equal to cdf (output)
     // lowerbound:  lower bound of probability distribution (optional input)
     // upperbound:  upper bound of probability distribution (optional input)
     // cjob:        pointer to job log file (optional input)

     // cdf must fulfill: 0 <= cdf <= 1
     // upperbound must not be equal to lowerbound


BOOLEAN uniform_randnum(REAL* r, REAL lowerbound=0, REAL upperbound=1,
                        std::ostream* cjob=0);

     // returns a single value from a random sample (that can be obtained
     // by succeeding calls) from a univariate uniform distribution

     // r:           calculated random value (output)
     // lowerbound:  lower bound of probability distribution (optional input)
     // upperbound:  upper bound of probability distribution (optional input)
     // cjob:        pointer to job log file (optional input)

     // upperbound must not be equal to lowerbound


//////////////////////////////////////////////////////////////////////////////


// univariate normal distribution:
// -------------------------------


BOOLEAN normal_pdf(REAL x, REAL* pdf, REAL mean=0, REAL sigma=1, 
                   std::ostream* cjob=0);

     // returns the probability densitiy of a univariate normal distribution
     // at the location x

     // x:           location where the value of the probability density is
     //              to be calculated (input)
     // pdf:         calculated probability density (output)    
     // mean:        mean of probability distribution (optional input)
     // sigma:       standard deviation of probability distribution (optional 
     //              input)
     // cjob:        pointer to job log file (optional input)

     // sigma must be positive


BOOLEAN normal_logpdf(REAL x, REAL* logpdf, REAL mean=0, REAL sigma=1, 
                      std::ostream* cjob=0);

     // returns the logarithm of the probability densitiy of a univariate 
     // normal distribution at the location x

     // x:           location where the value of the probability density is
     //              to be calculated (input)
     // logpdf:      calculated logarithm of the probability density (output)    
     // mean:        mean of probability distribution (optional input)
     // sigma:       standard deviation of probability distribution (optional 
     //              input)
     // cjob:        pointer to job log file (optional input)

     // sigma must be positive


BOOLEAN normal_cdf(REAL x, REAL* cdf, REAL mean=0, REAL sigma=1,
                   std::ostream* cjob=0);

     // returns the value of the cumulative distribution function of a 
     // univariate normal distribution at the location x

     // x:           location where the value of the cumulative probability
     //              distribution is to be calculated (input)
     // cdf:         calculated cumulative distribution function (output)    
     // mean:        mean of probability distribution (optional input)
     // sigma:       standard deviation of probability distribution (optional 
     //              input)
     // cjob:        pointer to job log file (optional input)

     // sigma must be positive

     // comment: single precision implementation only


BOOLEAN normal_inv_cdf(REAL cdf, REAL* x, REAL mean=0, REAL sigma=1,
                       std::ostream* cjob=0);

     // returns the value of the inverse cumulative distribution function of a 
     // univariate normal distribution

     // cdf:         given value of the cumulative distribution function
     //              (input)    
     // x:           calcuated location where the value of the cumulative 
     //              distribution function is equal to cdf (output)
     // mean:        mean of probability distribution (optional input)
     // sigma:       standard deviation of probability distribution (optional 
     //              input)
     // cjob:        pointer to job log file (optional input)

     // cdf must fulfill: 0 < cdf < 1
     // sigma must be positive

     // comment: single precision implementation only


BOOLEAN normal_randnum(REAL* r, REAL mean=0, REAL sigma=1, std::ostream* cjob=0);

     // returns a single value from a random sample (that can be obtained
     // by succeeding calls) from a univariate normal distribution

     // r:           calcuated random value (output)
     // mean:        mean of probability distribution (optional input)
     // sigma:       standard deviation of probability distribution (optional 
     //              input)
     // cjob:        pointer to job log file (optional input)

     // sigma must be positive


//////////////////////////////////////////////////////////////////////////////


// univariate lognormal distribution:
// ----------------------------------


BOOLEAN lognormal_pdf(REAL x, REAL* pdf, REAL mean, REAL sigma, 
                      std::ostream* cjob=0);

     // returns the probability densitiy of a univariate lognormal
     // distribution at the location x

     // x:           location where the value of the probability density is
     //              to be calculated (input)
     // pdf:         calculated probability density (output)    
     // mean:        mean of probability distribution (input)
     // sigma:       standard deviation of probability distribution (input)
     // cjob:        pointer to job log file (optional input)

     // mean and sigma must be positive


BOOLEAN lognormal_logpdf(REAL x, REAL* logpdf, REAL mean, REAL sigma, 
                         std::ostream* cjob=0);

     // returns the logarithm of the probability densitiy of a 
     // univariate lognormal distribution at the location x

     // x:           location where the value of the probability density is
     //              to be calculated (input)
     // logpdf:      calculated logarithm of the probability density (output)    
     // mean:        mean of probability distribution (input)
     // sigma:       standard deviation of probability distribution (input)
     // cjob:        pointer to job log file (optional input)

     // mean and sigma must be positive


BOOLEAN lognormal_cdf(REAL x, REAL* cdf, REAL mean, REAL sigma,
                      std::ostream* cjob=0);

     // returns the value of the cumulative distribution function of a 
     // univariate lognormal distribution at the location x

     // x:           location where the value of the cumulative probability
     //              distribution is to be calculated (input)
     // cdf:         calculated cumulative distribution function (output)    
     // mean:        mean of probability distribution (input)
     // sigma:       standard deviation of probability distribution (input)
     // cjob:        pointer to job log file (optional input)

     // mean and sigma must be positive

     // comment: single precision implementation only


BOOLEAN lognormal_inv_cdf(REAL cdf, REAL* x, REAL mean, REAL sigma,
                          std::ostream* cjob=0);

     // returns the value of the inverse cumulative distribution function of a 
     // univariate lognormal distribution

     // cdf:         given value of the cumulative distribution function
     //              (input)    
     // x:           calcuated location where the value of the cumulative 
     //              distribution function is equal to cdf (output)
     // mean:        mean of probability distribution (input)
     // sigma:       standard deviation of probability distribution (input)
     // cjob:        pointer to job log file (optional input)

     // cdf must fulfill: 0 <= cdf < 1
     // sigma must be positive

     // comment: single precision implementation only


BOOLEAN lognormal_randnum(REAL* r, REAL mean, REAL sigma, std::ostream* cjob=0);

     // returns a single value from a random sample (that can be obtained
     // by succeeding calls) from a univariate lognormal distribution

     // r:           calcuated random value (output)
     // mean:        mean of probability distribution (input)
     // sigma:       standard deviation of probability distribution (input)
     // cjob:        pointer to job log file (optional input)

     // sigma must be positive


//////////////////////////////////////////////////////////////////////////////


// univariate gamma distribution:
// ------------------------------


BOOLEAN gamma_pdf(REAL x, REAL* pdf, REAL mean, REAL sigma, 
                  std::ostream* cjob=0);

     // returns the probability densitiy of a univariate gamma
     // distribution at the location x

     // x:           location where the value of the probability density is
     //              to be calculated (input)
     // pdf:         calculated probability density (output)    
     // mean:        mean of probability distribution (input)
     // sigma:       standard deviation of probability distribution (input)
     // cjob:        pointer to job log file (optional input)

     // mean and sigma must be positive


BOOLEAN gamma_logpdf(REAL x, REAL* logpdf, REAL mean, REAL sigma, 
                     std::ostream* cjob=0);

     // returns the logarithm of the probability densitiy of a 
     // univariate gamma distribution at the location x

     // x:           location where the value of the probability density is
     //              to be calculated (input)
     // logpdf:      calculated logarithm of the probability density (output)    
     // mean:        mean of probability distribution (input)
     // sigma:       standard deviation of probability distribution (input)
     // cjob:        pointer to job log file (optional input)

     // mean and sigma must be positive


BOOLEAN gamma_cdf(REAL x, REAL* cdf, REAL mean, REAL sigma,
                  std::ostream* cjob=0);

     // returns the value of the cumulative distribution function of a 
     // univariate gamma distribution at the location x

     // x:           location where the value of the cumulative probability
     //              distribution is to be calculated (input)
     // cdf:         calculated cumulative distribution function (output)    
     // mean:        mean of probability distribution (input)
     // sigma:       standard deviation of probability distribution (input)
     // cjob:        pointer to job log file (optional input)

     // mean and sigma must be positive

     // comment: single precision implementation only


BOOLEAN gamma_inv_cdf(REAL cdf, REAL* x, REAL mean, REAL sigma,
                      std::ostream* cjob=0);

     // returns the value of the inverse cumulative distribution function of a 
     // univariate gamma distribution

     // cdf:         given value of the cumulative distribution function
     //              (input)    
     // x:           calcuated location where the value of the cumulative 
     //              distribution function is equal to cdf (output)
     // mean:        mean of probability distribution (input)
     // sigma:       standard deviation of probability distribution (input)
     // cjob:        pointer to job log file (optional input)

     // cdf must fulfill: 0 <= cdf < 1
     // sigma must be positive

     // comment: single precision implementation only


BOOLEAN gamma_randnum(REAL* r, REAL mean, REAL sigma, std::ostream* cjob=0);

     // returns a single value from a random sample (that can be obtained
     // by succeeding calls) from a univariate gamma distribution

     // r:           calcuated random value (output)
     // mean:        mean of probability distribution (input)
     // sigma:       standard deviation of probability distribution (input)
     // cjob:        pointer to job log file (optional input)

     // sigma must be positive


//////////////////////////////////////////////////////////////////////////////


// univariate triangular distribution:
// -----------------------------------


BOOLEAN triangular_pdf(REAL x, REAL* pdf, 
                       REAL mode=0.5, REAL leftwidth=0.5, REAL rightwidth=0.5,
                       std::ostream* cjob=0);

     // returns the probability densitiy of a univariate triangular
     // distribution at the location x

     // x:           location where the value of the probability density is
     //              to be calculated (input)
     // pdf:         calculated probability density (output)    
     // mode:        position of the maximum (optional input)
     // leftwidth:   distance between start and maximum of the distribution 
     //              (optional input)
     // rightwidth:  distance between maximum and end of the distribution 
     //              (optional input)
     // cjob:        pointer to job log file (optional input)

     // leftwidth and rightwidth must both be larger than zero


BOOLEAN triangular_logpdf(REAL x, REAL* logpdf, 
                          REAL mode=0.5, REAL leftwidth=0.5, REAL rightwidth=0.5,
                          std::ostream* cjob=0);

     // returns the logarithm of the probability densitiy of a 
     // univariate triangular distribution at the location x

     // x:           location where the value of the probability density is
     //              to be calculated (input)
     // logpdf:      calculated logarithm of the probability density (output)    
     // mode:        position of the maximum (optional input)
     // leftwidth:   distance between start and maximum of the distribution 
     //              (optional input)
     // rightwidth:  distance between maximum and end of the distribution 
     //              (optional input)
     // cjob:        pointer to job log file (optional input)

     // leftwidth and rightwidth must both be larger than zero


BOOLEAN triangular_cdf(REAL x, REAL* cdf, 
                       REAL mode=0.5, REAL leftwidth=0.5, REAL rightwidth=0.5,
                       std::ostream* cjob=0);

     // returns the value of the cumulative distribution function of a 
     // univariate triangular distribution at the location x

     // x:           location where the value of the cumulative distribution
     //              function is to be calculated (input)
     // cdf:         calculated cumulative distribution function (output)    
     // mode:        position of the maximum (optional input)
     // leftwidth:   distance between start and maximum of the distribution 
     //              (optional input)
     // rightwidth:  distance between maximum and end of the distribution 
     //              (optional input)
     // cjob:        pointer to job log file (optional input)

     // leftwidth and rightwidth must both be larger than zero


BOOLEAN triangular_inv_cdf(REAL cdf, REAL* x,
                           REAL mode=0.5, REAL leftwidth=0.5, REAL rightwidth=0.5,
                           std::ostream* cjob=0);

     // returns the value of the  inverse cumulative distribution function of a 
     // univariate triangular distribution

     // cdf:         given value of the cumulative distribution function
     //              (input)    
     // x:           calcuated location where the value of the cumulative 
     //              distribution function is equal to cdf (output)
     // mode:        position of the maximum (optional input)
     // leftwidth:   distance between start and maximum of the distribution 
     //              (optional input)
     // rightwidth:  distance between maximum and end of the distribution 
     //              (optional input)
     // cjob:        pointer to job log file (optional input)

     // cdf must fulfill: 0 <= cdf <= 1
     // leftwidth and rightwidth must both be larger than zero


BOOLEAN triangular_randnum(REAL* r, 
                           REAL mode=0.5, REAL leftwidth=0.5, REAL rightwidth=0.5,
                           std::ostream* cjob=0);

     // returns a single value from a random sample (that can be obtained
     // by succeeding calls) from a univariate triangular distribution

     // r:           calculated random value (output)
     // mode:        position of the maximum (optional input)
     // leftwidth:   distance between start and maximum of the distribution 
     //              (optional input)
     // rightwidth:  distance between maximum and end of the distribution 
     //              (optional input)
     // cjob:        pointer to job log file (optional input)

     // leftwidth and rightwidth must both be larger than zero


//////////////////////////////////////////////////////////////////////////////


// univariate empirical distribution:
// ----------------------------------


BOOLEAN empirical_pdf(REAL x, REAL* pdf, 
                      CARDLONG numfract, REAL* levels, REAL* fract,
                      std::ostream* cjob=0);

     // returns the probability densitiy of a univariate empirical
     // distribution (linear interpolation of the cdf between given
     // fractiles) at the location x

     // x:           location where the value of the probability density is
     //              to be calculated (input)
     // pdf:         calculated probability density (output)    
     // numfract:    number of fractiles specified (input)
     // levels:      fractile levels (input)
     // fract:       x-values corresponding to the fractile levels (input)
     // cjob:        pointer to job log file (optional input)

     // fractile levels must strictly increase from zero to unity
     // fractile values must also strictly increase
     // at least two fractiles must be specified


BOOLEAN empirical_logpdf(REAL x, REAL* logpdf, 
                         CARDLONG numfract, REAL* levels, REAL* fract,
                         std::ostream* cjob=0);

     // returns the logarithm of the probability densitiy of a univariate 
     // empirical distribution (linear interpolation of the cdf between given
     // fractiles) at the location x

     // x:           location where the value of the probability density is
     //              to be calculated (input)
     // logpdf:      calculated logarithm of the probability density (output)    
     // numfract:    number of fractiles specified (input)
     // levels:      fractile levels (input)
     // fract:       x-values corresponding to the fractile levels (input)
     // cjob:        pointer to job log file (optional input)

     // fractile levels must strictly increase from zero to unity
     // fractile values must also strictly increase
     // at least two fractiles must be specified


BOOLEAN empirical_cdf(REAL x, REAL* cdf, 
                      CARDLONG numfract, REAL* levels, REAL* fract,
                      std::ostream* cjob=0);

     // returns the value of the cumulative distribution function of a 
     // univariate empirical distribution (linear interpolation of the cdf
     // between given fractiles) at the location x

     // x:           location where the value of the cumulative distribution
     //              function is to be calculated (input)
     // cdf:         calculated cumulative distribution function (output)    
     // numfract:    number of fractiles specified (input)
     // levels:      fractile levels (input)
     // fract:       x-values corresponding to the fractile levels (input)
     // cjob:        pointer to job log file (optional input)

     // fractile levels must strictly increase from zero to unity
     // fractile values must also strictly increase
     // at least two fractiles must be specified


BOOLEAN empirical_inv_cdf(REAL cdf, REAL* x,
                          CARDLONG numfract, REAL* levels, REAL* fract,
                          std::ostream* cjob=0);

     // returns the value of the  inverse cumulative distribution function of a 
     // univariate empirical distribution (linear interpolation of the cdf
     // between given fractiles)

     // cdf:         given value of the cumulative distribution function
     //              (input)    
     // x:           calcuated location where the value of the cumulative 
     //              distribution function is equal to cdf (output)
     // numfract:    number of fractiles specified (input)
     // levels:      fractile levels (input)
     // fract:       x-values corresponding to the fractile levels (input)
     // cjob:        pointer to job log file (optional input)

     // cdf must fulfill: 0 <= cdf <= 1
     // fractile levels must strictly increase from zero to unity
     // fractile values must also strictly increase
     // at least two fractiles must be specified


BOOLEAN empirical_randnum(REAL* r, 
                          CARDLONG numfract, REAL* levels, REAL* fract,
                          std::ostream* cjob=0);

     // returns a single value from a random sample (that can be obtained
     // by succeeding calls) from a univariate empirical distribution
     // (linear interpolation of the cdf between given fractiles)

     // r:           calculated random value (output)
     // numfract:    number of fractiles specified (input)
     // levels:      fractile levels (input)
     // fract:       x-values corresponding to the fractile levels (input)
     // cjob:        pointer to job log file (optional input)

     // fractile levels must strictly increase from zero to unity
     // fractile values must also strictly increase
     // at least two fractiles must be specified


//////////////////////////////////////////////////////////////////////////////


// multivariate delta distribution:
// --------------------------------


BOOLEAN mv_delta_pdf(CARDLONG dim, REAL* x, REAL* pdf,
                     REAL* values=0, 
                     std::ostream* cjob=0); 

     // returns the probability densitiy of a multivariate delta distribution
     // at the array of locations x

     // dim:         dimension of distribution (input)
     // x:           array of locations where the value of the probability 
     //              density is to be calculated (input)
     // pdf:         calculated probability density (output)    
     // valuess:     array of locations of probability distribution 
     //              (optional input; defaults: 0)
     // cjob:        pointer to job log file (optional input)


BOOLEAN mv_delta_logpdf(CARDLONG dim, REAL* x, REAL* logpdf,
                        REAL* values=0, 
                        std::ostream* cjob=0); 

     // returns the logarithm of the probability densitiy of a multivariate 
     // delta distribution at the array of locations x

     // dim:         dimension of distribution (input)
     // x:           array of locations where the value of the probability 
     //              density is to be calculated (input)
     // logpdf:      calculated logarithm of the probability density (output)    
     // valuess:     array of locations of probability distribution 
     //              (optional input; defaults: 0)
     // cjob:        pointer to job log file (optional input)


BOOLEAN mv_delta_cdf(CARDLONG dim, REAL* x, REAL* cdf,
                     REAL* values=0, 
                     std::ostream* cjob=0); 

     // returns the cumulative distribution function of a multivariate delta
     // distribution at the array of locations x

     // dim:         dimension of distribution (input)
     // x:           array of locations where the value of the distribution 
     //              function is to be calculated (input)
     // cdf:         calculated value of the cumulative distribution function
     //              (output)    
     // values:      array of locations of probability distribution 
     //              (optional input; defaults: 0)
     // cjob:        pointer to job log file (optional input)


CARDLONG mv_delta_randsamp(CARDLONG dim, CARDLONG maxsize, SAMPTECH samptech,
                           REAL** sample, 
                           REAL* values=0,
                           std::ostream* cjob=0); 

     // creates a regular, quasi-random or low discrepancy sample from a
     // multivariate delta distribution

     // return value is the actual sample size (0 if a problem occurs)

     // dim:         dimension of the distribution (input)
     // maxsize:     maximum size of the sample; actual size can be different
     //              for regluar sample (input)
     // samptech:    selector for sampling technique (input):
     //              GRID:        regular grid
     //              RANDOM:      "random" (Monte Carlo) sample
     //              LHS:         latin hypercube sample
     //              HAMMERSLEY:  hammersley low discrepancy sequence
     //              HALTON:      halton low discrepancy sequence
     // sample:      space must be allocated for sample vectors of size dim
     //              sample[j][i] returns the value in dimension i of sample j
     // values:      array of locations of probability distribution 
     //              (optional input; defaults: 0)
     // cjob:        pointer to job log file (optional input)


//////////////////////////////////////////////////////////////////////////////


// multivariate uniform distribution (without correlations):
// ---------------------------------------------------------


BOOLEAN mv_uniform_pdf(CARDLONG dim, REAL* x, REAL* pdf,
                       REAL* lowerbounds=0, REAL* upperbounds=0, 
                       std::ostream* cjob=0); 

     // returns the probability densitiy of a multivariate uniform distribution
     // without correlation at the array of locations x

     // dim:         dimension of distribution (input)
     // x:           array of locations where the value of the probability 
     //              density is to be calculated (input)
     // pdf:         calculated probability density (output)    
     // lowerbounds: array of lower bounds of probability distribution 
     //              (optional input; defaults: 0)
     // upperbounds: array of upper bound of probability distribution 
     //              (optional input; defaults: 1)
     // cjob:        pointer to job log file (optional input)

     // arrays of lower and upper bound must provide different values for
     // lowerbound and upperbound in any dimension


BOOLEAN mv_uniform_logpdf(CARDLONG dim, REAL* x, REAL* logpdf,
                          REAL* lowerbounds=0, REAL* upperbounds=0, 
                          std::ostream* cjob=0); 

     // returns the logarithm of the probability densitiy of a multivariate 
     // uniform distribution without correlation at the array of locations x

     // dim:         dimension of distribution (input)
     // x:           array of locations where the value of the probability 
     //              density is to be calculated (input)
     // logpdf:      calculated logarithm of the probability density (output)    
     // lowerbounds: array of lower bounds of probability distribution 
     //              (optional input; defaults: 0)
     // upperbounds: array of upper bound of probability distribution 
     //              (optional input; defaults: 1)
     // cjob:        pointer to job log file (optional input)

     // arrays of lower and upper bound must provide different values for
     // lowerbound and upperbound in any dimension


BOOLEAN mv_uniform_cdf(CARDLONG dim, REAL* x, REAL* cdf,
                       REAL* lowerbounds=0, REAL* upperbounds=0, 
                       std::ostream* cjob=0); 

     // returns the cumulative distribution function of a multivariate uniform
     // distribution without correlation at the array of locations x

     // dim:         dimension of distribution (input)
     // x:           array of locations where the value of the distribution 
     //              function is to be calculated (input)
     // cdf:         calculated value of the cumulative distribution function
     //              (output)    
     // lowerbounds: array of lower bounds of probability distribution 
     //              (optional input; defaults: 0)
     // upperbounds: array of upper bound of probability distribution 
     //              (optional input; defaults: 1)
     // cjob:        pointer to job log file (optional input)

     // arrays of lower and upper bound must provide different values for
     // lowerbound and upperbound in any dimension


CARDLONG mv_uniform_randsamp(CARDLONG dim, CARDLONG maxsize, SAMPTECH samptech,
                             REAL** sample, 
                             REAL* lowerbounds=0, REAL* upperbounds=0,
                             std::ostream* cjob=0); 

     // creates a regular, quasi-random or low discrepancy sample from a
     // multivariate uniform distribution without correlation

     // return value is the actual sample size (0 if a problem occurs)

     // dim:         dimension of the distribution (input)
     // maxsize:     maximum size of the sample; actual size can be different
     //              for regluar sample (input)
     // samptech:    selector for sampling technique (input):
     //              GRID:        regular grid
     //              RANDOM:      "random" (Monte Carlo) sample
     //              LHS:         latin hypercube sample
     //              HAMMERSLEY:  hammersley low discrepancy sequence
     //              HALTON:      halton low discrepancy sequence
     // sample:      space must be allocated for sample vectors of size dim
     //              sample[j][i] returns the value in dimension i of sample j
     // lowerbounds: array of lower bounds of probability distribution 
     //              (optional input; defaults: 0)
     // upperbounds: array of upper bound of probability distribution 
     //              (optional input; defaults: 1)
     // cjob:        pointer to job log file (optional input)


//////////////////////////////////////////////////////////////////////////////


// multivariate normal distribution:
// ---------------------------------


BOOLEAN mv_normal_pdf(CARDLONG dim, REAL* x, REAL* pdf,
                      REAL* means=0, REAL* sigmas=0, REAL* corr=0,
                      std::ostream* cjob=0); 

     // returns the probability densitiy of a multivariate normal 
     // distribution at the array of locations x

     // dim:         dimension of distribution (input)
     // x:           array of locations where the value of the probability 
     //              density is to be calculated (input)
     // pdf:         calculated probability density (output)    
     // means:       array of means of probability distribution 
     //              (optional input, defaults: 0)
     // sigmas:      array of standard deviations of probability 
     //              distribution (optional input; defaults: 1)
     // corr:        correlation matrix (optional input; default: un-
     //              correlated)
     // cjob:        pointer to job log file (optional input)

     // provided values for sigmas must be positive, provided values for
     // corr must lead to a positive definite matrix with diagonal ele-
     // ments equal to unity


BOOLEAN mv_normal_logpdf(CARDLONG dim, REAL* x, REAL* logpdf,
                         REAL* means=0, REAL* sigmas=0, REAL* corr=0,
                         std::ostream* cjob=0); 

     // returns the logarithm of the probability densitiy of a  
     // multivariate normal distribution at the array of locations x

     // dim:         dimension of distribution (input)
     // x:           array of locations where the value of the probability 
     //              density is to be calculated (input)
     // logpdf:      calculated logarithm of the probability density (output)    
     // means:       array of means of probability distribution 
     //              (optional input, defaults: 0)
     // sigmas:      array of standard deviations of probability 
     //              distribution (optional input; defaults: 1)
     // corr:        correlation matrix (optional input; default: un-
     //              correlated)
     // cjob:        pointer to job log file (optional input)

     // provided values for sigmas must be positive, provided values for
     // corr must lead to a positive definite matrix with diagonal ele-
     // ments equal to unity


CARDLONG mv_normal_randsamp(CARDLONG dim, CARDLONG maxsize, SAMPTECH samptech,
                            REAL** sample, 
                            REAL* means=0, REAL* sigmas=0, REAL* corr=0,
                            std::ostream* cjob=0); 

     // creates a regular, quasi-random or low discrepancy sample from a
     // multivariate normal distribution

     // return value is the actual sample size (0 if a problem occurs)

     // dim:         dimension of the distribution (input)
     // maxsize:     maximum size of the sample; actual size can be different
     //              for regluar sample (input)
     // samptech:    selector for sampling technique (input):
     //                GRID:        regular grid
     //                RANDOM:      "random" (Monte Carlo) sample
     //                LHS:         latin hypercube sample
     //                HAMMERSLEY:  hammersley low discrepancy sequence
     //                HALTON:      halton low discrepancy sequence
     // sample:      space must be allocated for sample vectors of size dim
     //              sample[j][i] returns the value in dimension i of sample j
     // means:       array of means of probability distribution 
     //              (optional input, defaults: 0)
     // sigmas:      array of standard deviations of probability 
     //              distribution (optional input; defaults: 1)
     // corr:        correlation matrix (optional input; default: un-
     //              correlated)
     // cjob:        pointer to job log file (optional input)

     // provided values for sigmas must be positive, provided values for
     // corr must lead to a positive definite matrix with diagonal ele-
     // ments equal to unity


//////////////////////////////////////////////////////////////////////////////


// multivariate lognormal distribution:
// ------------------------------------


BOOLEAN mv_lognormal_pdf(CARDLONG dim, REAL* x, REAL* pdf,
                         REAL* means=0, REAL* sigmas=0, REAL* corr=0,
                         std::ostream* cjob=0); 

     // returns the probability densitiy of a multivariate lognormal 
     // distribution at the array of locations x

     // dim:         dimension of distribution (input)
     // x:           array of locations where the value of the probability 
     //              density is to be calculated (input)
     // pdf:         calculated probability density (output)    
     // means:       array of means of probability distribution 
     //              (input)
     // sigmas:      array of standard deviations of probability 
     //              distribution (input)
     // corr:        correlation matrix (optional input; default: un-
     //              correlated)
     // cjob:        pointer to job log file (optional input)

     // provided values for means and sigmas must be positive, provided 
     // values for corr must lead to a positive definite matrix with diagonal 
     // elements equal to unity


BOOLEAN mv_lognormal_logpdf(CARDLONG dim, REAL* x, REAL* logpdf,
                            REAL* means=0, REAL* sigmas=0, REAL* corr=0,
                            std::ostream* cjob=0); 

     // returns the logarithm of the probability densitiy of a  
     // multivariate lognormal distribution at the array of locations x

     // dim:         dimension of distribution (input)
     // x:           array of locations where the value of the probability 
     //              density is to be calculated (input)
     // logpdf:      calculated logarithm of the probability density (output)    
     // means:       array of means of probability distribution 
     //              (input)
     // sigmas:      array of standard deviations of probability 
     //              distribution (input)
     // corr:        correlation matrix (optional input; default: un-
     //              correlated)
     // cjob:        pointer to job log file (optional input)

     // provided values for means and sigmas must be positive, provided 
     // values for corr must lead to a positive definite matrix with diagonal 
     // elements equal to unity


CARDLONG mv_lognormal_randsamp(CARDLONG dim, CARDLONG maxsize, SAMPTECH samptech,
                               REAL** sample, 
                               REAL* means=0, REAL* sigmas=0, REAL* corr=0,
                               std::ostream* cjob=0); 

     // creates a regular, quasi-random or low discrepancy sample from a
     // multivariate lognormal distribution

     // return value is the actual sample size (0 if a problem occurs)

     // dim:         dimension of the distribution (input)
     // maxsize:     maximum size of the sample; actual size can be different
     //              for regluar sample (input)
     // samptech:    selector for sampling technique (input):
     //                GRID:        regular grid
     //                RANDOM:      "random" (Monte Carlo) sample
     //                LHS:         latin hypercube sample
     //                HAMMERSLEY:  hammersley low discrepancy sequence
     //                HALTON:      halton low discrepancy sequence
     // sample:      space must be allocated for sample vectors of size dim
     //              sample[j][i] returns the value in dimension i of sample j
     // means:       array of means of probability distribution 
     //              (input)
     // sigmas:      array of standard deviations of probability 
     //              distribution (input)
     // corr:        correlation matrix (optional input; default: un-
     //              correlated)
     // cjob:        pointer to job log file (optional input)

     // provided values for means and sigmas must be positive, provided 
     // values for corr must lead to a positive definite matrix with diagonal 
     // elements equal to unity


//////////////////////////////////////////////////////////////////////////////


// multivariate distribution with given marginals and rank correlations:
// ---------------------------------------------------------------------


BOOLEAN mv_copula_pdf(CARDLONG dim, REAL* x, REAL* pdf,
                      UV_DISTTYPE* marginals, REAL** pars=0, REAL* corr=0,
                      std::ostream* cjob=0); 

     // returns the probability densitiy of a multivariate distribution with 
     // given marginals and rank correlation coefficients at the array of 
     // locations x

     // dim:         dimension of distribution (input)
     // x:           array of locations where the value of the probability 
     //              density is to be calculated (input)
     // pdf:         calculated probability density (output)
     // marginals:   array of types of marginals (input)
     //                UV_UNIFORM
     //                UV_NORMAL
     //                UV_LOGNORMAL
     //                UV_GAMMA
     //                UV_TRIANGULAR
     //                UV_EMPIRICAL
     // pars:        array of arrays of parameters of marginals (optional 
     //              input)
     // corr:        rank correlation matrix (optional input; default: un-
     //              correlated)
     // cjob:        pointer to job log file (optional input)

     // provided values for corr must lead to a positive definite matrix with  
     // diagonal elements equal to unity


BOOLEAN mv_copula_logpdf(CARDLONG dim, REAL* x, REAL* logpdf,
                         UV_DISTTYPE* marginals, REAL** pars=0, REAL* corr=0,
                         std::ostream* cjob=0); 

     // returns the logarithm of the probability densitiy of a multivariate 
     // distribution withgiven marginals and rank correlation coefficients 
     // at the array of locations x

     // dim:         dimension of distribution (input)
     // x:           array of locations where the value of the probability 
     //              density is to be calculated (input)
     // log pdf:     calculated logarithm of the probability density (output)
     // marginals:   array of types of marginals (input)
     //                UV_UNIFORM
     //                UV_NORMAL
     //                UV_LOGNORMAL
     //                UV_GAMMA
     //                UV_TRIANGULAR
     //                UV_EMPIRICAL
     // pars:        array of arrays of parameters of marginals (optional 
     //              input)
     // corr:        rank correlation matrix (optional input; default: un-
     //              correlated)
     // cjob:        pointer to job log file (optional input)

     // provided values for corr must lead to a positive definite matrix with  
     // diagonal elements equal to unity


CARDLONG mv_copula_randsamp(CARDLONG dim, CARDLONG maxsize, SAMPTECH samptech,
                            REAL** sample, 
                            UV_DISTTYPE* marginals, REAL** pars=0, REAL* corr=0,
                            std::ostream* cjob=0); 

     // creates a regular, quasi-random or low discrepancy sample from a
     // multivariate distribution with given marginals and rank correlation
     // coefficients

     // return value is the actual sample size (0 if a problem occurs)

     // dim:         dimension of the distribution (input)
     // maxsize:     maximum size of the sample; actual size can be different
     //              for regluar sample (input)
     // samptech:    selector for sampling technique (input):
     //                GRID:        regular grid
     //                RANDOM:      "random" (Monte Carlo) sample
     //                LHS:         latin hypercube sample
     //                HAMMERSLEY:  hammersley low discrepancy sequence
     //                HALTON:      halton low discrepancy sequence
     // sample:      space must be allocated for sample vectors of size dim
     //              sample[j][i] returns the value in dimension i of sample j
     // marginals:   array of types of marginals (input):
     //                UV_UNIFORM
     //                UV_NORMAL
     //                UV_LOGNORMAL
     //                UV_GAMMA
     //                UV_TRIANGULAR
     //                UV_EMPIRICAL
     // pars:        array of arrays of parameters of marginals (optional 
     //              input)
     // corr:        rank correlation matrix (optional input; default: un-
     //              correlated)
     // cjob:        pointer to job log file (optional input)

     // provided values for corr must lead to a positive definite matrix with  
     // diagonal elements equal to unity


//////////////////////////////////////////////////////////////////////////////


// interface to any of the multivariate distributions given above:
// ---------------------------------------------------------------


BOOLEAN mv_pdf(DISTRIBUTION& dist, REAL* x, REAL* pdf,
               std::ostream* cjob=0); 

BOOLEAN mv_logpdf(DISTRIBUTION& dist, REAL* x, REAL* logpdf,
                  std::ostream* cjob=0); 

CARDLONG mv_randsamp(DISTRIBUTION& dist, 
                     CARDLONG maxsize, SAMPTECH samptech, REAL** sample, 
                     std::ostream* cjob=0); 

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
