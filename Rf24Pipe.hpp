//*************************************************************************************************
//
//  BITTAILOR.CH - BtCore
//
//-------------------------------------------------------------------------------------------------
//
//  Rf24Pipe
//  
//*************************************************************************************************

#ifndef INC__Rf24Pipe__hpp
#define INC__Rf24Pipe__hpp



class Rf24Pipes {
   public:

      enum Rf24Pipe
      {
         PIPE_0,
         PIPE_1,
         PIPE_2,
         PIPE_3,
         PIPE_4,
         PIPE_5,
      };

      enum { NUMBER_OF_PIPES = 6 };

      static Rf24Pipe ALL_PIPES[NUMBER_OF_PIPES];
};

#endif // INC__Rf24Pipe__hpp
