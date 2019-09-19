/** \file
 *	\brief definitions for the flags field of the run struct, containing
 *	runtime state information.
 */

#define VAR_SUPPRESS_FLAG 	0x01	/* model variance supression */
#define PRED_VOL_FLAG 		0x02	/* predator density/volume interaction*/
#define AGE_DEP_FGE_FLAG 	0x04	/* age dependent fge */
#define UNTRUNCATED_RESERVOIR_SURVIVAL_FLAG 	0x08	/* If reservoir survival should not be truncated (to [0, 1]) for calibration purposes */
#define ALL_RUN_FLAGS		0x0f    /* mask for all possible flags */

