#ifndef __HCM5883L_H_
#define __HCM5883L_H_

void hcm5883l_initialize();
void getHeading(int16_t *x, int16_t *y, int16_t *z);

#endif