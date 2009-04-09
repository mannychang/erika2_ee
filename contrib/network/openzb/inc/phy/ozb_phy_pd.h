/** 
* @file phy_pd.h
* @brief IEEE 802.15.4 PD-SAP Primitives
* @author Christian Nastasi
* @date 2009-03-19
*
* The file contains the declaration of the PD service primitives of the 
* IEEE 802.15.4 standard.
* The definition of the \b request funtion takes place in the phy subtree,
* while the \b confirm and \b indication ones are defined in the mac subree.
* \todo Make a specific reference to the files that implement these primitives.
*/
#ifndef __ozb_phy_pd_h__
#define __ozb_phy_pd_h__

#include <phy/ozb_phy_types.h>

int8_t ozb_PD_DATA_request(uint8_t psduLength, uint8_t *psdu); 
int8_t ozb_PD_DATA_confirm(enum ozb_phy_code_t status); 
int8_t ozb_PD_DATA_indication(uint8_t psduLength, uint8_t *psdu, 
			      uint8_t ppduLinkQuality); 

#endif /* Header Protection */
