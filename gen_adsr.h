#ifndef GEN_ADSR_H_
#define GEN_ADSR_H_

#include "globals.h"
#include <stdint.h>

void 		envelope_adsr_init(struct gen_adsr_struct *adsr);
uint16_t 	envelope(struct gen_adsr_struct *adsr);

#endif /* GEN_ADSR_H_ */
