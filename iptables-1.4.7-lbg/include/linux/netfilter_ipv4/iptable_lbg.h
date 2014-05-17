#ifndef IPT_IPTABLE_LBG_H
#define IPT_IPTABLE_LBG_H

#include <net/netfilter/nf_conntrack_extend.h>

enum nf_conn_lbg_status {
  NFC_LBG_CHROUTE_BIT = 0,
  NFC_LBG_CHROUTE = (1 << NFC_LBG_CHROUTE_BIT),

  /* Already chroute in pre_routing */
  NFC_LBG_PRE_CHROUTE_BIT = 1,
  NFC_LBG_PRE_CHROUTE = (1 << NFC_LBG_PRE_CHROUTE_BIT),
};

struct nf_conn;
struct net_device;

/* The structure embedded in the conntrack structure ext. */
struct nf_conn_lbg
{
  struct nf_conn *ct; 
  int old_out_idx;
  int new_out_idx; 
  __be32 new_gw;
  unsigned long status;
};

static inline struct nf_conn_lbg *nfct_lbg(const struct nf_conn *ct)
{
  return nf_ct_ext_find(ct, NF_CT_EXT_LBG);
}

static inline int nf_lbg_chroute_initialized(struct nf_conn_lbg *lbg_ext) {
  return test_bit(NFC_LBG_CHROUTE_BIT, &lbg_ext->status);
}

static inline int nf_lbg_pre_chroute(struct nf_conn_lbg *lbg_ext) {
  return test_bit(NFC_LBG_PRE_CHROUTE_BIT, &lbg_ext->status);
}

#endif /* IPT_IPTABLE_LBG_H */
