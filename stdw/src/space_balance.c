/* space_balance.c */

#include "config.h"
#include "space.h"

/* ------------------------------------------------------------------------ */

void balance_eng_power (void) /* Balances the eng alloc */
{
	double total = sdb[n].alloc.helm + sdb[n].alloc.tactical +
	 sdb[n].alloc.operations;

	if (total > 0.0) {
		sdb[n].alloc.helm /= total;
		sdb[n].alloc.tactical /= total;
		sdb[n].alloc.operations /= total;
	} else {
		sdb[n].alloc.helm = 0.0;
		sdb[n].alloc.tactical = 0.0;
		sdb[n].alloc.operations = 1.0;
	}
	return;
}

/* ------------------------------------------------------------------------ */

void balance_helm_power (void) /* Balances the helm alloc */
{
	double total;

	if (sdb[n].alloc.helm > 0.0) {
		total = sdb[n].alloc.movement + sdb[n].alloc.shields + sdb[n].alloc.cloak;
		if (total > 0.0) {
			total /= sdb[n].alloc.helm;
			sdb[n].alloc.movement /= total;
			sdb[n].alloc.shields /= total;
			sdb[n].alloc.cloak /= total;
		} else {
			sdb[n].alloc.shields = sdb[n].alloc.helm;
			sdb[n].alloc.movement = sdb[n].alloc.cloak = 0.0;
		}
	} else {
		sdb[n].alloc.movement = 0.0;
		sdb[n].alloc.shields = 0.0;
		sdb[n].alloc.cloak = 0.0;
	}
	return;
}

/* ------------------------------------------------------------------------ */

void balance_shield_power (void) /* Balances the shield alloc */
{
	register int i;
	double total = 0.0;

	if (sdb[n].alloc.shields > 0.0) {
		for (i = 0 ; i < MAX_SHIELD_NAME ; ++i)
			total += sdb[n].alloc.shield[i];
		if (total > 0.0) {
			total /= sdb[n].alloc.shields;
			for (i = 0 ; i < MAX_SHIELD_NAME ; ++i)
				sdb[n].alloc.shield[i] /= total;
		} else {
			for (i = 0 ; i < MAX_SHIELD_NAME ; ++i)
				sdb[n].alloc.shield[i] = sdb[n].alloc.shields / MAX_SHIELD_NAME;
		}
	} else {
		for (i = 0 ; i < MAX_SHIELD_NAME ; ++i)
			sdb[n].alloc.shield[i] = 0;
	}
	return;
}

/* ------------------------------------------------------------------------ */

void balance_tact_power (void) /* Balances the tactical alloc */
{
	double total;

	if (sdb[n].alloc.tactical > 0.0) {
		total = sdb[n].alloc.beams + sdb[n].alloc.missiles + sdb[n].alloc.sensors;
		if (total > 0.0) {
			total /= sdb[n].alloc.tactical;
			sdb[n].alloc.beams /= total;
			sdb[n].alloc.missiles /= total;
			sdb[n].alloc.sensors /= total;
		} else {
			sdb[n].alloc.beams = sdb[n].alloc.missiles = 0.0;
			sdb[n].alloc.sensors = sdb[n].alloc.tactical;
		}
	} else {
		sdb[n].alloc.beams = 0.0;
		sdb[n].alloc.missiles = 0.0;
		sdb[n].alloc.sensors = 0.0;
	}
	return;
}

/* ------------------------------------------------------------------------ */

void balance_sensor_power (void) /* Balances the sensor alloc */
{
	double total;

	if (sdb[n].alloc.sensors > 0.0) {
		total = sdb[n].alloc.ecm + sdb[n].alloc.eccm;
		if (total > 0.0) {
			total /= sdb[n].alloc.sensors;
			sdb[n].alloc.ecm /= total;
			sdb[n].alloc.eccm /= total;
		} else {
			sdb[n].alloc.ecm = sdb[n].alloc.eccm = sdb[n].alloc.sensors / 2.0;
		}
	} else {
		sdb[n].alloc.ecm = 0.0;
		sdb[n].alloc.eccm = 0.0;
	}
	return;
}

/* ------------------------------------------------------------------------ */

void balance_ops_power (void) /* Balances the operations alloc */
{
	double total;

	if (sdb[n].alloc.operations > 0.0) {
		total = sdb[n].alloc.transporters + sdb[n].alloc.tractors + sdb[n].alloc.miscellaneous;
		if (total > 0.0) {
			total /= sdb[n].alloc.operations;
			sdb[n].alloc.transporters /= total;
			sdb[n].alloc.tractors /= total;
			sdb[n].alloc.miscellaneous /= total;
		} else {
			sdb[n].alloc.transporters = sdb[n].alloc.tractors = 0.0;
			sdb[n].alloc.miscellaneous = sdb[n].alloc.operations;
		}
	} else {
		sdb[n].alloc.transporters = 0.0;
		sdb[n].alloc.tractors = 0.0;
		sdb[n].alloc.miscellaneous = 0.0;
	}
	return;
}

/* ------------------------------------------------------------------------ */
