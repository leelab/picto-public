#ifndef _SESSION_INFO_DEFINES_H_
#define _SESSION_INFO_DEFINES_H_

//! The number of Transitions to cache for connecting workstations.
#define MAX_RETAIN_TRANSITION_INFO 40
//! The number of property changes, per property, to cache in the CurrentSessionData.
#define MAX_RETAIN_PROPERTY_DATA 10
//! The key for the FrameState Data
#define FRAME_STATE_VAR_ID -1
//! The first key for StateTransition Data.  Keys go from -42 to -3
#define TRANSITION_STATE_VAR_ID -42
//! The key for RewardState Data
#define REWARD_STATE_VAR_ID -2
//! The Max key for SignalChannel Data.  Goes from -43 to -INTMAX
#define MAX_SIG_CHAN_VAR_ID -43
/*! The Starting key for Init changes.  Actually uses INT_MAX/2 + MAX_RETAIN_PROPERTY_DATA*assetId.
 *	Parallels the Prop keys of MAX_RETAIN_PROPERTY_DATA * assetId.
 */
#define PROP_INIT_START_VAR_ID (INT_MAX/2)

#define NEURAL_DATA_BUFFER_SECS 60
#define BEHAV_MATCHED_COLUMN 4
#define NEURAL_MATCHED_COLUMN 3
#define BEHAV_TIMESTAMP_COLUMN 1
#define NEURAL_TIMESTAMP_COLUMN 1
#define BEHAV_EVENT_CODE_COLUMN 2
#define NEURAL_EVENT_CODE_COLUMN 2
#define DISK_WRITE_FRAC_COEFF 10.0

#endif
