#ifndef I8080_FLAGS_H
#define I8080_FLAGS_H

namespace i8080 {

struct Flags 
{
   bool s = false; // sign
   bool z = false; // zero
   bool ac = false; // auxiliary carry
   bool p = false; // parity
   bool c = false; // carry 
}; 


} // namespace

#endif