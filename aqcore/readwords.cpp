//////////////////////////////   readwords.cpp   /////////////////////////////
//
//              date:       person:           comments:
//
// creation:    11.02.00    Peter Reichert    
// revisions:   17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include "readwords.h"

//////////////////////////////////////////////////////////////////////////////

const CARDINAL chunksize = 10;

const CARDINAL numcommentchar = 2;
char commentchar[numcommentchar] = { '#' ,  '!'  };

const CARDINAL numendlinechar = 2;
char endlinechar[numendlinechar] = { '\n' , '\r' };

//////////////////////////////////////////////////////////////////////////////

CARDINAL ReadWords(std::istream& in, char**& words)
{
   CARDINAL i;
   CARDINAL numwords = 0;
   CARDINAL numchars = 0;
   CARDINAL maxchars = 0;
   char c;
   while ( 1 )
   {
      in.get(c);
      if ( in.fail() )
      {
         if ( numchars > 0 ) // end word
         {
            if ( numchars < maxchars )
            {
               words[numwords][numchars] = '\0';
            }
            else
            {
               char* word = new char[numchars+1];
               for ( i=0; i<numchars; i++ ) word[i] = words[numwords][i];
               word[numchars] = '\0';
               delete [] words[numwords];
               words[numwords] = word;
            }
            numwords++;
            numchars = 0;
            maxchars = 0;
         }
         break;
      }

      // check for comment:

      BOOLEAN comment = FALSE;
      for ( i=0; i<numcommentchar; i++ ) 
      {
         if ( c == commentchar[i] ) 
         {
            comment = TRUE;
            break;
         }
      }
      if ( comment == TRUE )  // read to endline
      {
         while ( in.get(c) )
         {
            BOOLEAN endline = FALSE;
            for ( i=0; i<numendlinechar; i++ )
            {
               if ( c == endlinechar[i] )
               {
                  endline = TRUE;
                  break;
               }
            }
            if ( endline == TRUE ) break;
         }
         if ( numwords > 0 ) break; // cont. to next line if no words
      }
      else
      {
         // check for endline

         BOOLEAN endline = FALSE;
         for ( i=0; i<numendlinechar; i++ ) 
         {
            if ( c == endlinechar[i] )
            {
               endline = TRUE;
               break;
            }
         }
         if ( endline == TRUE )
         {
            if ( numchars > 0 ) // end word
            {
               if ( numchars < maxchars )
               {
                  words[numwords][numchars] = '\0';
               }
               else
               {
                  char* word = new char[numchars+1];
                  for ( i=0; i<numchars; i++ ) word[i] = words[numwords][i];
                  word[numchars] = '\0';
                  delete [] words[numwords];
                  words[numwords] = word;
               }
               numwords++;
               numchars = 0;
               maxchars = 0;
            }
            if ( numwords > 0 ) break;
         }
         else
         {
            if ( c > ' ' ) // start new word or continue word
            {
               if ( numchars == 0 ) // start word
               {
                  char** newwords = new char*[numwords+1];
                  if ( numwords > 0 )
                  {
                     for ( i=0; i<numwords; i++ ) newwords[i] = words[i];
                     delete [] words;
                  }
                  words = newwords;
                  words[numwords] = new char[chunksize];
                  maxchars = chunksize;
                  words[numwords][0] = c;
                  numchars = 1;
               }
               else // continue word
               {
                  if ( numchars == maxchars ) // allocate space if necessary
                  {
                     char* newword = new char[numchars+chunksize];
                     for ( i=0; i<numchars; i++ ) newword[i] = words[numwords][i];
                     delete [] words[numwords];
                     words[numwords] = newword;
                     maxchars = numchars+chunksize;
                  }
                  words[numwords][numchars] = c;
                  numchars++;
               }
            }
            else // end word or skip char
            {
               if ( numchars > 0 ) // end word
               {
                  if ( numchars < maxchars )
                  {
                     words[numwords][numchars] = '\0';
                  }
                  else
                  {
                     char* word = new char[numchars+1];
                     for ( i=0; i<numchars; i++ ) word[i] = words[numwords][i];
                     word[numchars] = '\0';
                     delete [] words[numwords];
                     words[numwords] = word;
                  }
                  numwords++;
                  numchars = 0;
                  maxchars = 0;
               }
            }
         }
      }
   }
   // cout << "\nReadWords: " << numwords;
   // for ( i=0; i<numwords; i++ ) cout << " " << words[i];
   return numwords;
}

//////////////////////////////////////////////////////////////////////////////

void DeleteWords(CARDINAL num, char**& words)
{
   if ( num == 0 )   return;
   if ( words == 0 ) return;
   for ( CARDINAL i=0; i<num; i++ )
   {
      if ( words[i] != 0 ) delete [] words[i];
   }
   delete [] words;
   return;
}

//////////////////////////////////////////////////////////////////////////////
