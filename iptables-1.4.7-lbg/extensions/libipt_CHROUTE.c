/*
 *  Change the next hop of an skb
 * (C) 2014 Li Jian <lijian2@ucweb.com>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License version 2 as
 *	published by the Free Software Foundation.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <xtables.h>
#include <iptables.h>
#include <linux/version.h>

#include <linux/netfilter_ipv4/ipt_CHROUTE.h>

#define IPT_SNAT_OPT_GW 0x01

static void CHROUTE_help(void)
{
	printf(
"CHROUTE target options:\n"
"--gw ipaddr             The dest gateway change to\n"
"--old-gw ipaddr             The dest gateway change to\n");
}

static void CHROUTE_init(struct xt_entry_target *target)
{
  struct ipt_chroute_info *chrouteinfo = (struct ipt_chroute_info *)(target)->data;
  chrouteinfo->new_gw = 0;
  chrouteinfo->old_gw = 0;
}

static int CHROUTE_parse(int c, char **argv, int invert, unsigned int *flags,
                      const void *e, struct xt_entry_target **target)
{
	struct ipt_chroute_info *chrouteinfo = (struct ipt_chroute_info *)(*target)->data;
  const struct in_addr *ip;
  
  switch(c) {
  case '1':
    if (xtables_check_inverse(optarg, &invert, NULL, 0, argv))
			xtables_error(PARAMETER_PROBLEM,
				   "Unexpected `!' after --gw"); 

    ip = xtables_numeric_to_ipaddr(optarg);        
    if (!ip)
      xtables_error(PARAMETER_PROBLEM, "Bad IP address \"%s\"\n",
           optarg);
      
    chrouteinfo->new_gw = ip->s_addr;
    *flags |= IPT_SNAT_OPT_GW;
    return 1;

  case '2':
    if (xtables_check_inverse(optarg, &invert, NULL, 0, argv))
			xtables_error(PARAMETER_PROBLEM,
				   "Unexpected `!' after --old-gw");

    ip = xtables_numeric_to_ipaddr(optarg);
    if (!ip)
      xtables_error(PARAMETER_PROBLEM, "Bad IP address \"%s\"\n",
           optarg);
      
    chrouteinfo->old_gw = ip->s_addr;
    return 1;

  default:
    break;
  }
  return 0;
}

static void CHROUTE_check(unsigned int flags)
{
	if (!(flags & IPT_SNAT_OPT_GW))
		xtables_error(PARAMETER_PROBLEM,
			   "You must specify --gw");
}

static void CHROUTE_print(const void *ip, const struct xt_entry_target *target,
                       int numeric)
{
  const struct ipt_chroute_info *chrouteinfo = (struct ipt_chroute_info *)(target)->data;
  struct in_addr a;
  
  a.s_addr = chrouteinfo->new_gw;
  printf("gw:%s ", xtables_ipaddr_to_numeric(&a));
  if (chrouteinfo->old_gw) {
    a.s_addr = chrouteinfo->old_gw;
    printf("old-gw %s ", xtables_ipaddr_to_numeric(&a));
  }
}

static void CHROUTE_save(const void *ip, const struct xt_entry_target *target)
{
	const struct ipt_chroute_info *chrouteinfo = (struct ipt_chroute_info *)(target)->data;
  struct in_addr a;
  
  a.s_addr = chrouteinfo->new_gw;
  printf("--gw %s ", xtables_ipaddr_to_numeric(&a));
  if (chrouteinfo->old_gw) {
    a.s_addr = chrouteinfo->old_gw;
    printf("--old-gw %s ", xtables_ipaddr_to_numeric(&a));
  }
}

static const struct option CHROUTE_opts[] = {
	{ "gw", 1, NULL, '1' },
  { "old-gw", 1, NULL, '2' },
	{ .name = NULL }
};

static struct xtables_target chroute_tg_reg = {
	.name		= "CHROUTE",
	.version	= XTABLES_VERSION,
	.family		= NFPROTO_IPV4,
	.size		= XT_ALIGN(sizeof(struct ipt_chroute_info)),
	.userspacesize	= XT_ALIGN(sizeof(struct ipt_chroute_info)),
	.help		= CHROUTE_help,
  .init		= CHROUTE_init,
	.parse		= CHROUTE_parse,
  .final_check	= CHROUTE_check,
	.print		= CHROUTE_print,
	.save		= CHROUTE_save,
	.extra_opts	= CHROUTE_opts,
};

void _init(void)
{
	xtables_register_target(&chroute_tg_reg);
}
