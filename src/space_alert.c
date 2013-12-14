/* space_alert.c */

#include "config.h"
#include "space.h"

/* ------------------------------------------------------------------------ */

void alert_main_balance (int x)
{
	console_message(x, "engineering", ansi_alert(tprintf("M/A reactor balanced at %.3f%%", sdb[x].main.out * 100.0)));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_aux_balance (int x)
{
	console_message(x, "engineering", ansi_alert(tprintf("Fusion reactor balanced at %.3f%%", sdb[x].aux.out * 100.0)));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_batt_balance (int x)
{
	console_message(x, "engineering", ansi_alert(tprintf("Batteries set at %.3f%%", sdb[x].batt.out * 100.0)));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_main_overload (int x)
{
	do_all_console_notify(x, ansi_warn("M/A REACTOR CORE BREACH IN PROGRESS"));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_aux_overload (int x)
{
	do_all_console_notify(x, ansi_warn("FUSION REACTOR CORE BREACH IN PROGRESS"));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_warp_overload (int x)
{
	console_message(x, "engineering damage", ansi_warn("WARP DRIVE BURNOUT: Warp drive now offline"));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_impulse_overload (int x)
{
	console_message(x, "engineering damage", ansi_warn("IMPULSE DRIVE BURNOUT: Impulse drive now offline"));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_anti_runout (int x)
{  
	console_message(x, "engineering", ansi_warn("ANTIMATTER DEPLETION: M/A reactor now offline"));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_deut_runout (int x)
{
	console_message(x, "engineering", ansi_warn("DEUTERIUM DEPLETION: All reactors now offline"));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_batt_runout (int x)
{
	console_message(x, "engineering", ansi_warn("BATTERY DEPLETION: Batteries now offline"));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_cloak_failure (int x)
{
	console_message(x, "engineering helm tactical", ansi_warn("Insufficient power: Cloaking device disengaged"));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_cloak_voided (int x)
{
	console_message(x, "helm tactical", ansi_warn("Cloaking device voided. Deactivating"));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_beam_balance (int x)
{
	console_message(x, "tactical", ansi_alert("Beam capacitor power balanced"));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_beam_charged (int x)
{
	console_message(x, "tactical", ansi_alert("Beam capacitor fully charged"));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_missile_balance (int x)
{
	console_message(x, "tactical", ansi_alert("Missile capacitor power balanced"));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_missile_charged (int x)
{
	console_message(x, "tactical", ansi_alert("Missile capacitor fully charged"));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_speed_warp (int x)
{
	console_message(x, "engineering helm", ansi_alert(tprintf("Speed now warp %.6f", sdb[x].move.out)));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_speed_impulse (int x)
{
	console_message(x, "engineering helm", ansi_alert(tprintf("Speed now %.3f%% impulse", sdb[x].move.out * 100.0)));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_speed_stop (int x)
{
	console_message(x, "engineering helm", ansi_alert("Speed now full stop"));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_enter_quadrant (int x)
{
	console_message(x, "helm", ansi_alert(tprintf("Entering %s quadrant", unparse_quadrant(x))));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_yaw (int x)
{
	console_message(x, "helm", ansi_alert(tprintf("Yaw now %.3f degrees", sdb[x].course.yaw_out)));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_pitch (int x)
{
	console_message(x, "helm", ansi_alert(tprintf("Pitch now %.3f degrees", sdb[x].course.pitch_out)));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_roll (int x)
{
	console_message(x, "helm", ansi_alert(tprintf("Roll now %.3f degrees", sdb[x].course.roll_out)));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_max_repair (int x)
{
	console_message(x, "damage", ansi_alert("Repair capacity maximized"));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_tract_unlock (int n1, int n2, dbref enactor)
{
	console_message(n1, "helm operation science", ansi_cmd(enactor, tprintf("Tractor beam unlocked from %s", unparse_identity(n1, n2))));
	console_message(n2, "helm operation science", ansi_alert(tprintf("Tractor beam from %s unlocked", unparse_identity(n2, n1))));
	do_space_notify_two(n1, n2, console_operation, console_helm, console_science, "has unlocked tractor beam from");
	do_ship_notify(n2, tprintf("%s is released from a tractor beam.", Name(sdb[n2].object)));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_tract_lock (int n1, int n2, dbref enactor)
{
	console_message(n1, "helm operation science", ansi_cmd(enactor, tprintf("Tractor beam locked on %s", unparse_identity(n1, n2))));
	console_message(n2, "helm operation science", ansi_alert(tprintf("Tractor beam locked from %s", unparse_identity(n2, n1))));
	do_space_notify_two(n1, n2, console_operation, console_helm, console_science, "has locked tractor beam on");
	do_ship_notify(n2, tprintf("%s is seized by a tractor beam.", Name(sdb[n2].object)));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_tract_attempt (int n1, int n2, dbref enactor)
{
	console_message(n1, "helm operation science", ansi_cmd(enactor, tprintf("Tractor beam lock on %s attempted and failed", unparse_identity(n1, n2))));
	console_message(n2, "helm operation science", ansi_alert(tprintf("Tractor beam lock from %s attempted and failed", unparse_identity(n2, n1))));
	do_space_notify_two(n1, n2, console_operation, console_helm, console_science, "attempts and fails to lock tractor beam on");
	return;
}

/* ------------------------------------------------------------------------ */

void alert_tract_lost (int n1, int n2)
{
	console_message(n1, "helm operation science", ansi_alert(tprintf("Tractor beam lock on %s lost", unparse_identity(n1, n2))));
	console_message(n2, "helm operation science", ansi_alert(tprintf("Tractor beam lock from %s lost", unparse_identity(n2, n1))));
	do_space_notify_two(n1, n2, console_operation, console_helm, console_science,  "has lost tractor beam lock on");
	do_ship_notify(n2, tprintf("%s is released from a tractor beam.", Name(sdb[n2].object)));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_ship_cloak_online (int x)
{
	do_ship_notify(x, tprintf("%s engages its cloaking device.", Name(sdb[x].object)));
	do_space_notify_one(x, console_helm, console_tactical, console_science, "engages its cloaking device");
	return;
}

/* ------------------------------------------------------------------------ */

void alert_ship_cloak_offline (int x)
{
	do_ship_notify(x, tprintf("%s disengages its cloaking device.", Name(sdb[x].object)));
	do_space_notify_one(x, console_helm, console_tactical, console_science,  "disengages its cloaking device");
	return;
}

/* ------------------------------------------------------------------------ */

void alert_ship_enter_warp (int x)
{
	do_ship_notify(x, tprintf("%s shifts into warp.", Name(sdb[x].object)));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_ship_exit_warp (int x)
{
	do_ship_notify(x, tprintf("%s drops out of warp.", Name(sdb[x].object)));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_ship_hit (int x)
{
	if (sdb[x].structure.type == 1 || sdb[x].structure.type == 2)
		do_ship_notify(x, tprintf("%s shudders from an impact.",
	  	  Name(sdb[x].object)));
	return;
}

/* ------------------------------------------------------------------------ */

void alert_ship_hurt (int x)
{
	if (sdb[x].structure.type == 1 || sdb[x].structure.type == 2) {
		do_ship_notify(x, tprintf("%s rocks violently from an impact.",
	  	  Name(sdb[x].object)));
	} else if (sdb[x].structure.type == 3 && get_random_long(0,10) == 1)
		do_ship_notify(x, tprintf("%s trembles from a surface impact.",
	  	  Name(sdb[x].object)));
	return;
}

/* ------------------------------------------------------------------------ */
