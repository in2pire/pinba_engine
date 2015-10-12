/* Copyright (c) 2007-2013 Antony Dovgal <tony@daylessday.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "pinba_limits.h"

#ifndef PINBA_TYPES_H
#define PINBA_TYPES_H

enum {
	PINBA_TABLE_UNKNOWN,
	PINBA_TABLE_STATUS, /* internal status table */
	PINBA_TABLE_REQUEST,
	PINBA_TABLE_TIMER,
	PINBA_TABLE_TIMERTAG,
	PINBA_TABLE_TAG,
	PINBA_TABLE_HISTOGRAM_VIEW,
	PINBA_TABLE_REPORT_INFO,
	PINBA_TABLE_REPORT1, /* group by script_name */
	PINBA_TABLE_REPORT2, /* group by virtual host */
	PINBA_TABLE_REPORT3, /* group by hostname */
	PINBA_TABLE_REPORT4, /* group by virtual host, script_name */
	PINBA_TABLE_REPORT5, /* group by hostname, script_name */
	PINBA_TABLE_REPORT6, /* group by hostname, virtual_host */
	PINBA_TABLE_REPORT7, /* group by hostname, virtual_host and script_name */
	PINBA_TABLE_REPORT8, /* group by status */
	PINBA_TABLE_REPORT9, /* group by script_name and status */
	PINBA_TABLE_REPORT10, /* group by virtual_host and status */
	PINBA_TABLE_REPORT11, /* group by hostname and status */
	PINBA_TABLE_REPORT12, /* group by hostname, script_name and status */
	PINBA_TABLE_REPORT13, /* group by schema */
	PINBA_TABLE_REPORT14, /* group by schema and script_name */
	PINBA_TABLE_REPORT15, /* group by schema and server_name */
	PINBA_TABLE_REPORT16, /* group by schema and hostname */
	PINBA_TABLE_REPORT17, /* group by schema, hostname and script_name */
	PINBA_TABLE_REPORT18, /* group by schema, hostname and status */
	PINBA_TABLE_TAG_INFO, /* tag report grouped by custom tag */
	PINBA_TABLE_TAG2_INFO, /* tag report grouped by 2 custom tags */
	PINBA_TABLE_TAGN_INFO, /* tag report grouped by N custom tags */
	PINBA_TABLE_TAG_REPORT, /* tag report grouped by script_name and custom tag */
	PINBA_TABLE_TAG2_REPORT, /* tag report grouped by script_name and 2 custom tags */
	PINBA_TABLE_TAGN_REPORT, /* tag report grouped by script_name and N custom tags */
	PINBA_TABLE_TAG_REPORT2, /* tag report grouped by script_name, host_name, server_name and custom tag */
	PINBA_TABLE_TAG2_REPORT2, /* tag report grouped by script_name, host_name, server_name and 2 custom tags */
	PINBA_TABLE_TAGN_REPORT2 /* tag report grouped by script_name, host_name, server_name and N custom tags */
};

#define PINBA_TABLE_REPORT_LAST PINBA_TABLE_REPORT18

enum {
	PINBA_REPORT_REGULAR = 1<<0,
	PINBA_REPORT_CONDITIONAL = 1<<1,
	PINBA_REPORT_TAGGED = 1<<2
};

typedef struct _pinba_socket { /* {{{ */
	int listen_sock;
	struct event *accept_event;
} pinba_socket;
/* }}} */

#if 0
typedef struct _struct timeval { /* {{{ */
	int tv_sec;
	int tv_usec;
} struct timeval;
/* }}} */
#endif

typedef struct _pinba_word { /* {{{ */
	char *str;
	unsigned char len;
	uint64_t hash;
} pinba_word;
/* }}} */

typedef struct _pinba_timer_record { /* {{{ */
	struct timeval value;
	int *tag_ids;
	pinba_word **tag_values;
	unsigned short tag_num;
	unsigned short tag_num_allocated;
	int hit_count;
	int index;
	size_t request_id;
	unsigned short num_in_request;
	struct timeval ru_utime;
	struct timeval ru_stime;
} pinba_timer_record;
/* }}} */

typedef struct _pinba_stats_record { /* {{{ */
	struct {
		char script_name[PINBA_SCRIPT_NAME_SIZE];
		char server_name[PINBA_SERVER_NAME_SIZE];
		char hostname[PINBA_HOSTNAME_SIZE];
		struct timeval req_time;
		struct timeval ru_utime;
		struct timeval ru_stime;
		unsigned char script_name_len;
		unsigned char server_name_len;
		unsigned char hostname_len;
		unsigned int req_count;
		float doc_size;
		float mem_peak_usage;
		unsigned short status;
		float memory_footprint;
		char schema[PINBA_SCHEMA_SIZE];
		unsigned char schema_len;
		char **tag_names; //PINBA_TAG_NAME_SIZE applies here
		char **tag_values; //PINBA_TAG_VALUE_SIZE applies here
		unsigned int tags_cnt;
		unsigned int tags_alloc_cnt;
	} data;
	struct timeval time;
	size_t timers_start;
	unsigned short timers_cnt;
} pinba_stats_record;
/* }}} */

typedef struct _pinba_stats_record_ex { /* {{{ */
	pinba_stats_record record;
	Pinba__Request *request;
	size_t request_id;
	char can_free;
} pinba_stats_record_ex;
/* }}} */

typedef void (*pool_dtor_func_t)(void *pool);

typedef struct _pinba_pool { /* {{{ */
	size_t size;
	size_t element_size;
	pool_dtor_func_t dtor;
	size_t in;
	size_t out;
	void **data;
} pinba_pool;
/* }}} */

typedef struct _pinba_tag { /* {{{ */
	size_t id;
	char name[PINBA_TAG_NAME_SIZE];
	unsigned char name_len;
	uint64_t hash;
} pinba_tag;
/* }}} */

typedef struct _pinba_conditions {
	double min_time;
	double max_time;
	unsigned int tags_cnt;
	char **tag_names;
	char **tag_values;
} pinba_conditions;

typedef struct _pinba_std_report {
	pinba_conditions cond;
	int flags;
	int type;
	int histogram_max_time;
	float histogram_segment;
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	char tag_report;
	uint8_t *index;
	pthread_rwlock_t lock;
	size_t results_cnt;
	time_t time_interval;
	unsigned use_cnt;
	struct timeval start;
} pinba_std_report;

typedef struct _pinba_report pinba_report;
typedef void (pinba_report_update_function)(size_t request_id, pinba_report *report, const pinba_stats_record *record);

struct _pinba_report { /* {{{ */
	pinba_std_report std;
	Pvoid_t results;
	struct timeval time_total;
	double kbytes_total;
	double memory_footprint;
	struct timeval ru_utime_total;
	struct timeval ru_stime_total;
	pinba_report_update_function *add_func;
	pinba_report_update_function *delete_func;
};
/* }}} */

typedef struct _pinba_tag_report pinba_tag_report;
typedef void (pinba_tag_report_update_function)(size_t request_id, pinba_tag_report *report, const pinba_stats_record *record);

struct _pinba_tag_report { /* {{{ */
	pinba_std_report std;
	int *tag_id;
	int tag_cnt;
	uint8_t *index;
	Pvoid_t results;
	pinba_tag_report_update_function *add_func;
	pinba_tag_report_update_function *delete_func;
	pinba_word **words;
};
/* }}} */

typedef struct _pinba_daemon_settings { /* {{{ */
	int port;
	int stats_history;
	int stats_gathering_period;
	size_t request_pool_size;
	size_t data_pool_size;
	size_t timer_pool_size;
	size_t temp_pool_size;
	size_t temp_pool_size_limit;
	int show_protobuf_errors;
	char *address;
	int cpu_start;
} pinba_daemon_settings;
/* }}} */

typedef struct _pinba_data_bucket { /* {{{ */
	char *buf;
	int len;
	int alloc_len;
} pinba_data_bucket;
/* }}} */

typedef struct _pinba_int_stats {
	size_t lost_tmp_records;
	size_t invalid_packets;
	size_t invalid_request_data;
} pinba_int_stats_t;

typedef struct _pinba_daemon { /* {{{ */
	pthread_rwlock_t collector_lock;
	pthread_rwlock_t data_lock;
	pthread_rwlock_t tag_reports_lock;
	pthread_rwlock_t base_reports_lock;
	pthread_rwlock_t timer_lock;
	pthread_rwlock_t words_lock;
	pinba_socket *collector_socket;
	struct event_base *base;
	pinba_pool data_pool[2];
	int data_pool_num;
	pinba_pool request_pool;
	pinba_pool timer_pool;
	pthread_mutex_t temp_mutex;
	pinba_pool *per_thread_request_pools;
	Pvoid_t dictionary;
	size_t timertags_cnt;
	struct {
		Pvoid_t table; /* ID -> NAME */
		Pvoid_t name_index; /* NAME -> */
	} tag;
	pinba_daemon_settings settings;
	Pvoid_t base_reports;
	void **base_reports_arr;
	unsigned int base_reports_arr_size;
	Pvoid_t tag_reports;
	void **tag_reports_arr;
	unsigned int tag_reports_arr_size;
	thread_pool_t *thread_pool;
	pinba_int_stats_t stats;
	pthread_rwlock_t stats_lock;
	Pvoid_t tables_to_reports;
	int in_shutdown;
} pinba_daemon;
/* }}} */

struct reports_job_data {
	unsigned int prefix;
	unsigned int count;
	void *report;
	int add;
};

struct pinba_report_data_header {
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
};

struct pinba_tag_report_data_header {
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	size_t hit_count;
};

struct pinba_report1_data { /* {{{ */
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	struct timeval req_time_total;
	struct timeval ru_utime_total;
	struct timeval ru_stime_total;
	double kbytes_total;
	double memory_footprint;
};
/* }}} */

struct pinba_report2_data { /* {{{ */
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	struct timeval req_time_total;
	struct timeval ru_utime_total;
	struct timeval ru_stime_total;
	double kbytes_total;
	double memory_footprint;
};
/* }}} */

struct pinba_report3_data { /* {{{ */
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	struct timeval req_time_total;
	struct timeval ru_utime_total;
	struct timeval ru_stime_total;
	double kbytes_total;
	double memory_footprint;
};
/* }}} */

struct pinba_report4_data { /* {{{ */
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	struct timeval req_time_total;
	struct timeval ru_utime_total;
	struct timeval ru_stime_total;
	double kbytes_total;
	double memory_footprint;
	char server_name[PINBA_SERVER_NAME_SIZE];
	char script_name[PINBA_SCRIPT_NAME_SIZE];
};
/* }}} */

struct pinba_report5_data { /* {{{ */
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	struct timeval req_time_total;
	struct timeval ru_utime_total;
	struct timeval ru_stime_total;
	double kbytes_total;
	double memory_footprint;
	char hostname[PINBA_HOSTNAME_SIZE];
	char script_name[PINBA_SCRIPT_NAME_SIZE];
};
/* }}} */

struct pinba_report6_data { /* {{{ */
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	struct timeval req_time_total;
	struct timeval ru_utime_total;
	struct timeval ru_stime_total;
	double kbytes_total;
	double memory_footprint;
	char hostname[PINBA_HOSTNAME_SIZE];
	char server_name[PINBA_SERVER_NAME_SIZE];
};
/* }}} */

struct pinba_report7_data { /* {{{ */
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	struct timeval req_time_total;
	struct timeval ru_utime_total;
	struct timeval ru_stime_total;
	double kbytes_total;
	double memory_footprint;
	char hostname[PINBA_HOSTNAME_SIZE];
	char server_name[PINBA_SERVER_NAME_SIZE];
	char script_name[PINBA_SCRIPT_NAME_SIZE];
};
/* }}} */

struct pinba_report8_data { /* {{{ */
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	struct timeval req_time_total;
	struct timeval ru_utime_total;
	struct timeval ru_stime_total;
	double kbytes_total;
	double memory_footprint;
	int status;
};
/* }}} */

struct pinba_report9_data { /* {{{ */
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	struct timeval req_time_total;
	struct timeval ru_utime_total;
	struct timeval ru_stime_total;
	double kbytes_total;
	double memory_footprint;
	int status;
	char script_name[PINBA_SCRIPT_NAME_SIZE];
};
/* }}} */

struct pinba_report10_data { /* {{{ */
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	struct timeval req_time_total;
	struct timeval ru_utime_total;
	struct timeval ru_stime_total;
	double kbytes_total;
	double memory_footprint;
	int status;
	char server_name[PINBA_SERVER_NAME_SIZE];
};
/* }}} */

struct pinba_report11_data { /* {{{ */
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	struct timeval req_time_total;
	struct timeval ru_utime_total;
	struct timeval ru_stime_total;
	double kbytes_total;
	double memory_footprint;
	int status;
	char hostname[PINBA_HOSTNAME_SIZE];
};
/* }}} */

struct pinba_report12_data { /* {{{ */
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	struct timeval req_time_total;
	struct timeval ru_utime_total;
	struct timeval ru_stime_total;
	double kbytes_total;
	double memory_footprint;
	int status;
	char hostname[PINBA_HOSTNAME_SIZE];
	char script_name[PINBA_SCRIPT_NAME_SIZE];
};
/* }}} */

struct pinba_report13_data { /* {{{ */
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	struct timeval req_time_total;
	struct timeval ru_utime_total;
	struct timeval ru_stime_total;
	double kbytes_total;
	double memory_footprint;
};
/* }}} */

struct pinba_report14_data { /* {{{ */
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	struct timeval req_time_total;
	struct timeval ru_utime_total;
	struct timeval ru_stime_total;
	double kbytes_total;
	double memory_footprint;
	char schema[PINBA_SCHEMA_SIZE];
	char script_name[PINBA_SCRIPT_NAME_SIZE];
};
/* }}} */

struct pinba_report15_data { /* {{{ */
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	struct timeval req_time_total;
	struct timeval ru_utime_total;
	struct timeval ru_stime_total;
	double kbytes_total;
	double memory_footprint;
	char schema[PINBA_SCHEMA_SIZE];
	char server_name[PINBA_SERVER_NAME_SIZE];
};
/* }}} */

struct pinba_report16_data { /* {{{ */
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	struct timeval req_time_total;
	struct timeval ru_utime_total;
	struct timeval ru_stime_total;
	double kbytes_total;
	double memory_footprint;
	char schema[PINBA_SCHEMA_SIZE];
	char hostname[PINBA_HOSTNAME_SIZE];
};
/* }}} */

struct pinba_report17_data { /* {{{ */
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	struct timeval req_time_total;
	struct timeval ru_utime_total;
	struct timeval ru_stime_total;
	double kbytes_total;
	double memory_footprint;
	char schema[PINBA_SCHEMA_SIZE];
	char hostname[PINBA_HOSTNAME_SIZE];
	char script_name[PINBA_SCRIPT_NAME_SIZE];
};
/* }}} */

struct pinba_report18_data { /* {{{ */
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	struct timeval req_time_total;
	struct timeval ru_utime_total;
	struct timeval ru_stime_total;
	double kbytes_total;
	double memory_footprint;
	int status;
	char schema[PINBA_SCHEMA_SIZE];
	char hostname[PINBA_HOSTNAME_SIZE];
};
/* }}} */

struct pinba_tag_info_data { /* {{{ */
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	size_t hit_count;
	struct timeval timer_value;
	struct timeval ru_utime_value;
	struct timeval ru_stime_value;
	size_t prev_add_request_id;
	size_t prev_del_request_id;
};
/* }}} */

struct pinba_tag2_info_data { /* {{{ */
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	size_t hit_count;
	struct timeval timer_value;
	struct timeval ru_utime_value;
	struct timeval ru_stime_value;
	char tag1_value[PINBA_TAG_VALUE_SIZE];
	char tag2_value[PINBA_TAG_VALUE_SIZE];
	size_t prev_add_request_id;
	size_t prev_del_request_id;
};
/* }}} */

struct pinba_tag_report_data { /* {{{ */
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	size_t hit_count;
	struct timeval timer_value;
	struct timeval ru_utime_value;
	struct timeval ru_stime_value;
	char script_name[PINBA_SCRIPT_NAME_SIZE];
	char tag_value[PINBA_TAG_VALUE_SIZE];
	size_t prev_add_request_id;
	size_t prev_del_request_id;
};
/* }}} */

struct pinba_tag2_report_data { /* {{{ */
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	size_t hit_count;
	struct timeval timer_value;
	struct timeval ru_utime_value;
	struct timeval ru_stime_value;
	char script_name[PINBA_SCRIPT_NAME_SIZE];
	char tag1_value[PINBA_TAG_VALUE_SIZE];
	char tag2_value[PINBA_TAG_VALUE_SIZE];
	size_t prev_add_request_id;
	size_t prev_del_request_id;
};
/* }}} */

struct pinba_tag_report2_data { /* {{{ */
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	size_t hit_count;
	struct timeval timer_value;
	struct timeval ru_utime_value;
	struct timeval ru_stime_value;
	char hostname[PINBA_HOSTNAME_SIZE];
	char server_name[PINBA_SERVER_NAME_SIZE];
	char script_name[PINBA_SCRIPT_NAME_SIZE];
	char tag_value[PINBA_TAG_VALUE_SIZE];
	size_t prev_add_request_id;
	size_t prev_del_request_id;
};
/* }}} */

struct pinba_tag2_report2_data { /* {{{ */
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	size_t hit_count;
	struct timeval timer_value;
	struct timeval ru_utime_value;
	struct timeval ru_stime_value;
	char hostname[PINBA_HOSTNAME_SIZE];
	char server_name[PINBA_SERVER_NAME_SIZE];
	char script_name[PINBA_SCRIPT_NAME_SIZE];
	char tag1_value[PINBA_TAG_VALUE_SIZE];
	char tag2_value[PINBA_TAG_VALUE_SIZE];
	size_t prev_add_request_id;
	size_t prev_del_request_id;
};
/* }}} */

struct pinba_tagN_info_data { /* {{{ */
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	size_t hit_count;
	struct timeval timer_value;
	struct timeval ru_utime_value;
	struct timeval ru_stime_value;
	char *tag_value;
	unsigned int tag_num;
	size_t prev_add_request_id;
	size_t prev_del_request_id;
};
/* }}} */

struct pinba_tagN_report_data { /* {{{ */
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	size_t hit_count;
	struct timeval timer_value;
	struct timeval ru_utime_value;
	struct timeval ru_stime_value;
	char script_name[PINBA_SCRIPT_NAME_SIZE];
	char *tag_value;
	int tag_num;
	size_t prev_add_request_id;
	size_t prev_del_request_id;
};
/* }}} */

struct pinba_tagN_report2_data { /* {{{ */
	int histogram_data[PINBA_HISTOGRAM_SIZE];
	size_t req_count;
	size_t hit_count;
	struct timeval timer_value;
	struct timeval ru_utime_value;
	struct timeval ru_stime_value;
	char script_name[PINBA_SCRIPT_NAME_SIZE];
	char hostname[PINBA_HOSTNAME_SIZE];
	char server_name[PINBA_SERVER_NAME_SIZE];
	char *tag_value;
	int tag_num;
	size_t prev_add_request_id;
	size_t prev_del_request_id;
};
/* }}} */

#endif /* PINBA_TYPES_H */

/*
 * vim600: sw=4 ts=4 fdm=marker
 */
