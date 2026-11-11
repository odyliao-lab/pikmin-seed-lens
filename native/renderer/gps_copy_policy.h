#pragma once
#include <cmath>
#include <cstdint>
#include <string>
namespace seed_lens::gps {
inline bool coordinate(double lat,double lng){return std::isfinite(lat)&&std::isfinite(lng)&&lat>=-90&&lat<=90&&lng>=-180&&lng<=180&&(std::fabs(lat)>0.0000001||std::fabs(lng)>0.0000001);}
// Adapted from control-center 8abb7ee selection policy; independent owner and
// bounded retry. Never persist account/item IDs or coordinates.
class Selection {
 std::string owner_;std::u16string key_;int64_t since_{};unsigned attempts_{};bool delivered_{};
public:
 bool same(const std::string& owner,const std::u16string& key)const{return owner==owner_&&key==key_;}
 void cancel(){owner_.clear();key_.clear();since_=0;attempts_=0;delivered_=false;}
 bool observe(const std::string& owner,const std::u16string& key,int64_t now,bool eligible){
  if(!eligible||owner.empty()||key.empty()){cancel();return false;}
  if(owner!=owner_||key!=key_){owner_=owner;key_=key;since_=now;attempts_=0;delivered_=false;}
  return !delivered_&&attempts_<3&&now>=since_&&now-since_>=400;
 }
 void attempted(int64_t now,bool success){++attempts_;since_=now;delivered_=success;}
};
}
