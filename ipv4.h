/* ip.h
 */
#ifndef _SENDIP_IP_H
#define _SENDIP_IP_H

/* IP HEADER
 * Taken from glibc 2.2, and modified
 */
typedef struct {
#if __BYTE_ORDER == __LITTLE_ENDIAN
	unsigned int header_len:4;
	unsigned int version:4;
#elif __BYTE_ORDER == __BIG_ENDIAN
	unsigned int version:4;
	unsigned int header_len:4;
#else
#  error "Please fix <bits/endian.h>"
#endif
	u_int8_t tos;
	u_int16_t tot_len;
	u_int16_t id;
// FreeBSD automatically hotnses this, for some reason.
#if defined(__FreeBSD__) || defined(__FreeBSD)
	u_int16_t res:1;
	u_int16_t df:1;
	u_int16_t mf:1;
	u_int16_t frag_off:13;
#define IP_SET_FRAGOFF(ip,v) (ip)->frag_off=(v)        
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	u_int16_t frag_off1:5;
	u_int16_t mf:1;
	u_int16_t df:1;
	u_int16_t res:1;
	u_int16_t frag_off2:8;
#define IP_SET_FRAGOFF(ip,v) {(ip)->frag_off1=((v)&0x1F00)>>8;(ip)->frag_off2=(v)&0x00FF;}
#elif __BYTE_ORDER == __BIG_ENDIAN
	u_int16_t res:1;
	u_int16_t df:1;
	u_int16_t mf:1;
	u_int16_t frag_off:13;
#define IP_SET_FRAGOFF(ip,v) (ip)->frag_off=(v)
#else
#  error "Please fix <bits/endian.h>"
#endif
	u_int8_t ttl;
	u_int8_t protocol;
	u_int16_t check;
	u_int32_t saddr;
	u_int32_t daddr;
} ip_header;

/* Defines for which parts have been modified
 */
#define IP_MOD_HEADERLEN  (1)
#define IP_MOD_VERSION    (1<<1)
#define IP_MOD_TOS        (1<<2)
#define IP_MOD_TOTLEN     (1<<3)
#define IP_MOD_ID         (1<<4)
#define IP_MOD_RES        (1<<5)
#define IP_MOD_DF         (1<<6)
#define IP_MOD_MF         (1<<7)
#define IP_MOD_FRAGOFF    (1<<8)
#define IP_MOD_TTL        (1<<9)
#define IP_MOD_PROTOCOL   (1<<10)
#define IP_MOD_CHECK      (1<<11)
#define IP_MOD_SADDR      (1<<12)
#define IP_MOD_DADDR      (1<<13)

/* Options
 */
sendip_option ip_opts[] = {
	{"s","source",1,"Source IP address (see README)","127.0.0.1"},
	{"d","dest",1,"Desitnation IP address","Correct"},
	{"h","header_len",1,"IP header length (see README)","Correct"},
	{"v","version",1,"IP version (you almost definately don't want to change this)","4"},
	{"y","tos",1,"IP type of service","0"},
	{"l","tot_len",1,"Total IP packet length (see README)","Correct"},
	{"i","id",1,"IP packet ID (see README)","Random"},
	{"fr","reserved",1,"IP reservced flag (see README)","0 (options are 0,1,r)"},
	{"fd","flag_df",1,"IP don't fragment flag (see README)","0 (options are 0,1,r)"},
	{"fm","flag_mf",1,"IP more fragments flag (see README)","0 (options are 0,1,r)"},
	{"f","frag_off",1,"IP fragment offset","0"},
	{"t","ttl",1,"IP time to live","255"},
	{"p","proto",1,"IP protcol","0, or set by underlying protocol"},
	{"c","checksum",1,"IP checksum (see README)","Correct"},

	{"onum","option",1,"IP option as string of hex bytes (length is always correct)","(no options)"},
	{"oeol","option_eol",0,"IP option: end of list",NULL},
	{"onop","option_noop",0,"IP option: no-op",NULL},
	{"orr","option_rr",1,"IP option: record route. Format: pointer:addr1:addr2:...",NULL},
	{"ots","option_ts",1,"IP option: timestamp. Format: pointer:overflow:flag:(ip1:)ts1:(ip2:)ts2:...",NULL},
	{"olsr","option_lsr",1,"IP option: loose source route. Format: pointer:addr1:addr2:...",NULL},
	{"osid","option_sid",1,"IP option: stream identifier",NULL},
	{"ossr","option_ssr",1,"IP option: strict source route. Format: pointer:addr1:addr2:...",NULL}
};

#endif  /* _SENDIP_IP_H */
