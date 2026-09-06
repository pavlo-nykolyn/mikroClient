/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 06-09-2026 */
/**************************************/

#include <stdlib.h>
#include "mikro_validation.h"

int main(void)
{
   // value decoding
   mikro_decode_value_from_one_byte();
   mikro_decode_length_from_one_byte();

   mikro_decode_value_from_two_bytes();
   mikro_decode_length_from_two_bytes();

   mikro_decode_value_from_three_bytes();
   mikro_decode_length_from_three_bytes();

   mikro_decode_value_from_four_bytes();
   mikro_decode_length_from_four_bytes();

   mikro_decode_value_from_five_bytes();
   mikro_decode_length_from_five_bytes();
   // value encoding
   /*mikro_encode_value_into_one_byte();

   mikro_encode_value_into_two_bytes();

   mikro_encode_value_into_three_bytes();

   mikro_encode_value_into_four_bytes();

   mikro_encode_value_into_five_bytes();*/

   return EXIT_SUCCESS;
}