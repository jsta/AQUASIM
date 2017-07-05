////////////////////////////////   main_b.C   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    10.03.93    Peter Reichert
// revisions:   07.05.99    Peter Reichert    Output of sens. funct. added
//              17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include "aquasys.h"

//////////////////////////////////////////////////////////////////////////////


const char Progname[]      = "aquasimb";

const char CmdLine[]       = "command line";
const char Sim[]           = "performing batch simulation ... ";
const char Sens[]          = "performing batch sensitivity analysis ... ";
const char Fit[]           = "performing batch parameter estimation ... ";
const char SetFit[]        = "performing batch set of parameter estimations ... ";
const char Fit0[]          = "performing batch calculation of chi^2 ... ";
const char Inv[]           = "performing batch identifiability analysis ... ";
const char Chi[]           = "performing batch calculation of chi^2s ... ";
const char Res[]           = "performing batch calculation of results ... ";
const char MaxPost[]       = "performing batch Bayesian posterior maximization ... ";
const char MCMC[]          = "performing batch Bayesian MCMC calculation ... ";
const char ListRes[]       = "performing batch listing of results ... ";
const char PlotRes[]       = "performing batch plotting of results ... ";
const char EndJob[]        = "end of job";
const char EndProg[]       = "end of processing (normal termination)";

const char StartError[]    = "\n\n*** ERROR: ";
const char EndError[]      = " ***\n\n";

const char LogError1[]     = "logfile refers to an AQUASIM system file";
const char LogError2[]     = "unable to open logfile";
const char LogError3[]     = "filename equal to name of logfile";
const char LoadError1[]    = "unable to open loadfile";
const char LoadError2[]    = "problem during reading loadfile";
const char SimError1[]     = "unable to open scmdfile";
const char SimError2[]     = "error reading from scmdfile";
const char SimError3[]     = "problem during simulation";
const char SimError4[]     = "simulation not initialized";
const char SensError1[]    = "problem during sensitivity analysis";
const char SensError2[]    = "problem during sensitivity ranking";
const char SensError3[]    = "unable to open output file";
const char FitError1[]     = "unable to open fitfile";
const char FitError2[]     = "problem during parameter estimation";
const char IdError1[]      = "illigal number of iteration steps";
const char IdError2[]      = "unable to open fitfile";
const char IdError3[]      = "unable to open fcmdfile";
const char IdError4[]      = "problem during identifiability analysis";
const char SetFitError1[]  = "unable to open vcmdfile";
const char SetFitError2[]  = "unable to open parsfile";
const char SetFitError3[]  = "unable to open fitfile";
const char SetFitError4[]  = "problem during set of parameter estimations";
const char ChiError1[]     = "unable to open vcmdfile";
const char ChiError2[]     = "unable to open chifile";
const char ChiError3[]     = "problem during calculation of chi2s";
const char ResError1[]     = "unable to open vcmdfile";
const char ResError2[]     = "unable to open rcmdfile";
const char ResError3[]     = "unable to open resfile";
const char ResError4[]     = "problem during calculation of model results";
const char ResError5[]     = "calculation of model results only partially successful";
const char MaxPostError1[] = "unable to open prifile";
const char MaxPostError2[] = "unable to open maxfile";
const char MaxPostError3[] = "unable to open varfile";
const char MaxPostError4[] = "problem during maximization of posterior";
const char MCMCError1[]    = "unable to open prifile";
const char MCMCError2[]    = "unable ot open jmpfile";
const char MCMCError3[]    = "unable to open mcfile";
const char MCMCError4[]    = "problem during calculation of Markov Chain";
const char ListError1[]    = "unable to open pcmdfile";
const char ListError2[]    = "problem reading from pcmdfile";
const char ListError3[]    = "problem opening plotfile";
const char PlotError1[]    = "unable to open pcmdfile";
const char PlotError2[]    = "problem reading from pcmdfile";
const char PlotError3[]    = "problem opening plotfile";
const char SaveError1[]    = "unable to open savefile";
const char SaveError2[]    = "problem during writing savefile";


std::ofstream logfile;


static void WriChars(std::ostream& out, char c, int n)
{
   for ( int i=0; i<n; i++ ) out << c;
   return;
}


static void WriteIllegalArgs(std::ostream& out, BOOLEAN extended=FALSE)
{
   out << "\nIllegal number of arguments,"
        << " please use one of the command lines:\n";
   out << "\nExecution with a job file:      "
       << Progname << " jobfil";
   out << "\nSimulation:                     "
       << Progname << " -s log load save [scmd]";
   out << "\nSensitivity analysis:           "
       << Progname << " -a log load save [rank [sens [rcmd]]]";
   out << "\nParameter estimation:           "
       << Progname << " -e log load save fit";
   out << "\nSet of parameter estimations:   "
       << Progname << " -f log load vcmd pars fit";
   out << "\nCalculation of Chi^2s:          "
       << Progname << " -c log load vcmd chi";
   out << "\nCalculation of results:         "
       << Progname << " -r log load vcmd rcmd res1";
   out << "\nCalculation of result (UNCSIM): "
       << Progname << " -u log load ucmd rcmd res2";
   out << "\nMaximization of posterior:      "
       << Progname << " -b log load pri  max  var";
   out << "\nCalculation of Markov chain:    "
       << Progname << " -m log load pri  jmp  mc";
   out << "\nPlotting of results:            "
       << Progname << " -p log load pcmd";
   out << "\nListing of results:             "
       << Progname << " -l log load lcmd\n";
   if ( extended == FALSE )
   {
      out << "\nSome more explanations: "
          << Progname << " -h\n";
   }
   else
   {
      out << "\nThe arguments have the following meaning:"
          << "\n   job    AQUASIM job file"
          << "\n   log    output text file for job information"
          << "\n   load   input aquasim system file with model definition" 
          << "\n   save   output aquasim system file for saving results"
          << "\n   scmd   optional input text file with simulation time steps"
          << "\n   rank   optional output text file for sensitivity ranking"
          << "\n   sens   optional output text file for sensitivity functions"
          << "\n   fit    output text file for parameter estimation"
          << "\n   vcmd   input text file with names and values of parameters"
          << "\n   ucmd   input text file with names and values of parameters"
          << "\n   pars   output text file with given and fitted parameters"
          << "\n   chi    output text file with parameter and chi^2 values"
          << "\n   rcmd   input text file defining results to be calculated"
          << "\n   res1   output text file with parameter and result values"
          << "\n   res2   output text file with parameter and result values"
          << "\n   pri    input text file defining prior distribution"
          << "\n   max    output text file with parameter and result values"
          << "\n   var    output text file with estimated mean and variance"
          << "\n   jmp    input text file with sample of symmetric jumping dist."
          << "\n   mc     output text file with the Markov Chain"
          << "\n   pcmd   input text file defining plots to be plotted"
          << "\n   lcmd   input text file defining plots to be listed"
          << "\n";
   }
   out.flush();
   return;
}


static void IllegalArgs()
{
   WriteIllegalArgs(*cjob);
   if ( cjob != &std::cout ) WriteIllegalArgs(std::cout);
   logfile.close();
   aqexit();
}


static void TermError(const char* message)
{
   *cjob << StartError << message << EndError; cjob->flush();
   if ( cjob != &std::cout )
   {
      std::cout << StartError << message << EndError; std::cout.flush();
   }
   logfile.close();
   aqexit();
}
   

void calc(int argc, char* argv[], CARDINAL jobnum=0)
{
   int   i;
   const CARDINAL BufSize = 128;
   char  Buf[BufSize];

   std::ifstream testfile;
   std::ifstream loadfile;
   std::ofstream savefile;
   std::ifstream cmdfile;

   AQUASYS system;

   if ( strlen(argv[1]) != 2 ) IllegalArgs();
   if ( argv[1][0] != '-' )    IllegalArgs();
   if ( argc < 5 )             IllegalArgs();

   // check if logfile is AQUASIM system file

   if ( ExistFile(argv[2]) )
   {
      testfile.open(argv[2],std::ios::in);
      if ( system.CheckSysFile(testfile) == TRUE ) TermError(LogError1);
      testfile.close();
   }
   logfile.open(argv[2],std::ios::out|std::ios::trunc);
   if ( logfile.fail() )
   {
      TermError(LogError2);
   }
   SetFileType(argv[2],FTlog,FClog);
   SetJobOut(&logfile);
   system.InitLogFile();
   *cjob << "\n\n\n\n" << CmdLine << ":\n";
   for ( i=0; i<argc; i++ ) *cjob << argv[i] << " ";
   *cjob << "\n\n";
   cjob->flush();

   for ( i=3; i<argc; i++ )
   {
      if ( strcmp(argv[i],argv[2]) == 0 ) TermError(LogError3);
   }

   loadfile.open(argv[3],std::ios::in);
   if ( loadfile.fail() )                      TermError(LoadError1);
   if ( system.Load(loadfile) != LoadSuccess ) TermError(LoadError2);
   loadfile.close();

   JOBSTATUS status;
   switch ( argv[1][1] )
   {
      case 's':
         std::cout << "\n" << ProgName() << ": " << Sim; std::cout.flush();
         if ( argc < 6 )
         {
            if ( system.Simulate() != OK ) TermError(SimError3);
         }
         else
         {
            cmdfile.open(argv[5],std::ios::in);
            if ( cmdfile.fail() ) TermError(SimError1);

            while ( cmdfile.getline(Buf,BufSize) )
            {
               CALC calc;
			   calc.AddStepSeries(0.1,10,0);
               CARDINAL calcnum, numsteps; REAL time;
               std::istrstream in(Buf,BufSize);
               in >> calcnum;
               if ( !in.fail() )
               {
                  in >> time;     if ( in.fail() ) TermError(SimError2);
                  BOOLEAN initialize = FALSE;
                  in >> numsteps; if ( in.fail() ) initialize = TRUE;
                  calc.CalcNum(calcnum);
                  if ( initialize == TRUE )
                  {
                     calc.InitTime(time);
                     if ( system.InitCalc(&calc) != OK ) TermError(SimError3);
                  }
                  else
                  {
                     if ( system.NumStates(calcnum) == 0 ) TermError(SimError4);
                     calc.ReplaceStepSeries(time,numsteps,0);
                     if ( system.Calculate(&calc) != OK ) TermError(SimError3);
                  }
               }
            }
         }
         savefile.open(argv[4],std::ios::out|std::ios::trunc);
         if ( savefile.fail() )             TermError(SaveError1);
         SetFileType(argv[4],FTaqu,FCaqu);
         if ( system.Save(savefile) != OK )
         {
            savefile.close();
            TermError(SaveError2);
         }
         savefile.close();
         break;
      case 'a':
         {
            std::cout << "\n" << ProgName() << ": " << Sens; 
            std::cout.flush();
            std::ofstream  rankfile;
            std::ofstream* rankfileptr = 0;
            std::ofstream  sensfile;
            std::ofstream* sensfileptr = 0;
            std::ifstream  cmdfile;
            std::ifstream* cmdfileptr = 0;
            if ( argc < 6 )
            {
               rankfileptr = 0;
            }
            else
            {
               rankfile.open(argv[5],std::ios::out|std::ios::trunc);
               SetFileType(argv[5],FTfit,FCfit);
               if ( !rankfile.fail() )
               {
                  rankfileptr = &rankfile;
               }
               if ( argc >= 7 )
               {
                  sensfile.open(argv[6],std::ios::out|std::ios::trunc);
                  SetFileType(argv[6],FTfit,FCfit);
                  if ( !sensfile.fail() )
                  {
                     sensfileptr = &sensfile;
                  }
                  if ( argc >= 8 )
                  {
                     cmdfile.open(argv[7],std::ios::in);
                     if ( !cmdfile.fail() )
                     {
                        cmdfileptr = &cmdfile;
                        if ( sensfileptr==0 || rankfileptr==0 )
                           TermError(SensError3);
                     }
                  }
               }
            }
            BOOLEAN calcstates = FALSE;
            if ( system.NumCalc() > 0 )
            {
               if ( system.NumPar(system.CalcNums(0)) > 0 ) calcstates = TRUE;
            }
            if ( calcstates != TRUE )
            {
               if ( system.SensAnal(TRUE) != OK ) TermError(SensError1);
               savefile.open(argv[4],std::ios::out|std::ios::trunc);
               if ( savefile.fail() )             TermError(SaveError1);
               SetFileType(argv[4],FTaqu,FCaqu);
               if ( system.Save(savefile) != OK )
               {
                  savefile.close();
                  TermError(SaveError2);
               }
               savefile.close();
            }
            if ( rankfileptr != 0 )
            {
               if ( system.SensRank(TRUE,rankfileptr,sensfileptr,cmdfileptr) != OK )
                  TermError(SensError2);
               rankfile.close();
            }
            if ( sensfileptr != 0 ) sensfile.close();
         }
         break;
      case 'e':
         {
            CARDINAL numsteps = system.FitIter();
            CARDINAL maxsteps = numsteps;
            std::cout << "\n" << ProgName() << ": " ;
            if ( numsteps == 0 ) std::cout << Fit0;
            else                 std::cout << Fit;
            std::cout.flush();
            if ( argc < 6 ) IllegalArgs();
            std::ofstream fitfile;
            fitfile.open(argv[5],std::ios::out|std::ios::trunc);
            SetFileType(argv[5],FTfit,FCfit);
            if ( fitfile.fail() ) TermError(FitError1);
            status = system.Fit(fitfile,&numsteps);
            if ( status==PROBLEM && numsteps==maxsteps ) status = OK;
            fitfile.close();
            *cjob << "\n\n";
            if ( status != OK )
            {
               *cjob << StartError << FitError2 << EndError;
            } 
            cjob->flush();
            savefile.open(argv[4],std::ios::out|std::ios::trunc);
            if ( savefile.fail() )
            {
               TermError(SaveError1);
            }
            SetFileType(argv[4],FTaqu,FCaqu);
            if ( system.Save(savefile) != OK )
            {
               savefile.close();
               TermError(SaveError2);
            }
            savefile.close();
         }
         break;
      case 'f':
         {
            std::cout << "\n" << ProgName() << ": " << SetFit; std::cout.flush();
            if ( argc < 7 ) IllegalArgs();
            std::ifstream varfile;
            varfile.open(argv[4],std::ios::in);
            if ( varfile.fail() ) TermError(SetFitError1);
            std::ofstream parfile;
            parfile.open(argv[5],std::ios::out|std::ios::trunc);
            SetFileType(argv[5],FTfit,FCfit);
            if ( parfile.fail() ) TermError(SetFitError2);
            std::ofstream fitfile;
            fitfile.open(argv[6],std::ios::out|std::ios::trunc);
            SetFileType(argv[6],FTfit,FCfit);
            if ( fitfile.fail() ) TermError(SetFitError3);
            status = system.SetFit(varfile,parfile,fitfile);
            parfile.close();
            fitfile.close();
			varfile.close();
            *cjob << "\n\n";
            if ( status != OK ) TermError(SetFitError4);
            cjob->flush();
            *cjob << "\n\n"; cjob->flush();
         }
         break;
      case 'i':
         {
            if ( system.FitIter() == 0 ) TermError(IdError1);
            std::cout << "\n" << ProgName() << ": " ;
            std::cout << Inv;
            std::cout.flush();
            if ( argc < 6 ) IllegalArgs();
            std::ofstream fitfile;
            fitfile.open(argv[4],std::ios::out|std::ios::trunc);
            SetFileType(argv[4],FTfit,FCfit);
            if ( fitfile.fail() ) TermError(IdError2);
            cmdfile.open(argv[5],std::ios::in);
            if ( cmdfile.fail() ) TermError(IdError3);
            if ( system.IdAnal(cmdfile,fitfile) != OK )
               TermError(IdError4);
            fitfile.close();
            *cjob << "\n\n";
         }
         break;
      case 'c':
         {
            std::cout << "\n" << ProgName() << ": " << Chi; std::cout.flush();
            if ( argc < 6 ) IllegalArgs();
            std::ifstream parfile;
            parfile.open(argv[4],std::ios::in);
            if ( parfile.fail() ) TermError(ChiError1);
            std::ofstream chifile;
            chifile.open(argv[5],std::ios::out|std::ios::trunc);
            SetFileType(argv[5],FTfit,FCfit);
            if ( chifile.fail() ) TermError(ChiError2);
            status = system.CalcChi2(parfile,chifile);
            chifile.close();
			parfile.close();
            *cjob << "\n\n";
            if ( status != OK ) TermError(ChiError3);
            cjob->flush();
            *cjob << "\n\n"; cjob->flush();
         }
         break;
      case 'r':
         {
            std::cout << "\n" << ProgName() << ": " << Res; std::cout.flush();
            if ( argc < 7 ) IllegalArgs();
            std::ifstream vcmdfile;
            vcmdfile.open(argv[4],std::ios::in);
            if ( vcmdfile.fail() ) TermError(ResError1);
            std::ifstream rcmdfile;
            rcmdfile.open(argv[5],std::ios::in);
            if ( rcmdfile.fail() ) TermError(ResError2);
            std::ofstream resfile;
            resfile.open(argv[6],std::ios::out|std::ios::trunc);
            SetFileType(argv[6],FTfit,FCfit);
            if ( resfile.fail() ) TermError(ResError3);
            status = system.CalcRes(vcmdfile,rcmdfile,resfile);
            resfile.close();
			rcmdfile.close();
			vcmdfile.close();
            *cjob << "\n\n";
            if ( status != OK )
            {
               if ( status == PROBLEM ) TermError(ResError4);
               else                     TermError(ResError5);
            }
            cjob->flush();
            *cjob << "\n\n"; cjob->flush();
         }
         break;
      case 'u':
         {
            std::cout << "\n" << ProgName() << ": " << Res; std::cout.flush();
            if ( argc < 7 ) IllegalArgs();
            std::ifstream ucmdfile;
            ucmdfile.open(argv[4],std::ios::in);
            if ( ucmdfile.fail() ) TermError(ResError1);
            std::ifstream rcmdfile;
            rcmdfile.open(argv[5],std::ios::in);
            if ( rcmdfile.fail() ) TermError(ResError2);
            std::ofstream resfile;
            resfile.open(argv[6],std::ios::out|std::ios::trunc);
            SetFileType(argv[6],FTfit,FCfit);
            if ( resfile.fail() ) TermError(ResError3);
            status = system.CalcSingleRes(ucmdfile,rcmdfile,resfile);
            resfile.close();
			rcmdfile.close();
			ucmdfile.close();
            *cjob << "\n\n";
            if ( status != OK )
            {
               if ( status == PROBLEM ) TermError(ResError4);
               else                     TermError(ResError5);
            }
            cjob->flush();
            *cjob << "\n\n"; cjob->flush();
         }
         break;
      case 'b':
         {
            std::cout << "\n" << ProgName() << ": " << MaxPost; std::cout.flush();
            if ( argc < 7 ) IllegalArgs();
            std::ifstream prifile;
            prifile.open(argv[4],std::ios::in);
            if ( prifile.fail() ) TermError(MaxPostError1);
            std::ofstream maxfile;
            maxfile.open(argv[5],std::ios::out|std::ios::trunc);
            SetFileType(argv[5],FTfit,FCfit);
            if ( maxfile.fail() ) TermError(MaxPostError2);
            std::ofstream varfile;
            varfile.open(argv[6],std::ios::out|std::ios::trunc);
            SetFileType(argv[6],FTfit,FCfit);
            if ( varfile.fail() ) TermError(MaxPostError3);
            status = system.MaxPost(prifile,maxfile,varfile);
            maxfile.close();
            prifile.close();
            *cjob << "\n\n";
            if ( status != OK )
            {
               TermError(MaxPostError4);
            }
            cjob->flush();
            *cjob << "\n\n"; cjob->flush();
         }
         break;
      case 'm':
         {
            std::cout << "\n" << ProgName() << ": " << MCMC; std::cout.flush();
            if ( argc < 7 ) IllegalArgs();
            std::ifstream prifile;
            prifile.open(argv[4],std::ios::in);
            if ( prifile.fail() ) TermError(MCMCError1);
            std::ifstream jmpfile;
            jmpfile.open(argv[5],std::ios::in);
            if ( jmpfile.fail() ) TermError(MCMCError2);
            std::ofstream mcfile;
            mcfile.open(argv[6],std::ios::out|std::ios::trunc);
            SetFileType(argv[6],FTfit,FCfit);
            if ( mcfile.fail() ) TermError(MCMCError3);
            status = system.MCMC(prifile,jmpfile,mcfile);
            mcfile.close();
            prifile.close();
            jmpfile.close();
            *cjob << "\n\n";
            if ( status != OK )
            {
               TermError(MCMCError4);
            }
            cjob->flush();
            *cjob << "\n\n"; cjob->flush();
         }
         break;
      case 'l':
         std::cout << "\n" << ProgName() << ": " << ListRes; std::cout.flush();
         cmdfile.open(argv[4],std::ios::in);
         if ( cmdfile.fail() ) TermError(ListError1);
         cmdfile.width(BufSize);
         while ( 1 )
         {
            cmdfile >> Buf; if ( cmdfile.fail() ) break;
            PLOT* plot = system.GetPlot(Buf);
            std::ofstream listfile;
            if ( plot == 0 ) 
            {
               *cjob << "\n*** plot " << Buf << " not found ***";
               cjob->flush();
               cmdfile >> Buf; if ( cmdfile.fail() ) break;
            }
            else
            {
               cmdfile >> Buf; if ( cmdfile.fail() ) TermError(ListError2);
               listfile.open(Buf,std::ios::out|std::ios::app);
               SetFileType(Buf,FTlis,FClis);
               if ( listfile.fail() ) TermError(ListError3);
               if ( system.ListResults(listfile,plot) == OK )
               {
                  *cjob << "\n" << plot->Symbol() << " listed to " << Buf;
                  cjob->flush();
               }
               else
               {
                  *cjob << "\n*** unable to list plot " << plot->Symbol()
                        << " ***";
                  cjob->flush();
               }
            }
            listfile.close();
         }
         *cjob << "\n\n"; cjob->flush();
         cmdfile.close();
         break;
      case 'p':
         std::cout << "\n" << ProgName() << ": " << PlotRes; std::cout.flush();
         cmdfile.open(argv[4],std::ios::in);
         if ( cmdfile.fail() ) TermError(PlotError1);
         cmdfile.width(BufSize);
         while ( 1 )
         {
            cmdfile >> Buf; if ( cmdfile.fail() ) break;
            PLOT* plot = system.GetPlot(Buf);
            if ( plot == 0 ) 
            {
               *cjob << "\n*** plot " << Buf << " not found ***";
               cjob->flush();
               cmdfile >> Buf; if ( cmdfile.fail() ) break;
            }
            else
            {
               cmdfile >> Buf; if ( cmdfile.fail() ) TermError(PlotError2);
               std::ifstream oplotfile;
               std::ofstream plotfile;
               if ( ExistFile(Buf) == TRUE ) oplotfile.open(Buf,std::ios::in);
               plotfile.open(ptstmpfile,std::ios::out|std::ios::trunc);
               SetFileType(ptstmpfile,FTpts,FCpts);
               if ( plotfile.fail() ) TermError(PlotError3);
               status = system.PlotResults(oplotfile,plotfile,plot,system.PlotFilOpt()->Eps());
               plotfile.close();
               oplotfile.close();
               if ( status == OK )
               {
                  if ( ExistFile(Buf) == TRUE ) RemoveFile(Buf);
                  RenameFile(ptstmpfile,Buf);
                  *cjob << "\n" << plot->Symbol() << " plotted to " << Buf;
                  cjob->flush();
               }
               else
               {
                  RemoveFile(ptstmpfile);
                  *cjob << "\n*** unable to plot " << plot->Symbol()
                        << " ***";
                  cjob->flush();
               }
            }
         }
         *cjob << "\n\n"; cjob->flush();
         cmdfile.close();
         break;
      default:
         IllegalArgs();
   }

   logfile.close();
   if ( jobnum > 0 )
   {
      std::cout << EndJob << " " << jobnum; std::cout.flush();
   }
   return;
}


int main(int argc, char* argv[])
{
   std::cout << "\n" << ProgName() << " - " << ProgVersion() << "\n";
   WriChars(std::cout,'-',strlen(ProgName())+3+strlen(ProgVersion()));
   std::cout << "\n";
   std::cout.flush();

   CARDINAL i;
   for ( i=0; i<argc; i++ ) // eliminate <CR> at the end of word after transfer to unix
   {
      CARDINAL lastpos = strlen(argv[i])-1;
      if ( argv[i][lastpos] < ' ' ) argv[i][lastpos] = '\0';
   }

   const int maxarg = 8;
   const int arglen = 128;
   char* filename;
   if ( argc > maxarg ) IllegalArgs();
   if ( argc==2 && argv[1][0]=='-' && argv[1][1]=='h' )
   {
      WriteIllegalArgs(std::cout,TRUE);
      aqexit();
   }
   if ( argc > 2 ) // job specified with command line args
   {
      calc(argc,argv);
   }
   else
   {
      filename = new char[arglen];
      if ( argc == 2 )       // job specified in job-file
      {
         filename[0] = '\0';
         if ( argv[1] != 0 )
         {
            if ( strlen(argv[1]) < arglen ) strcpy(filename,argv[1]);
         }
      }
      else                   // args-file is "aquasim.job"
      {
         strcpy(filename,"aquasim.job");
      }
      if ( ExistFile(filename) == FALSE ) IllegalArgs();
      std::ifstream in; in.open(filename,std::ios::in);
      if ( in.fail() ) IllegalArgs();
      int ARGC;  
      char* ARGV[maxarg]; for ( i=0; i<maxarg; i++ ) ARGV[i] = new char[arglen];
      CARDINAL jobnum = 1;
      BOOLEAN found   = FALSE;
      BOOLEAN endfile = FALSE;
      while ( 1 )            // loop for command lines in job file
      {
         char c;
         while ( 1 )                         // skip until '-'
         {
            if ( !in.get(c) )
            {
               endfile = TRUE;
               break;
            }
            if ( c == '#' )                  // skip until end of line
            {
               while ( in.get(c) )
               {
                  if ( c < ' ' ) break;
               }
            }
            else
            {
               if ( c == '-' ) break;        // '-' found
            }
         }
         if ( endfile == TRUE ) break;       // end of file
         if ( !in.get(c) ) IllegalArgs();    // read task switch (arg 1)
         ARGV[0][0] = '\0';                  // arg 0
         ARGV[1][0] = '-';                   // arg 1
         ARGV[1][1] = c;
         ARGV[1][2] = '\0';
         if ( !in.get(c) ) IllegalArgs();    // read space before arg 2
         if ( c != ' ' )   IllegalArgs();
         ARGC = 2;
         while ( ARGC < maxarg )             // loop for args > 2
         {
            i = 0;
            while ( 1 )                      // loop for characters of arg
            {
               if ( i+1 >= arglen ) IllegalArgs();  // arg too long
               if ( !in.get(c) )  break;     // end of file
               if ( c == ' ' )
               {
                  if ( i > 0 ) break;        // end of arg (skip leading blancs)
               }
               else
               {
                  if ( c <  ' ' ) break;     // end of line (line feed)
                  if ( c == '#' ) break;     // end of line (comment)
                  ARGV[ARGC][i] = c;
                  i++;
               }
            }
            ARGV[ARGC][i] = '\0';
            if ( strlen(ARGV[ARGC]) > 0 ) ARGC++;
            else                          break;

			if ( c < ' ' || c == '#' ) break;
         }
         if ( c >= ' ' )                    // skip to end of line
         {
            while ( in.get(c) )
            {
               if ( c < ' ' ) break;
            }
         }
         found = TRUE;
         calc(ARGC,ARGV,jobnum);
         jobnum++;
      }
      in.close();
      if ( found == FALSE ) IllegalArgs();
   }
   std::cout << "\n\n" << ProgName() << ": " << EndProg << "\n"; 
   std::cout.flush();
   return 0;
}


//////////////////////////////////////////////////////////////////////////////
