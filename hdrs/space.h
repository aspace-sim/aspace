/* space.h */

#ifndef _SPACE_H_
#define _SPACE_H_

/* ------------------------------------------------------------------------ */

/* This stuff is included here, cos all the space_*.c files would
 * normally include them
 */

#include <ctype.h>
#include <string.h>
#include <math.h>
#ifdef I_SYS_TIME
#include <sys/time.h>
#else
#include <time.h>
#endif
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "copyrite.h"
#include "compile.h"
#include "conf.h"
#include "externs.h"
#include "ansi.h"
#include "intrface.h"
#include "strutil.h"
#include "parse.h"
#include "confmagic.h"
#include "dbdefs.h"
#include "flags.h"
#include "match.h"
#include "attrib.h"
#include "log.h"
#include "extchat.h"
#include "lock.h"
#include "game.h"
#include "function.h"
#include "funs.h"
#include "mushdb.h"
#include "command.h"
#include "intmap.h"
#include "version.h"
#include "mymalloc.h"

/* ------------------------------------------------------------------------ */

#define SpaceObj(x) (has_flag_by_name(x, "SPACE-OBJECT", TYPE_THING|TYPE_PLAYER))

#ifndef PI
#define PI 3.1415926535898
#endif /* PI */

/**
	Definitions for the new features of space_log.c
	space_log.c will now be capable of the following:-
		* writing to a log file
		* writing to a channel (falling back on the logfile if the channel doesn't exist)
		* write to both the channel and the logfile
**/
#define LOG_TYPE 3 // 1 = write to log file, 2 = send to log channel, 3 = do both 1 and 2
#define LOG_FILE "log/space.log"
#define LOG_CHANNEL "space-log"

#define MIN_OBJNUM			0
#define MAX_OBJNUM			(db_top)
#define MAX_LONG			((long) 0x7ffffffe)
#define MAX_DOUBLE          ((double) 1.7976931348623157E+308)
#define MAX_NAME			(30)
#define MAX_LIST			(360)
#define MAX_BUFF			(1024)

#define MIN_LIST_TOO_SMALL          1
#define MAX_LIST_TOO_LARGE          2
#define MIN_LIST_NOT_MET            3
#define MAX_LIST_EXCEEDED           4
#define CONVERSION_TOO_SMALL       14
#define CONVERSION_TOO_LARGE       15

typedef int return_t;
typedef unsigned int bool_t;
typedef char array_t[MAX_LIST][MAX_NAME+1];

#define COCHRANE			12927.238000
#define PARSEC				3085659622.014257
#define LIGHTYEAR			946057498.117920
#define LIGHTSPEED			29.979246

#define SUCCESS				((return_t) 0)

#define EMPTY_LIST 0

#define CONVERSION_ERROR	501
#define CANT_UPDATE			502

/**
 * PennMUSH 1.8.3p8 changed get_random_long() to get_random32
 *
 * So now we must make sure aspace works in previous versions
 * of penn as well as newer versions.
 */

#if ( NUMVERSION > 1008003007 )

  #define get_random_long(x,y) get_random32(x,y)

#endif

#define FILENAME "space.conf"
#define MAXBUF 1024
#define DELIM "="

/* ------------------------------------------------------------------------ */

#define SENSOR_FAIL				  -100
#define VACANCY_FAIL              -101
#define BAD_SDB_FAIL              -102
#define ARC_FAIL                  -103

#define MAX_SHIELD_NAME           6
#define MAX_EMPIRE_NAME           12
#define MAX_SYSTEM_NAME           19
#define MAX_QUADRANT_NAME         4
#define MAX_TYPE_NAME             12
#define MAX_BEAM_NAME             22
#define MAX_MISSILE_NAME          18

#define MIN_SPACE_OBJECTS         1
#define MAX_SPACE_OBJECTS         1500

#define MIN_CONSOLE_COUNT         1
#define MAX_CONSOLE_COUNT         20

#define MAX_DOCKING_DISTANCE      1.0
#define MAX_LANDING_DISTANCE      1.0
#define MAX_WORMHOLE_DISTANCE     10.0
#define MAX_TRANSPORTER_DISTANCE  5.0
#define MAX_NOTIFICATION_DISTANCE 100.0 * PARSEC

#define MIN_COMMS_OBJECTS         1
#define MAX_COMMS_OBJECTS         2000
#define MIN_COMMS_FREQUENCY       1.0
#define MAX_COMMS_FREQUENCY       1000.0
#define EXECUTE_ATTR_NAME         "EXECUTE"
#define ENCRYPTION_ATTR_NAME      "ENCRYPTION"

#define CONSOLE_SECURITY          7950
#define CONSOLE_HELM              2087
#define CONSOLE_ENGINEERING       2088
#define CONSOLE_OPERATION         12337
#define CONSOLE_SCIENCE           2954
#define CONSOLE_DAMAGE            2086
#define CONSOLE_COMMUNICATION     2085
#define CONSOLE_TACTICAL          2084
#define CONSOLE_TRANSPORTER       2083
#define CONSOLE_MONITOR           19362
#define CONSOLE_FIGHTER           23308

#define NO_SDB_ATTR               2000
#define NO_LOCATION_ATTR          2002
#define NO_SPACE_ATTR             2003

#define NO_ALLOCATE_ATTR          1002
#define NO_BEAM_ATTR              1003
#define NO_BEAM_ACTIVE_ATTR       1004
#define NO_BEAM_NAME_ATTR         1005
#define NO_BEAM_DAMAGE_ATTR       1006
#define NO_BEAM_BONUS_ATTR        1007
#define NO_BEAM_COST_ATTR         1008
#define NO_BEAM_RANGE_ATTR        1009
#define NO_BEAM_ARCS_ATTR         1010
#define NO_BEAM_LOCK_ATTR         1011
#define NO_BEAM_LOAD_ATTR         1012
#define NO_MISSILE_ATTR           1013
#define NO_MISSILE_ACTIVE_ATTR    1014
#define NO_MISSILE_NAME_ATTR      1015
#define NO_MISSILE_DAMAGE_ATTR    1016
#define NO_MISSILE_WARHEAD_ATTR   1017
#define NO_MISSILE_COST_ATTR      1018
#define NO_MISSILE_RANGE_ATTR     1019
#define NO_MISSILE_ARCS_ATTR      1020
#define NO_MISSILE_LOCK_ATTR      1021
#define NO_MISSILE_LOAD_ATTR      1022
#define NO_ENGINE_ATTR            1023
#define NO_STRUCTURE_ATTR         1024
#define NO_POWER_ATTR             1025
#define NO_SENSOR_ATTR            1026
#define NO_SENSOR_LIST_ATTR       1027
#define NO_SHIELD_ATTR            1028
#define NO_TECHNOLOGY_ATTR        1029
#define NO_MOVEMENT_ATTR          1030
#define NO_CLOAK_ATTR             1031
#define NO_TRANS_ATTR             1032
#define NO_TRACT_ATTR             1033
#define NO_COORDS_ATTR            1034
#define NO_COURSE_ATTR            1035
#define NO_MAIN_ATTR              1036
#define NO_AUX_ATTR               1037
#define NO_BATT_ATTR              1038
#define NO_FUEL_ATTR              1039
#define NO_STATUS_ATTR            1040
#define NO_BEAM_RECYCLE_ATTR      1041
#define NO_MISSILE_RECYCLE_ATTR   1042
#define NO_CONSOLE_ATTR           1100


#define SDB_ATTR_NAME             "SDB"
#define LOCATION_ATTR_NAME        "LOCATION"
#define SPACE_ATTR_NAME           "SPACE"
#define CLASS_ATTR_NAME           "CLASS"
#define CONSOLE_ATTR_NAME         "CONSOLES"
#define CONSOLE_USER_ATTR_NAME    "USER"

#define ALLOCATE_ATTR_NAME        "ALLOCATE"
#define ALLOCATE_DATA_NUMBER      21

#define BEAM_ATTR_NAME            "BEAM"
#define BEAM_DATA_NUMBER          5
#define BEAM_ACTIVE_ATTR_NAME     "BEAM_ACTIVE"
#define BEAM_NAME_ATTR_NAME       "BEAM_NAME"
#define BEAM_RECYCLE_ATTR_NAME    "BEAM_RECYCLE" 
#define BEAM_DAMAGE_ATTR_NAME     "BEAM_DAMAGE"
#define BEAM_BONUS_ATTR_NAME      "BEAM_BONUS"
#define BEAM_COST_ATTR_NAME       "BEAM_COST"
#define BEAM_RANGE_ATTR_NAME      "BEAM_RANGE"
#define BEAM_ARCS_ATTR_NAME       "BEAM_ARCS"
#define BEAM_LOCK_ATTR_NAME       "BEAM_LOCK"
#define BEAM_LOAD_ATTR_NAME       "BEAM_LOAD"
#define MAX_BEAM_BANKS            20 

#define MISSILE_ATTR_NAME         "MISSILE"
#define MISSILE_DATA_NUMBER       5
#define MISSILE_ACTIVE_ATTR_NAME  "MISSILE_ACTIVE"
#define MISSILE_NAME_ATTR_NAME    "MISSILE_NAME"
#define MISSILE_RECYCLE_ATTR_NAME "MISSILE_RECYCLE"
#define MISSILE_DAMAGE_ATTR_NAME  "MISSILE_DAMAGE"
#define MISSILE_WARHEAD_ATTR_NAME "MISSILE_WARHEAD"
#define MISSILE_COST_ATTR_NAME    "MISSILE_COST"
#define MISSILE_RANGE_ATTR_NAME   "MISSILE_RANGE"
#define MISSILE_ARCS_ATTR_NAME    "MISSILE_ARCS"
#define MISSILE_LOCK_ATTR_NAME    "MISSILE_LOCK"
#define MISSILE_LOAD_ATTR_NAME    "MISSILE_LOAD"
#define MAX_MISSILE_TUBES         20

#define ENGINE_ATTR_NAME          "ENGINE"
#define ENGINE_DATA_NUMBER        9

#define STRUCTURE_ATTR_NAME       "STRUCTURE"
#define STRUCTURE_DATA_NUMBER     12

#define POWER_ATTR_NAME           "POWER"
#define POWER_DATA_NUMBER         5

#define SENSOR_ATTR_NAME          "SENSOR"
#define SENSOR_DATA_NUMBER        17

#define SENSOR_LIST_ATTR_NAME     "SENSOR_LIST"
#define SENSOR_LIST_DATA_NUMBER   3
#define MAX_SENSOR_CONTACTS       30

#define SHIELD_ATTR_NAME          "SHIELD"
#define SHIELD_DATA_NUMBER        16

#define TECHNOLOGY_ATTR_NAME      "TECHNOLOGY"
#define TECHNOLOGY_DATA_NUMBER    9

#define MOVEMENT_ATTR_NAME        "MOVEMENT"
#define MOVEMENT_DATA_NUMBER      9

#define CLOAK_ATTR_NAME           "CLOAK"
#define CLOAK_DATA_NUMBER         6

#define TRANS_ATTR_NAME           "TRANS"
#define TRANS_DATA_NUMBER         7

#define TRACT_ATTR_NAME           "TRACT"
#define TRACT_DATA_NUMBER         6

#define COORDS_ATTR_NAME          "COORDS"
#define COORDS_DATA_NUMBER        9

#define COURSE_ATTR_NAME          "COURSE"
#define COURSE_DATA_NUMBER        17

#define MAIN_ATTR_NAME            "MAIN"
#define MAIN_DATA_NUMBER          5

#define AUX_ATTR_NAME             "AUX"
#define AUX_DATA_NUMBER           5

#define BATT_ATTR_NAME            "BATT"
#define BATT_DATA_NUMBER          5

#define FUEL_ATTR_NAME            "FUEL"
#define FUEL_DATA_NUMBER          3

#define STATUS_ATTR_NAME          "STATUS"
#define STATUS_DATA_NUMBER        10

#define IFF_ATTR_NAME			  "IFF"
#define IFF_DATA_NUMBER           1

/* ------------------------------------------------------------------------ */

typedef struct _spaceconfig {
	char cochrane[20]; // Yes this is weird. But @config doesn't support floating point numbers (yet).
	dbref security;
	dbref helm;
	dbref engineering;
	dbref operation;
	dbref science;
	dbref damage;
	dbref communication;
	dbref tactical;
	dbref transporter;
	dbref monitor;
	dbref fighter;
	int nebula; // 0 = ASpace default, 1 = CustomNebs + aspace (default), 2 = Custom only
} ASPACE_CONFIG;

ASPACE_CONFIG aspace_config;

struct aspace_empire_info {
	const char* name;
	double radius;
	double x;
	double y;
	double z;
};

intmap *border_map;
intmap *nebula_map;


typedef struct _space_border_info_ {
	char* border_id;
	struct aspace_border_info* data;
} spaceborder_info;

typedef struct _aspace_border_info {
	const char *name;
	int empire_id;
	double radius;
	double x;
	double y;
	double z;
} aspace_borders;

struct allocate_t {
	 int    version;
     double helm;
     double tactical;
     double operations;
	 double subsystems;
     double movement;
     double shields;
     double shield[MAX_SHIELD_NAME];
     double cloak;
     double beams;
     double missiles;
     double sensors;
     double ecm;
     double eccm;
     double transporters;
     double tractors;
     double miscellaneous;
};

struct beam_t {
     double in;
     double out;
     double freq;
     int    exist;
     int    banks;
};

struct beam_list_t {
    int    active[MAX_BEAM_BANKS];
    int    name[MAX_BEAM_BANKS];
    double damage[MAX_BEAM_BANKS];
    int    bonus[MAX_BEAM_BANKS];
    int    cost[MAX_BEAM_BANKS];
    int    range[MAX_BEAM_BANKS];
    int    arcs[MAX_BEAM_BANKS];
    int    lock[MAX_BEAM_BANKS];
    int    load[MAX_BEAM_BANKS];
	int    recycle[MAX_BEAM_BANKS];
};

struct missile_t {
     double in;
     double out;
     double freq;
     int    exist;
     int    tubes;
};

struct missile_list_t {
     int    active[MAX_MISSILE_TUBES];
     int    name[MAX_MISSILE_TUBES];
     double damage[MAX_MISSILE_TUBES];
     int    warhead[MAX_MISSILE_TUBES];
     int    cost[MAX_MISSILE_TUBES];
     int    range[MAX_MISSILE_TUBES];
     int    arcs[MAX_MISSILE_TUBES];
     int    lock[MAX_MISSILE_TUBES];
     int    load[MAX_MISSILE_TUBES];
	 int    recycle[MAX_MISSILE_TUBES];
};

struct engine_t {
     int    version;
     double warp_damage;
     double warp_max;
     int    warp_exist;
     double warp_cruise;
     double impulse_damage;
     double impulse_max;
     int    impulse_exist;
     double impulse_cruise;
};

struct structure_t {
     int    type;
     double displacement;
     double cargo_hold;
     double cargo_mass;
     double superstructure;
     int    max_structure;
     int    has_landing_pad;
     int    has_docking_bay;
     int    can_land;
     int    can_dock;
     double repair;
     int    max_repair;
};

struct power_t {
     int    version;
     double main;
     double aux;
     double batt;
     double total;
};

struct sensor_t {
     int    version;
     double lrs_damage;
     int    lrs_active;
     int    lrs_exist;
     double lrs_resolution;
     double lrs_signature;
     double srs_damage;
     int    srs_active;
     int    srs_exist;
     double srs_resolution;
     double srs_signature;
     double ew_damage;
     int    ew_active;
     int    ew_exist;
     double visibility;
     int    contacts;
     int    counter;
};

struct sensor_list_t {
     int    sdb[MAX_SENSOR_CONTACTS];
     int    num[MAX_SENSOR_CONTACTS];
     double lev[MAX_SENSOR_CONTACTS];
};

struct shield_t {
     double ratio;
     int    maximum;
     double freq;
     int    exist;
     int    active[MAX_SHIELD_NAME];
     double damage[MAX_SHIELD_NAME];
};

struct tech_t {
     double firing;
     double fuel;
     double stealth;
     double cloak;
     double sensors;
     double aux_max;
     double main_max;
     double armor;
     double ly_range;
};

struct move_t {
     int    time;
     int    dt;
     double in;
     double out;
     double ratio;
     double cochranes;
     double v;
     int    empire;
     int    quadrant;
};

struct cloak_t {
     int    version;
     int    cost;
     double freq;
     int    exist;
     int    active;
     double damage;
     double level;
};

struct trans_t {
     int    cost;
     double freq;
     int    exist;
     int    active;
     double damage;
     int    d_lock;
     int    s_lock;
};

struct tract_t {
     int    cost;
     double freq;
     int    exist;
     int    active;
     double damage;
     int    lock;
};

struct coords_t {
     double x;
     double y;
     double z;
     double xd;
     double yd;
     double zd;
     double xo;
     double yo;
     double zo;
};

struct course_t {
     int    version;
     double yaw_in;
     double yaw_out;
     double pitch_in;
     double pitch_out;
     double roll_in;
     double roll_out;
     double d[3][3];
     double rate;
};

struct main_t {
     double damage;
     int    exist;
     double gw;
     double in;
     double out;
};

struct aux_t {
     double damage;
     int    exist;
     double gw;
     double in;
     double out;
};

struct batt_t {
     double damage;
     int    exist;
     double gw;
     double in;
     double out;
};

struct fuel_t {
     double antimatter;
     double deuterium;
     double reserves;
};

struct status_t {
     int    active;
     int    docked;
     int    landed;
     int    connected;
     int    crippled;
     int    open_landing;
     int    open_docking;
     int    tractoring;
     int    tractored;
     int    time;
     int    autopilot;
     int    link;
};

struct iff_t {
	double frequency;
};

struct tmp_t {
     int    i0;
     int    i1;
     int    i2;
     int    i3;
     int    i4;
     int    i5;
     double d0;
     double d1;
     double d2;
};

struct space_database_t {
     dbref  object;
     int    location;
     int    space;
     struct allocate_t     alloc;
     struct beam_t         beam;
     struct beam_list_t    blist;
     struct missile_t      missile;
     struct missile_list_t mlist;
     struct engine_t       engine;
     struct structure_t    structure;
     struct power_t        power;
     struct sensor_t       sensor;
     struct sensor_list_t  slist;
     struct shield_t       shield;
     struct tech_t         tech;
     struct move_t         move;
     struct cloak_t        cloak;
     struct trans_t        trans;
     struct tract_t        tract;
     struct coords_t       coords;
     struct course_t       course;
     struct main_t         main;
     struct aux_t          aux;
     struct batt_t         batt;
     struct fuel_t         fuel;
     struct status_t       status;
	 struct iff_t          iff;
     struct tmp_t          tmp;
};

struct comms_database_t {
     dbref  object;
     double upper;
     double lower;
};

/* ------------------------------------------------------------------------ */

/* from space_nebula.c */
extern void free_nebulainfo(void *ptr);
extern void addNewNebula(dbref executor, int index, const char* name, double radius, double x, double y, double z, char *buff, char **bp);
extern void deleteNebula(dbref executor, int index, char *buff, char **bp);
extern void list_nebulae(char *buff, char **bp);

/* from space_output.c - Raw outputs */
extern char* output_shields_raw();

/* from space_border.c - not everything yet */
extern void free_borderinfo(void *ptr);
extern void addNewBorder(dbref executor, int border_number, const char* name, double radius, double x, double y, double z, char *buff, char **bp);
extern void deleteBorder(dbref executor, int border, char *buff, char **bp);
extern void list_borders(char *buff, char **bp);
extern void edit_border(dbref executor, int border_id, const char* setting, const char* new_value, char *buff, char **bp);
extern int get_empire_id (int ship);

/* from space_crypt.c */
extern char *space_crunch_code(char *code);
extern char *space_crypt_code(char *code, char *text, int type);

/* from space_convert.c */
extern int convert_weapons(dbref ship, dbref executor, char *weaponType, char *weaponCountAttr, char *newAttr, char *oldAttr);
extern int do_space_db_convert (dbref ship, dbref executor);

/* from space_variables.c */
extern struct space_database_t sdb[];
extern struct comms_database_t cdb[];
extern double cochrane;
extern int n;
extern int max_space_objects;
extern int max_comms_objects;

extern const char *shield_name[];
extern const char *cloak_name[];
extern const char *type_name[];
extern const char *beam_name[];
extern const char *missile_name[];
extern const char *quadrant_name[];
extern const char *system_name[];
extern const char *damage_name[];
extern double repair_mult[];

/* from space_main.c */
extern void setupAspaceConfig();
extern void initSpace();
extern void dump_space(dbref);
extern void console_message(int x, char *consoles, char *msg);
extern void console_notify(int x, char *msg, int numargs, char **args);
extern void console_notify_all(int x, char *msg);
extern return_t convert_double (char *input, double deflt, double *output);
extern return_t convert_long (char *input, long deflt, int *output);

/* from space_log.c */
extern void write_spacelog(dbref executor, dbref object, char *fmt);
extern void open_spacelog();

/* from space_ansi.c */
extern char *ansi_red (const char *msg);
extern char *ansi_alert (const char *msg);
extern char *ansi_cmd (dbref enactor, const char *msg);
extern char *ansi_warn (const char *msg);
extern char *ansi_rainbow_scale (double a, int max);
extern char *ansi_red_scale (double a, int max);
extern char *ansi_stoplight_scale (double a, int max);

/* from space_utils.c */
extern int dbref2sdb(dbref x);
extern int GoodSDB (int x);
extern double ly2pc (double dist);
extern double pc2ly (double dist);
extern double ly2su (double dist);
extern double pc2su (double dist);
extern double su2ly (double dist);
extern double su2pc (double dist);
extern double xy2bearing (double x, double y);
extern double sdb2bearing (int n1, int n2);
extern double xyz2elevation (double x, double y, double z);
extern double sdb2elevation (int n1, int n2);
extern double xyz2range (double xa, double ya, double za, double xb, double yb, double zb);
extern double sdb2range (int n1, int n2);
extern int sdb2arc (int n1, int n2);
extern int sdb2shield (int n1, int n2);
extern int arc_check (int contact, int weapon);
extern int get_empty_sdb (void);
extern int contact2sdb (int x, int c);
extern int sdb2contact (int x, int s);
extern int contact2slist (int x, int c);
extern int sdb2slist (int x, int s);
extern int name2sdb (char *name);
extern double sdb2max_antimatter (int x);
extern double sdb2max_deuterium (int x);
extern double sdb2max_reserve (int x);
extern double sdb2max_warp (int x);
extern double sdb2max_impulse (int x);
extern double sdb2cruise_warp (int x);
extern double sdb2cruise_impulse (int x);
extern double sdb2ecm_lrs (int x);
extern double sdb2ecm_srs (int x);
extern double sdb2eccm_lrs (int x);
extern double sdb2eccm_srs (int x);
extern double sdb2dissipation (int x, int shield);
extern double xyz2vis (double x, double y, double z);
extern double xyz2cochranes (double x, double y, double z);
extern double sdb2angular (int n1, int n2);

/* from space_notify.c */
extern void do_console_notify (int x, dbref parent1, dbref parent2, dbref parent3, const char *msg);
extern void do_all_console_notify (int x, const char *msg);
extern void do_ship_notify(int x, const char *msg);
extern void do_space_notify_one (int x, dbref parent1, dbref parent2, dbref parent3, const char *msg);
extern void do_space_notify_two (int n1, int n2, dbref parent1, dbref parent2, dbref parent3, const char *msg);

/* from space_error.c */
extern int error_on_contact (int n1, int n2, dbref enactor);
extern int error_on_console (dbref enactor);

/*from space_unparse.c */
extern char *unparse_percent (double a);
extern char *unparse_percent_3 (double a);
extern char *unparse_percent_6 (double a);
extern char *unparse_damage (double a);
extern char *unparse_shield (int x);
extern char *unparse_beam (int x);
extern char *unparse_missile (int x);
extern char *unparse_range (double r);
extern char *unparse_distance (double r);
extern char *unparse_speed (double s);
extern char *unparse_movement (int x);
extern char *unparse_velocity (int x);
extern char *unparse_bearing (int n1, int n2);
extern char *unparse_power (double a);
extern char *unparse_freq (double a);
extern char *unparse_cargo (double a);
extern char *unparse_course (int x);
extern char *unparse_arc (int a);
extern char *unparse_type (int x);
extern char *unparse_quadrant (int x);
extern char *unparse_empire (int x);
extern char *unparse_class (int x);
extern char *unparse_identity (int n1, int n2);

/* from move.c */
extern void moveit(dbref what, dbref where, int nomovemsgs);

/* from space_report.c */
extern void report_eng_power (void);
extern void report_helm_power (void);
extern void report_tact_power (void);
extern void report_ops_power (void);
extern void report_shield_power (void);
extern void report_sensor_power (void);

/* from space_alert.c */
extern void alert_enter_nebula (int x);
extern void alert_exit_nebula (int x);
extern void alert_main_balance (int x);
extern void alert_aux_balance (int x);
extern void alert_batt_balance (int x);
extern void alert_main_overload (int x);
extern void alert_aux_overload (int x);
extern void alert_warp_overload (int x);
extern void alert_impulse_overload (int x);
extern void alert_anti_runout (int x);
extern void alert_deut_runout (int x);
extern void alert_batt_runout (int x);
extern void alert_cloak_failure (int x);
extern void alert_cloak_voided (int x);
extern void alert_beam_balance (int x);
extern void alert_beam_charged (int x);
extern void alert_missile_balance (int x);
extern void alert_missile_charged (int x);
extern void alert_speed_warp (int x);
extern void alert_speed_impulse (int x);
extern void alert_speed_stop (int x);
extern void alert_enter_quadrant (int x);
extern void alert_enter_empire (int x);
extern void alert_exit_empire (int x);
extern void alert_yaw (int x);
extern void alert_pitch (int x);
extern void alert_roll (int x);
extern void alert_max_repair (int x);
extern void alert_border_cross (int x, int a, int way);
extern void alert_tract_unlock (int n1, int n2, dbref enactor);
extern void alert_tract_lock (int n1, int n2, dbref enactor);
extern void alert_tract_attempt (int n1, int n2, dbref enactor);
extern void alert_tract_lost (int n1, int n2);
extern void alert_ship_cloak_online (int x);
extern void alert_ship_cloak_offline (int x);
extern void alert_ship_enter_warp (int x);
extern void alert_ship_exit_warp (int x);
extern void alert_ship_hit (int x);
extern void alert_ship_hurt (int x);

/* from space_balance.c */
extern void balance_eng_power (void);
extern void balance_helm_power (void);
extern void balance_shield_power (void);
extern void balance_tact_power (void);
extern void balance_sensor_power (void);
extern void balance_ops_power (void);

/* from space_read.c */
extern int do_space_db_read (dbref ship, dbref executor);

/* from space_write.c */
extern int do_space_db_write (dbref ship, dbref executor);

/* from space_get.c */
extern void do_space_db_get (int x, char *f1, char *f2, char *f3, char *f4, char *buff, char **bp);

/* from space_put.c */
extern void do_space_db_put (int x, char *value, char *f1, char *f2, char *f3, char *f4, char *buff, char **bp);

/* from space_misc.c */
extern void dump_space (dbref executor);
extern int debug_space (int x);
extern void assess_damage (int n1, int n2, int hits, double damage, double freq, int mode);
extern int repair_everything (void);
extern void damage_structure (int x, double damage);
extern void damage_shield (int x, int shield, double damage);
extern void damage_aux (int x, double damage);
extern void damage_batt (int x, double damage);
extern void damage_beam (int x, int beam, double damage);
extern void damage_cloak (int x, double damage);
extern void damage_ew (int x, double damage);
extern void damage_impulse (int x, double damage);
extern void damage_lrs (int x, double damage);
extern void damage_main (int x, double damage);
extern void damage_missile (int x, int missile, double damage);
extern void damage_srs (int x, double damage);
extern void damage_tract (int x, double damage);
extern void damage_trans (int x, double damage);
extern void damage_warp (int x, double damage);

/* from space_iterate.c */
extern void up_alloc_balance (void);
extern void up_main_io (void);
extern void up_aux_io (void);
extern void up_batt_io (void);
extern void up_main_damage (void);
extern void up_aux_damage (void);
extern void up_fuel (void);
extern void up_reserve (void);
extern void up_total_power (void);
extern void up_warp_damage (void);
extern void up_impulse_damage (void);
extern void up_warp_max (void);
extern void up_impulse_max (void);
extern void up_tract_status (void);
extern void up_cloak_status (void);
extern void up_beam_io (void);
extern void up_missile_io (void);
extern void up_speed_io (void);
extern void up_turn_rate (void);
extern void up_cochranes (void);
extern void up_velocity (void);
extern void up_quadrant (void);
extern void up_empire (void);
extern void up_visibility (void);
extern void up_yaw_io (void);
extern void up_pitch_io (void);
extern void up_roll_io (void);
extern void up_vectors (void);
extern void up_position (void);
extern void up_resolution (void);
extern void up_signature (int x);
extern void up_sensor_message (int contacts);
extern void up_sensor_list (void);
extern void up_repair (void);
extern int do_space_db_iterate (void);

/* from space_set.c */
extern int do_set_iff_frequency (double frequency, dbref enactor);
extern void do_fed_shield_bug_check(int x);
extern int do_lock_weapon(int first, int last, int contact, int weapon, dbref enactor);
extern int do_unlock_weapon(int first, int last, int weapon, dbref enactor);
extern int do_set_parallel(int contact, dbref enactor);
extern int do_set_autopilot(int flag, dbref enactor);
extern int do_set_main_reactor (double level, dbref enactor);
extern int do_set_aux_reactor (double level, dbref enactor);
extern int do_set_battery (double level, dbref enactor);
extern int do_set_lrs (int active, dbref enactor);
extern int do_set_srs (int active, dbref enactor);
extern int do_set_ew (int active, dbref enactor);
extern int do_set_cloak (int active, dbref enactor);
extern int do_set_course (double yaw, double pitch, double roll, dbref enactor);
extern int do_set_intercept (int contact, dbref enactor);
extern int do_set_evade (int contact, dbref enactor);
extern int do_set_speed (double speed, dbref enactor);
extern int do_set_shield (int shield, int active, dbref enactor);
extern int do_set_weapon (int first, int last, int active, int weapon, dbref enactor);
extern int do_set_beams_locked (int first, int last, int contact, dbref enactor);
extern int do_set_beams_unlocked (int first, int last, dbref enactor);
extern int do_set_missiles_locked (int first, int last, int contact, dbref enactor);
extern int do_set_missiles_unlocked (int first, int last, dbref enactor);
extern int do_set_trans (int active, dbref enactor);
extern int do_set_trans_locked (int contact, int target, dbref enactor);
extern int do_set_trans_unlocked (dbref enactor);
extern int do_set_tract (int active, dbref enactor);
extern int do_set_tract_locked (int contact, dbref enactor);
extern int do_set_tract_unlocked (dbref enactor);
extern int do_set_ftr_alloc (double helm, double tactical, double operations, dbref enactor);
extern int do_set_eng_alloc (double helm, double tactical, double operations, dbref enactor);
extern int do_set_helm_alloc (double movement, double shields, double cloak, dbref enactor);
extern int do_set_shield_alloc (double forward, double starboard, double aft, double port, double dorsal, double ventral, dbref enactor);
extern int do_set_tactical_alloc (double beams, double missiles, double sensors, dbref enactor);
extern int do_set_sensor_alloc (double ecm, double eccm, dbref enactor);
extern int do_set_operations_alloc (double transporters, double tractors, double miscellaneous, dbref enactor);
extern int do_set_active (dbref enactor);
extern int do_set_inactive (dbref enactor);
extern int do_set_docking_doors (int active, dbref enactor);
extern int do_set_landing_doors (int active, dbref enactor);
extern int do_set_dock (int contact, dbref enactor);
extern int do_set_undock (dbref enactor);
extern int do_set_land (int contact, dbref enactor);
extern int do_set_launch (dbref enactor);
extern int do_set_enter_wormhole (int contact, dbref enactor);
extern int do_set_coords_manual (double x, double y, double z, dbref enactor);
extern int do_set_coords_reset (dbref enactor);
extern int do_set_coords_layin (double x, double y, double z, dbref enactor);
extern int do_set_coords_engage (dbref enactor);
extern int do_set_eta (double speed, dbref enactor);
extern int do_set_fix_damage (char *sys1, char *sys2, int type, char *name, dbref enactor);
extern int do_set_shield_freq (double frequency, dbref enactor);
extern int do_set_cloak_freq (double frequency, dbref enactor);
extern int do_set_beam_freq (double frequency, dbref enactor);
extern int do_set_missile_freq (double frequency, dbref enactor);
extern int do_set_trans_freq (double frequency, dbref enactor);
extern int do_set_tract_freq (double frequency, dbref enactor);
extern int do_set_refuel (char *name, char *fuel, double tons, dbref enactor);
extern int do_set_defuel (char *fuel, double tons, dbref enactor);
extern int do_set_yaw (double yaw, dbref enactor);
extern int do_set_pitch (double pitch, dbref enactor);
extern int do_set_roll (double roll, dbref enactor);
extern int do_set_fire (int first, int last, int weapon, int mode, dbref enactor);
extern int do_set_intercept (int contact, dbref enactor);


/* from space_status.c */
extern int do_iff_check (int contact);
extern int do_sensor_contacts (char *a, dbref enactor);
extern int do_sensor_report (int contact, dbref enactor);
extern int do_eng_status (dbref enactor);
extern int do_helm_status (dbref enactor);
extern int do_beam_status (dbref enactor);
extern int do_missile_status (dbref enactor);
extern int do_tactical_status (dbref enactor);
extern int do_operations_status (dbref enactor);
extern int do_allocation_status (dbref enactor);
extern int do_damage_status (int type, char *name, dbref enactor);
extern int do_science_status (dbref enactor);
extern int do_fighter_status (dbref enactor);
extern int do_scanner_report (int a, char *s, dbref enactor);
extern int do_nebula_report (dbref enactor);
extern int do_border_report (dbref enactor);
extern int do_planet_report (const char *s, int t, dbref enactor);
extern void do_sensor_contacts_bot (char *a, dbref enactor, char *buff, char **bp);
extern char *contact_flags (int x);
extern char *contact_line (int contact);
extern void contact_line_bot (int contact, char *buff, char **bp);
extern char *planet_eta (int x, double r);

/* from space_format.c */
extern char *format_Iff_Freq (int x);
extern char *format_Location (int x);
extern char *format_Name (int x);
extern char *format_Type (int x);
extern char *format_Class (int x);
extern char *format_Range (int n1, int n2);
extern char *format_Firing_Arc (int n1, int n2);
extern char *format_Contact_Arc (int n1, int n2);
extern char *format_Facing_Shield (int n1, int n2);
extern char *format_Contact_Shield (int n1, int n2);
extern char *format_Bearing (int n1, int n2);
extern char *format_Course (int x);
extern char *format_Speed (int x);
extern char *format_Displacement (int x);
extern char *format_Cargo_Carried (int x);
extern char *format_Cargo_Cap (int x);
extern char *format_Docking_Doors (int x);
extern char *format_Landing_Doors (int x);
extern char *format_Resolution (double a);
extern char *format_Shield_Freq (int x);
extern char *format_Cloak_Freq (int x);
extern char *format_Beam_Freq (int x);
extern char *format_Missile_Freq (int x);
extern char *format_Trans_Freq (int x);
extern char *format_Tract_Freq (int x);
extern char *format_Quadrant (int x);
extern char *format_Territory (int x);
extern char *format_Velocity (int x);
extern char *format_Helm_Power (int x);
extern char *format_Operations_Power (int x);
extern char *format_Tactical_Power (int x);
extern char *format_Total_Power (int x);
extern char *format_Cochranes (int x);
extern char *format_Visibility (int x);
extern char *format_LRS_Res (int x);
extern char *format_SRS_Res (int x);
extern char *format_LRS_Sig (int x);
extern char *format_SRS_Sig (int x);
extern char *format_Warp_Cruise (int x);
extern char *format_Impulse_Cruise (int x);
extern char *format_Warp_Maximum (int x);
extern char *format_Impulse_Maximum (int x);
extern char *format_l_line (void);
extern char *format_l_end (void);
extern char *format_l_Antimatter (int x);
extern char *format_l_Deuterium (int x);
extern char *format_l_Reserves (int x);
extern char *format_l_Galactic (int x);
extern char *format_l_Relative (int x);
extern char *format_l_Destination (int x);
extern char *format_l_Beam_Cap (int x);
extern char *format_l_Missile_Cap (int x);
extern char *format_l_Docked (int x);
extern char *format_l_Landed (int x);
extern char *format_s_header (void);
extern char *format_s_MA_Reactor (int x);
extern char *format_s_Fusion_Reactor (int x);
extern char *format_s_Batteries (int x);
extern char *format_s_Warp_Drive (int x);
extern char *format_s_Impulse_Drive (int x);
extern char *format_s_Structure (int x);
extern char *format_s_Transporters (int x);
extern char *format_s_Tractors (int x);
extern char *format_s_Cloak (int x);
extern char *format_s_LRS (int x);
extern char *format_s_SRS (int x);
extern char *format_s_EW (int x);
extern char *format_s_Shield (int x);
extern char *format_s_Beam (int x, int a);
extern char *format_s_Missile (int x, int a);

/* ------------------------------------------------------------------------ */

#endif    /* _SPACE_H_ */
