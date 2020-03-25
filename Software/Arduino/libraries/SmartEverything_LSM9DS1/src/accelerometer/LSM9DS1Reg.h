/*
 * LSM9DS1Reg.h
 *
 * Created: 26/04/2015 14:17:29
 *  Author: speirano
 */


#ifndef LSM9DS1_REG_H_
#define LSM9DS1_REG_H_



/* Accelerometer / Gyroscope */
#define LSM9DS1_AG_ADDRESS     0x6A

#define AG_WHO_AM_I            0x0F
#define AG_WHO_AM_I_RETURN     0x68

#define AG_CTRL_REG1_G         0x10
#define AG_ODR_SET             0x20

#define AG_STATUS_REG          0x27
#define AG_ACCELEROMETER_READY 0x01
#define AG_GYROSCOPE_READY     0x02
#define AG_ACC_X_L             0x28
#define AG_ACC_X_H             0x29
#define AG_ACC_Y_L             0x2A
#define AG_ACC_Y_H             0x2B
#define AG_ACC_Z_L             0x2C
#define AG_ACC_Z_H             0x2D

#define AG_GYR_X_L             0x18
#define AG_GYR_X_H             0x19
#define AG_GYR_Y_L             0x1A
#define AG_GYR_Y_H             0x1B
#define AG_GYR_Z_L             0x1C
#define AG_GYR_Z_H             0x1D
/* Magnetometer */

#define LSM9DS1_M_ADDRESS      0x1C

#define M_WHO_AM_I            0x0F
#define M_WHO_AM_I_RETURN     0x3D

#define M_CTRL_REG1_G         0x20
#define M_ODR_SET             0x20
#define M_CTRL_REG3_G         0x22
#define M_OPER_MODE_CONT      0x00
#define M_OPER_MODE_SINGLE    0x01
#define M_OPER_MODE_DIS       0x03

#define M_STATUS_REG          0x27
//#define M_ZYX_X_AXIS_READY    0x01
//#define M_ZYX_Y_AXIS_READY    0x02
//#define M_ZYX_Z_AXIS_READY    0x03
#define M_ZYX_AXIS_READY      0x08

#define M_X_L                 0x28
#define M_X_H                 0x29
#define M_Y_L                 0x2A
#define M_Y_H                 0x2B
#define M_Z_L                 0x2C
#define M_Z_H                 0x2D


/*Sensors Sensitivity */

/*
 * Linear acceleration : Reg6_XL  FS[1:0] = 00g +-2, 10+-4g 11+-8g
 * ranges:
 * +-2  g       mg/LSB = 0.061
 * +-4  g       mg/LSB = 0.122
 * +-8  g       mg/LSB = 0.244
 */
#define A_LSB_SENSIT_2MG       0.061
#define A_LSB_SENSIT_4MG       0.122
#define A_LSB_SENSIT_8MG       0.244

/*
 * Angular rate: CTRL_REG1  FS_G[1:0] 00 245, 01 500, 11 2000
 * +-245    mdps/LSB = 8.75
 * +-500    mdps/LSB = 17.50
 * +-2000   mdps/LSB = 70
 */
#define G_LSB_SENSIT_245MDPS    8.75
#define G_LSB_SENSIT_500DPS    17.5
#define G_LSB_SENSIT_2KMDPS    70
/*
 * Magnetic :  REG2_M FS[1:0] 00 +-4 01 +-8 10 +-12 11+-16
 * +-4       mgauss/LSB = 0.14
 * +-8       mgauss/LSB = 0.29
 * +-12      mgauss/LSB = 0.43
 * +-16      mgauss/LSB = 0.58
 */
#define M_LSB_SENSIT_4MG        0.14
#define M_LSB_SENSIT_8MG        0.29
#define M_LSB_SENSIT_12MG       0.43
#define M_LSB_SENSIT_16MG       0.58


#endif /* INCFILE1_H_ */
