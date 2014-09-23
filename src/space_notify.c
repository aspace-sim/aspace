/* space_notify.c */

#include "config.h"
#include "space.h"

/* ------------------------------------------------------------------------ */

void do_console_notify (int x, dbref parent1, dbref parent2, dbref parent3, const char *msg)
{
	ATTR *a, *b;
	dbref console, user, parent;
	char *q, *pq;

	if (!GoodObject(sdb[x].object) || !SpaceObj(sdb[x].object)) {
		write_spacelog(sdb[x].object, sdb[x].object, "BUG: invalid zone to zemit to.");
		return;
	}

	a = atr_get(sdb[x].object, CONSOLE_ATTR_NAME);
	if (a != NULL) {
		q = safe_atr_value(a);
		pq = trim_space_sep(q, ' ');
		while (pq) {
			console = parse_dbref(split_token(&pq, ' '));
			if (GoodObject(console)) {
				parent = Parent(console);
				if ((parent == parent1) ||
				  (parent == parent2) ||
				  (parent == parent3) ||
				  (parent == aspace_config.monitor) ||
				  (parent == aspace_config.fighter)) {
					b = atr_get(console, CONSOLE_USER_ATTR_NAME);
					if (b != NULL) {
						user = parse_dbref(atr_value(b));
						if (GoodObject(user)) {
							notify(user, msg);
						}
					}
				}
			}
		}
		free(q);
	}
	return;
}

/* ------------------------------------------------------------------------ */

void do_all_console_notify (int x, const char *msg)
{
	ATTR *a, *b;
	dbref console, user;
	char *q, *pq;

	if (!GoodObject(sdb[x].object) || !SpaceObj(sdb[x].object)) {
		write_spacelog(sdb[x].object, sdb[x].object, "BUG: invalid zone to zemit to.");
		return;
	}

	a = atr_get(sdb[x].object, CONSOLE_ATTR_NAME);
	if (a != NULL) {
		q = safe_atr_value(a);
		pq = trim_space_sep(q, ' ');
		if (*pq) {
			while (pq) {
				console = parse_dbref(split_token(&pq, ' '));
				if (GoodObject(console)) {
					b = atr_get(console, CONSOLE_USER_ATTR_NAME);
					if (b != NULL) {
						user = parse_dbref(atr_value(b));
						if (GoodObject(user)) {
							notify(user, msg);
						}
					}
				}
			}
		}
		free(q);
	}
	return;
}

/* ------------------------------------------------------------------------ */

void do_ship_notify(int x, const char *msg)
{
	ATTR *a;
	dbref room;
	char *q, *pq;

	if (!GoodObject(sdb[x].object) || !SpaceObj(sdb[x].object)) {
		write_spacelog(sdb[x].object, sdb[x].object, "BUG: invalid zone to zemit to.");
		return;
	}

	a = atr_get(sdb[x].object, "ROOMS");
	if (a != NULL) {
		q = safe_atr_value(a);
		pq = trim_space_sep(q, ' ');
		if (*pq) {
			while (pq) {
				room = parse_dbref(split_token(&pq, ' '));
				if (Zone(room) == sdb[x].object) {
					if (Typeof(room) == TYPE_ROOM) {
						notify_except2(sdb[x].object, room, NOTHING, NOTHING, msg,0);
					}
				}
			}
		}
		free(q);
	}
	return;
}

/* ------------------------------------------------------------------------ */

void do_space_notify_one (int x, dbref parent1, dbref parent2, dbref parent3, const char *msg)
{
	ATTR *a, *b;
	static char buffer[500];
	register int i;
	int contact;
	dbref console, user, parent;
	char *q, *pq;

	for (i = MIN_SPACE_OBJECTS ; i <= max_space_objects ; ++i) {
		if (sdb[i].status.active) {
			if (sdb[i].structure.type) {
				if (sdb[i].location == sdb[x].location) {
					if (sdb[i].space == sdb[x].space) {
						if (i != x) {
							contact = sdb2contact(i, x);
							if (contact != SENSOR_FAIL) {
								a = atr_get(sdb[i].object, CONSOLE_ATTR_NAME);
								if (a != NULL) {
									q = safe_atr_value(a);
									pq = trim_space_sep(q, ' ');
									if (*pq) {
										while (pq) {
											console = parse_dbref(split_token(&pq, ' '));
											if (GoodObject(console)) {
												parent = Parent(console);
												if ((parent == parent1) ||
												  (parent == parent2) ||
												  (parent == parent3) ||
												  (parent == aspace_config.monitor) ||
												  (parent == aspace_config.fighter)) {
													b = atr_get(console, CONSOLE_USER_ATTR_NAME);
													if (b != NULL) {
														user = parse_dbref(atr_value(b));
														if (GoodObject(user)) {
															snprintf(buffer, sizeof(buffer),
															  "%s[%s%s %s%s]%s",
															  ANSI_BLUE, ANSI_CYAN, unparse_identity(i, x),
															  msg, ANSI_BLUE, ANSI_WHITE);
															notify(user, buffer);
														}
													}
												}
											}
										}
									}
									free(q);
								}
							}
						}
					}
				}
			}
		}
	}
	return;
}

/* ------------------------------------------------------------------------ */

void do_space_notify_two (int n1, int n2, dbref parent1, dbref parent2, dbref parent3, const char *msg)
{
	ATTR *a, *b;
	static char buffer[500];
	register int i;
	int contact1, contact2;
	dbref console, user, parent;
	char *q, *pq;

	for (i = MIN_SPACE_OBJECTS ; i <= max_space_objects ; ++i) {
		if (sdb[i].status.active) {
			if (sdb[i].structure.type) {
				if (sdb[i].location == sdb[n1].location) {
					if (sdb[i].space == sdb[n1].space) {
						if (i != n1 && i != n2) {
							contact1 = sdb2contact(i, n1);
							contact2 = sdb2contact(i, n2);
							if (contact1 != SENSOR_FAIL || contact2 != SENSOR_FAIL) {
								a = atr_get(sdb[i].object, CONSOLE_ATTR_NAME);
								if (a != NULL) {
									q = safe_atr_value(a);
									pq = trim_space_sep(q, ' ');
									if (*pq) {
										while (pq) {
											console = parse_dbref(split_token(&pq, ' '));
											if (GoodObject(console)) {
												parent = Parent(console);
												if ((parent == parent1) ||
												  (parent == parent2) ||
												  (parent == parent3) ||
												  (parent == aspace_config.monitor) ||
												  (parent == aspace_config.fighter)) {
													b = atr_get(console, CONSOLE_USER_ATTR_NAME);
													if (b != NULL) {
														user = parse_dbref(atr_value(b));
														if (GoodObject(user)) {
															snprintf(buffer, sizeof(buffer), "%s[%s%s %s ",
															  ANSI_BLUE, ANSI_CYAN, unparse_identity(i, n1), msg);
															strncat(buffer, tprintf("%s%s]%s",
															  unparse_identity(i, n2), ANSI_BLUE, ANSI_WHITE), sizeof(buffer) - 1);
															notify(user, buffer);
														}
													}
												}
											}
										}
									}
									free(q);
								}
							}
						}
					}
				}
			}
		}
	}
	return;
}

/* ------------------------------------------------------------------------ */
