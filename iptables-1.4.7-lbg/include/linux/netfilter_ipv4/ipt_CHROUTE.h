#ifndef IPT_LBG_CHROUTE_H
#define IPT_LBG_CHROUTE_H

struct ipt_chroute_info {
  __be32 new_gw;
  __be32 old_gw;
};

#endif /* IPT_LBG_CHROUTE_H */
